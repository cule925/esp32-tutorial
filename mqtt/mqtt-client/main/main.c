#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "esp_log.h"                                                            // Logging operations
#include "mqtt_client.h"                                                        // MQTT client operations

#include "config.h"
#include "wifi_station.h"

// Tag names
#define MAIN_TAG                    "main_task"
#define MQTT_EVENT_TAG              "mqtt_event_handler"

// LED information
#define LED_PIN                     GPIO_NUM_22
#define LED_ON                      1
#define LED_OFF                     0

// MQTT topics
#define LED_TOPIC                   "/machine/led"
#define SERVO_TOPIC                 "/machine/servo"

// MQTT LED messages
#define LED_ON_MESSAGE              "ON"
#define LED_OFF_MESSAGE             "OFF"

// Servo initial position
#define SET_SERVO_INITIAL_POS       "90"

// MQTT client config
static esp_mqtt_client_config_t mqtt_client_config = {
    .broker.address.uri = MQTT_BROKER_URL,
    .credentials.username = MQTT_USERNAME,
    .credentials.authentication.password = MQTT_PASSWORD,
};

// Set LED handler
static void eventHandlerLEDSet(esp_mqtt_event_handle_t *event) {

    // If topic matches (comparing without '\0' terminators in the original string because the messages come with none)
    if ((*event)->topic_len == (sizeof(LED_TOPIC) - 1) && strncmp((*event)->topic, LED_TOPIC, (*event)->topic_len) == 0) {

        // If command matches (comparing without '\0' terminators in the original string because the messages come with none)
        if ((*event)->data_len == (sizeof(LED_ON_MESSAGE) - 1) && strncmp((*event)->data, LED_ON_MESSAGE, (*event)->data_len) == 0) gpio_set_level(LED_PIN, LED_ON);
        else if ((*event)->data_len == (sizeof(LED_OFF_MESSAGE) - 1) && strncmp((*event)->data, LED_OFF_MESSAGE, (*event)->data_len) == 0) gpio_set_level(LED_PIN, LED_OFF);

    }

}

// MQTT event handler
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {

    ESP_LOGD(MQTT_EVENT_TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;                     // Event handle
    esp_mqtt_client_handle_t client = event->client;                // Client handle
    int msg_id;                                                     // Message ID
    
    // Handle event
    switch ((esp_mqtt_event_id_t)event_id) {
    
        // When connected to broker
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, SERVO_TOPIC, SET_SERVO_INITIAL_POS, 0, 1, 0);
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_CONNECTED, sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, LED_TOPIC, 0);
            ESP_LOGI(MQTT_EVENT_TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;

        // When disconnected from broker
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        // When subscribed to a topic
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        // When subscribed to a topic
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        // When published a message to a topic
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        // When received data as a subscriber
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_DATA");
            eventHandlerLEDSet(&event);
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;

        // When error
        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI(MQTT_EVENT_TAG, "MQTT_EVENT_ERROR, Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;

        default:
            ESP_LOGI(MQTT_EVENT_TAG, "Other event id: %d", event->event_id);
            break;

    }

}

// Initialize MQTT client
static void mqtt_client_init() {

    // Initialize client
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_client_config);
    
    ESP_LOGI(MAIN_TAG, "Initializing MQTT client");
    
    
    // Register event handler
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    
    // Start client
    esp_mqtt_client_start(client);

}

// Setup LED GPIO
static void gpio_led_setup() {

    // LED setup
    gpio_reset_pin(LED_PIN);                                    // Reset pin to default state
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);              // Set pin output mode
    gpio_set_level(LED_PIN, LED_OFF);                           // Set pin level to 0

}

// Entrypoint
void app_main(void) {

    // Initialize LED GPIO
    gpio_led_setup();

    // Initialize Wi-Fi station mode (will initialize system event loop)
    wifi_init();

    // Initialize MQTT client
    mqtt_client_init();

}
