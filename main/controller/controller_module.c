#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "controller_module.h"

#define CONTROLLER_TAG "Controller"
#define BUTTON_UP 25
#define BUTTON_SELECT 26
#define BUTTON_DOWN 27

static button_cb buttonUpCb;
static button_cb buttonSelectCb;
static button_cb buttonDownCb;
static xQueueHandle interputQueue;

static void callButtonPressedCallback(button_cb cb);
static void buttonPushedTask(void *params);
static void IRAM_ATTR gpio_isr_handler(void *args);

void controllerInit()
{
    ESP_LOGI(CONTROLLER_TAG, "Start initializing...");

    gpio_config_t config;
    config.intr_type = GPIO_INTR_POSEDGE;
    config.mode = GPIO_MODE_INPUT;
    config.pull_down_en = true;
    config.pull_up_en = false;
    config.pin_bit_mask = ((1ULL << BUTTON_UP) | (1ULL << BUTTON_SELECT) | (1ULL << BUTTON_DOWN));
    gpio_config(&config);

    interputQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(buttonPushedTask, "buttonPushedTask", 8000, NULL, 1, NULL);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(BUTTON_UP, gpio_isr_handler, (void *)BUTTON_UP);
    gpio_isr_handler_add(BUTTON_SELECT, gpio_isr_handler, (void *)BUTTON_SELECT);
    gpio_isr_handler_add(BUTTON_DOWN, gpio_isr_handler, (void *)BUTTON_DOWN);

    ESP_LOGI(CONTROLLER_TAG, "Initializing done!");
}

void controllerRegisterButtonUp(button_cb cb)
{
    buttonUpCb = cb;
}

void controllerRegisterButtonSelect(button_cb cb)
{
    buttonSelectCb = cb;
}

void controllerRegisterButtonDown(button_cb cb)
{
    buttonDownCb = cb;
}

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

static void buttonPushedTask(void *params)
{
    int pinNumber = 0;
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
            switch (pinNumber)
            {
            case BUTTON_UP:
                callButtonPressedCallback(buttonUpCb);
                break;
            case BUTTON_SELECT:
                callButtonPressedCallback(buttonSelectCb);
                break;
            case BUTTON_DOWN:
                callButtonPressedCallback(buttonDownCb);
                break;
            }

            ESP_LOGI(CONTROLLER_TAG, "Button Pressed PIN = %d", pinNumber);

            // re-enable the interrupt
            gpio_isr_handler_add(pinNumber, gpio_isr_handler, (void *)pinNumber);
        }
    }
}

static void callButtonPressedCallback(button_cb cb)
{
    if (cb != NULL)
        cb();
}