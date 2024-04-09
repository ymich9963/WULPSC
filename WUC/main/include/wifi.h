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

/* Credentials used to test the WUC */
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

/**
 * @brief Event handler for smart config using ESPTouch
*/
void sc_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Task that gets created for smart config to be executed
*/
void smartconfig_task(void * parm);

/**
 * @brief Initialise WiFi with the stored credentials
 * 
 * @return ESP_OK on success
*/
esp_err_t wifi_init(void);

/**
 * @brief Initialise WiFi with smart config
 * 
 * @return ESP_OK on success
*/
esp_err_t wifi_init_sc(void);
