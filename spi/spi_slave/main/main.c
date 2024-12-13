#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "driver/spi_slave.h"                                                   // SPI slave functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define TASK_SPI_SLAVE_TAG              "spi_slave"

// SPI information
#define SPI_SLAVE                       SPI3_HOST                               // Use VSPI (SPI3)

#define SPI_SLAVE_CS                    GPIO_NUM_5
#define SPI_SLAVE_SCLK                  GPIO_NUM_18
#define SPI_SLAVE_MISO                  GPIO_NUM_19
#define SPI_SLAVE_MOSI                  GPIO_NUM_23
#define SPI_SLAVE_DMA_CH                SPI_DMA_CH2

#define SPI_DELAY_TRANSACTION_MS        1000                                    // Delay between each transaction

#define SPI_TX_SLAVE_SIZE               40                                      // Transmit message size from slave
#define SPI_RX_SLAVE_SIZE               40                                      // Receive message size to slave

// Messages that will be sent over SPI from slave
char msg_slave_tx_1[SPI_TX_SLAVE_SIZE] = "Hello this is slave.";
char msg_slave_tx_2[SPI_TX_SLAVE_SIZE] = "Any master there?";

char *dma_slave_tx = NULL;

// Message that will be received over SPI to slave
char msg_slave_rx[SPI_RX_SLAVE_SIZE] = {0};

char *dma_slave_rx = NULL;

// Slave bus configuration
spi_bus_config_t spi_slave_bus_cfg = {
    .sclk_io_num = SPI_SLAVE_SCLK,                              // VSPI SCLK GPIO
    .miso_io_num = SPI_SLAVE_MISO,                              // VSPI MISO GPIO
    .mosi_io_num = SPI_SLAVE_MOSI,                              // VSPI MOSI GPIO
    .quadwp_io_num = -1,                                        // Not used
    .quadhd_io_num = -1                                         // Not used
};

// Slave interface configuration
spi_slave_interface_config_t spi_slave_if_cfg = {
    .spics_io_num = SPI_SLAVE_CS,                               // VSPI CS GPIO
    .queue_size = 1,                                            // Number of transactions in the SPI slave queue
};

// SPI slave task information
TaskHandle_t xTaskHandleSPISlave = NULL;                                                                // Handle of the task (maximum size 16 characters)
char* pcTaskNameSPISlave = "SPI_SLAVE";                                                                 // Name of the task
UBaseType_t uxTaskPrioritySPISlave = tskIDLE_PRIORITY + 1;                                              // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSPISlave = configMINIMAL_STACK_SIZE * 8;                   // Task stack size

// SPI slave task function
void vTaskSPISlave(void* argument) {

    // Create transaction information
    spi_slave_transaction_t spi_slave_transaction = {
        .length = (SPI_TX_SLAVE_SIZE * sizeof(char) + SPI_RX_SLAVE_SIZE * sizeof(char)) * 8,    // In bits
        .trans_len = (SPI_TX_SLAVE_SIZE * sizeof(char)) * 8,                                    // In bits
        .tx_buffer = dma_slave_tx,
        .rx_buffer = dma_slave_rx,
    };

    // Message choice
    int choice = 0;

    // Loop it forever
    while (1) {

        // Copy the text for the transaction
        if (choice == 0) { memcpy(dma_slave_tx, msg_slave_tx_1, SPI_TX_SLAVE_SIZE * sizeof(char)); choice++; }
        else { memcpy(dma_slave_tx, msg_slave_tx_2, SPI_TX_SLAVE_SIZE * sizeof(char)); choice--; }

        // Do the transaction (ISR blocking)
        ESP_ERROR_CHECK(spi_slave_transmit(SPI_SLAVE, &spi_slave_transaction, portMAX_DELAY));

        // Print transaction received data
        ESP_LOGI(TASK_SPI_SLAVE_TAG, "Received '%s'", dma_slave_rx);

    }

}

// SPI initialize bus and slave
void spi_slave_init() {

    // Initialize bus and slave
    ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE, &spi_slave_bus_cfg, &spi_slave_if_cfg, SPI_SLAVE_DMA_CH));

}

// DMA buffers initialize
void dma_buffers_init() {

    // Allocate slave DMA buffers
    dma_slave_tx = (char *)heap_caps_malloc(SPI_TX_SLAVE_SIZE * sizeof(char), MALLOC_CAP_DMA);
    dma_slave_rx = (char *)heap_caps_malloc(SPI_RX_SLAVE_SIZE * sizeof(char), MALLOC_CAP_DMA);

}

// Entrypoint
void app_main(void) {

    // Initialize DMA buffers
    dma_buffers_init();
    if (dma_slave_tx == NULL || dma_slave_rx == NULL) {
        ESP_LOGE(MAIN_TAG, "Buffer allocation error %s", __func__);
        ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);
    }

    // Initialize SPI slave
    spi_slave_init();

    // Create the SPI slave task
    xTaskCreate(vTaskSPISlave, pcTaskNameSPISlave, usTaskStackDepthSPISlave, NULL, uxTaskPrioritySPISlave, &xTaskHandleSPISlave);

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}