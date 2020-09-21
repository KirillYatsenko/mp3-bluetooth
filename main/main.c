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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "storage.h"
#include "bt_module.h"

int app_main(void)
{
    // printf("enabling bluetooth\n");
    // enableBluetooth();


    enableBluetooth();
    vTaskDelay(30000 / portTICK_PERIOD_MS);
    
    int songsCount = 0;
    Song *songs = getSongs(&songsCount);
    printf("songs count = %d\n", songsCount);

    // BtDevice *devices = NULL;
    // int devicesCount = 0;
    // while (devicesCount == 0)
    // {
    //     printf("coundn't find any devices\n");
    //     devices = btGetAvaibleDevices(&devicesCount);
    // }

    // printf("device count = %d\n", devicesCount);
    // for (int i = 0; i < devicesCount; i++)
    //     printf("device #%d - %s\n", i + 1, devices[i].name);

    // printf("IsConnected = %d\n", btIsConnected());

    // if (devicesCount > 0)
    // {
    //     btConnectToDevice(&(devices[0]));
        btPlay(songs, songsCount);
    // }

    vTaskDelay(10000 / portTICK_PERIOD_MS);
    printf("IsConnected = %d\n", btIsConnected());

    // vTaskDelay(20000 / portTICK_PERIOD_MS);
    // printf("Pausing = %d\n", btPause());

    // vTaskDelay(10000 / portTICK_PERIOD_MS);
    // printf("Resuming = %d\n", btResume());

    return 0;
}