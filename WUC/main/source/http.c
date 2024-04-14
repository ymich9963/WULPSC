#include "http.h"

const char* TAG = "WULPSC - WUC HTTP";

/* System configuration defined in main */
extern wuc_config_t wuc_config;

/* Our URI handler function to be called during GET /uri request */ 
esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entered get handler");

    /* Send a simple response to test functionality */ 
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t exit_handler(httpd_req_t *req)
{
    /* Set to true to exit the main server loop and enter deep sleep*/
    wuc_config.exit = true;

    /* Send response to signify it has been executed */
    const char resp[] = "Exited server loop.";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t reset_handler(httpd_req_t *req)
{
    wuc_config.reset = true;

    /* Send response to signify it has been executed */
    const char resp[] = "Erased flash and restarted system.";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


esp_err_t set_sleep_time_handler(httpd_req_t *req)
{

    /* Max character length */
    int MAX = 511;    

    /* Make sure bytes dont go over max length */
    size_t recv_size = req->content_len;
    if (recv_size >= MAX){
        ESP_LOGE(TAG, "Over %d bytes sent", MAX);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Size %u bytes", recv_size);

    /* Allocate the size of the received string */
    char* content = malloc(sizeof(char) * recv_size + 1);    

    /* Set the received data to the content variable */
    int ret = httpd_req_recv(req, content, recv_size);

    /* Add a null terminator character to signify the end of the string */
    content[recv_size] = '\0';

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
    sscanf(content, "%lu", &wuc_config.sleep_time_sec);
    ESP_LOGI(TAG, "Sleep time set to %lu", wuc_config.sleep_time_sec);
    
    /* Send a response to */
    const char resp[] = "Sleep parameter set";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* GET handlers */
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

httpd_uri_t reset_get = {
    .uri      = "/reset",
    .method   = HTTP_GET,
    .handler  = reset_handler,
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
        httpd_register_uri_handler(server, &reset_get);

        /* Register POST handlers */
        httpd_register_uri_handler(server, &set_sleep_time_post);

    }
    /* If server failed to start, handle will be NULL */
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}