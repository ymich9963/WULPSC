#include "http.h"

static const char *TAG = "WULPSC - http protocol";
extern camera_fb_t * fb;

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;

// Our URI handler function to be called during GET /uri request  
esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entered get handler");

    // Send a simple response 
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

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

// Function for starting the webserver  
httpd_handle_t start_webserver(void)
{
    // Generate default configuration
    // TODO: try to change settings so that it gives up quickly
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 19520;

    // Empty handle to esp_http_server
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) 
    {
        // Register URI handlers

        // POST
        httpd_register_uri_handler(server, &uri_get);   // simple handler for testing
        httpd_register_uri_handler(server, &jpg_get);   // to get the image

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