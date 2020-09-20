#ifndef BT_MODULE_INCLUDED
#define BT_MODULE_INCLUDED

#include <stdbool.h>
#include "esp_bt_defs.h"

typedef struct
{
    char *name;
    esp_bd_addr_t *bt_address;
} BtDevice;

bool btIsConnected();
BtDevice *btGetAvaibleDevices(int *deviceCount);
bool btConnectToDevice(BtDevice *btDevice);

bool btPlaySongs(Song *songsParam, uint8_t count);
bool btPauseSong();

#endif