#include "http.h"

const char* TAG = "WULPSC - WUC HTTP";

extern wuc_config_t wuc_config;

/* Our URI handler function to be called during GET /uri request */ 
esp_err_t get_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Entered get handler");

    /* Send a simple response to test functionality */ 
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t exit_handler(httpd_req_t *req){
    wuc_config.exit = true;
    return ESP_OK;
}

/* URI handlers */
httpd_uri_t uri_get = {
    .uri      = "/uri",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

httpd_uri_t exit_get = {
    .uri      = "/exit",
    .method   = HTTP_GET,
    .handler  = exit_handler,
    .user_ctx = NULL
};

/* Function for starting the webserver */  
httpd_handle_t start_webserver(void){
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 19520;

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) 
    {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &exit_get);

    }
    /* If server failed to start, handle will be NULL */
    return server;
}

void stop_webserver(httpd_handle_t server){
    if (server) {
        httpd_stop(server);
    }
}