#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "driver/i2c_master.h"                                                  // I2C master functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define TASK_TRANSMIT_TAG               "transmit_task"

// I2C information       
#define I2C_SLAVE_ADDRESS               0x22                                    // Slave address
#define I2C_BUS_0                       I2C_NUM_0                               // Master bus

#define I2C_MASTER_SDA_IO               GPIO_NUM_22                             // GPIO 22 for SDA
#define I2C_MASTER_SCL_IO               GPIO_NUM_21                             // GPIO 21 for SCL

#define I2C_MASTER_SCL_FREQ             100000                                  // SCL frequency from master

#define I2C_TX_SIZE                     2                                       // Transmit message size
#define I2C_DELAY_TRANSMIT_MS           1000                                    // Delay between each transmition
#define I2C_TRANSMIT_REPEAT             20                                      // Repeat transmitions

// Transmit buffer over I2C
char command_tx[I2C_TX_SIZE] = {0};

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

// I2C bus and master handle
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t master_handle;

// I2C transmit task information
TaskHandle_t xTaskHandleI2CTransmit = NULL;                                                             // Handle of the task (maximum size 16 characters)
char* pcTaskNameI2CTransmit = "CMD_TX";                                                                 // Name of the task
UBaseType_t uxTaskPriorityI2CTransmit = tskIDLE_PRIORITY + 1;                                           // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthI2CTransmit = configMINIMAL_STACK_SIZE * 8;                // Task stack size

// I2C transmit command task function
void vTaskI2CTransmit(void* argument) {

    uint8_t select[4] = {0, 0, 1, 1}, set[4] = {1, 0, 1, 0};

    // Interval in ticks
    TickType_t xTaskSleepPeriod = I2C_DELAY_TRANSMIT_MS / portTICK_PERIOD_MS;

    // Start the transactions
	for (int i = 0; i < I2C_TRANSMIT_REPEAT; i++) {

		command_tx[0] = select[i % 4];          // Set first byte to be sent
		command_tx[1] = set[i % 4];             // Set second byte to be sent

		ESP_LOGI(TASK_TRANSMIT_TAG, "Sending data: select [%x], set [%x]", command_tx[0], command_tx[1]);

		ESP_ERROR_CHECK(i2c_master_transmit(master_handle, (uint8_t *)command_tx, I2C_TX_SIZE, -1));

        // Delay a bit        
        vTaskDelay(xTaskSleepPeriod);

	}

    vTaskDelete(NULL);

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

// Entrypoint
void app_main(void) {

    // Initialize bus and master
    i2c_bus_and_master_init();

    // Create the I2C transmit task
    xTaskCreate(vTaskI2CTransmit, pcTaskNameI2CTransmit, usTaskStackDepthI2CTransmit, NULL, uxTaskPriorityI2CTransmit, &xTaskHandleI2CTransmit);

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}