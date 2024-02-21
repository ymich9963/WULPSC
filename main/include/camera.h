/**
 *  File contains all camera related function declarations
 * 
*/
#ifndef CAM_H
#define CAM_H

/* Camera definitions for ESP32-CAM sys_config */
#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
#define CAM_PIN_FLASH 4

#include "esp_camera.h"
#include "esp_log.h"
#include "http.h"

/**
 * @brief Used to set up the flash LED on the ESP
*/
void setup_flash();

/**
 * @brief Turn on the flash
*/
void turn_on_flash();

/**
 * @brief Turn off the flash
*/
void turn_off_flash();

/**
 * @brief Output data from the picture taken
 * 
 * @param fb pointer to the camera frame buffer
*/
void pic_data_output(camera_fb_t *fb);

/**
 * @brief Initialise the camera
 * 
 * @return ESP_OK on success
*/
esp_err_t init_camera();

/**
 * @brief Switch the cameras. Used in the HTTP GET handlers
 * 
 * @param cam_switched to check if the cameras have already been switched
 * 
 * @return ESP_OK on success
*/
esp_err_t camera_switch(bool cam_switched);



#endif