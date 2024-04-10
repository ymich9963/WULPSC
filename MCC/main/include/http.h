#pragma once

#include "esp_http_server.h"
#include "esp_err.h"
#include "mcc_config.h"
#include "esp_log.h"
#include "camera.h"
#include "sd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Used as a callback to encode the frame buffer to JPEG when the pixel format is not 
 * JPEG and must be converted for efficient data transmission. Function was provided by Esspressif.
 * 
 * @param arg Used to set the JPEG chunking variable
 * @param index To check length
 * @param data Image data
 * @param len Size of the data
 * 
 * @return len or the size of the data
*/
static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len);

/**
 * @brief Handler to take a picture and send it via HTTP. 
 * 
 * @param req pointer to HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t picture_handler(httpd_req_t *req);

/**
 * @brief A simple HTTP handler. Used to test server connection.
 * 
 * @param req pointer to the HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t get_handler(httpd_req_t *req);

/**
 * @brief Start the HTTP server
 * 
 * @param void
 * 
 * @return handle to the HTTP server
*/
httpd_handle_t start_webserver();

/**
 * @brief Stop the HTTP server
 * 
 * @param server handle to the HTTP server
*/
void stop_webserver(httpd_handle_t server);

/**
 * @brief Handler to change the camera configuration and system settings
 * 
 * @param req pointer to the HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t config_settings_post_handler(httpd_req_t *req);


/* Sample cURL command to use with the terminal to set the system settings,

curl -ContentType 'application/json' -Body ' {"brightness":0, "contrast":0, "saturation":0, "sharpness":0, "denoise":0,  "special_effect":0,"wb_mode":3,"ae_level":0,"aec_value":200,"agc_gain":2,"gainceiling":6,"lenc":1,"agc":0,"aec":0, "hmirror":0,"vflip":0,"aec2":0, "bpc":1, "wpc":1, "sd_save":0}' -Method Post http://IP_ADDRESS_HERE:19520/config

*/

#ifdef __cplusplus
}
#endif