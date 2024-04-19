#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_vfs_dev.h"
#include "esp_smartconfig.h"
#include "driver/uart.h"
#include "wuc_config.h"

/* Credentials used to test the WUC without the smart config process  */
#define TEST_CRED 0
    #if TEST_CRED
        #define ESP_WIFI_SSID      "test-ssid"
        #define ESP_WIFI_PASS      "test-pass"
    #endif

/* Maximum retries to connect to the WiFi. Only used when stored credentials are detected */
#define ESP_MAXIMUM_RETRY  5

/* Bits used in the WiFi event group */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event handler for smart config using ESPTouch. Parameters are required for smart config to take place.
*/
void sc_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Task that gets created for smart config to be executed
*/
void smartconfig_task(void * parm);

/**
 * @brief Event handler for the WiFi initialisation with only using the stored credentials. Parameters are the required event handler parameters.
*/
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Initialise WiFi with the stored credentials. Only uses the credentials stored in the NVS. For the credentials to change
 * the flash must be erased.
 * 
 * @return ESP_OK on success
*/
esp_err_t wifi_init(void);

/**
 * @brief Initialise WiFi with smart config. It requires the ESPTouch app downloaded to pass the credentials from the 
 * connected device with the app to the ESP32.
 * 
 * @return ESP_OK on success
*/
esp_err_t wifi_init_sc(void);

#ifdef __cplusplus
}
#endif
