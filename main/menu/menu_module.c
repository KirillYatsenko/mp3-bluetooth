#include <string.h>
#include <stdint.h>

#include "menu_module.h"
#include "display_module.h"

typedef struct
{
    char name[20];
    menuItem *menuItems;
    uint8_t menuItemsCount;
    onClickPtr onClick;
} menu;

void menuDisplayMain()
{
    menuItem items[2];
    strcpy(items[0].name, "Songs");
    strcpy(items[1].name, "Bluetooth");

    displayPrintHeader("Main");
    displayPrintItems(items, 2);
}

void menuPressUp()
{
    displayScrollUp();
}

void menuPressDown()
{
    displayScrollDown();
}
