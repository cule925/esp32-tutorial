#include <stdio.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/semphr.h"                                                    // FreeRTOS semaphore and mutex operations
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define FIRST_LED_PATTERN_TASK_TAG      "first_led_pattern_task"
#define SECOND_LED_PATTERN_TASK_TAG     "second_led_pattern_task"

// LED information
#define LED_PIN                 GPIO_NUM_22
#define LED_ON                  1
#define LED_OFF                 0
#define LED_REPEAT              5

// Semaphore info
SemaphoreHandle_t xFirstSemaphore, xSecondSemaphore;

// First LED pattern task info
TaskHandle_t xTaskHandleFirstLEDPattern = NULL;                                                         // Handle of the task
char* pcTaskNameFirstLEDPattern = "MY_FIRST_LED_PATTERN_TASK";                                          // Name of the task (maximum size is 16 characters)
UBaseType_t uxTaskPriorityFirstLEDPattern = tskIDLE_PRIORITY + 1;                                       // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthFirstLEDPattern = configMINIMAL_STACK_SIZE * 8;            // Task stack size

// Second LED pattern task info
TaskHandle_t xTaskHandleSecondLEDPattern = NULL;                                                        // Handle of the task
char* pcTaskNameSecondLEDPattern = "MY_SECOND_LED_PATTERN_TASK";                                        // Name of the task (maximum size is 16 characters)
UBaseType_t uxTaskPrioritySecondLEDPattern = tskIDLE_PRIORITY + 1;                                      // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSecondLEDPattern = configMINIMAL_STACK_SIZE * 8;           // Task stack size

// Pattern function
void vFirstLEDPattern() {

    // Periods in ticks
    TickType_t xTaskSleepPeriod;

    // Do the pattern
    for (int i = 0; i < 10; i++) {

        xTaskSleepPeriod = (i + 1) * 70 / portTICK_PERIOD_MS;

        gpio_set_level(LED_PIN, LED_ON);
        vTaskDelay(xTaskSleepPeriod);
        
        gpio_set_level(LED_PIN, LED_OFF);
        vTaskDelay(xTaskSleepPeriod);

    }

}

// Pattern function
void vSecondLEDPattern() {

    // Interval in ticks
    TickType_t xTaskSleepPeriod;

    // Do the pattern
    for (int i = 0; i < 10; i++) {

        xTaskSleepPeriod = (10 - i) * 70 / portTICK_PERIOD_MS;

        gpio_set_level(LED_PIN, LED_ON);
        vTaskDelay(xTaskSleepPeriod);
        
        gpio_set_level(LED_PIN, LED_OFF);
        vTaskDelay(xTaskSleepPeriod);

    }

}

// First LED pattern function
void vTaskFirstLEDPattern(void* argument) {

    int counter = 0;

    // Loop it
    while (counter < LED_REPEAT) {

        // Take the first semaphore
        xSemaphoreTake(xFirstSemaphore, portMAX_DELAY);
        ESP_LOGI(FIRST_LED_PATTERN_TASK_TAG, "First semaphore successfully taken");
        
        // Execute first LED pattern
        ESP_LOGI(FIRST_LED_PATTERN_TASK_TAG, "Executing first pattern");
        vFirstLEDPattern();

        // Give the second semaphore
        xSemaphoreGive(xSecondSemaphore);
        ESP_LOGI(FIRST_LED_PATTERN_TASK_TAG, "Second semaphore successfully given");

        counter++;

    }

    // Self delete
    ESP_LOGI(FIRST_LED_PATTERN_TASK_TAG, "Self deleting, goodbye");
    vTaskDelete(NULL);

}

// Second LED pattern function
void vTaskSecondLEDPattern(void* argument) {

    int counter = 0;

    // Loop it
    while (counter < LED_REPEAT) {

        // Take the first semaphore
        xSemaphoreTake(xSecondSemaphore, portMAX_DELAY);
        ESP_LOGI(SECOND_LED_PATTERN_TASK_TAG, "Second semaphore successfully taken");

        // Execute second LED pattern
        ESP_LOGI(SECOND_LED_PATTERN_TASK_TAG, "Executing second pattern");
        vSecondLEDPattern();

        // Give the second semaphore
        xSemaphoreGive(xFirstSemaphore);
        ESP_LOGI(SECOND_LED_PATTERN_TASK_TAG, "First semaphore successfully given");

        counter++;

    }

    // Self delete
    ESP_LOGI(SECOND_LED_PATTERN_TASK_TAG, "Self deleting, goodbye");
    vTaskDelete(NULL);

}

// Setup the GPIO
void gpio_setup() {

    gpio_reset_pin(LED_PIN);                            // Reset pin to default state
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);      // Set pin output mode
    gpio_set_level(LED_PIN, LED_OFF);                   // Set pin level to 0

}

// Entrypoint
void app_main(void) {

    // Setup the LED pin
    gpio_setup();
    ESP_LOGI(MAIN_TAG, "GPIO setup complete");

    // Create binary semaphores
    xFirstSemaphore = xSemaphoreCreateBinary();
    xSecondSemaphore = xSemaphoreCreateBinary();
    if (xFirstSemaphore == NULL || xSecondSemaphore == NULL) {
        ESP_LOGE(MAIN_TAG, "Error creating binary semaphores, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Created binary semaphores");

    // Create the first LED pattern task
    if (xTaskCreate(vTaskFirstLEDPattern, pcTaskNameFirstLEDPattern, usTaskStackDepthFirstLEDPattern, NULL, uxTaskPriorityFirstLEDPattern, &xTaskHandleFirstLEDPattern) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "First LED pattern task creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "First LED pattern task creation complete");

    // Create the second LED pattern task
    if (xTaskCreate(vTaskSecondLEDPattern, pcTaskNameSecondLEDPattern, usTaskStackDepthSecondLEDPattern, NULL, uxTaskPrioritySecondLEDPattern, &xTaskHandleSecondLEDPattern) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Second LED pattern task creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Second LED pattern task creation complete");

    // Release the first semaphore, start the sequence
    ESP_LOGI(MAIN_TAG, "Giving green light to the first task");
    xSemaphoreGive(xFirstSemaphore);
    ESP_LOGI(MAIN_TAG, "First semaphore successfully given");

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
