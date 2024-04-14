#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "esp_timer.h"
#include "wuc_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GPIO pin the MCC is connected to */
#define MCC_PIN         GPIO_NUM_2

/* WUC System Configuration structure. */
typedef struct{
    bool exit;                  // to check to exit the main server loop and enter deep sleep
    bool reset;                 // to reset the system to accept new WiFi credentials
    uint32_t sleep_time_sec;    // to store how long the system will sleep
    uint32_t active_time_sec;   // to store the default active time
    char ssid[33];              // to store the WiFi SSID
    char pswd[65];              // to store the WiFi Password
    bool stored_creds;          // to check if credentials are stored in the NVS
}wuc_config_t;

/**
 * @brief Setup the deep sleep configuration based on the sleep_time_sec in the config structure. Happens at every boot.
 * 
 * @return ESP_OK on success
*/
esp_err_t deep_sleep_setup();

/**
 * @brief GPIO setup to power on the MCC
 * 
 * @return ESP_OK
*/
esp_err_t setup_mcc_power_switch();

/**
 * @brief Call to power up the MCC.
 * 
 * @return ESP_OK on success
*/
esp_err_t mcc_powerup();

/**
 * @brief Call to power down the MCC
 * 
 * @return ESP_OK on success
*/
esp_err_t mcc_powerdown();

#ifdef __cplusplus
}
#endif