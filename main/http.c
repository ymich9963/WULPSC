#include "http.h"

static const char *TAG = "WULPSC - http protocol";
extern camera_fb_t * fb;
extern system_config_t sys_config;

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

size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
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

esp_err_t picture_handler(httpd_req_t *req){
    esp_err_t res = ESP_OK;
    esp_err_t ret = ESP_OK;

    // if frame buffer null
    if(!fb){
        fb = fb_refresh(fb);
        fb = sys_take_picture(sys_config);
        sys_sd_save_check(sys_config, fb);
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

    //! TODO: make chunking availiable without it being in a non-JPEG format, change 80 in frame2jpg
    if(res == ESP_OK){
        if(fb->format == PIXFORMAT_JPEG){
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
            ESP_LOGI(TAG, "Response Sent");
        } else {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            ret = httpd_resp_send_chunk(req, NULL, 0);
            ESP_LOGI(TAG, "Response Sent in chunks");
        }
        if(ret==ESP_OK){
            sys_config.pic_poll = true;
        }
    }
    esp_camera_fb_return(fb);
    fb = NULL; // reset to null to recheck if at the top

    return res;
}

esp_err_t exit_handler(httpd_req_t *req){
    sys_config.exit = 1;
    return ESP_OK;
}

esp_err_t cam1_handler(httpd_req_t *req){
    /* 
       handler takes a picture then de-initialises and powers down the camera
       after the MUXs are switched it will power up, re-initialise the second camera
       to have it ready for the next GET request
    */

    esp_err_t ret;
    
    ret = picture_handler(req);
    if(ret != ESP_OK){
        ESP_LOGW(TAG, "JPEG Handler returned badly");
    }
    
    ret = camera_switch(sys_config.cam_switched);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "Camera switch returned badly");
    }

    return ret;
}

esp_err_t cam2_handler(httpd_req_t *req){
    /* 
       handler for the second camera. takes the picture but does not switch back.
       checks if pic is taken before also taking a pic.
    */

    esp_err_t ret;
    
    ESP_LOGI(TAG, "Waiting for first picture to finish sending.");
    while(!sys_config.pic_poll){
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    

    ret = picture_handler(req);
    if(ret != ESP_OK){
        ESP_LOGW(TAG, "JPEG Handler returned badly");
    }

    ret = camera_switch(sys_config.cam_switched);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "Camera switch returned badly");
    }

    return ret;
}


esp_err_t config_settings_post_handler(httpd_req_t *req){

    /* Buffer for recieving content/params, content is expected to be a JSON string
    * such as {saturation: 2, contrast: 0, brightness: 1, ...} */
    int MAX = 511;
    char *content;

    /* Make sure bytes dont go over max length*/
    size_t recv_size = req->content_len;
    if (recv_size >= MAX) {
        ESP_LOGE(TAG, "Over %d bytes sent", MAX);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    content = malloc(sizeof(char) * recv_size + 1);
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
    /* Add null terminating byte to convert content buffer into string */
    content[recv_size] = '\0';
	

    sys_config = JSON_config_set(content, sys_config);
    sys_config.sensor = esp_camera_sensor_get();
    camera_set_settings(sys_config);

    
    const char resp[] = "Paramaters set";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;

    /* Sample cURL command is in http.h */
}

/* GET Handlers */
httpd_uri_t uri_get = {
    .uri      = "/uri",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

httpd_uri_t pic_get = {
    .uri      = "/pic",
    .method   = HTTP_GET,
    .handler  = picture_handler,
    .user_ctx = NULL
};

httpd_uri_t exit_get = {
    .uri      = "/exit",
    .method   = HTTP_GET,
    .handler  = exit_handler,
    .user_ctx = NULL
};

httpd_uri_t cam1_get = {
    .uri      = "/cam1",
    .method   = HTTP_GET,
    .handler  = cam1_handler,
    .user_ctx = NULL
};

httpd_uri_t cam2_get = {
    .uri      = "/cam2",
    .method   = HTTP_GET,
    .handler  = cam2_handler,
    .user_ctx = NULL
};


/* POST Handlers */
httpd_uri_t config_settings_post = {
    .uri      = "/config",
    .method   = HTTP_POST,
    .handler  = config_settings_post_handler,
    .user_ctx = NULL
};

httpd_handle_t start_webserver(void)
{
    // Generate default configuration
    //! TODO: try to change settings so that it gives up quickly
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 19520;
    // config.keep_alive_enable = true;
    // config.keep_alive_idle = 3;
    // config.keep_alive_interval = 3;
    // config.keep_alive_count = 3;
    // config.recv_wait_timeout = 1;
    // config.send_wait_timeout = 1;

    // Empty handle to esp_http_server
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) 
    {
        // Register URI handlers

        // GET
        httpd_register_uri_handler(server, &uri_get);   // simple handler for testing
        httpd_register_uri_handler(server, &pic_get);   // to get the image
        httpd_register_uri_handler(server, &exit_get);  // to exit main loop and shutdown
        httpd_register_uri_handler(server, &cam1_get);  // camera 1 handler
        httpd_register_uri_handler(server, &cam2_get);  // camera 2 handler

        // POST
        httpd_register_uri_handler(server, &config_settings_post); // change camera settings

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