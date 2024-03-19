#ifndef CONFIG_H
#define CONFIG_H

#include "esp_camera.h"
#include "esp_err.h"
#include "cJSON.h"
#include "esp_random.h"
#include "sd.h"
#include "camera.h"

// Pin to change MUX
#define SEL_PIN 3

/* States used for checking the SD configuration */
typedef enum{
    NO_SD,
    SD_SAVE,
    SD_SAVING
}sd_state_t;

/* System Configuration structure. Stores all system settings */
typedef struct{
    sd_state_t sd_save;
    bool flash;
    bool exit;
    bool pic_poll;
    bool active_cam;
    sensor_t* sensor;
    camera_status_t camera;
}system_config_t;



/**
 * @brief Takes all camera variables in the system config and sets them
 * 
 * @param sys_config the system config variable
 * 
 * @return ESP_OK on success
*/
esp_err_t camera_set_settings(system_config_t sys_config);

/**
 * @brief Used for debugging the camera sensor settings
 * 
 * @param sys_config the system config variable
*/
void camera_get_settings(system_config_t sys_config);

/**
 * @brief Uses the content received and parses it to the corresponding system config variables
 * 
 * @param content pointer to the JSON string
 * @param sys_config the system config variable
 * 
 * @return variable of type system_config, i.e. the system config variable
*/
system_config_t JSON_config_set(char* content, system_config_t sys_config);


/**
 * @brief Checks SD configuration and saves the image if appropriate
 * 
 * @param sys_config the system config variable
 * @param fb camera frame buffer
 * 
 * @return ESP_OK
*/
esp_err_t sys_sd_save_check(system_config_t* sys_config, camera_fb_t* fb);


/**
 * @brief Take a picture. Used to check if picture should be taken with flash or not
 * 
 * @param sys_config the system config variable
 * 
 * @return camera frame buffer
*/
camera_fb_t* sys_take_picture(system_config_t sys_config);


/**
 * @brief Switch the cameras. Used in the HTTP GET handlers
 * 
 * @param sys_config the system config variable
 * 
 * @return ESP_OK on success
*/
esp_err_t sys_camera_switch(system_config_t sys_config);

#endif