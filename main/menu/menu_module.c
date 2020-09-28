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
static int currentSongIndx;

void btConnectedCb(char *deviceName);
void nextSongCb(uint8_t songIndx);
void openBtConnectedTask(void *param);
void openPlayerMenuTask();
void playNextSong();
void playNextSongTask();
void playPrevSong();
void playPrevSongTask();
void playSong(uint8_t songIndx);

static void openBluetoothMenu();
static void connectToBdDeviceMenu();
static void openSongsMenu();
static void openBtNotFoundMenu();
static void openPlayerMenu();

void menuDisplayMain()
{
    displayClearItemsArea();
    displayClearHeaderArea();

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
    ESP_LOGI(MENU_MODULE_TAG, "select button pressed");
    uint8_t selectedIndx = displayGetSelectedItemIndx();

    ESP_LOGI(MENU_MODULE_TAG, "menuPressSelect selectedIndx = %d", selectedIndx);

    menuItem item = selectedMenu.menuItems[selectedIndx];
    printf("item name = %s\n", item.name);

    if (selectedMenu.menuItems[selectedIndx].onClick != NULL)
        selectedMenu.menuItems[selectedIndx].onClick();
    else
        selectedMenu.onClick();
}

void menuPressUp()
{
    ESP_LOGI(MENU_MODULE_TAG, "up button pressed");
    displaySelectPrev();
}

void menuPressDown()
{
    ESP_LOGI(MENU_MODULE_TAG, "down button pressed");
    displaySelectNext();
}

static void openBluetoothMenu()
{
    if (btIsConnected() == true)
    {
        btConnectedCb("toDo");
        return;
    }

    displayClearItemsArea();
    displayPrintHeader("Scanning...");

    int devicesCount = 0;
    devices = btGetAvaibleDevices(&devicesCount, btConnectedCb);

    if (btIsConnected() == true)
        return;

    if (devicesCount == 0)
    {
        openBtNotFoundMenu();
        return;
    }

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

static void openBtNotFoundMenu()
{
    displayClearItemsArea();
    displayPrintHeader("Scan again");

    uint8_t btConnectedMenuItemsCount = 2;
    menuItem items[btConnectedMenuItemsCount];

    strcpy(selectedMenu.name, "BtNotFound");
    selectedMenu.menuItemsCount = btConnectedMenuItemsCount;
    selectedMenu.menuItems = (menuItem *)realloc(selectedMenu.menuItems, sizeof(menuItem) * btConnectedMenuItemsCount);

    strcpy(items[0].name, "Scan");
    items[0].onClick = openBluetoothMenu;

    strcpy(items[1].name, "Back");
    items[1].onClick = menuDisplayMain;

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * btConnectedMenuItemsCount);

    displayPrintItems(items, btConnectedMenuItemsCount);
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
    selectedMenu.menuItemsCount = songsCount + 1;
    selectedMenu.menuItems = (menuItem *)realloc(selectedMenu.menuItems, sizeof(menuItem) * (songsCount + 1));

    strcpy(items[0].name, "Back");
    items[0].onClick = menuDisplayMain;

    for (uint8_t i = 0; i < songsCount; i++)
    {
        strcpy(items[i + 1].name, songs[i].d->d_name);
        items[i + 1].data = i;
        items[i + 1].onClick = openPlayerMenu;
    }

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * (songsCount + 1));
    displayPrintItems(items, songsCount + 1);
}

static void openPlayerMenu()
{
    ESP_LOGI(MENU_MODULE_TAG, "openPlayerMenu called");
    xTaskCreate(&openPlayerMenuTask, "openPlayerMenu", 8000, NULL, 1, NULL);
}

void openPlayerMenuTask()
{
    displayClearItemsArea();
    displayClearHeaderArea();

    uint8_t songIndx = displayGetSelectedItem().data;

    uint8_t btConnectedMenuItemsCount = 4;
    menuItem items[btConnectedMenuItemsCount];

    strcpy(selectedMenu.name, "playerMenu");
    selectedMenu.menuItemsCount = btConnectedMenuItemsCount;
    selectedMenu.menuItems = (menuItem *)realloc(selectedMenu.menuItems, sizeof(menuItem) * btConnectedMenuItemsCount);

    strcpy(items[0].name, "Back");
    items[0].onClick = openSongsMenu;

    strcpy(items[1].name, "Pause");
    items[1].onClick = btPause;

    strcpy(items[2].name, "Next");
    items[2].onClick = playNextSong;

    strcpy(items[3].name, "Prev");
    items[3].onClick = playPrevSong;

    memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * btConnectedMenuItemsCount);

    displayPrintHeader(songs[songIndx].d->d_name);
    displayPrintItems(items, btConnectedMenuItemsCount);

    playSong(songIndx);
}

void playNextSong()
{
    ESP_LOGI(MENU_MODULE_TAG, "playNextSong called");
    xTaskCreate(&playNextSongTask, "playNextSongTask", 8000, NULL, 1, NULL);
}

void playNextSongTask()
{
    ESP_LOGI(MENU_MODULE_TAG, "playNextSongTask called currentSongIndx = %d", currentSongIndx);

    if (currentSongIndx == songsCount - 1)
        currentSongIndx = 0;
    else
        currentSongIndx++;

    displayPrintHeader(songs[currentSongIndx].d->d_name);
    playSong(currentSongIndx);
}

void playPrevSong()
{
    ESP_LOGI(MENU_MODULE_TAG, "playPrevSong called");
    xTaskCreate(&playNextSongTask, "playPrevSong", 8000, NULL, 1, NULL);
}

void playPrevSongTask()
{
    ESP_LOGI(MENU_MODULE_TAG, "playPrevSongTask called currentSongIndx = %d", currentSongIndx);

    if (currentSongIndx == 0)
        currentSongIndx = songsCount - 1;
    else
        currentSongIndx--;

    displayPrintHeader(songs[currentSongIndx].d->d_name);
    playSong(currentSongIndx);
}

void playSong(uint8_t songIndx)
{
    ESP_LOGI(MENU_MODULE_TAG, "playSongsTask called songIndx = %d", songIndx);
    printf("songIndx = %d\n", songIndx);

    btPlay(songs, songsCount, songIndx, nextSongCb);

    if (btIsPaused())
        btResume();

    while (true)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void nextSongCb(uint8_t songIndx)
{
    ESP_LOGI(MENU_MODULE_TAG, "nextSongCb is called songIndx = %d songsCount = %d", songIndx, songsCount);

    currentSongIndx = songIndx;

    if (strcmp("playerMenu", selectedMenu.name) == 0)
    {
        displayClearHeaderArea();
        displayPrintHeader(songs[songIndx].d->d_name);
    }
}