#include <esp_log.h>
#include "esp_http_server.h"
#include "sys/param.h" // contains MIN() macro
#include "wuc_config.h"

/**
 * @brief GET Handler, only used for testing 
 * 
 * @return ESP_OK on success 
*/
esp_err_t get_handler(httpd_req_t *req);

/**
 * @brief GET handler to notify WUC to should down
 * 
 * @return ESP_OK on success
*/
esp_err_t exit_handler(httpd_req_t *req);


/**
 * @brief Initialise the HTTP server
 * 
 * @return Handle to the HTTP server
*/
httpd_handle_t start_webserver(void);


/**
 * @brief Call to stop the HTTP server
*/
void stop_webserver(httpd_handle_t server);