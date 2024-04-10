#pragma once

#include <string.h>
#include "sys/param.h" // contains MIN() macro
#include "esp_http_server.h"
#include "esp_log.h"
#include "wuc_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GET Handler, only used for testing 
 * 
 * @param req HTTP Request
 * 
 * @return ESP_OK on success 
*/
esp_err_t get_handler(httpd_req_t *req);

/**
 * @brief GET handler to notify WUC to should down
 * 
 * @param req HTTP Request
 * 
 * @return ESP_OK on success
*/
esp_err_t exit_handler(httpd_req_t *req);

/**
 * @brief POST handler to receive the specified sleep time
 * 
 * @param req HTTP Request
 * 
 * @return ESP_OK on success 
*/
esp_err_t set_sleep_time_handler(httpd_req_t *req);

/**
 * @brief Initialise the HTTP server
 * 
 * @return Handle to the HTTP server
*/
httpd_handle_t start_webserver(void);


/**
 * @brief Call to stop the HTTP server
 * 
 * @param server handle to server
*/
void stop_webserver(httpd_handle_t server);

/*

curl -ContentType 'text/plain' -Body "SLEEP_TIME_SECONDS" -Method Post http://IP_ADDRESS_HERE:19520/sleep  

*/

#ifdef __cplusplus
}
#endif