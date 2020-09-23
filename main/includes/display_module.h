#ifndef DISPLAY_MODULE_H_INCLUDDED
#define DISPLAY_MODULE_H_INCLUDDED

typedef  void (*onClickPtr)(void);

typedef struct
{
    char name[30];
    onClickPtr onClick;
} menuItem;

void displayPrintHeader(char *header);
void displayPrintItems(menuItem *items, uint8_t itemsCount);
void displayScrollDown();
void displayScrollUp();

#endif