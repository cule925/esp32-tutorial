#include <stdio.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                            "main_task"
#define LED_TASK_TAG                        "led_task"

// LED information
#define LED_PIN                 GPIO_NUM_22
#define LED_ON                  1
#define LED_OFF                 0
#define LED_INTERVAL_ON_MS      1000
#define LED_INTERVAL_OFF_MS     1000
#define LED_REPEAT              5

// Task info
TaskHandle_t xTaskHandleLED = NULL;                                                 // Handle of the task
char* pcTaskNameLED = "LED_TASK";                                                   // Name of the task
UBaseType_t uxTaskPriorityLED = tskIDLE_PRIORITY + 1;                               // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthLED = configMINIMAL_STACK_SIZE * 8;    // Task stack size

// Task function to execute
void vTaskFunctionLED(void* argument) {

    // Counter
    int counter = 0;

    // Periods in ticks
    TickType_t xTaskWakeTime;
    TickType_t xTaskSleepLEDOn = LED_INTERVAL_ON_MS / portTICK_PERIOD_MS;
    TickType_t xTaskSleepLEDOff = LED_INTERVAL_OFF_MS / portTICK_PERIOD_MS;

    // Initial tick count
    xTaskWakeTime = xTaskGetTickCount();

    // Loop it
    while (counter < LED_REPEAT) {

        // Set pin level to 1
        ESP_LOGI(LED_TASK_TAG, "LED ON");
        gpio_set_level(LED_PIN, LED_ON);

        // Wait for LED ON period
        xTaskDelayUntil(&xTaskWakeTime, xTaskSleepLEDOn);

        // Set pin level to 1
        ESP_LOGI(LED_TASK_TAG, "LED OFF");
        gpio_set_level(LED_PIN, LED_OFF);

        // Wait for LED OFF period
        xTaskDelayUntil(&xTaskWakeTime, xTaskSleepLEDOff);

        // Increment counter
        counter++;

    }

    // Self delete
    ESP_LOGI(LED_TASK_TAG, "Self deleting, goodbye");
    vTaskDelete(NULL);

}

// Setup the GPIO
void gpio_setup() {

    gpio_reset_pin(LED_PIN);                          // Reset pin to default state
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);    // Set pin output mode
    gpio_set_level(LED_PIN, LED_OFF);                 // Set pin level to 0

}

// Entrypoint
void app_main(void) {

    // Setup the LED pin
    gpio_setup();
    ESP_LOGI(MAIN_TAG, "GPIO setup complete");

    // Create the task
    if (xTaskCreate(vTaskFunctionLED, pcTaskNameLED, usTaskStackDepthLED, NULL, uxTaskPriorityLED, &xTaskHandleLED) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Task creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Task creation complete");

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
