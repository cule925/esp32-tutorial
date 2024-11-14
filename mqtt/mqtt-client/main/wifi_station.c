#include <stdio.h>
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "freertos/event_groups.h"                                              // FreeRTOS event groups
#include "nvs_flash.h"                                                          // NVM flash operations
#include "esp_event.h"                                                          // Event loop operations
#include "esp_netif.h"                                                          // Network interface functions
#include "esp_netif_ip_addr.h"                                                  // For IP type to string conversion, macros IPSTR and IP2STR(...)
#include "esp_wifi.h"                                                           // Wi-Fi operations
#include "esp_wifi_default.h"                                                   // Wi-Fi get default operations
#include "esp_log.h"                                                            // Logging operations
#include "esp_err.h"                                                            // For error codes and macro ESP_ERROR_CHECK(...)

#include "config.h"
#include "wifi_station.h"

// Tag names
#define WIFI_TAG                            "wifi_task"
#define WIFI_EVENT_TAG                      "wifi_event_handler"
#define IP_EVENT_TAG                        "ip_event_handler"

// Masks for event group bits
#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1

// Event handler instances
static esp_event_handler_instance_t event_handler_instance_wifi;
static esp_event_handler_instance_t event_handler_instance_ip;

// Event group handle
static EventGroupHandle_t s_wifi_event_group;

// Number of retries if connecting fails
static int s_retry_num = 0;

// Initialize device as a station Wi-Fi device
static void wifi_init_station(void)
{

    // Wi-Fi network configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Initializing and allocating resources for Wi-Fi driver
    ESP_LOGI(WIFI_TAG, "Initializing and allocating resources for Wi-Fi driver");
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Wi-Fi network driver
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_OPEN,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_pk_mode = WPA3_SAE_PK_MODE_AUTOMATIC,
        },
    };

    // Settinng Wi-Fi mode station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGI(WIFI_TAG, "Settinng Wi-Fi mode station");

    // Applying the Wi-Fi station network configuration
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_LOGI(WIFI_TAG, "Applying Wi-Fi configuration");

    // Starting Wi-Fi station
    ESP_LOGI(WIFI_TAG, "Starting Wi-Fi");
    ESP_ERROR_CHECK(esp_wifi_start());

    // Wait until Wi-Fi station is connected
    ESP_LOGI(WIFI_TAG, "Waiting for network connection");
    EventBits_t event_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    // Report what happened
    if (event_bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFI_TAG, "Connected to an access point with SSID: %s, password: %s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else if (event_bits & WIFI_FAIL_BIT) {
        ESP_LOGI(WIFI_TAG, "Failed to connect to an access point with SSID: %s, password: %s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else {
        ESP_LOGE(WIFI_TAG, "Unexpected event!");
    }
}

// Initialize network interface
static void network_interface_init() {

    // Initializing TCP/IP network stack
    ESP_LOGI(WIFI_TAG, "Initializing TCP/IP network stack");
    ESP_ERROR_CHECK(esp_netif_init());

    // Creating a default station network interface
    ESP_LOGI(WIFI_TAG, "Creating default station network interface");
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();

    // Set hostname of network interface
    ESP_LOGI(WIFI_TAG, "Setting hostname " ESP_WIFI_HOSTNAME);
    esp_netif_set_hostname(netif, ESP_WIFI_HOSTNAME);

}

// Event handler for WI-FI events
static void event_handler_wifi(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {                 // If this base and specific event happened (Wi-Fi just started)

        // Tryp to connect to the network given by the Wi-Fi configuration
        ESP_LOGI(WIFI_EVENT_TAG, "Connecting to the access point");
        esp_wifi_connect();
    
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {   // If this base and specific event happened (Wi-Fi disconnected)
    
        // If there are any retry numbers left
        if (s_retry_num < ESP_WIFI_RETRY_NUM) {
    
            // Try to connect to the network given by the Wi-Fi configuration again
            ESP_LOGI(WIFI_EVENT_TAG,"Failed to connect to access point, connecting again");
            esp_wifi_connect();
            s_retry_num++;

        } else {

            // After a number of failed connect repeats, set event group bit
            ESP_LOGI(WIFI_EVENT_TAG,"Failed to connect to access point, no more trying");
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    
        }
    
    }

}

// Event handler for IP events
static void event_handler_ip(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {                    // If this base and specific event happened (Just connected and got an IP address)

        // Get the data
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        
        // Reset retry numbers
        s_retry_num = 0;
        
        // Print the receiverd IP address using the macros for string conversion
        ESP_LOGI(IP_EVENT_TAG, "Connected to access point, IP address: " IPSTR, IP2STR(&event->ip_info.ip));

        // After a successful connect, set event group bit
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    
    }

}

// Initialize event handlers
static void event_handlers_init() {

    // Create system event loop
    ESP_LOGI(WIFI_TAG, "Creating system event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Register Wi-Fi event handler instance
    ESP_LOGI(WIFI_TAG, "Registering Wi-Fi event handler instance");
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler_wifi, NULL, &event_handler_instance_wifi));

    // Register IP event handler instance
    ESP_LOGI(WIFI_TAG, "Registering IP event handler instance");
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler_ip, NULL, &event_handler_instance_ip));

}

// Initialize event group handler
static void event_group_handler_init() {

    // Create event group handler
    s_wifi_event_group = xEventGroupCreate();

}

// Initialize default NVS partition
static void nvs_init() {

    // Initialize default NVS partition
    esp_err_t ret = nvs_flash_init();

    // If no more space delete contents of NVS partition
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(WIFI_TAG, "No space or new version detected, cleaning up default NVS partition");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_LOGI(WIFI_TAG, "Initializing default NVS partition again");
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

}

// Entrypoint
void wifi_init(void) {

    // Initialize default NVS partition
    ESP_LOGI(WIFI_TAG, "Initializing default NVS partition");
    nvs_init();

    // Initialize event group handler
    ESP_LOGI(WIFI_TAG, "Initializing event group handler");
    event_group_handler_init();

    // Initialize event handlers
    ESP_LOGI(WIFI_TAG, "Initializing event handlers");
    event_handlers_init();

    // Initialize network interface
    ESP_LOGI(WIFI_TAG, "Initializing network interface");
    network_interface_init();

    // Initialize Wi-Fi station
    ESP_LOGI(WIFI_TAG, "Initializing Wi-Fi station");
    wifi_init_station();

}