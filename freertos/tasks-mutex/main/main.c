#include <stdio.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/semphr.h"                                                    // FreeRTOS semaphore and mutex operations
#include "esp_log.h"                                                            // Logging operations

// Definitions
#define LED_PIN                 GPIO_NUM_22
#define LED_ON                  1
#define LED_OFF                 0
#define LED_REPEAT              3
#define LED_TASK_NUMBERS        10

// Mutex info
SemaphoreHandle_t xMutex;

// Debug
char *mainTag = "main_task";
char *regularTaskTag = "regular_task";

// LED pattern task info
UBaseType_t uxTaskPriorityLEDPattern = tskIDLE_PRIORITY + 1;                                        // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthLEDPattern = configMINIMAL_STACK_SIZE * 8;             // Task stack size
int taskIdArray[LED_TASK_NUMBERS] = {0};

// Pattern function
void vLEDPattern(int taskId, TickType_t xTaskSleepPeriod) {

    // Do the pattern
    for(int i = 0; i < 5; i++) {

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
    while(counter < LED_REPEAT) {

        // Take mutex
        if(xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(regularTaskTag, "Mutex take error, id: %d! Iteration %d.", taskId, counter + 1);
        } else {
            ESP_LOGI(regularTaskTag, "Mutex successfully taken, id: %d.", taskId);
        }

        // Execute LED pattern
        ESP_LOGI(regularTaskTag, "Executing pattern id: %d.", taskId);
        vLEDPattern(taskId, xTaskSleepPeriod);

        // Give mutex
        if(xSemaphoreGive(xMutex) != pdTRUE) {
            ESP_LOGE(regularTaskTag, "Mutex give error, id: %d! Iteration %d.", taskId, counter + 1);
        } else {
            ESP_LOGI(regularTaskTag, "Mutex successfully given, id: %d.", taskId);
        }

        counter++;

    }

    // Self delete
    ESP_LOGI(regularTaskTag, "Self deleting id: %d, goodbye!", taskId);
    vTaskDelete(NULL);

}

// Setup the GPIO
esp_err_t gpio_setup() {

    esp_err_t esp_err = ESP_OK;                                 // For debugging
    esp_err = gpio_reset_pin(LED_PIN);                          // Reset pin to default state
    esp_err = gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);    // Set pin output mode
    esp_err = gpio_set_level(LED_PIN, LED_OFF);                 // Set pin level to 0
    return esp_err;

}

// Entrypoint
void app_main(void) {

    // Setup the LED pin
    if(gpio_setup() != ESP_OK) {
        ESP_LOGE(mainTag, "GPIO setup error! Exiting...");
        return;
    }
    ESP_LOGI(mainTag, "GPIO setup complete!");

    // Create mutex
    xMutex = xSemaphoreCreateMutex();
    if(xMutex == NULL) {
        ESP_LOGE(mainTag, "Error creating mutex! Exiting...");
        return;
    }
    ESP_LOGI(mainTag, "Created mutex!");

    // Create the sender task
    for(int i = 0; i < LED_TASK_NUMBERS; i++) {
        taskIdArray[i] = i;
        if(xTaskCreate(vTaskLEDPattern, NULL, usTaskStackDepthLEDPattern, &taskIdArray[i], uxTaskPriorityLEDPattern, NULL) != pdPASS) {
            ESP_LOGE(mainTag, "LED pattern task %d creation error! Exiting...", i);
            return;
        }
    }
    ESP_LOGI(mainTag, "LED pattern tasks creation complete!");

    ESP_LOGI(mainTag, "Exiting...");

}
