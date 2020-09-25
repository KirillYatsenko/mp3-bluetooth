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
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "storage.h"
#include "bt_module.h"
#include "menu_module.h"

#define PIN_SWITCH 25
#define PIN_SWITCH_2 26
#define PIN_SWITCH_3 27

void testButtons();
void testDisplay();
void testPlayer();

int app_main(void)
{
    testButtons();
    // testDisplay();
    // testPlayer();
    return 0;
}

xQueueHandle interputQueue;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

void buttonPushedTask(void *params)
{
    int pinNumber, count = 0;
    while (true)
    {
        if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
        {
            // disable the interrupt
            gpio_isr_handler_remove(pinNumber);

            // wait some time while we check for the button to be released
            do
            {
                vTaskDelay(20 / portTICK_PERIOD_MS);
            } while (gpio_get_level(pinNumber) == 1);

            //do some work
            printf("GPIO %d was pressed %d times. The state is %d\n", pinNumber, count++, gpio_get_level(PIN_SWITCH));

            // re-enable the interrupt
            gpio_isr_handler_add(pinNumber, gpio_isr_handler, (void *)pinNumber);
        }
    }
}

void testButtons()
{
    gpio_pad_select_gpio(PIN_SWITCH);
    gpio_set_direction(PIN_SWITCH, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIN_SWITCH);
    gpio_pullup_dis(PIN_SWITCH);
    gpio_set_intr_type(PIN_SWITCH, GPIO_INTR_POSEDGE);

    gpio_pad_select_gpio(PIN_SWITCH_2);
    gpio_set_direction(PIN_SWITCH_2, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIN_SWITCH_2);
    gpio_pullup_dis(PIN_SWITCH_2);
    gpio_set_intr_type(PIN_SWITCH_2, GPIO_INTR_POSEDGE);

    gpio_pad_select_gpio(PIN_SWITCH_3);
    gpio_set_direction(PIN_SWITCH_3, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIN_SWITCH_3);
    gpio_pullup_dis(PIN_SWITCH_3);
    gpio_set_intr_type(PIN_SWITCH_3, GPIO_INTR_POSEDGE);

    interputQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(buttonPushedTask, "buttonPushedTask", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_SWITCH, gpio_isr_handler, (void *)PIN_SWITCH);
    gpio_isr_handler_add(PIN_SWITCH_2, gpio_isr_handler, (void *)PIN_SWITCH_2);
    gpio_isr_handler_add(PIN_SWITCH_3, gpio_isr_handler, (void *)PIN_SWITCH_2);
}

void testDisplay()
{
    // menuDisplayMain();
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // printf("pressing down button one times\n");
    // menuPressDown(); // select bluetooth
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // menuPressSelect(); // open bluetooth
    // menuPressSelect(); // select device
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
    // menuPressSelect(); // device connectd -> go back

    menuDisplayMain();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    menuPressSelect(); // Open songs
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    menuPressDown(); // select next song
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    menuPressDown(); // select next song
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    menuPressDown(); // select next song
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    menuPressSelect(); // start playing

    // vTaskDelay(3000 / portTICK_PERIOD_MS);
    // printf("pressing up button two times\n");
    // menuPressUp();
    // menuPressUp();
}

// void testPlayer()
// {
//     printf("enabling bluetooth\n");
//     enableBluetooth();

//     enableBluetooth();
//     vTaskDelay(30000 / portTICK_PERIOD_MS);

//     int songsCount = 0;
//     Song *songs = getSongs(&songsCount);
//     printf("songs count = %d\n", songsCount);

//     BtDevice *devices = NULL;
//     int devicesCount = 0;
//     while (devicesCount == 0)
//     {
//         printf("coundn't find any devices\n");
//         devices = btGetAvaibleDevices(&devicesCount);
//     }

//     printf("device count = %d\n", devicesCount);
//     for (int i = 0; i < devicesCount; i++)
//         printf("device #%d - %s\n", i + 1, devices[i].name);

//     printf("IsConnected = %d\n", btIsConnected());

//     if (devicesCount > 0)
//     {
//         btConnectToDevice(&(devices[0]), NULL);
//         btPlay(songs, songsCount);
//     }

//     vTaskDelay(10000 / portTICK_PERIOD_MS);
//     printf("IsConnected = %d\n", btIsConnected());

//     vTaskDelay(20000 / portTICK_PERIOD_MS);
//     printf("Pausing = %d\n", btPause());

//     vTaskDelay(10000 / portTICK_PERIOD_MS);
//     printf("Resuming = %d\n", btResume());
// }