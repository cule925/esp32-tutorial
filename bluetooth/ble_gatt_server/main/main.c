#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "driver/gpio.h"                                                        // GPIO operations
#include "freertos/FreeRTOS.h"                                                  // FreeRTOS kernel
#include "freertos/task.h"                                                      // FreeRTOS task functions
#include "esp_log.h"                                                            // Logging operations

#include "nvs_flash.h"                                                          // NVM flash operations
#include "esp_bt.h"                                                             // Bluetooth controller functions
#include "esp_bt_defs.h"                                                        // Bluetooth macros and structures
#include "esp_bt_main.h"                                                        // Bluetooth Bluedroid stack functions
#include "esp_gap_ble_api.h"                                                    // GAP functionality
#include "esp_gatt_common_api.h"                                                // GATT common functionality
#include "esp_gatts_api.h"                                                      // GATT server functionality

// Tag names
#define MAIN_TAG                            "main_task"
#define EVENT_HANDLER_GAP_TAG               "event_handler_gap"
#define EVENT_HANDLER_GATTS_TAG             "event_handler_gatts"

// Maximum packet size
#define LOCAL_MTU                           500

// GAP configuration flags
#define GAP_ADV_CONFIG_DONE_FLAG            (1 << 0)
#define GAP_SCAN_RSP_CONFIG_DONE_FLAG       (1 << 1)

// GATTS profile, service and characteristic information
#define PROFILE_ID                  0
#define GATTS_SERVICE_UUID          0xFF01              // Custom 16 bit GATT service UUID
#define GATTS_CHAR_LED_UUID         0xFF02              // Custom 16 bit custom LED ON/OFF characteristic UUID
#define GATTS_NUM_HANDLE            3                   // Number of handles, one handle for service and two handles for one characteristic

// LED information
#define LED_PIN                     GPIO_NUM_22
#define LED_ON                      1
#define LED_OFF                     0

// GATT server profile event handler declaration
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// GAP peripheral UUID for advertising
static uint8_t adv_service_uuid128[16] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
};

// GAP advertising data
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp               = false,                                                            // Set this structure as an advertising data structure
    .include_name               = false,                                                            // Include name of the device in advertising data
    .include_txpower            = false,                                                            // Include transmitting power in advertising data
    .min_interval               = 0x0006,                                                           // Minimum advertising interval
    .max_interval               = 0x0010,                                                           // Maximum advertising interval
    .appearance                 = 0x00,                                                             // Broader categorization of this device, in this case 0 means unknown
    .manufacturer_len           = 0,                                                                // Manufacturer data length
    .p_manufacturer_data        = NULL,                                                             // No manufacturer data
    .service_data_len           = 0,                                                                // Service data lenght
    .p_service_data             = NULL,                                                             // No service data
    .service_uuid_len           = sizeof(adv_service_uuid128),                                      // Service 128 bit UUID length
    .p_service_uuid             = adv_service_uuid128,                                              // Service 128 bit UUID which is used for advertising
    .flag                       = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),     // Properties of advertising: visible to all other devices and no support for Bluetooth Classic + BLE mode
};

// GAP scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp               = true,                                                             // Set this structure as a response data structure
    .include_name               = true,                                                             // Include name of the device in response
    .include_txpower            = false,                                                            // Include transmitting power in response data
    .appearance                 = 0x00,                                                             // Broader categorization of this device, in this case 0 means unknown
    .manufacturer_len           = 0,                                                                // Manufacturer data length
    .p_manufacturer_data        = NULL,                                                             // No manufacturer data
    .service_data_len           = 0,                                                                // Service data lenght
    .p_service_data             = NULL,                                                             // No service data
    .service_uuid_len           = sizeof(adv_service_uuid128),                                      // Service 128 bit UUID length
    .p_service_uuid             = adv_service_uuid128,                                              // Service 128 bit UUID which is used for response
    .flag                       = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),     // Properties of response: visible to all other devices and no support for Bluetooth Classic + BLE mode
};

// GAP advertising parameters
static esp_ble_adv_params_t adv_params = {
    .adv_int_min                = 0x20,                                     // Minimum advertising interval in units of 0.625 ms
    .adv_int_max                = 0x40,                                     // Maximum advertising interval in units of 0.625 ms
    .adv_type                   = ADV_TYPE_IND,                             // Advertising type: connectable, scannable, undirected
    .own_addr_type              = BLE_ADDR_TYPE_PUBLIC,                     // Type of Bluetooth address to use: static and public
    //.peer_addr                =
    //.peer_addr_type           =
    .channel_map                = ADV_CHNL_ALL,                             // Advertising channel: all three (37, 38, 39)
    .adv_filter_policy          = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,        // Policy of scanning and connecting to this device: allow everybody to see and connect
};

// Flag for when advertising configuration is done
static uint8_t adv_config_done = 0;

// GATT server profile information
static uint16_t profile_gatts_if = ESP_GATT_IF_NONE;                            // GATT server profile interface, initialize it to none
static esp_gatts_cb_t profile_event_handler = gatts_profile_event_handler;      // GATT server profile callback, initialize it to the handler

// GATT server connection information
static uint16_t conn_id;                                                        // GATT server connection ID

// GATT server service information
static uint16_t service_handle;                                                 // GATT server service handle
static esp_gatt_srvc_id_t service_id;                                           // GATT server service ID

// GATT server LED ON/OFF characteristic information
static uint16_t char_led_handle;                                                // Characteristic handler
static esp_bt_uuid_t char_led_uuid;                                             // Characteristic UUID
static esp_gatt_perm_t char_led_perm;                                           // Characteristic permissions (read, write, ...)
static esp_gatt_char_prop_t char_led_property;                                  // Characteristic property
static uint8_t led_initial_value = 0;                                           // Characteristic initial value

static esp_attr_value_t char_initial_value = {                          // Characteristic initial value data structure
    .attr_max_len = sizeof(uint8_t),                                    // Maximum length of the value
    .attr_len     = sizeof(uint8_t),                                    // Initial attribute length
    .attr_value   = &led_initial_value,                                 // Pointer to the initial value
};

// GAP advertising data setup
static void gap_setup_adv_and_rsp_data() {

    // Configure advertising data
    esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret) ESP_LOGE(EVENT_HANDLER_GATTS_TAG, "Configuring advertising data failed, error code %x", ret);
    adv_config_done |= GAP_ADV_CONFIG_DONE_FLAG;                                                    // |= 0x00..01

    // Configure scan response data
    ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
    if (ret) ESP_LOGE(EVENT_HANDLER_GATTS_TAG, "Configuring scan response data failed, error code %x", ret);
    adv_config_done |= GAP_SCAN_RSP_CONFIG_DONE_FLAG;                                               // |= 0x00..02

}

// GATT server setup service
static void gatts_setup_service(esp_gatt_if_t gatts_if) {

    // Set service data
    service_id.is_primary = true;                                       // Service is primary
    service_id.id.inst_id = 0x00;                                       // Service instance ID
    service_id.id.uuid.len = ESP_UUID_LEN_16;                           // Service UUID length
    service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID;                // Service UUID

    // Configure advertising and response data
    gap_setup_adv_and_rsp_data();

    // Create the service, generates ESP_GATTS_CREATE_EVT event upon competion
    esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLE);

}

// GATT server setup LED characteristic
static void gatts_setup_led_characteristic() {

    // Return value
    esp_err_t ret;

    // LED ON/OFF characteristic data
    char_led_uuid.len = ESP_UUID_LEN_16;                                                    // Characteristic UUID length
    char_led_uuid.uuid.uuid16 = GATTS_CHAR_LED_UUID;                                        // Characteristic UUID
    char_led_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;                               // Characteristic permissions
    char_led_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;         // Characteristic property

    // Add the characteristic to the service, generates ESP_GATTS_ADD_CHAR_EVT event upon competion
    ret = esp_ble_gatts_add_char(service_handle, &char_led_uuid, char_led_perm, char_led_property, &char_initial_value, NULL);
    if (ret) ESP_LOGE(EVENT_HANDLER_GATTS_TAG, "REG_EVT, adding LED characteristic failed, error code %x", ret);

    // Start service
    esp_ble_gatts_start_service(service_handle);

}

// Read handler for LED characteristic appropriately called when ESP_GATTS_READ_EVT event
static void char_led_read_handler(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {

    // Get current LED state
    uint8_t led_state = (uint8_t)gpio_get_level(LED_PIN);

    // Response to client
    esp_gatt_rsp_t rsp;

    // Initialize it to 0, the actual size of the response structure is defined in header esp_gatt_defs.h as a macro ESP_GATT_MAX_ATTR_LEN
    memset(&rsp, 0, sizeof(esp_gatt_rsp_t));

    // Set handle
    rsp.attr_value.handle = param->read.handle;

    // Set response value length
    rsp.attr_value.len = 1;

    // Set response value
    rsp.attr_value.value[0] = led_state;

    // Send response
    esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);

}

// Write handler for LED characteristic appropriately called when ESP_GATTS_WRITE_EVT event
static void char_led_write_handler(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {

    // Response to client
    esp_gatt_rsp_t rsp;

    // Initialize it to 0, the actual size of the response structure is defined in header esp_gatt_defs.h as a macro ESP_GATT_MAX_ATTR_LEN
    memset(&rsp, 0, sizeof(esp_gatt_rsp_t));

    // Set handle
    rsp.attr_value.handle = param->write.handle;

    // Set response value length
    rsp.attr_value.len = 0;

    // LED state
    uint8_t led_state;

    // If length of data is 1 set LED state
    if (param->write.len == 1) {

        // Extract value
        led_state = param->write.value[0];
        
        // Set LED state
        if (led_state) gpio_set_level(LED_PIN, LED_ON);
        else gpio_set_level(LED_PIN, LED_OFF);

        // Send response
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &rsp);

    } else {

        // Send response
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_REQ_NOT_SUPPORTED, &rsp);
    
    }

}

// GATT server profile event handler
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {

    // Handle event
    switch (event) {

        // Call appropriate handler when write event
        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "WRITE_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
            if (param->write.handle == char_led_handle) char_led_write_handler(gatts_if, param);
            break;

        // Call appropriate handler when read event
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "READ_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
            if (param->read.handle == char_led_handle) char_led_read_handler(gatts_if, param);
            break;

        // Setup service when register event
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "REG_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
            gatts_setup_service(gatts_if);                              // Setup service
            break;

        // Setup characteristic when register event
        case ESP_GATTS_CREATE_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "CREATE_EVT, status %d, service_handle %d", param->create.status, param->create.service_handle);
            service_handle = param->create.service_handle;              // Store service handle
            gatts_setup_led_characteristic();                           // Setup LED characteristic
            break;

        // When start service complete event
        case ESP_GATTS_START_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;

        // Setup handler and initial value when characteristic event
        case ESP_GATTS_ADD_CHAR_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "ADD_CHAR_EVT, status %d, attr_handle %d, service_handle %d", param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
            if (param->add_char.char_uuid.uuid.uuid16 == GATTS_CHAR_LED_UUID) char_led_handle = param->add_char.attr_handle;
            break;

        // Set global connection id when connect event
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x",          // Print GATT client Bluetooth address
                param->connect.conn_id,
                param->connect.remote_bda[0],
                param->connect.remote_bda[1],
                param->connect.remote_bda[2],
                param->connect.remote_bda[3],
                param->connect.remote_bda[4],
                param->connect.remote_bda[5]);
            conn_id = param->connect.conn_id;
            esp_ble_gap_stop_advertising();
            break;
        
        // Start advertising again when disconnect event
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "DISCONNECT_EVT, conn_id %d", param->disconnect.conn_id);
            esp_ble_gap_start_advertising(&adv_params);
            break;

        // When change MTU event
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTS_TAG, "MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        
        // In case of other events
        /*
        case ESP_GATTS_NOTIFY:
        case ESP_GATTS_EXEC_WRITE_EVT:
        case ESP_GATTS_ADD_INCL_SRVC_EVT:
        case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        case ESP_GATTS_DELETE_EVT:
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_CONF_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_RESPONSE_EVT:
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        case ESP_GATTS_SET_ATTR_VAL_EVT:
        case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
        ...
        */

        default:
            break;

    }

}

// GATT server event handler
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {

    // If event is a register application id event
    if (event == ESP_GATTS_REG_EVT) {

        // If registration status okay, store the new GATT server profile interface in the global variable
        if (param->reg.status == ESP_GATT_OK) {
            profile_gatts_if = gatts_if;
        } else {
            ESP_LOGE(EVENT_HANDLER_GATTS_TAG, "ESP_GATTS_REG_EVT, registering application profile failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }

    }

    // If GATT interface parameter is equal to none of the profiles (broadcast) or it equal to the profile GATT interface
    if (gatts_if == ESP_GATT_IF_NONE || gatts_if == profile_gatts_if) {

        // Call handler if handler pointer not NULL
        if (profile_event_handler) profile_event_handler(event, gatts_if, param);
            
    }

}

// GAP event handler
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {

    // Handle event
    switch (event) {

        // When setting advertising data is complete event
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~GAP_ADV_CONFIG_DONE_FLAG);                             // &= 0xFF..FFE
            if (adv_config_done == 0) esp_ble_gap_start_advertising(&adv_params);       // If configuring advertising and response data is complete
            break;

        // When setting response data is complete event
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~GAP_SCAN_RSP_CONFIG_DONE_FLAG);                        // &= 0xFF..FFD
            if (adv_config_done == 0) esp_ble_gap_start_advertising(&adv_params);       // If configuring advertising and response data is complete
            break;

        // When advertising start is complete event
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) ESP_LOGE(EVENT_HANDLER_GAP_TAG, "Advertising start failed, error code %x", param->adv_start_cmpl.status);
            break;

        // When advertising stop is complete event
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) ESP_LOGE(EVENT_HANDLER_GAP_TAG, "Advertising stop failed, error code %x", param->adv_stop_cmpl.status);
            break;

        // When update connection parameters event
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Update connection parameters: status %d, conn_int %d, latency %d, timeout %d",
                  param->update_conn_params.status,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;

        // When set MTU packet length event
        case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Packet length updated: rx %d, tx %d, status %d",
                  param->pkt_data_length_cmpl.params.rx_len,
                  param->pkt_data_length_cmpl.params.tx_len,
                  param->pkt_data_length_cmpl.status);
            break;

        default:
            break;
    
    }

}

// Setup GATT server
static esp_err_t gatts_setup() {

    // Register GATT server callback function
    esp_err_t ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "GATT server register callback error, error code %x", ret);
        return ret;
    }

    // Register profile, generates an ESP_GATTS_REG_EVT event upon completion
    ret = esp_ble_gatts_app_register(PROFILE_ID);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "GATT server register profile error, error code %x", ret);
        return ret;
    }

    // Sets maximum packet transmission size, generates ESP_GATTS_MTU_EVT event upon completion
    ret = esp_ble_gatt_set_local_mtu(LOCAL_MTU);
    if (ret) ESP_LOGE(MAIN_TAG, "Set local MTU failed, error code %x", ret);

    return ret;

}

// Setup GAP
static esp_err_t gap_setup() {

    // Register GAP callback function
    esp_err_t ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "GAP register error, error code %x", ret);
        return ret;
    }

    return ret;

}

// Setup Bluedroid Bluetooth host stack
static esp_err_t bluedroid_stack_setup() {

    // Initial error value
    esp_err_t ret = ESP_OK;

    // Initialize Bluetooth host stack
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(MAIN_TAG, "Init Bluetooth host stack failed: %s, in function %s", esp_err_to_name(ret), __func__);
        return ret;
    }

    // Enable Bluedroid Bluetooth host stack
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(MAIN_TAG, "Enable Bluetooth host stack failed: %s, in function %s", esp_err_to_name(ret), __func__);
        return ret;
    }

    return ret;

}

// Setup BLE controller
static esp_err_t ble_controller_setup() {

    // Initial error value
    esp_err_t ret = ESP_OK;

    // Release Bluetooth Clasic memory
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    // Initialize Bluetooth controller with default configuration
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "Initialize bluetooth controller failed: %s, in function %s", esp_err_to_name(ret), __func__);
        return ret;
    }

    // Choose BLE mode
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "Enable Bluetooth controller failed: %s, in function %s", esp_err_to_name(ret), __func__);
        return ret;
    }

    return ret;

}

// Initialize default NVS partition
static esp_err_t nvs_init() {

    // Initialize default NVS partition
    esp_err_t ret = nvs_flash_init();

    // If no more space delete contents of NVS partition
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(MAIN_TAG, "NVS init flash failed: %s, in function %s", esp_err_to_name(ret), __func__);
        ret = nvs_flash_erase();
        ESP_ERROR_CHECK(ret);
        ESP_LOGE(MAIN_TAG, "NVS erase flash failed: %s, in function %s", esp_err_to_name(ret), __func__);
        ret = nvs_flash_init();
        ESP_ERROR_CHECK(ret);
        ESP_LOGE(MAIN_TAG, "NVS init flash failed: %s, in function %s", esp_err_to_name(ret), __func__);
    }

    return ret;

}

// Setup LED GPIO
static void gpio_led_setup() {

    // LED setup
    gpio_reset_pin(LED_PIN);                                    // Reset pin to default state
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);        // Set pin input-output mode (so both get and set are possible)
    gpio_set_level(LED_PIN, LED_OFF);                           // Set pin level to 0

}

// Entrypoint
void app_main(void) {

    // Setup LED GPIO
    gpio_led_setup();

    // Initialize default NVS partition 
    if (nvs_init() != ESP_OK) return;

    // Setup BLE controller
    if (ble_controller_setup() != ESP_OK) return;

    // Setup Bluedroid Bluetooth host stack
    if (bluedroid_stack_setup() != ESP_OK) return;

    // Setup GAP
    if (gap_setup() != ESP_OK) return;

    // Setup GATT server
    if (gatts_setup() != ESP_OK) return;

    return;

}
