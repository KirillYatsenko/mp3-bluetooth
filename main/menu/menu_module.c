#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "menu_module.h"
#include "display_module.h"
#include "bt_module.h"
#include "storage.h"

typedef struct
{
    char name[20];
    menuItem *menuItems;
    uint8_t menuItemsCount;
    onClickPtr onClick;
} menu;

static menu selectedMenu;
static BtDevice *devices;

void btConnectedCb(char *deviceName);
void playSongsTask(void *param);
static void openBluetoothMenu();
static void connectToBdDevice();

void menuDisplayMain()
{
    int mainMenuItemsCount = 2;

    menuItem items[mainMenuItemsCount];
    strcpy(items[0].name, "Songs");
    strcpy(items[1].name, "Bluetooth");
    items[1].onClick = openBluetoothMenu;

    strcpy(selectedMenu.name, "Main");
    selectedMenu.menuItemsCount = mainMenuItemsCount;
    selectedMenu.menuItems = malloc(sizeof(menuItem) * mainMenuItemsCount);
    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * mainMenuItemsCount);

    displayPrintHeader("Main");
    displayPrintItems(items, mainMenuItemsCount);
}

void menuPressSelect()
{
    uint8_t selectedIndx = displayGetSelectedItemIndx();

    if (selectedMenu.menuItems[selectedIndx].onClick != NULL)
        selectedMenu.menuItems[selectedIndx].onClick();
    else
        selectedMenu.onClick();
}

void menuPressUp()
{
    displayScrollUp();
}

void menuPressDown()
{
    displayScrollDown();
}

static void openBluetoothMenu()
{
    displayClearItemsArea();
    displayPrintHeader("Scanning...");

    int devicesCount = 0;
    devices = btGetAvaibleDevices(&devicesCount, btConnectedCb);

    menuItem items[devicesCount];

    printf("Devices found=%d\n", devicesCount);

    strcpy(selectedMenu.name, "Main");
    selectedMenu.menuItemsCount = devicesCount;
    selectedMenu.menuItems = realloc(selectedMenu.menuItems, sizeof(menuItem) * devicesCount);

    for (int i = 0; i < devicesCount; i++)
    {
        strcpy(items[i].name, devices[i].name);
        items[i].data = i;
        items[i].onClick = connectToBdDevice;
    }

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * devicesCount);

    displayPrintHeader("Devices");
    displayPrintItems(items, devicesCount);
}

static void connectToBdDevice()
{
    printf("connectToBdDevice called\n");

    uint8_t deviceIndx = displayGetSelectedItem().data;
    BtDevice device = devices[deviceIndx];

    btConnectToDevice(&device, btConnectedCb);
}

void btConnectedCb(char *deviceName)
{
    printf("Hura bt connected!\n");
    xTaskCreate(&playSongsTask, "play song task", 8000, NULL, 1, NULL);
}

void playSongsTask(void *param)
{
    int stackMem = uxTaskGetStackHighWaterMark(NULL);
    printf("stack = %d\n", stackMem);

    int songsCount = 0;
    Song *songs = getSongs(&songsCount);
    printf("songs count = %d\n", songsCount);

    btPlay(songs, songsCount);

    while(true) vTaskDelay(1000);
}