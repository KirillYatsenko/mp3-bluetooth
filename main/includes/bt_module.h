#ifndef BT_MODULE_INCLUDED
#define BT_MODULE_INCLUDED

#include <stdbool.h>
#include "esp_bt_defs.h"

typedef struct
{
    char *name;
    esp_bd_addr_t *bt_address;
} BtDevice;

bool isConnected();
BtDevice *getAvaibleDevices(int *deviceCount);
bool connectToDevice(BtDevice *btDevice);

bool playSongs(Song *songsParam, uint8_t count);
bool pauseSong();

#endif