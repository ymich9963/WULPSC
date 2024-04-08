#ifndef SD_H
#define SD_H

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_camera.h"
#include "esp_err.h"

// SD card pins used in SPI communication
//based on sdspi example
#define SD_DATA0 2
#define SD_DATA3 13
#define SD_CLK 14
#define SD_CMD 15

// defined to be used as SPI pins
#define PIN_NUM_MISO SD_DATA0
#define PIN_NUM_CS SD_DATA3
#define PIN_NUM_CLK SD_CLK
#define PIN_NUM_MOSI SD_CMD 

// Folder to mount at
#define MOUNT_POINT "/sdcard"
#define WIFICRED_FILE_NAME MOUNT_POINT"/wificred.txt" 
#define MAX_CHAR_LINE 30

/**
 * @brief Initialise the SD card, save the picture and de-initialise
 * 
 * @param fb Pointer to the camera frame buffer
 * 
 * @return ESP_OK on success
*/
esp_err_t sd_init();


/**
 * @brief Writes all of the image data to a file
 * 
 * @param path File path
 * @param fb Pointer to the camera frame buffer
 * 
 * @return ESP_OK on success
*/
esp_err_t sd_write_arr(const char *path, camera_fb_t *fb);

/**
 * @brief De-initialise the SD card, variables used are the globals defined in sd.c
*/
esp_err_t sd_deinit();
/**
 * @brief Read WiFi credentials from the file wificred located on the SD card 
*/
esp_err_t read_wifi_credentials();



esp_err_t sys_sd_var_setup();



esp_err_t sys_sd_save(camera_fb_t* fb);

#endif