#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"

#include "storage.h"

static const char *TAG = "SD";

#define MOUNT_POINT "/sdcard"
#define SPI_DMA_CHAN 1

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5

#define CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED false

static bool init();

Song *getSongs(int *count)
{
    // ToDo: check if not initialize
    init();

    unsigned int file_count = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(MOUNT_POINT "/songs");
    while (d && (dir = readdir(d)) != NULL && (dir->d_type == DT_REG))
        file_count++;

    rewinddir(d);

    unsigned int i = 0;
    Song *songs = (Song *)malloc(file_count * sizeof(Song));
    while (d && (dir = readdir(d)) != NULL && (dir->d_type == DT_REG))
    {
        songs[i].d = malloc(sizeof(struct dirent));
        memcpy(songs[i++].d, dir, sizeof(struct dirent));
    }

    // *count = file_count;
    closedir(d);

    return songs;
}

static bool init()
{
    esp_err_t ret;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return false;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return false;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
    return true;
}