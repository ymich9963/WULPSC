#ifndef CONFIG_H
#define CONFIG_H

#include "camera.h"
#include "http.h"
#include "esp_err.h"
#include "cJSON.h"

/* System Configuration structure. Stores all system settings*/
typedef struct{
    bool sd_save;
    bool flash;
    bool done;
    bool pic_taken;
    bool cam_switched;
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
 * @brief Uses the content received and parses it to the corresponding system config variables
 * 
 * @param content pointer to the JSON string
 * @param sys_config the system config variable
 * 
 * @return variable of type system_config, i.e. the system config variable
*/
system_config_t JSON_config_set(char* content, system_config_t sys_config);




#endif