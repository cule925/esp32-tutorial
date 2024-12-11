#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "driver/uart.h"                                                        // UART functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define TASK_PRINT_TAG                  "print_received_task"
#define TASK_TRANSMIT_TAG               "transmit_task"

// UART information
#define UART_PORT                       UART_NUM_2                              // UART port
#define UART_BAUD_RATE                  115200                                  // Baud rate
#define UART_DATA_BITS                  UART_DATA_8_BITS                        // Symbol width 8 bits
#define UART_PARITY                     UART_PARITY_DISABLE                     // No parity
#define UART_STOP_BITS                  UART_STOP_BITS_1                        // 1 stop bit
#define UART_FLOW_CTRL                  UART_HW_FLOWCTRL_DISABLE                // No RTS or CTS
#define UART_SOURCE_CLOCK               UART_SCLK_DEFAULT                       // Default source clock (APB CLK)
#define UART_TX                         GPIO_NUM_25                             // TX multiplexed pin
#define UART_RX                         GPIO_NUM_26                             // RX multiplexed pin

#define UART_TX_MSG_SIZE                40                                      // Transmit message size
#define UART_TX_RINGBUFFER_SIZE         256                                     // Must be a power of 2
#define UART_DELAY_TRANSMIT_MS          1000                                    // Delay between each transmit of character

#define UART_RX_MSG_SIZE                40                                      // Receive message size
#define UART_RX_RINGBUFFER_SIZE         256                                     // Must be a power of 2

// Messages that will be sent over UART
char msg_tx_1[UART_TX_MSG_SIZE] = "Hello this is ESP32.";
char msg_tx_2[UART_TX_MSG_SIZE] = "Anyone there?";

// Message that will be received over UART
char msg_rx[UART_RX_MSG_SIZE] = {0};

// UART port configuration
uart_config_t uart_config = {
    .baud_rate = UART_BAUD_RATE,
    .data_bits = UART_DATA_BITS,
    .parity = UART_PARITY,
    .stop_bits = UART_STOP_BITS,
    .flow_ctrl = UART_FLOW_CTRL,
    .source_clk = UART_SOURCE_CLOCK,
};

// UART transmit task information
TaskHandle_t xTaskHandleUARTTransmit = NULL;                                                            // Handle of the task (maximum size 16 characters)
char* pcTaskNameUARTTransmit = "UART_TX";                                                               // Name of the task
UBaseType_t uxTaskPriorityUARTTransmit = tskIDLE_PRIORITY + 1;                                          // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthUARTTransmit = configMINIMAL_STACK_SIZE * 8;               // Task stack size

// Print received task information
TaskHandle_t xTaskHandlePrint = NULL;                                                                   // Handle of the task (maximum size 16 characters)
char* pcTaskNamePrint = "PRINT_RX";                                                                     // Name of the task
UBaseType_t uxTaskPriorityPrint = tskIDLE_PRIORITY + 1;                                                 // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthPrint = configMINIMAL_STACK_SIZE * 8;                      // Task stack size

// UART transmit task function
void vTaskUARTTransmit(void* argument) {

    // Interval in ticks
    TickType_t xTaskSleepPeriod = UART_DELAY_TRANSMIT_MS / portTICK_PERIOD_MS;

    // Loop it forever
    while (1) {

        // Write the first message to the TX ring buffer
        ESP_LOGI(TASK_TRANSMIT_TAG, "Transmiting first message");
        if (uart_write_bytes(UART_PORT, (void *)msg_tx_1, UART_TX_MSG_SIZE) < 0) ESP_LOGE(TASK_TRANSMIT_TAG, "Transmit error!");        // Copy into the TX ring buffer, ISR will to TX FIFO buffer
        ESP_ERROR_CHECK(uart_wait_tx_done(UART_PORT, portMAX_DELAY));                                                                   // Wait until all data was sent, that is the TX FIFO buffer is empty
        
        // Delay a bit        
        vTaskDelay(xTaskSleepPeriod);

        // Write the second message to the TX ring buffer
        ESP_LOGI(TASK_TRANSMIT_TAG, "Transmiting second message");
        if (uart_write_bytes(UART_PORT, (void *)msg_tx_2, UART_TX_MSG_SIZE) < 0) ESP_LOGE(TASK_TRANSMIT_TAG, "Transmit error!");        // Copy into the TX ring buffer, ISR will to TX FIFO buffer
        ESP_ERROR_CHECK(uart_wait_tx_done(UART_PORT, portMAX_DELAY));                                                                   // Wait until all data was sent, that is the TX FIFO buffer is empty

        // Delay a bit        
        vTaskDelay(xTaskSleepPeriod);

    }

}

// Print task
void vTaskPrint(void* argument) {

    // Loop it forever
    while (1) {

        // Read the message from the RX ring buffer
        if (uart_read_bytes(UART_PORT, msg_rx, UART_RX_MSG_SIZE, portMAX_DELAY) < 0) ESP_LOGE(TASK_PRINT_TAG, "Receive error!");        // Copy from the RX ring buffer when there is exactly 40 bytes in it

        // Print received message
        ESP_LOGI(TASK_PRINT_TAG, "Received message %s", msg_rx);

    }

}

// Initialize UART
void uart_init() {

    // Initialize the UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RX_RINGBUFFER_SIZE, UART_TX_RINGBUFFER_SIZE, 0, NULL, 0));

    // Configure the UART port
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

    // Set UART TX and RX pins
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX, UART_RX, -1, -1));

}

// Entrypoint
void app_main(void) {

    // Initialize slave
    uart_init();

    // Create the print UART received task
    xTaskCreate(vTaskPrint, pcTaskNamePrint, usTaskStackDepthPrint, NULL, uxTaskPriorityPrint, &xTaskHandlePrint);

    // Create the UART transmit task
    xTaskCreate(vTaskUARTTransmit, pcTaskNameUARTTransmit, usTaskStackDepthUARTTransmit, NULL, uxTaskPriorityUARTTransmit, &xTaskHandleUARTTransmit);

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}