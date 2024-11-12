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
#define LED_ON                  1
#define LED_OFF                 0
#define LED_1_PIN               GPIO_NUM_22
#define LED_2_PIN               GPIO_NUM_21

// Button information
#define BUTTON_1_PIN            GPIO_NUM_19
#define BUTTON_2_PIN            GPIO_NUM_18

// Semaphore handles
SemaphoreHandle_t xFirstSemaphore, xSecondSemaphore;

// Debouncing protection
int firstReady = 1;
int secondReady = 1;

// First LED pattern task information
TaskHandle_t xTaskHandleFirstLEDPattern = NULL;                                                         // Handle of the task (maximum size 16 characters)
char* pcTaskNameFirstLEDPattern = "PATTERN_1";                                                          // Name of the task
UBaseType_t uxTaskPriorityFirstLEDPattern = tskIDLE_PRIORITY + 1;                                       // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthFirstLEDPattern = configMINIMAL_STACK_SIZE * 8;            // Task stack size

// Second LED pattern task information
TaskHandle_t xTaskHandleSecondLEDPattern = NULL;                                                        // Handle of the task
char* pcTaskNameSecondLEDPattern =  "PATTERN_2";                                                        // Name of the task (maximum size 16 characters)
UBaseType_t uxTaskPrioritySecondLEDPattern = tskIDLE_PRIORITY + 1;                                      // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSecondLEDPattern = configMINIMAL_STACK_SIZE * 8;           // Task stack size

// Pattern function
void vFirstLEDPattern() {

    // Periods in ticks
    TickType_t xTaskSleepPeriod;

    // Do the pattern
    for (int i = 0; i < 10; i++) {

        xTaskSleepPeriod = (i + 1) * 50 / portTICK_PERIOD_MS;

        gpio_set_level(LED_1_PIN, LED_ON);
        vTaskDelay(xTaskSleepPeriod);
        
        gpio_set_level(LED_1_PIN, LED_OFF);
        vTaskDelay(xTaskSleepPeriod);

    }

}

// Pattern function
void vSecondLEDPattern() {

    // Interval in ticks
    TickType_t xTaskSleepPeriod;

    // Do the pattern
    for (int i = 0; i < 10; i++) {

        xTaskSleepPeriod = (10 - i) * 50 / portTICK_PERIOD_MS;

        gpio_set_level(LED_2_PIN, LED_ON);
        vTaskDelay(xTaskSleepPeriod);
        
        gpio_set_level(LED_2_PIN, LED_OFF);
        vTaskDelay(xTaskSleepPeriod);

    }

}

// First LED pattern function
void vTaskFirstLEDPattern(void* argument) {

    // Loop it forever
    while (1) {

        // Take the first semaphore
        if (xSemaphoreTake(xFirstSemaphore, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(FIRST_LED_PATTERN_TASK_TAG, "Error in taking the first semaphore");
        } else {
            ESP_LOGI(FIRST_LED_PATTERN_TASK_TAG, "First semaphore successfully taken");
        }

        // Execute first LED pattern
        ESP_LOGI(FIRST_LED_PATTERN_TASK_TAG, "Executing first pattern");
        vFirstLEDPattern();

        // Protection against debouncing
        firstReady = 1;

    }

}

// Second LED pattern function
void vTaskSecondLEDPattern(void* argument) {

    // Loop it forever
    while (1) {

        // Take the first semaphore
        if (xSemaphoreTake(xSecondSemaphore, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(SECOND_LED_PATTERN_TASK_TAG, "Error in taking the second semaphore");
        } else {
            ESP_LOGI(SECOND_LED_PATTERN_TASK_TAG, "Second semaphore successfully taken");
        }

        // Execute second LED pattern
        ESP_LOGI(SECOND_LED_PATTERN_TASK_TAG, "Executing second pattern");
        vSecondLEDPattern();

        // Protection against debouncing
        secondReady = 1;

    }

}

// Setup LED GPIO
void gpio_led_setup() {

    // LED 1 setup
    gpio_reset_pin(LED_1_PIN);                                  // Reset pin to default state
    gpio_set_direction(LED_1_PIN, GPIO_MODE_OUTPUT);            // Set pin output mode
    gpio_set_level(LED_1_PIN, LED_OFF);                         // Set pin level to 0
    
    // LED 2 setup
    gpio_reset_pin(LED_2_PIN);                                  // Reset pin to default state
    gpio_set_direction(LED_2_PIN, GPIO_MODE_OUTPUT);            // Set pin output mode
    gpio_set_level(LED_2_PIN, LED_OFF);                         // Set pin level to 0
    
}

// ISR handler function for first button
void firstButtonHandler(void* argument) {

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Debouncing protection
    if (firstReady) {
        firstReady = 0;

        // Give first semaphore from ISR (handler)
        xSemaphoreGiveFromISR(xFirstSemaphore, &xHigherPriorityTaskWoken);

    }

}

// ISR handler function for second button
void secondButtonHandler(void* argument) {

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Debouncing protection
    if (secondReady) {
        secondReady = 0;

        // Give second semaphore from ISR (handler)
        xSemaphoreGiveFromISR(xSecondSemaphore, &xHigherPriorityTaskWoken);

    }

}

// Setup button interrupts GPIO
void gpio_button_interrupts_setup() {

    // Set first button pin
    gpio_reset_pin(BUTTON_1_PIN);                           // Reset pin to default state
    gpio_set_direction(BUTTON_1_PIN, GPIO_MODE_INPUT);      // Set pin input mode
    gpio_set_intr_type(BUTTON_1_PIN, GPIO_INTR_NEGEDGE);    // Rising edge interrupt trigger on pin
    gpio_pullup_en(BUTTON_1_PIN);                           // Enable pull-up internal resistor

    // Set second button pin
    gpio_reset_pin(BUTTON_2_PIN);                           // Reset pin to default state
    gpio_set_direction(BUTTON_2_PIN, GPIO_MODE_INPUT);      // Set pin input mode
    gpio_set_intr_type(BUTTON_2_PIN, GPIO_INTR_NEGEDGE);    // Rising edge interrupt trigger on pin
    gpio_pullup_en(BUTTON_2_PIN);                           // Enable pull-up internal resistor
    
    // Register the already implemented global GPIO ISR, use default flags
    gpio_install_isr_service(0);
    
    // Register the handlers for the two button pins
    gpio_isr_handler_add(BUTTON_1_PIN, firstButtonHandler, NULL);
    gpio_isr_handler_add(BUTTON_2_PIN, secondButtonHandler, NULL);

    // Enable interrupts on the button two pins
    gpio_intr_enable(BUTTON_1_PIN);
    gpio_intr_enable(BUTTON_2_PIN);

}

// Entrypoint
void app_main(void) {

    // Create binary semaphores
    xFirstSemaphore = xSemaphoreCreateBinary();
    xSecondSemaphore = xSemaphoreCreateBinary();
    if (xFirstSemaphore == NULL || xSecondSemaphore == NULL) {
        ESP_LOGE(MAIN_TAG, "Error creating binary semaphores, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Binary semaphores created");

    // Setup the LED pins
    gpio_led_setup();
    ESP_LOGI(MAIN_TAG, "GPIO LED setup completed");

    gpio_button_interrupts_setup();
    ESP_LOGI(MAIN_TAG, "GPIO button interrupt setup completed");

    // Create the first LED pattern task
    if (xTaskCreate(vTaskFirstLEDPattern, pcTaskNameFirstLEDPattern, usTaskStackDepthFirstLEDPattern, NULL, uxTaskPriorityFirstLEDPattern, &xTaskHandleFirstLEDPattern) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "First LED pattern task creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "First LED pattern task creation completed");

    // Create the second LED pattern task
    if (xTaskCreate(vTaskSecondLEDPattern, pcTaskNameSecondLEDPattern, usTaskStackDepthSecondLEDPattern, NULL, uxTaskPrioritySecondLEDPattern, &xTaskHandleSecondLEDPattern) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Second LED pattern task creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Second LED pattern task creation completed");

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
