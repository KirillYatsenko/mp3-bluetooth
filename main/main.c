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
    // int songsCount = 0;
    // getSongs(&songsCount);
    // printf("songs count = %d\n", songsCount);

    int devicesCount = 0;
    BtDevice *devices = getAvaibleDevices(&devicesCount);
    printf("device count = %d\n",devicesCount);
    for (int i = 0; i < devicesCount; i++)
        printf("device #%d - %s\n", i + 1, devices[i].name);

    return 0;
}