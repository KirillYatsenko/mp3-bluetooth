#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>

#include "display_module.h"
#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

#define PIN_SDA 21
#define PIN_SCL 22

#define MAX_ITEMS 2;

const char *TAG = "Display";

static u8g2_t u8g2; // a structure which will contain all the data for one display

static menuItem *menuItems;
static uint8_t menuItemsCount;
static uint8_t selectedItemIdx = 0;
static bool displayStarted = false;

static void displayStart();
static void fillDisplay(menuItem firstItem, menuItem *secondItem);

void displayPrintHeader(char *header)
{
    if (displayStarted == false)
        displayStart();

    displayClearHeaderArea();

    ESP_LOGI(TAG, "displayPrintItems called header = %s\n", header);
    u8g2_DrawStr(&u8g2, 6, 8, header);
    u8g2_SendBuffer(&u8g2);
}

void displayPrintItems(menuItem *items, uint8_t itemsCount)
{
    ESP_LOGI(TAG, "displayPrintItems called\n");

    if (displayStarted == false)
        displayStart();

    free(menuItems);

    menuItems = items;
    menuItems = (menuItem *)malloc(sizeof(menuItem) * itemsCount);
    memcpy(menuItems, items, sizeof(menuItem) * itemsCount);

    menuItemsCount = itemsCount;
    selectedItemIdx = 0;

    if (menuItemsCount > 1)
        fillDisplay(items[0], &(items[1]));
    else
        fillDisplay(items[0], NULL);
}

void displaySelectNext()
{
    ESP_LOGI(TAG, "displaySelectNext called\n");

    if (selectedItemIdx + 1 == menuItemsCount) // there is no items to scroll down
        return;

    selectedItemIdx++;

    printf("selectedItemIdx = %d, menuItemsCount = %d\n", selectedItemIdx, menuItemsCount);
    if (selectedItemIdx + 1 != menuItemsCount)
        fillDisplay(menuItems[selectedItemIdx], &(menuItems[selectedItemIdx + 1]));
    else
        fillDisplay(menuItems[selectedItemIdx], NULL);
}

void displaySelectPrev()
{
    ESP_LOGI(TAG, "displaySelectPrev called\n");

    if (selectedItemIdx == 0) // there is no items to scroll up
        return;

    selectedItemIdx--;

    if (selectedItemIdx != menuItemsCount + 1)
        fillDisplay(menuItems[selectedItemIdx], &(menuItems[selectedItemIdx + 1]));
    else
        fillDisplay(menuItems[selectedItemIdx], NULL);
}

void displaySelectIndx(uint8_t indx)
{
    selectedItemIdx = indx;

    if (selectedItemIdx != menuItemsCount + 1)
        fillDisplay(menuItems[selectedItemIdx], &(menuItems[selectedItemIdx + 1]));
    else
        fillDisplay(menuItems[selectedItemIdx], NULL);
}

menuItem displayGetSelectedItem()
{
    return menuItems[selectedItemIdx];
}

uint8_t displayGetSelectedItemIndx()
{
    return selectedItemIdx;
}

void displayClearHeaderArea()
{
    ESP_LOGI(TAG, "displayClearHeaderArea called");
    if (displayStarted)
    {
        u8g2_DrawStr(&u8g2, 6, 8, "               ");
        u8g2_SendBuffer(&u8g2);
    }
}

void displayClearItemsArea()
{
    ESP_LOGI(TAG, "displayClearItemsArea called");
    if (displayStarted)
    {
        u8g2_DrawStr(&u8g2, 2, 19, "                ");
        u8g2_DrawStr(&u8g2, 2, 27, "                ");
        u8g2_SendBuffer(&u8g2);
    }
}

static void fillDisplay(menuItem firstItem, menuItem *secondItem)
{
    ESP_LOGI(TAG, "fillDisplay called\n");

    displayClearItemsArea();
    char firstItemText[50];
    sprintf(firstItemText, "-> %s", firstItem.name);

    u8g2_DrawStr(&u8g2, 2, 19, firstItemText);

    if (secondItem != NULL)
        u8g2_DrawStr(&u8g2, 2, 27, secondItem->name);

    u8g2_SendBuffer(&u8g2);
}

static void displayStart()
{
    ESP_LOGI(TAG, "starting display....\n");

    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.sda = PIN_SDA;
    u8g2_esp32_hal.scl = PIN_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1306_128x32_univision_f(
        &u8g2,
        U8G2_R0,
        //u8x8_byte_sw_i2c,
        u8g2_esp32_msg_i2c_cb,
        u8g2_esp32_msg_i2c_and_delay_cb); // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

    ESP_LOGI(TAG, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

    ESP_LOGI(TAG, "u8g2_SetPowerSave");
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    ESP_LOGI(TAG, "u8g2_ClearBuffer");
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_amstrad_cpc_extended_8f);

    displayStarted = true;
}