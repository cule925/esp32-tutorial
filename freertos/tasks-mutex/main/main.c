#include <stdio.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/semphr.h"                                                    // FreeRTOS semaphore and mutex operations
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define REGULAR_TASK_TAG                "regular_task"

// LED information
#define LED_PIN                 GPIO_NUM_22
#define LED_ON                  1
#define LED_OFF                 0
#define LED_REPEAT              3
#define LED_TASK_NUMBERS        10

// Mutex info
SemaphoreHandle_t xMutex;

// LED pattern task info
UBaseType_t uxTaskPriorityLEDPattern = tskIDLE_PRIORITY + 1;                                        // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthLEDPattern = configMINIMAL_STACK_SIZE * 8;             // Task stack size
int taskIdArray[LED_TASK_NUMBERS] = {0};

// Pattern function
void vLEDPattern(int taskId, TickType_t xTaskSleepPeriod) {

    // Do the pattern
    for (int i = 0; i < 5; i++) {

        gpio_set_level(LED_PIN, LED_ON);
        vTaskDelay(xTaskSleepPeriod);
        
        gpio_set_level(LED_PIN, LED_OFF);
        vTaskDelay(xTaskSleepPeriod);

    }

}

// LED pattern function
void vTaskLEDPattern(void* argument) {

    int taskId = *((int *)argument);
    int counter = 0;

    // Periods in ticks
    TickType_t xTaskSleepPeriod = ((taskId + 1) * 70) / portTICK_PERIOD_MS;

    // Loop it
    while (counter < LED_REPEAT) {

        // Take mutex
        xSemaphoreTake(xMutex, portMAX_DELAY);
        ESP_LOGI(REGULAR_TASK_TAG, "Mutex successfully taken, id: %d", taskId);

        // Execute LED pattern
        ESP_LOGI(REGULAR_TASK_TAG, "Executing pattern id: %d", taskId);
        vLEDPattern(taskId, xTaskSleepPeriod);

        // Give mutex
        xSemaphoreGive(xMutex);
        ESP_LOGI(REGULAR_TASK_TAG, "Mutex successfully given, id: %d", taskId);

        counter++;

    }

    // Self delete
    ESP_LOGI(REGULAR_TASK_TAG, "Self deleting id: %d, goodbye", taskId);
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

    // Create mutex
    xMutex = xSemaphoreCreateMutex();
    if(xMutex == NULL) {
        ESP_LOGE(MAIN_TAG, "Error creating mutex, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Created mutex");

    // Create the sender task
    for (int i = 0; i < LED_TASK_NUMBERS; i++) {
        taskIdArray[i] = i;
        if(xTaskCreate(vTaskLEDPattern, NULL, usTaskStackDepthLEDPattern, &taskIdArray[i], uxTaskPriorityLEDPattern, NULL) != pdPASS) {
            ESP_LOGE(MAIN_TAG, "LED pattern task %d creation error, exiting", i);
            return;
        }
    }
    ESP_LOGI(MAIN_TAG, "LED pattern tasks creation complete");

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
