#pragma once

#include "esp_camera.h"
#include "esp_err.h"
#include "cJSON.h"
#include "esp_random.h"
#include "sd.h"
#include "camera.h"

/* Pin to change MUX */
#define SEL_PIN  4

#ifdef __cplusplus
extern "C" {
#endif

/* States used for checking the SD configuration */
typedef enum{
    NO_SD_SAVE,
    SD_SETUP,
    SD_SAVE
}sd_state_t;

/* System Configuration structure. Stores all system settings */
typedef struct{
    sd_state_t sd_save;
    bool flash;
    bool exit;
    bool pic_done;
    bool sel_status;
    sensor_t* sensor;
    camera_status_t camera;
}mcc_config_t;

/**
 * @brief Takes all camera variables in the system config and sets them
 * 
 * @return ESP_OK on success
*/
esp_err_t camera_set_settings();

/**
 * @brief Used for debugging the camera sensor settings. Prints the logs to the terminal
 * 
*/
void camera_get_settings();

/**
 * @brief Uses the content received and parses it to the corresponding system config variables
 * 
 * @param content pointer to the JSON string
 * 
 * @return variable of type mcc_config, i.e. the system config variable
*/
esp_err_t JSON_config_set(char* content);

/**
 * @brief Take a picture. Used to check if picture should be taken with flash or not
 * 
 * @return camera frame buffer
*/
camera_fb_t* sys_take_picture();

/**
 * @brief Setup the system to allow efficient saving time of the frame buffers. 
 * Creates the variables for the file names by creating random string of six caracters, one being the null terminator.
 * 
 * @return ESP_OK on success
*/
esp_err_t sys_sd_var_setup();

/**
 * @brief Save the frame buffers to the SD card based on the file names defined by the sys_sd_var_setup() function.
 * 
*/
esp_err_t sys_sd_save(camera_fb_t* fb);

/**
 * @brief Switch the cameras. Used in the HTTP GET handlers
 * 
 * @return ESP_OK on success
*/
esp_err_t sys_camera_switch();

#ifdef __cplusplus
}
#endif