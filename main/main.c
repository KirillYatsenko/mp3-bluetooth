/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "storage.h"
#include "bt_module.h"

int app_main(void)
{
    int songsCount = 0;
    Song *songs = getSongs(&songsCount);
    printf("songs count = %d\n", songsCount);

    BtDevice *devices = NULL; 
    int devicesCount = 0;
    while (devicesCount == 0)
        devices = getAvaibleDevices(&devicesCount);

    printf("device count = %d\n", devicesCount);
    for (int i = 0; i < devicesCount; i++)
        printf("device #%d - %s\n", i + 1, devices[i].name);

    if (devicesCount > 0)
    {
        connectToDevice(&(devices[0]));
        playSongs(songs, songsCount);
    }

    return 0;
}