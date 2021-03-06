#ifndef BT_MODULE_INCLUDED
#define BT_MODULE_INCLUDED

#include <stdbool.h>
#include "esp_bt_defs.h"

#include "storage.h"

typedef struct
{
    char *name;
    esp_bd_addr_t *bt_address;
} BtDevice;

typedef void (*connect_cb)(char *);
typedef void (*nextSong_cb)(uint8_t);

void enableBluetooth(void);

bool btIsConnected();
BtDevice *btGetAvaibleDevices(int *deviceCount, connect_cb cb);
void btConnectToDevice(BtDevice *btDevice, connect_cb cb);
bool btIsPaused();

bool btPlay(Song *songsParam, uint8_t count, uint8_t startPlayIndx, nextSong_cb nextSong_cb);
bool btPause();
bool btResume();

#endif