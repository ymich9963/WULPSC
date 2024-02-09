#include "http.h"

static const char *TAG = "WULPSC - http protocol";
extern camera_fb_t * fb;
extern system_config_t sys_config;

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;

static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if(!index){
        j->len = 0;
    }
    if(httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK){
        return 0;
    }
    j->len += len;
    return len;
}

esp_err_t jpg_httpd_handler(httpd_req_t *req){
    esp_err_t res = ESP_OK;

    // if frame buffer null
    if(!fb){
        fb = esp_camera_fb_get();
        ESP_LOGI(TAG,"Took new picture.");
        pic_data_output(fb);
    }

    // check again 
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG,"Picture received. Trying to send now.");
    res = httpd_resp_set_type(req, "image/jpeg");
    if(res == ESP_OK){
        res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    }

    // TODO: make chunking availiable without it being in a non-JPEG format
    if(res == ESP_OK){
        if(fb->format == PIXFORMAT_JPEG){
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
            ESP_LOGI(TAG, "Response Sent");
        } else {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            ESP_LOGI(TAG, "Response Sent in chunks");
        }
    }
    esp_camera_fb_return(fb);
    fb = NULL; // reset to null to recheck if at the top

    return res;
}

esp_err_t done_handler(httpd_req_t *req){
    sys_config.done = 1;
    return ESP_OK;
}

// Our URI handler function to be called during GET /uri request  
esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entered get handler");

    // Send a simple response 
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t change_settings_handler(httpd_req_t *req){
    sys_config.sensor->set_exposure_ctrl(sys_config.sensor,0);
    ESP_LOGI(TAG, "Settings changed.");

    return ESP_OK;
}

esp_err_t revert_settings_handler(httpd_req_t *req){
    sys_config.sensor->set_exposure_ctrl(sys_config.sensor,1);
    ESP_LOGI(TAG, "Settings changed.");

    return ESP_OK;
}

esp_err_t reset_handler(httpd_req_t *req){
    sys_config.sensor->reset(sys_config.sensor);
    ESP_LOGI(TAG, "Settings changed.");

    return ESP_OK;
}

// Our URI handler function to be called during POST /uri request
esp_err_t post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    char content[100];

    // Truncate if content length larger than the buffer
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  // 0 return value indicates connection closed 
        // Check if timeout occurred
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
    // Spliting paramters recieved

    char * token = strtok(content, ","); // split string with ,
    int i = 0;
    long param[] = {0, 0, 0};
    while( token != NULL ) {
      param[i] = strtol(token, NULL, 10); // convert to long?
      token = strtok(NULL, ",");
      i++;
    }
    ESP_LOGI(TAG, "Saturation %ld", param[0] );
    ESP_LOGI(TAG, "Contrast %ld", param[1] );
    ESP_LOGI(TAG, "Brightness %ld", param[2] );
    // Send a simple response
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;

    // curl -ContentType 'text/plain' -Body '0,2,1' -Method Post http://95.147.177.160:19520/post
}

// URI handler structure for GET /uri  and /jpeg
httpd_uri_t uri_get = {
    .uri      = "/uri",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

httpd_uri_t jpg_get = {
    .uri      = "/jpg",
    .method   = HTTP_GET,
    .handler  = jpg_httpd_handler,
    .user_ctx = NULL
};

httpd_uri_t done_get = {
    .uri      = "/done",
    .method   = HTTP_GET,
    .handler  = done_handler,
    .user_ctx = NULL
};

httpd_uri_t change_settings_get = {
    .uri      = "/cs",
    .method   = HTTP_GET,
    .handler  = change_settings_handler,
    .user_ctx = NULL
};

httpd_uri_t revert_settings_get = {
    .uri      = "/rs",
    .method   = HTTP_GET,
    .handler  = revert_settings_handler,
    .user_ctx = NULL
};

httpd_uri_t reset_get = {
    .uri      = "/reset",
    .method   = HTTP_GET,
    .handler  = reset_handler,
    .user_ctx = NULL
};

// URI handler structure for POST /post
httpd_uri_t uri_post = {
    .uri      = "/post",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};

// Function for starting the webserver  
httpd_handle_t start_webserver(void)
{
    // Generate default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 19520;

    // Empty handle to esp_http_server
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) 
    {
        // Register URI handlers

        // POST
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &jpg_get);
        httpd_register_uri_handler(server, &done_get);
        httpd_register_uri_handler(server, &reset_get);
        httpd_register_uri_handler(server, &change_settings_get);
        httpd_register_uri_handler(server, &revert_settings_get);

        // GET
        httpd_register_uri_handler(server, &uri_post);

    }
    // If server failed to start, handle will be NULL
    return server;
}

// Function for stopping the webserver
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        // Stop the httpd server
        httpd_stop(server);
    }
}