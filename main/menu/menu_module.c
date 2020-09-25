#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "menu_module.h"
#include "display_module.h"
#include "bt_module.h"
#include "storage.h"

#define MENU_MODULE_TAG "MENU"

typedef struct
{
    char name[20];
    menuItem *menuItems;
    uint8_t menuItemsCount;
    onClickPtr onClick;
} menu;

static menu selectedMenu;
static BtDevice *devices;
static Song *songs;
static int songsCount;

void btConnectedCb(char *deviceName);
void nextSongCb(uint8_t songIndx);
void openBtConnectedTask(void *param);
void playSongsTask(void *param);

static void playSong();
static void openBluetoothMenu();
static void connectToBdDeviceMenu();
static void openSongsMenu();


void menuDisplayMain()
{
    int mainMenuItemsCount = 2;

    menuItem items[mainMenuItemsCount];
    strcpy(items[0].name, "Songs");
    strcpy(items[1].name, "Bluetooth");
    items[0].onClick = openSongsMenu;
    items[1].onClick = openBluetoothMenu;

    strcpy(selectedMenu.name, "Main");
    selectedMenu.menuItemsCount = mainMenuItemsCount;
    selectedMenu.menuItems = (menuItem *)malloc(sizeof(menuItem) * mainMenuItemsCount);
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
    displaySelectPrev();
}

void menuPressDown()
{
    displaySelectNext();
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
    selectedMenu.menuItems = (menuItem *)realloc(selectedMenu.menuItems, sizeof(menuItem) * devicesCount);

    for (int i = 0; i < devicesCount; i++)
    {
        strcpy(items[i].name, devices[i].name);
        items[i].data = i;
        items[i].onClick = connectToBdDeviceMenu;
    }

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * devicesCount);

    displayPrintHeader("Devices");
    displayPrintItems(items, devicesCount);
}

static void connectToBdDeviceMenu()
{
    printf("connectToBdDeviceMenu called\n");

    uint8_t deviceIndx = displayGetSelectedItem().data;
    BtDevice device = devices[deviceIndx];

    btConnectToDevice(&device, btConnectedCb);
}

void btConnectedCb(char *deviceName)
{
    ESP_LOGI(MENU_MODULE_TAG, "bt device connected: %s", deviceName);
    xTaskCreate(&openBtConnectedTask, deviceName, 8000, NULL, 1, NULL);
}

void openBtConnectedTask(void *deviceName)
{
    displayClearItemsArea();
    displayPrintHeader("Connected!");

    uint8_t btConnectedMenuItemsCount = 1;
    menuItem items[btConnectedMenuItemsCount];

    strcpy(selectedMenu.name, "BtConnected");
    selectedMenu.menuItemsCount = btConnectedMenuItemsCount;
    selectedMenu.menuItems = (menuItem *)realloc(selectedMenu.menuItems, sizeof(menuItem) * btConnectedMenuItemsCount);

    strcpy(items[0].name, "Back");
    items[0].onClick = menuDisplayMain;

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * btConnectedMenuItemsCount);

    displayPrintItems(items, btConnectedMenuItemsCount);
    vTaskDelete(NULL);
}

static void openSongsMenu()
{
    displayClearItemsArea();
    displayPrintHeader("Songs");

    songs = getSongs(&songsCount);
    printf("songs count = %d\n", songsCount);

    menuItem items[songsCount + 1];

    printf("Songs found=%d\n", songsCount);

    strcpy(selectedMenu.name, "Songs");
    selectedMenu.menuItemsCount = songsCount;
    selectedMenu.menuItems = (menuItem *)realloc(selectedMenu.menuItems, sizeof(menuItem) * (songsCount + 1));

    strcpy(items[0].name, "Back");
    items[0].onClick = menuDisplayMain;

    for (int i = 0; i < songsCount; i++)
    {
        strcpy(items[i + 1].name, songs[i].d->d_name);
        items[i].data = i;
        items[i].onClick = playSong;
    }

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * songsCount + 1);
    displayPrintItems(items, songsCount + 1);
}

static void playSong()
{
    ESP_LOGI(MENU_MODULE_TAG, "playSongs called");
    xTaskCreate(&playSongsTask, "playSong", 8000, NULL, 1, NULL);
}

void playSongsTask(void *param)
{
    uint8_t songIndx = displayGetSelectedItem().data;
    btPlay(songs, songsCount, songIndx, nextSongCb);

    while (true)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void nextSongCb(uint8_t songIndx)
{
    displaySelectNext();
}