#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "driver/spi_master.h"                                                  // SPI master functions
#include "driver/spi_slave.h"                                                   // SPI slave functions
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define TASK_SPI_MASTER_TAG             "spi_master"
#define TASK_SPI_SLAVE_TAG              "spi_slave"

// SPI information
#define SPI_MASTER                      SPI2_HOST                               // Use HSPI (SPI2)
#define SPI_SLAVE                       SPI3_HOST                               // Use VSPI (SPI3)

#define SPI_MASTER_CS                   GPIO_NUM_15
#define SPI_MASTER_SCLK                 GPIO_NUM_14
#define SPI_MASTER_MISO                 GPIO_NUM_12
#define SPI_MASTER_MOSI                 GPIO_NUM_13
#define SPI_MASTER_DMA_CH               SPI_DMA_CH1

#define SPI_SLAVE_CS                    GPIO_NUM_5
#define SPI_SLAVE_SCLK                  GPIO_NUM_18
#define SPI_SLAVE_MISO                  GPIO_NUM_19
#define SPI_SLAVE_MOSI                  GPIO_NUM_23
#define SPI_SLAVE_DMA_CH                SPI_DMA_CH2

#define SPI_MASTER_SCLK_FREQ            1000000                                 // SCLK frequency from master

#define SPI_TX_MASTER_SIZE              40                                      // Transmit message size from master
#define SPI_RX_MASTER_SIZE              40                                      // Receive message size to master
#define SPI_DELAY_TRANSACTION_MS        1000                                    // Delay between each transaction

#define SPI_TX_SLAVE_SIZE               40                                      // Transmit message size from slave
#define SPI_RX_SLAVE_SIZE               40                                      // Receive message size to slave

// Messages that will be sent over SPI from master
char msg_master_tx_1[SPI_TX_MASTER_SIZE] = "Hello this is master.";
char msg_master_tx_2[SPI_TX_MASTER_SIZE] = "Any slave there?";

char *dma_master_tx = NULL;

// Message that will be received over SPI to master
char msg_master_rx[SPI_RX_MASTER_SIZE] = {0};

char *dma_master_rx = NULL;

// Messages that will be sent over SPI from slave
char msg_slave_tx_1[SPI_TX_SLAVE_SIZE] = "Hello this is slave.";
char msg_slave_tx_2[SPI_TX_SLAVE_SIZE] = "Any master there?";

char *dma_slave_tx = NULL;

// Message that will be received over SPI to slave
char msg_slave_rx[SPI_RX_SLAVE_SIZE] = {0};

char *dma_slave_rx = NULL;

// Master bus configuration
spi_bus_config_t spi_master_bus_cfg = {
    .sclk_io_num = SPI_MASTER_SCLK,                             // HSPI SCLK GPIO
    .miso_io_num = SPI_MASTER_MISO,                             // HSPI MISO GPIO
    .mosi_io_num = SPI_MASTER_MOSI,                             // HSPI MOSI GPIO
    .quadwp_io_num = -1,                                        // Not used
    .quadhd_io_num = -1                                         // Not used
};

// Slave bus configuration
spi_bus_config_t spi_slave_bus_cfg = {
    .sclk_io_num = SPI_SLAVE_SCLK,                              // VSPI SCLK GPIO
    .miso_io_num = SPI_SLAVE_MISO,                              // VSPI MISO GPIO
    .mosi_io_num = SPI_SLAVE_MOSI,                              // VSPI MOSI GPIO
    .quadwp_io_num = -1,                                        // Not used
    .quadhd_io_num = -1                                         // Not used
};

// Master interface configuration
spi_device_interface_config_t spi_master_if_cfg = {
    .clock_speed_hz = SPI_MASTER_SCLK_FREQ,                     // HSPI SCLK frequency
    .mode = 0,                                                  // SPI mode 0 (CPOL = 0, CPHA = 0)
    .spics_io_num = SPI_MASTER_CS,                              // HSPI CS GPIO
    .queue_size = 1,                                            // Number of transactions in the SPI master queue
};

// Slave interface configuration
spi_slave_interface_config_t spi_slave_if_cfg = {
    .spics_io_num = SPI_SLAVE_CS,                               // VSPI CS GPIO
    .queue_size = 1,                                            // Number of transactions in the SPI slave queue
};

// SPI master handle
spi_device_handle_t spi_master_handle;

// SPI master task information
TaskHandle_t xTaskHandleSPIMaster = NULL;                                                               // Handle of the task (maximum size 16 characters)
char* pcTaskNameSPIMaster = "SPI_MASTER";                                                               // Name of the task
UBaseType_t uxTaskPrioritySPIMaster = tskIDLE_PRIORITY + 1;                                             // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSPIMaster = configMINIMAL_STACK_SIZE * 8;                  // Task stack size

// SPI slave task information
TaskHandle_t xTaskHandleSPISlave = NULL;                                                                // Handle of the task (maximum size 16 characters)
char* pcTaskNameSPISlave = "SPI_SLAVE";                                                                 // Name of the task
UBaseType_t uxTaskPrioritySPISlave = tskIDLE_PRIORITY + 1;                                              // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSPISlave = configMINIMAL_STACK_SIZE * 8;                   // Task stack size

// SPI master task function
void vTaskSPIMaster(void* argument) {

    // Interval in ticks
    TickType_t xTaskSleepPeriod = SPI_DELAY_TRANSACTION_MS / portTICK_PERIOD_MS;

    // Create transaction information
    spi_transaction_t spi_master_transaction = {
        .length = (SPI_TX_MASTER_SIZE * sizeof(char) + SPI_RX_MASTER_SIZE * sizeof(char)) * 8,  // In bits
        .rxlength = (SPI_RX_MASTER_SIZE * sizeof(char)) * 8,                                    // In bits
        .tx_buffer = dma_master_tx,
        .rx_buffer = dma_master_rx,
    };

    // Message choice
    int choice = 0;

    // Loop it forever
    while (1) {

        // Delay a bit
        vTaskDelay(xTaskSleepPeriod);

        // Copy the text for the transaction
        if (choice == 0) { memcpy(dma_master_tx, msg_master_tx_1, SPI_TX_MASTER_SIZE * sizeof(char)); choice++; }
        else { memcpy(dma_master_tx, msg_master_tx_2, SPI_TX_MASTER_SIZE * sizeof(char)); choice--; }

        // Do the transaction (ISR blocking)
        ESP_ERROR_CHECK(spi_device_transmit(spi_master_handle, &spi_master_transaction));

        // Print transaction received data
        ESP_LOGI(TASK_SPI_MASTER_TAG, "Received '%s'", dma_master_rx);

    }

}

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

// SPI initialize bus and master
void spi_master_init() {

    // Initialize bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_MASTER, &spi_master_bus_cfg, SPI_MASTER_DMA_CH));

    // Add master
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_MASTER, &spi_master_if_cfg, &spi_master_handle));

}

// SPI initialize bus and slave
void spi_slave_init() {

    // Initialize bus and slave
    ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE, &spi_slave_bus_cfg, &spi_slave_if_cfg, SPI_SLAVE_DMA_CH));

}

// DMA buffers initialize
void dma_buffers_init() {

    // Allocate master DMA buffers
    dma_master_tx = (char *)heap_caps_malloc(SPI_TX_MASTER_SIZE * sizeof(char), MALLOC_CAP_DMA);
    dma_master_rx = (char *)heap_caps_malloc(SPI_RX_MASTER_SIZE * sizeof(char), MALLOC_CAP_DMA);

    // Allocate slave DMA buffers
    dma_slave_tx = (char *)heap_caps_malloc(SPI_TX_SLAVE_SIZE * sizeof(char), MALLOC_CAP_DMA);
    dma_slave_rx = (char *)heap_caps_malloc(SPI_RX_SLAVE_SIZE * sizeof(char), MALLOC_CAP_DMA);

}

// Entrypoint
void app_main(void) {

    // Initialize DMA buffers
    dma_buffers_init();
    if (dma_master_tx == NULL || dma_master_rx == NULL || dma_slave_tx == NULL || dma_slave_rx == NULL) {
        ESP_LOGE(MAIN_TAG, "Buffer allocation error %s", __func__);
        ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);
    }

    // Initialize SPI slave
    spi_slave_init();

    // Initialize SPI master
    spi_master_init();

    // Create the SPI slave task
    xTaskCreate(vTaskSPISlave, pcTaskNameSPISlave, usTaskStackDepthSPISlave, NULL, uxTaskPrioritySPISlave, &xTaskHandleSPISlave);

    // Create the SPI master transmit task
    xTaskCreate(vTaskSPIMaster, pcTaskNameSPIMaster, usTaskStackDepthSPIMaster, NULL, uxTaskPrioritySPIMaster, &xTaskHandleSPIMaster);

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}