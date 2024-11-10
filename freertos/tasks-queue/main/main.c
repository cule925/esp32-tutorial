#include <stdio.h>
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/queue.h"                                                     // FreeRTOS queue operations
#include "esp_log.h"                                                            // Logging operations

// Tag names
#define MAIN_TAG                        "main_task"
#define QUEUE_SIZE_CHECKER_TAG          "queue_size_checker_task"
#define SENDER_TASK_TAG                 "sender_task"
#define RECEIVER_TASK_TAG               "receiver_task"

// Queue information
#define QUEUE_NUM_OF_ELEMENTS       5           // Maximum number of elements in queue
#define QUEUE_SIZE_CHECKER_DELAY_MS 500         // The delay the queue size is checked
#define MESAGE_NUM                  10          // Number of messages to be sent by the sender task
#define SENDER_DELAY_MS             700         // The delay the sender does between writing messages in the queue
#define RECEIVER_DELAY_MS           1500        // The delay the receiver is reading the messages

// Queue handle
QueueHandle_t xQueue;

// Queue size checker task information
TaskHandle_t xTaskHandleQueueSizeChecker = NULL;                                                // Handle of the task
char* pcTaskNameQueueSizeChecker = "MY_QUEUE_SIZE_CHECKER_TASK";                                // Name of the task
UBaseType_t uxTaskPriorityQueueSizeChecker = tskIDLE_PRIORITY + 1;                              // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthQueueSizeChecker = configMINIMAL_STACK_SIZE * 8;   // Task stack size

// Sender task information
TaskHandle_t xTaskHandleSender = NULL;                                                          // Handle of the task
char* pcTaskNameSender = "MY_SENDER_TASK";                                                      // Name of the task
UBaseType_t uxTaskPrioritySender = tskIDLE_PRIORITY + 1;                                        // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthSender = configMINIMAL_STACK_SIZE * 8;             // Task stack size
char* pcFirstMessage = "Hello from sender!";
int senderAlive = 1;

// Receiver task information
TaskHandle_t xTaskHandleReceiver = NULL;                                                        // Handle of the task
char* pcTaskNameReceiver = "MY_RECEIVER_TASK";                                                  // Name of the task
UBaseType_t uxTaskPriorityReceiver = tskIDLE_PRIORITY + 1;                                      // Task priority (same as task main)
const configSTACK_DEPTH_TYPE usTaskStackDepthReceiver = configMINIMAL_STACK_SIZE * 8;           // Task stack size
int receiverAlive = 1;

// Element to be compied in and out the queue
struct QueueElement {
    int msgNumber;
    char* msg;
};

// Print queue size function
void vTaskQueueSizeChecker(void* argument) {

    // Time to delay in ticks
    TickType_t xTaskSleepQueueElementChecker = QUEUE_SIZE_CHECKER_DELAY_MS / portTICK_PERIOD_MS;

    // Iterate and check size stack while there are active sender and receiver tasks
    while (1) {

        vTaskDelay(xTaskSleepQueueElementChecker);

        // Check if tasks are dead
        if(!senderAlive && !receiverAlive) break;

        int numberOfMessagesInQueue = uxQueueMessagesWaiting(xQueue);                           // Read number of elements in queue
        ESP_LOGI(QUEUE_SIZE_CHECKER_TAG, "Elements in stack: %d", numberOfMessagesInQueue);     // Print the number of elements in queue

    }
    
    // Self delete
    ESP_LOGI(QUEUE_SIZE_CHECKER_TAG, "Self deleting, goodbye");
    vTaskDelete(NULL);

}

// Sender function
void vTaskSender(void* argument) {
    
    // Construct element
    struct QueueElement queueElement;
    queueElement.msgNumber = 1;
    queueElement.msg = pcFirstMessage;

    // Time to delay in ticks
    TickType_t xTaskSleepSender = SENDER_DELAY_MS / portTICK_PERIOD_MS;

    // Send messages
    for (int i = 0; i < MESAGE_NUM; i++) { 

        // Delay
        vTaskDelay(xTaskSleepSender);

        // Blocking write
        xQueueSendToBack(xQueue, (void *) &queueElement, portMAX_DELAY);
        ESP_LOGI(SENDER_TASK_TAG, "Sent message");
        queueElement.msgNumber++;

    }

    // Self delete
    ESP_LOGI(SENDER_TASK_TAG, "Self deleting, goodbye");
    receiverAlive = 0;
    vTaskDelete(NULL);

}

// Receiver function
void vTaskReceiver(void* argument) {

    // Construct element
    struct QueueElement queueElement = {0, NULL};
    
    // Time to delay in ticks
    TickType_t xTaskSleepReceiver = RECEIVER_DELAY_MS / portTICK_PERIOD_MS;

    // Receive message
    for (int i = 0; i < MESAGE_NUM; i++) { 

        // Delay
        vTaskDelay(xTaskSleepReceiver);

        // Non-blocking read
        xQueueReceive(xQueue, (void *) &queueElement, (TickType_t) 0);
        ESP_LOGI(RECEIVER_TASK_TAG, "Message number: %d, received: %s", queueElement.msgNumber, queueElement.msg);

    }

    // Self delete
    ESP_LOGI(RECEIVER_TASK_TAG, "Self deleting, goodbye");
    senderAlive = 0;
    vTaskDelete(NULL);

}

// Entrypoint
void app_main(void) {

    // Create queue
    xQueue = xQueueCreate(QUEUE_NUM_OF_ELEMENTS, sizeof(struct QueueElement));
    if(xQueue == NULL) {
        ESP_LOGE(MAIN_TAG, "Error creating queue, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Created queue");

    // Create the queue size checker task
    if(xTaskCreate(vTaskQueueSizeChecker, pcTaskNameQueueSizeChecker, usTaskStackDepthQueueSizeChecker, NULL, uxTaskPriorityQueueSizeChecker, &xTaskHandleQueueSizeChecker) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Queue size checker creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Queue size checker task creation complete");

    // Create the sender task
    if(xTaskCreate(vTaskSender, pcTaskNameSender, usTaskStackDepthSender, NULL, uxTaskPrioritySender, &xTaskHandleSender) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "First sender task creation error exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "First sender task creation complete");

    // Create the second sender task
    if(xTaskCreate(vTaskReceiver, pcTaskNameReceiver, usTaskStackDepthReceiver, NULL, uxTaskPriorityReceiver, &xTaskHandleReceiver) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Receiver task creation error, exiting");
        return;
    }
    ESP_LOGI(MAIN_TAG, "Receiver task creation complete");

    ESP_LOGI(MAIN_TAG, "Exiting %s", __func__);

}
