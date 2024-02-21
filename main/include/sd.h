#ifndef SD_H
#define SD_H

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
#define FILE_NAME MOUNT_POINT"/img.jpg"

#include "esp_err.h"
#include "esp_camera.h"
#include <nvs_flash.h>

// from SD card example
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"


/**
 * @brief Initialise the SD card, save the picture and de-initialise
 * 
 * @param fb Pointer to the camera frame buffer
 * 
 * @return ESP_OK on success
*/
esp_err_t sd_init(camera_fb_t *fb);

/**
 * @brief Writes all of the image data to a file
 * 
 * @param path File path
 * @param fb Pointer to the camera frame buffer
 * 
 * @return ESP_OK on success
*/
esp_err_t write_arr(const char *path, camera_fb_t *fb);

#endif