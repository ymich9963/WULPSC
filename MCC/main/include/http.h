#ifndef HTTP_H
#define HTTP_H

#include "esp_http_server.h"
#include "mcc_config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "camera.h"
#include "sd.h"

/**
 * @brief For JPEG Video streaming. Not used currently.
 * 
 * @param unkown
*/
static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len);

/**
 * @brief JPEG HTTP handler. Used to send a JPG using HTTP and handles if the image is taken with flash or if the image is saved
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
httpd_handle_t start_webserver(void);

/**
 * @brief Stop the HTTP server
 * 
 * @param server handle to the HTTP server
*/
void stop_webserver(httpd_handle_t server);

/**
 * @brief Handler to signify when the server is done
 * 
 * @param req pointer to the HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t done_handler(httpd_req_t *req);

/**
 * @brief Handler to take picture from the first camera and switch to the second
 * 
 * @param req pointer to the HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t cam1_handler(httpd_req_t *req);

/**
 * @brief Handler to poll for the first picture and take a picture from the second camera
 * 
 * @param req pointer to the HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t cam2_handler(httpd_req_t *req);


/**
 * @brief Handler to change the camera configuration
 * 
 * @param req pointer to the HTTP request 
 * 
 * @return ESP_OK on success
*/
esp_err_t config_settings_post_handler(httpd_req_t *req);

/* Sample cURL command,

curl -ContentType 'application/json' -Body ' {"brightness":       0, "contrast":0, "saturation":0, "sharpness":0, "denoise":0,  "special_effect":0,"wb_mode":3,"ae_level":0,"aec_value":200,"agc_gain":2,"gainceiling":6,"lenc":1,"agc":0,"aec":0, "hmirror":0,"vflip":0,"aec2":0, "bpc":1, "wpc":1}' -Method Post http://192.168.124.145:19520/config

*/

#endif