#include <stdio.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "esp_log.h"                                                            // Logging operations

// Definitions
#define LED_PIN                 GPIO_NUM_22
#define LED_ON                  1
#define LED_OFF                 0
#define LED_INTERVAL_ON_MS      1000
#define LED_INTERVAL_OFF_MS     1000
#define LED_REPEAT              5

// Debug
char *mainTag = "main_task";
char *ledTaskTag = "led_task";

// Task info
TaskHandle_t xTaskHandleLED = NULL;                                                 // Handle of the task
char* pcTaskNameLED = "MY_LED_TASK";                                                // Name of the task
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
    while(counter < LED_REPEAT) {

        // Set pin level to 1
        ESP_LOGI(ledTaskTag, "LED ON!");
        if(gpio_set_level(LED_PIN, LED_ON) != ESP_OK) {
            ESP_LOGE(ledTaskTag, "GPIO set level error! Iteration %d.", counter + 1);
            break;
        }

        // Wait for LED ON period
        if(xTaskDelayUntil(&xTaskWakeTime, xTaskSleepLEDOn) != pdTRUE) {
            ESP_LOGE(ledTaskTag, "Time delay error! Iteration %d.", counter + 1);
            break;
        }

        // Set pin level to 1
        ESP_LOGI(ledTaskTag, "LED OFF!");
        if(gpio_set_level(LED_PIN, LED_OFF) != ESP_OK) {
            ESP_LOGE(ledTaskTag, "GPIO set level error! Iteration %d.", counter + 1);
            break;
        }

        // Wait for LED OFF period
        if(xTaskDelayUntil(&xTaskWakeTime, xTaskSleepLEDOff) != pdTRUE) {
            ESP_LOGE(ledTaskTag, "Time delay error! Iteration %d.", counter + 1);
            break;
        }

        // Increment counter
        counter++;

    }

    // Self delete
    ESP_LOGI(ledTaskTag, "Self deleting, goodbye!");
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

    // Create the task
    if(xTaskCreate(vTaskFunctionLED, pcTaskNameLED, usTaskStackDepthLED, NULL, uxTaskPriorityLED, &xTaskHandleLED) != pdPASS) {
        ESP_LOGE(mainTag, "Task creation error! Exiting...");
        return;
    }
    ESP_LOGI(mainTag, "Task creation complete!");

    ESP_LOGI(mainTag, "Exiting...");

}
