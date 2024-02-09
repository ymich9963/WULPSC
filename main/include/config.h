#ifndef CONFIG_H
#define CONFIG_H

#include "camera.h"
#include "esp_err.h"

typedef struct{
    bool sd_save;
    bool flash;
    bool done;
    sensor_t* sensor;
    camera_status_t camera;
}system_config_t;

esp_err_t camera_set_settings(camera_status_t camera, system_config_t sys_config);

#endif