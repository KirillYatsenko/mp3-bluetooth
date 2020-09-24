#ifndef DISPLAY_MODULE_H_INCLUDDED
#define DISPLAY_MODULE_H_INCLUDDED

typedef  void (*onClickPtr)(void);

typedef struct
{
    char name[30];
    uint8_t data;
    onClickPtr onClick;
} menuItem;

void displayPrintHeader(char *header);
void displayPrintItems(menuItem *items, uint8_t itemsCount);
void displayScrollDown();
void displayScrollUp();
menuItem displayGetSelectedItem();
uint8_t displayGetSelectedItemIndx();
void displayClearItemsArea();


#endif