#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/semphr.h"                                                    // FreeRTOS semaphore and mutex operations
#include "driver/i2c_master.h"                                                  // I2C master functions
#include "driver/i2c_slave.h"                                                   // I2C slave functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define TASK_SET_LED_TAG                "set_led_task"
#define TASK_TRANSMIT_TAG               "transmit_task"

// LED information
#define LED_ON                  1
#define LED_OFF                 0
#define LED_1_PIN               GPIO_NUM_22
#define LED_2_PIN               GPIO_NUM_21

// I2C information       
#define I2C_SLAVE_ADDRESS               0x22
#define I2C_BUS_1                       I2C_NUM_1                               // Slave bus

#define I2C_SLAVE_SDA_IO                GPIO_NUM_19                             // GPIO 22 for SDA
#define I2C_SLAVE_SCL_IO                GPIO_NUM_18                             // GPIO 21 for SCL

#define I2C_RX_SIZE                     2                                       // Receive buffer size
#define I2C_SLAVE_RINGBUFFER_SIZE       32                                      // Must be a power of 2

// Declaration of I2C slave callback
bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg);

// Receive buffer over I2C
uint8_t command_rx[I2C_RX_SIZE] = {0};

// Slave device configuration
i2c_slave_config_t slave_config = {
    .i2c_port = I2C_BUS_1,                                      // Assign to bus 1
    .sda_io_num = I2C_SLAVE_SDA_IO,                             // SDA pin
    .scl_io_num = I2C_SLAVE_SCL_IO,                             // SCL pin
    .clk_source = I2C_CLK_SRC_DEFAULT,                          // Default clock source (APB)
    .send_buf_depth = I2C_SLAVE_RINGBUFFER_SIZE,                // Set uint8_t ring buffer size, for best performanse it should be a power of 2
    .slave_addr = I2C_SLAVE_ADDRESS,                            // Slave address
    .addr_bit_len = I2C_ADDR_BIT_LEN_7,                         // 7 bit slave address length
    .intr_priority = 0,                                         // ISR interrupt priority default
};

// Slave device rx callback
i2c_slave_event_callbacks_t i2c_slave_event_callback = {
    .on_recv_done = i2c_slave_rx_done_callback,                 // Pointer to callback function
};

// Semaphore handle
SemaphoreHandle_t xSemaphoreSetLED;

// I2C slave handle
i2c_slave_dev_handle_t slave_handle;

// Set LED task information
TaskHandle_t xTaskHandleSetLED = NULL;                                                                  // Handle of the task (maximum size 16 characters)
char* pcTaskNameSetLED = "SET_LED_RX";                                                                  // Name of the task
UBaseType_t uxTaskPrioritySetLED = tskIDLE_PRIORITY + 1;                                                // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSetLED = configMINIMAL_STACK_SIZE * 8;                     // Task stack size

// I2C slave rx done ISR callback (called when all bytes arrive in slave ring buffer)
bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg) {

    // Copy from ring buffer to the receive buffer
    memcpy(command_rx, evt_data->buffer, I2C_RX_SIZE);

    // Set the right LED to the right state
    xSemaphoreGiveFromISR(xSemaphoreSetLED, NULL);
    
    // If a higher priority task has been waken up after this function executed
    return false;

}

// Set LED task
void vTaskISRSetLED(void* argument) {

    // Loop it forever
    while (1) {

        // When the given amount of data arrives in the ring buffer, after the ISR the aforementioned callback will be called
        ESP_LOGI(TASK_SET_LED_TAG, "Listening for command");
        i2c_slave_receive(slave_handle, command_rx, I2C_RX_SIZE);

        // Take semaphore (this one is released from the ISR)
        xSemaphoreTake(xSemaphoreSetLED, portMAX_DELAY);

        // Set the state of the right LED
        ESP_LOGI(TASK_SET_LED_TAG, "Received data: select [%x], set [%x]", command_rx[0], command_rx[1]);

        // set the right LED
        switch (command_rx[0]) {
            case 0:
                if (command_rx[1] == 0) gpio_set_level(LED_1_PIN, LED_OFF);
                if (command_rx[1] == 1) gpio_set_level(LED_1_PIN, LED_ON);
                break;
            case 1:
                if (command_rx[1] == 0) gpio_set_level(LED_2_PIN, LED_OFF);
                if (command_rx[1] == 1) gpio_set_level(LED_2_PIN, LED_ON);
                break;
            default:
                ESP_LOGE(TASK_SET_LED_TAG, "Wrongly chosen LED");        
        }

    }

}

// I2C initialize
void i2c_slave_init() {

    // Initialize slave
    ESP_ERROR_CHECK(i2c_new_slave_device(&slave_config, &slave_handle));
    
    // Register callback for slave
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &i2c_slave_event_callback, NULL));

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

// Entrypoint
void app_main(void) {

    // Create binary semaphores
    xSemaphoreSetLED = xSemaphoreCreateBinary();
    if (xSemaphoreSetLED == NULL) {
        ESP_LOGE(MAIN_TAG, "Error creating binary semaphores, exiting");
        return;
    }

    // Setup the LED pins
    gpio_led_setup();

    // Initialize slave
    i2c_slave_init();

    // Create the ISR set LED task
    xTaskCreate(vTaskISRSetLED, pcTaskNameSetLED, usTaskStackDepthSetLED, NULL, uxTaskPrioritySetLED, &xTaskHandleSetLED);

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
