#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "menu_module.h"
#include "display_module.h"
#include "bt_module.h"

typedef struct
{
    char name[20];
    menuItem *menuItems;
    uint8_t menuItemsCount;
    onClickPtr onClick;
} menu;

static menu selectedMenu;
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

    // int devicesCount = 0;
    // BtDevice *devices = btGetAvaibleDevices(&devicesCount);

    // menuItem items[devicesCount];

    // printf("Devices found=%d\n", devicesCount);

    // strcpy(selectedMenu.name, "Main");
    // selectedMenu.menuItemsCount = devicesCount;
    // selectedMenu.menuItems = realloc(selectedMenu.menuItems, sizeof(menuItem) * devicesCount);

    // for (int i = 0; i < devicesCount; i++)
    // {
    //     strcpy(items[i].name, devices[i].name);
    //     items[i].onClick = connectToBdDevice;
    // }

    // memcpy(selectedMenu.menuItems, items, sizeof(menuItem) * devicesCount);

    // displayPrintHeader("Bluetooth devices");
    // displayPrintItems(items, devicesCount);
}

static void connectToBdDevice()
{
    printf("connectToBdDevice called\n");
    return;
}