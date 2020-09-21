#ifndef BT_MODULE_INCLUDED
#define BT_MODULE_INCLUDED

#include <stdbool.h>
#include "esp_bt_defs.h"

typedef struct
{
    char *name;
    esp_bd_addr_t *bt_address;
} BtDevice;

void enableBluetooth(void);

bool btIsConnected();
BtDevice *btGetAvaibleDevices(int *deviceCount);
bool btConnectToDevice(BtDevice *btDevice);

bool btPlay(Song *songsParam, uint8_t count);
bool btPause();
bool btResume();

#endif