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
#include "esp_gattc_api.h"                                                      // GATT client functionality

// Tag names
#define MAIN_TAG                            "main_task"
#define EVENT_HANDLER_GAP_TAG               "event_handler_gap"
#define EVENT_HANDLER_GATTC_TAG             "event_handler_gattc"
#define SET_REMOTE_TASK_TAG                 "set_remote_task"
#define GET_REMOTE_TASK_TAG                 "get_remote_task"

// Maximum packet size
#define LOCAL_MTU                           500

// Scan duration
#define SCAN_DURATION                       30

// GATTS profile, service and characteristic information
#define PROFILE_ID                  0
#define GATTS_SERVICE_UUID          0xFF01              // Custom 16 bit GATT service UUID
#define GATTS_CHAR_LED_UUID         0xFF02              // Custom 16 bit custom LED ON/OFF characteristic UUID

// LED information
#define LED_PIN                     GPIO_NUM_22
#define LED_ON                      1
#define LED_OFF                     0
#define LED_ON_INTERVAL_1           700
#define LED_OFF_INTERVAL_1          400
#define LED_ON_INTERVAL_2           700
#define LED_OFF_INTERVAL_2          2000
#define LED_REMOTE_GET_INTERVAL     1000 

// GATT client profile event handler declaration
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

// GAP target peripheral UUID that is being advertised, this central device will connect to the peripheral device that is advertising
static uint8_t adv_service_uuid128[16] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
};

// GAP scan parameters
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,                     // Active scan, also sends scan requests for more info about the advertising peripheral Bluetooth devices
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,                     // Type of Bluetooth address to use: static and public
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,                // Accept all packets except not the ones not directed towards this central Bluetooth device
    .scan_interval          = 0x50,                                     // Interval between two scans in units of 0.625 ms
    .scan_window            = 0x30,                                     // Duration of LE scan in units of 0.625 ms
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE,               // Filter out duplicate advertisements
};

// Flags
static bool connected = false;                                                  // If GATT client is connected to the GATT server
static bool got_service = false;                                                // If GATT client got the services of the GATT server
static bool got_characteristic = false;                                         // If GATT client got the target characteristic of the GATT server

// GATT client profile information
static uint16_t profile_gattc_if = ESP_GATT_IF_NONE;                            // GATT client profile interface, initialize it to none
static esp_gattc_cb_t profile_event_handler = gattc_profile_event_handler;      // GATT client profile callback, initialize it to the handler

// GATT client connection information
static uint16_t conn_id;                                                        // GATT client connection ID

// GATT client service information
static uint16_t service_start_handle;                                           // GATT client first service handle
static uint16_t service_end_handle;                                             // GATT client last service handle

// GATT client LED ON/OFF characteristic information
static uint16_t char_led_handle;                                                                                            // Characteristic handler
static esp_gatt_char_prop_t char_led_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;                 // Characteristic property

// Peripheral Bluetooth address
static esp_bd_addr_t remote_bda;

// UUID for filtering service
static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = GATTS_SERVICE_UUID,},
};

// UUID for filtering characteristic
static esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = GATTS_CHAR_LED_UUID,},
};

// LED current local level
static uint8_t local_led_level = 0;

// Task remote set state info
static TaskHandle_t xTaskHandleRemoteSet = NULL;                                                    // Handle of the task
static char* pcTaskNameRemoteSet = "R_SET";                                                         // Name of the task (maximum size is 16 characters)
static UBaseType_t uxTaskPriorityRemoteSet = tskIDLE_PRIORITY + 1;                                  // Task priority (same as task main)
static const configSTACK_DEPTH_TYPE usTaskStackDepthRemoteSet = configMINIMAL_STACK_SIZE * 8;       // Task stack size

// Task remote get state info
static TaskHandle_t xTaskHandleRemoteGet = NULL;                                                    // Handle of the task
static char* pcTaskNameRemoteGet = "R_GET";                                                         // Name of the task (maximum size is 16 characters)
static UBaseType_t uxTaskPriorityRemoteGet = tskIDLE_PRIORITY + 1;                                  // Task priority (same as task main)
static const configSTACK_DEPTH_TYPE usTaskStackDepthRemoteGet = configMINIMAL_STACK_SIZE * 8;       // Task stack size

// Extract characteristic from service
static void extract_characteristic(esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {

    // If discovered services from remote devices or if discovered from NVS flash
    if (param->search_cmpl.searched_service_source == ESP_GATT_SERVICE_FROM_REMOTE_DEVICE) ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, get service information from remote device");
    else if (param->search_cmpl.searched_service_source == ESP_GATT_SERVICE_FROM_NVS_FLASH) ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, get service information from flash");
    else ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, unknown service source");

    // If got service flag set
    if (got_service) {

        // Find the characteristic count in the services
        uint16_t count = 0;
        esp_gatt_status_t status = esp_ble_gattc_get_attr_count(gattc_if, param->search_cmpl.conn_id, ESP_GATT_DB_CHARACTERISTIC, service_start_handle, service_end_handle, 0, &count);
        if (status != ESP_GATT_OK) {
            ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, get attribute count error");
            return;
        }

        // If more than 0 characteristics
        if (count > 0) {

            // Allocate temporary memory storage
            esp_gattc_char_elem_t *char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
            if (!char_elem_result) {
                ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, gattc no memory");
                return;
            } else {

                status = esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id, service_start_handle, service_end_handle, remote_filter_char_uuid, char_elem_result, &count);
                if (status != ESP_GATT_OK) {
                    ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, get characteristic by uuid error");
                    free(char_elem_result);
                    char_elem_result = NULL;
                    return;
                }

                // Get first found characteristic in the service and set characteristic flag
                if (count > 0 && (char_elem_result[0].properties & char_led_property)) {
                    char_led_handle = char_elem_result[0].char_handle;
                    got_characteristic = true;
                }
        
            }

            free(char_elem_result); // Free the allocated memory

        } else ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, no characteristic found");

    }

}

// GATT client profile event handler
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {

    // Handle event
    switch (event) {

        // When write characteristic event occurs
        case ESP_GATTC_WRITE_CHAR_EVT:
            if (param->write.status != ESP_GATT_OK) ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "WRITE_CHAR_EVT, write characteristic failed, error status = %x", param->write.status);   
            break;

        // Print value when read characteristic event occurs
        case ESP_GATTC_READ_CHAR_EVT:
            if (param->read.status != ESP_GATT_OK) {
                ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "READ_CHAR_EVT, read characteristic failed, error status = %x", param->read.status);   
            } else {
                if (param->read.value_len == 1) ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "READ_CHAR_EVT, read value is: %d", param->read.value[0]);
            }
            break;

        // Setup scan parameters when register event
        case ESP_GATTC_REG_EVT:
            esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
            if (scan_ret) ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "REG_EVT, set scan params error, error code = %x", scan_ret);
            break;

        // When open event
        case ESP_GATTC_OPEN_EVT:
            if (param->open.status != ESP_GATT_OK) ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "OPEN_EVT, open failed, status %d", param->open.status);
            else ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "OPEN_EVT, open success");
            break;

        // Get services from the local cache when discover services complete event
        case ESP_GATTC_DIS_SRVC_CMPL_EVT:
            if (param->dis_srvc_cmpl.status != ESP_GATT_OK) {
                ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "DIS_SRVC_CMPL_EVT, discover service failed, status %d", param->dis_srvc_cmpl.status);
            } else {
                ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "DIS_SRVC_CMPL_EVT, discover service complete conn_id %d", param->dis_srvc_cmpl.conn_id);
                esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id, &remote_filter_service_uuid);
            }
            break;

        // Set the service flag, start handle and stop flag when search result event (a service discovered)
        case ESP_GATTC_SEARCH_RES_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_RES_EVT, conn_id = %x, is primary service %d", param->search_res.conn_id, param->search_res.is_primary);
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_RES_EVT, start handle %d, end handle %d, current handle value %d", param->search_res.start_handle, param->search_res.end_handle, param->search_res.srvc_id.inst_id);
            if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16 && param->search_res.srvc_id.uuid.uuid.uuid16 == GATTS_SERVICE_UUID) {
                ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_RES_EVT, service found");
                got_service = true;
                service_start_handle = param->search_res.start_handle;
                service_end_handle = param->search_res.end_handle;
                ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SEARCH_RES_EVT, UUID16: %x", param->search_res.srvc_id.uuid.uuid.uuid16);
            }
            break;

        // Extract the characteristic when search result complete event (service discovery complete)
        case ESP_GATTC_SEARCH_CMPL_EVT:            
            if (param->search_cmpl.status != ESP_GATT_OK) {
                ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "SEARCH_CMPL_EVT, search service failed, error status = %x", param->search_cmpl.status);
                break;
            }
            extract_characteristic(gattc_if, param);
            break;

        // When service changed event
        case ESP_GATTC_SRVC_CHG_EVT:
            esp_bd_addr_t bda;
            memcpy(bda, param->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "SRVC_CHG_EVT, bd_addr:");
            esp_log_buffer_hex(EVENT_HANDLER_GATTC_TAG, bda, sizeof(esp_bd_addr_t));
            break;

        // Set the connection id, copy the remote Bluetooth and negotiate the MTU packet size when connect event
        case ESP_GATTC_CONNECT_EVT:
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "CONNECT_EVT, conn_id %d, gattc_if %d", param->connect.conn_id, gattc_if);
            conn_id = param->connect.conn_id;

            memcpy(remote_bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "CONNECT_EVT, remote BDA:");
            esp_log_buffer_hex(EVENT_HANDLER_GATTC_TAG, remote_bda, sizeof(esp_bd_addr_t));
            
            esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, param->connect.conn_id);
            if (mtu_ret) ESP_LOGE(EVENT_HANDLER_GATTC_TAG, "CONNECT_EVT, config MTU error, error code = %x", mtu_ret);
            break;

        // When diconnect event occurs
        case ESP_GATTC_DISCONNECT_EVT:
            connected = false;                  // Unset the connect flag
            got_service = false;                // Unset the got services flag
            got_characteristic = false;         // Unset the got characteristic flag
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "DISCONNECT_EVT, reason = %d, rescanning", param->disconnect.reason);
            esp_ble_gap_start_scanning(SCAN_DURATION);
            break;

        // When configure MTU event
        case ESP_GATTC_CFG_MTU_EVT:
            if (param->cfg_mtu.status != ESP_GATT_OK) ESP_LOGE(EVENT_HANDLER_GATTC_TAG,"CFG_MTU_EVT, config mtu failed, error code = %x", param->cfg_mtu.status);
            else ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "CFG_MTU_EVT, status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
            break;

        // In case of other events
        /*
        case ESP_GATTC_REG_EVT:
        case ESP_GATTC_CLOSE_EVT:
        case ESP_GATTC_READ_DESCR_EVT:
        case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        case ESP_GATTC_WRITE_DESCR_EVT:
        case ESP_GATTC_NOTIFY_EVT:
        case ESP_GATTC_PREP_WRITE_EVT:
        case ESP_GATTC_EXEC_EVT:
        case ESP_GATTC_ACL_EVT:
        case ESP_GATTC_CANCEL_OPEN_EVT:
        case ESP_GATTC_ENC_CMPL_CB_EVT:
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

// GATT client event handler
static void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {

    // If event is a register application id event
    if (event == ESP_GATTC_REG_EVT) {

        // If event is a register application id event
        if (param->reg.status == ESP_GATT_OK) {
            profile_gattc_if = gattc_if;
        } else {
            ESP_LOGI(EVENT_HANDLER_GATTC_TAG, "ESP_GATTS_REG_EVT, registering application profile failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    // If GATT interface parameter is equal to none of the profiles (broadcast) or it equal to the profile GATT interface
    if (gattc_if == ESP_GATT_IF_NONE || gattc_if == profile_gattc_if) {
                
        // Call handler if handler pointer not NULL
        if (profile_event_handler) profile_event_handler(event, gattc_if, param);
    
    }

}

// Resolve scan results
static void scan_result_handler(esp_ble_gap_cb_param_t *param) {

    // Pointer to advertising data
    uint8_t *adv_uuid = NULL;
    uint8_t adv_uuid_len = 0;

    // Handle event
    switch (param->scan_rst.search_evt) {

        // When advertisement packet received event
        case ESP_GAP_SEARCH_INQ_RES_EVT:

            // Print Bluetooth device address of the device that
            esp_log_buffer_hex(EVENT_HANDLER_GAP_TAG, param->scan_rst.bda, 6);
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Searched advertising data length %d, scan response length %d", param->scan_rst.adv_data_len, param->scan_rst.scan_rsp_len);

            // Get pointer to target advertisement data by 128 bit advertising service UUID of the GATT server
            adv_uuid = esp_ble_resolve_adv_data(param->scan_rst.ble_adv, ESP_BLE_AD_TYPE_128SRV_CMPL, &adv_uuid_len);
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Searched device name length %d", adv_uuid_len);
            esp_log_buffer_hex(EVENT_HANDLER_GAP_TAG, adv_uuid, adv_uuid_len);

            // Compare if the UUID matches
            if (adv_uuid != NULL) {
                if (sizeof(adv_service_uuid128) == adv_uuid_len && memcmp(adv_uuid, adv_service_uuid128, adv_uuid_len) == 0) {

                    // Set the connected flag
                    if (connected == false) {

                        // Stop scanning
                        esp_ble_gap_stop_scanning();

                        // Set connected flag
                        ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Connecting to the remote device");
                        connected = true;

                        // Open a direct connection to GATT server
                        esp_ble_gattc_open(profile_gattc_if, param->scan_rst.bda, param->scan_rst.ble_addr_type, true);
                    
                    }
                }
            }

            break;
        
        // When scanning process complete event
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            break;

        default:
            break;

    }

}

// GAP event handler
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {

    // Handle event
    switch (event) {

        // When setting scanning parameters complete event
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
            esp_ble_gap_start_scanning(SCAN_DURATION);
            break;

        // When start scanning complete event
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:

            // If start scanning was successful
            if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(EVENT_HANDLER_GAP_TAG, "Scan start failed, error code = %x", param->scan_start_cmpl.status);
                break;
            }
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Scan start success");
            break;

        // When new BLE device detected event
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            scan_result_handler(param);
            break;

        // When stop scanning complete event
        case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
            if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) ESP_LOGE(EVENT_HANDLER_GAP_TAG, "Scan stop failed, error code = %x", param->scan_stop_cmpl.status);
            else ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Scan stop success");
            break;

        // When stop scanning complete event
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) ESP_LOGE(EVENT_HANDLER_GAP_TAG, "Advertising stop failed, error code = %x", param->adv_stop_cmpl.status);
            else ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Advertising stop success");
            break;

        // When update connection parameters complete event
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Update connection params status = %d, conn_int = %d, latency = %d, timeout = %d", param->update_conn_params.status, param->update_conn_params.conn_int, param->update_conn_params.latency, param->update_conn_params.timeout);
            break;

        // When set packet length complete event
        case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
            ESP_LOGI(EVENT_HANDLER_GAP_TAG, "Packet length updated: rx = %d, tx = %d, status = %d", param->pkt_data_length_cmpl.params.rx_len, param->pkt_data_length_cmpl.params.tx_len, param->pkt_data_length_cmpl.status);
            break;

        default:
            break;
    
    }

}

// Setup GATT client
static esp_err_t gattc_setup() {

    // Register GATT client callback function
    esp_err_t ret = esp_ble_gattc_register_callback(gattc_event_handler);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "GATT client register callback error, error code %x", ret);
        return ret;
    }

    // Register profile, generates an ESP_GATTS_REG_EVT event upon completion
    ret = esp_ble_gattc_app_register(PROFILE_ID);
    if (ret) {
        ESP_LOGE(MAIN_TAG, "GATT client register profile error, error code %x", ret);
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

// Remote read
static void remoteRead() {

    // Read from characteristic from GATT server, result will be in the handler function as a parameter
    esp_err_t ret = esp_ble_gattc_read_char(profile_gattc_if, conn_id, char_led_handle, ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) ESP_LOGI(GET_REMOTE_TASK_TAG, "Read characteristic failed, error status = %x", ret);

}

// Remote set task
static void taskRemoteGet(void *argument) {

    // The delay
    TickType_t remoteGetDelay = LED_REMOTE_GET_INTERVAL / portTICK_PERIOD_MS;

    // Loop it forever
    while (1) {

        if (got_characteristic) remoteRead();
        vTaskDelay(remoteGetDelay);

    }

}

// Remote write
static void remoteWrite(uint32_t level) {

    // Write characteristic to GATT server
    local_led_level = (uint8_t)level;
    esp_err_t ret = esp_ble_gattc_write_char(profile_gattc_if, conn_id, char_led_handle, 1, &local_led_level, ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) ESP_LOGI(SET_REMOTE_TASK_TAG, "Write characteristic failed, error status = %x", ret);

}

// Remote get task
static void taskRemoteSet(void *argument) {

    // The delays
    TickType_t led_on_interval_1 = LED_ON_INTERVAL_1 / portTICK_PERIOD_MS;
    TickType_t led_off_interval_1 = LED_OFF_INTERVAL_1 / portTICK_PERIOD_MS;
    TickType_t led_on_interval_2 = LED_ON_INTERVAL_2 / portTICK_PERIOD_MS;
    TickType_t led_off_interval_2 = LED_OFF_INTERVAL_2 / portTICK_PERIOD_MS;

    // Loop it forever
    while (1) {

        // ON
        gpio_set_level(LED_PIN, LED_ON);
        if (got_characteristic) remoteWrite(LED_ON);
        vTaskDelay(led_on_interval_1);

        // OFF
        gpio_set_level(LED_PIN, LED_OFF);
        if (got_characteristic) remoteWrite(LED_OFF);
        vTaskDelay(led_off_interval_1);

        // ON
        gpio_set_level(LED_PIN, LED_ON);
        if (got_characteristic) remoteWrite(LED_ON);
        vTaskDelay(led_on_interval_2);

        // OFF
        gpio_set_level(LED_PIN, LED_OFF);
        if (got_characteristic) remoteWrite(LED_OFF);
        vTaskDelay(led_off_interval_2);

    }

}

// Setup LED GPIO
static void gpio_led_setup() {

    // LED setup
    gpio_reset_pin(LED_PIN);                                    // Reset pin to default state
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);              // Set pin input-output mode (so both get and set are possible)
    gpio_set_level(LED_PIN, LED_OFF);                           // Set pin level to 0

}

// Entrypoint
void app_main(void) {

    // Setup LED GPIO
    gpio_led_setup();

    // Create the set task
    if (xTaskCreate(taskRemoteSet, pcTaskNameRemoteSet, usTaskStackDepthRemoteSet, NULL, uxTaskPriorityRemoteSet, &xTaskHandleRemoteSet) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Task creation error, exiting");
        return;
    }

    // Create the get task
    if (xTaskCreate(taskRemoteGet, pcTaskNameRemoteGet, usTaskStackDepthRemoteGet, NULL, uxTaskPriorityRemoteGet, &xTaskHandleRemoteGet) != pdPASS) {
        ESP_LOGE(MAIN_TAG, "Task creation error, exiting");
        return;
    }

    // Initialize default NVS partition 
    if (nvs_init() != ESP_OK) return;

    // Setup BLE controller
    if (ble_controller_setup() != ESP_OK) return;

    // Setup Bluedroid Bluetooth host stack
    if (bluedroid_stack_setup() != ESP_OK) return;

    // Setup GAP
    if (gap_setup() != ESP_OK) return;

    // Setup GATT client
    if (gattc_setup() != ESP_OK) return;

    return;

}
