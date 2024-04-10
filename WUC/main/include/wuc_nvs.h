#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "wuc_config.h"

#define STORAGE_NAMESPACE "storage"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Used to check the size (in bytes) allocated for the credentials in the NVS. 
 * 
 * @return ESP_OK on success
*/
esp_err_t get_nvs_sizes();

/**
 * @brief Check the NVS storage for the  Wifi credentials. If a size of 0 was detected by get_nvs_sizes(), then they are assumed to not exist.
 * 
 * @return ESO_OK on success
*/
esp_err_t check_nvs();

/**
 * @brief For storing the new credentials to the NVS. Sizes of the credentials is not required. Called after smart config is executed
 * to get the credentials.
 * 
 * @return ESP_OK on success
*/
esp_err_t store_creds_to_nvs();

#ifdef __cplusplus
}
#endif