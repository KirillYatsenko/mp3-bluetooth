#ifndef BT_MODULE_INCLUDED
#define BT_MODULE_INCLUDED

#include <stdbool.h>

typedef struct
{
    char *name;
} BtDevice;

bool isConnected();
BtDevice *getAvaibleDevices(int *deviceCount);
bool connect(BtDevice *btDevice);

bool playSongs(Song *songs);
bool pauseSong();

#endif