#ifndef HTTP_H
#define HTTP_H

#include "esp_http_server.h"
#include "esp_err.h"
#include "esp_log.h"
#include "camera.h"


static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len);


esp_err_t jpg_httpd_handler(httpd_req_t *req);


esp_err_t get_handler(httpd_req_t *req);


httpd_handle_t start_webserver(void);


void stop_webserver(httpd_handle_t server);

#endif