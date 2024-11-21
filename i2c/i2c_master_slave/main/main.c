#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/semphr.h"                                                    // FreeRTOS semaphore and mutex operations
#include "driver/i2c_master.h"                                                  // I2C master functions
#include "driver/i2c_slave.h"                                                   // I2C slave functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define TASK_PRINT_TAG                  "print_received_task"
#define TASK_TRANSMIT_TAG               "transmit_task"

// I2C information       
#define I2C_SLAVE_ADDRESS               0x22                                    // Slave address
#define I2C_BUS_0                       I2C_NUM_0                               // Master bus
#define I2C_BUS_1                       I2C_NUM_1                               // Slave bus

#define I2C_MASTER_SDA_IO               GPIO_NUM_22                             // GPIO 22 for SDA
#define I2C_MASTER_SCL_IO               GPIO_NUM_21                             // GPIO 21 for SCL
#define I2C_SLAVE_SDA_IO                GPIO_NUM_19                             // GPIO 22 for SDA
#define I2C_SLAVE_SCL_IO                GPIO_NUM_18                             // GPIO 21 for SCL

#define I2C_MASTER_SCL_FREQ             100000                                  // SCL frequency from master

#define I2C_TX_SIZE                     40                                      // Transmit message size
#define I2C_DELAY_TRANSMIT_MS           1000                                    // Delay between each transmit of character

#define I2C_RX_SIZE                     40                                      // Receive message size
#define I2C_SLAVE_RINGBUFFER_SIZE       256                                     // Must be a power of 2

// Declaration of I2C slave callback
bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg);

// Messages that will be sent over I2C
char msg_tx_1[I2C_TX_SIZE] = "Hello this is master.";
char msg_tx_2[I2C_TX_SIZE] = "Anyone there?";

// Message that will be received over I2C
char msg_rx[I2C_RX_SIZE] = {0};

// Master bus configuration
i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_BUS_0,                                      // Assign to bus 0
    .sda_io_num = I2C_MASTER_SDA_IO,                            // SDA pin
    .scl_io_num = I2C_MASTER_SCL_IO,                            // SCL pin
    .clk_source = I2C_CLK_SRC_DEFAULT,                          // Default clock source (APB)
    .glitch_ignore_cnt = 7,                                     // Glitch period (filtering)
    .intr_priority = 0,                                         // ISR interrupt priority default
    .trans_queue_depth = 0,                                     // Will not be using asynchronous transmitions
    .flags.enable_internal_pullup = false,                      // Will be using external pull-up resistors
};

// Master device configuration
i2c_device_config_t master_config = {
    .device_address = I2C_SLAVE_ADDRESS,                        // Target slave address
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,                      // 7 bit target slave address length
    .scl_speed_hz = I2C_MASTER_SCL_FREQ,                        // Master SCL frequency
    .scl_wait_us = 0,                                           // Use default SCL await time
};

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
SemaphoreHandle_t xSemaphorePrint;

// I2C bus and master handle
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t master_handle;

// I2C slave handle
i2c_slave_dev_handle_t slave_handle;

// I2C transmit task information
TaskHandle_t xTaskHandleI2CTransmit = NULL;                                                             // Handle of the task (maximum size 16 characters)
char* pcTaskNameI2CTransmit = "I2C_TX";                                                                 // Name of the task
UBaseType_t uxTaskPriorityI2CTransmit = tskIDLE_PRIORITY + 1;                                           // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthI2CTransmit = configMINIMAL_STACK_SIZE * 8;                // Task stack size

// Print received task information
TaskHandle_t xTaskHandlePrint = NULL;                                                                   // Handle of the task (maximum size 16 characters)
char* pcTaskNamePrint = "PRINT_RX";                                                                     // Name of the task
UBaseType_t uxTaskPriorityPrint = tskIDLE_PRIORITY + 1;                                                 // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthPrint = configMINIMAL_STACK_SIZE * 8;                      // Task stack size

// I2C slave rx done ISR callback (called when all bytes arrive in slave ring buffer)
bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg) {

    // Copy from ring buffer to the message received array
    memcpy(msg_rx, evt_data->buffer, I2C_RX_SIZE);

    // Finally print the whole message
    xSemaphoreGiveFromISR(xSemaphorePrint, NULL);
    
    // If a higher priority task has been waken up after this function executed
    return false;

}

// I2C transmit task function
void vTaskI2CTransmit(void* argument) {

    // Interval in ticks
    TickType_t xTaskSleepPeriod = I2C_DELAY_TRANSMIT_MS / portTICK_PERIOD_MS;

    // Loop it forever
    while (1) {

        // Transmit first message with I2C (blocking)
        ESP_LOGI(TASK_TRANSMIT_TAG, "Transmiting first message");
        ESP_ERROR_CHECK(i2c_master_transmit(master_handle, (uint8_t *)msg_tx_1, I2C_TX_SIZE, -1));

        // Delay a bit        
        vTaskDelay(xTaskSleepPeriod);

        // Transmit second message with I2C (blocking)
        ESP_LOGI(TASK_TRANSMIT_TAG, "Transmiting second message");
        ESP_ERROR_CHECK(i2c_master_transmit(master_handle, (uint8_t *)msg_tx_2, I2C_TX_SIZE, -1));

        // Delay a bit        
        vTaskDelay(xTaskSleepPeriod);

    }

}

// Final print task
void vTaskISRFinalPrint(void* argument) {

    // Loop it forever
    while (1) {

        // When the given amount of data arrives in the ring buffer, after the ISR the aforementioned callback will be called
        ESP_LOGI(TASK_PRINT_TAG, "Receiving message");
        i2c_slave_receive(slave_handle, (uint8_t *)msg_rx, I2C_RX_SIZE);

        // Take semaphore (this one is released from the ISR)
        xSemaphoreTake(xSemaphorePrint, portMAX_DELAY);

        // Print received message
        ESP_LOGI(TASK_PRINT_TAG, "Received message %s", msg_rx);

    }

}

// I2C initialize bus and master
void i2c_bus_and_master_init() {

    // Initialize bus
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    // Checking if address exists
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, I2C_SLAVE_ADDRESS, 10000));
    ESP_LOGI(MAIN_TAG, "Slave with address %x detected", I2C_SLAVE_ADDRESS);

    // Add master
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &master_config, &master_handle));

}

// I2C initialize
void i2c_slave_init() {

    // Initialize slave
    ESP_ERROR_CHECK(i2c_new_slave_device(&slave_config, &slave_handle));
    
    // Register callback for slave
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &i2c_slave_event_callback, NULL));

}

// Entrypoint
void app_main(void) {

    // Create binary semaphores
    xSemaphorePrint = xSemaphoreCreateBinary();
    if (xSemaphorePrint == NULL) {
        ESP_LOGE(MAIN_TAG, "Error creating binary semaphores, exiting");
        return;
    }

    // Initialize slave
    i2c_slave_init();

    // Initialize bus and master
    i2c_bus_and_master_init();

    // Create the ISR print task
    xTaskCreate(vTaskISRFinalPrint, pcTaskNamePrint, usTaskStackDepthPrint, NULL, uxTaskPriorityPrint, &xTaskHandlePrint);

    // Create the I2C transmit task
    xTaskCreate(vTaskI2CTransmit, pcTaskNameI2CTransmit, usTaskStackDepthI2CTransmit, NULL, uxTaskPriorityI2CTransmit, &xTaskHandleI2CTransmit);

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
