/**
 *  File contains all camera related function declarations
 * 
*/
#ifndef CAM_H
#define CAM_H

/* Camera definitions for ESP32-CAM mcc_config */
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
#include "mcc_config.h"



/**
 * @brief Used to set up the flash LED on the ESP
*/
esp_err_t setup_flash();

/**
 * @brief Turn on the flash
*/
esp_err_t turn_on_flash();

/**
 * @brief Turn off the flash
*/
esp_err_t turn_off_flash();

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


void change_pixformat_to_jpeg();

/**
 * @brief Refresh the frame buffer by taking a pic a discarding it
 * 
 * @param fb - frame buffer
 * 
 * @note It was observed that for an updated picture with the correct settings, the picture had to be taken twice, this function simplifies that process.
*/
camera_fb_t* fb_refresh(camera_fb_t * fb);


#endif