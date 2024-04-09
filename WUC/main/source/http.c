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
    const char resp[] = "Exited server loop.";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t set_sleep_time_handler(httpd_req_t *req){

    uint32_t MAX = sizeof(wuc_config.sleep_time_sec);
    uint32_t *content;

    /* Make sure bytes dont go over max length*/
    size_t recv_size = req->content_len;
    if (recv_size <= 0) {
        ESP_LOGE(TAG, "0 or less bytes sent");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    } else if (recv_size >= MAX){
        ESP_LOGE(TAG, "Over %d bytes sent", MAX);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Size %d bytes", recv_size);
    
    int ret = httpd_req_recv(req, content, recv_size);
    ESP_LOGI(TAG, "Data %s", content);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }

    /* Set the received conent to the system config */
    wuc_config.sleep_time_sec = content;
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

/* POST Handlers */
httpd_uri_t set_sleep_time_post = {
    .uri      = "/sleep",
    .method   = HTTP_POST,
    .handler  = set_sleep_time_handler,
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

        /* Register POST handlers */
        httpd_register_uri_handler(server, &set_sleep_time_post);

    }
    /* If server failed to start, handle will be NULL */
    return server;
}

void stop_webserver(httpd_handle_t server){
    if (server) {
        httpd_stop(server);
    }
}