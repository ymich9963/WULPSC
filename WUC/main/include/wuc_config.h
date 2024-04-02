#pragma once

#include "stdbool.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "esp_timer.h"
#include "wuc_config.h"

/* GPIO pin the MCC is connected to */
#define MCC_PIN         GPIO_NUM_2

/* WUC System Configuration structure*/
typedef struct{
    bool exit;
    int sleep_time_sec;
    int active_time_sec;
    char ssid[33];
    char pswd[65];
    bool stored_creds;
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