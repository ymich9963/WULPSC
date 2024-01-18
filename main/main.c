#include <esp_log.h>
#include <string.h>
#include <nvs_flash.h>
#include "camera.h"
#include "wifi.h"
#include "esp_http_server.h"
#include "esp_timer.h"

#define LOOP_DELAY_MS 5000    // delay between each loop in ms

static const char *TAG = "WULPSC - http test";

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
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t fb_len = 0;
    int64_t fr_start = esp_timer_get_time();

    fb = esp_camera_fb_get(); // gets a picture
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG,"Picture taken. Trying to send now.");
    res = httpd_resp_set_type(req, "image/jpeg");
    if(res == ESP_OK){
        res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
        ESP_LOGI(TAG, "Set header");
    }
    if(res == ESP_OK){
        if(fb->format == PIXFORMAT_JPEG){
            fb_len = fb->len;
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
            ESP_LOGI(TAG, "Response Sent");
        } else {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            fb_len = jchunk.len;
            ESP_LOGI(TAG, "Response Sent in chunks");
        }
    }
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "JPG: %luKB %lums", (uint32_t)(fb_len/1024), (uint32_t)((fr_end - fr_start)/1000));
    return res;
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
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 19520;

    // Empty handle to esp_http_server
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) 
    {
        // Register URI handlers
        // httpd_register_uri_handler(server, &uri_post);

        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &jpg_get);

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

void app_main(void)
{   
    // Variable for function returns
    esp_err_t ret;

    httpd_handle_t server;

    //// Wi-Fi stuff
    //initialise the non volatile storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if(ret != ESP_OK) 
    {
        return;
    }

    wifi_init_general();
    wifi_scan();
    wifi_init_sta();

    server = start_webserver();

    if(server == NULL)
    {
        ESP_LOGE(TAG,"Error Server is NULL");
        return;
    }

    ret = init_camera();
    if(ret != ESP_OK) // if camera is not initialised, return 
    { 
        return;
    }

    while(server)
    {
        vTaskDelay(LOOP_DELAY_MS/portTICK_PERIOD_MS);
        //ESP_LOGI(TAG,"In the loop");
    }

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}