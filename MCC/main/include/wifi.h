#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "sd.h"

#define SD_CRED    1
    #if !SD_CRED
        #define ESP_WIFI_SSID      "Gavrilis"
        #define ESP_WIFI_PASS      "123456789"
    #endif

#define ESP_MAXIMUM_RETRY  3
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#ifdef __cpluplus
extern "C" {
#endif

/**
 * @brief General required calls to initialise WiFi functionality for an ESP32.
*/
void wifi_init_general();

/**
 * @brief Used to set the CONNECTED or FAIL bits for the event.
 * 
 * @param arg General arguement pointer
 * @param event_base Event type
 * @param event_id Event ID
 * @param event_data Data received from the event.
*/
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Initialise Station Mode
 * 
 * @return ESP_OK on success, ESP_FAIL on failing to connect to the WiFi network
*/
esp_err_t wifi_init_sta(void);

#ifdef __cplusplus
}
#endif