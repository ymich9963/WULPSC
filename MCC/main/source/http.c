#include "http.h"

static const char *TAG = "WULPSC - http protocol";
extern camera_fb_t * fb;
extern mcc_config_t mcc_config;

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;


esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Entered get handler");

    /* Send a simple response. Mainly used for testing */ 
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len)
{
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index) {
        j->len = 0;
    }

    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK) {
        return 0;
    }

    j->len += len;
    return len;
}

esp_err_t picture_handler(httpd_req_t *req){
    esp_err_t res = ESP_OK;

    /* Refresh the image and take a new picture */
    fb = fb_refresh(fb);
    fb = sys_take_picture();
    
    /* Check if the frame buffer is ok to send */ 
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG,"Took new picture.");

    /* Output image information to the terminal */
    pic_data_output(fb);

    ESP_LOGI(TAG,"Picture received. Trying to send now.");

    /* Set the content type header to a JPEG image */
    res = httpd_resp_set_type(req, "image/jpeg");
    if (res == ESP_OK) {
        res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    }

    /* Check if the response is as expected and if yes then try to send the image */
    if (res == ESP_OK) {
        /* If the image is JPEG it gets sent as one batch of data */
        if (fb->format == PIXFORMAT_JPEG) {
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
            ESP_LOGI(TAG, "Response Sent");
        } else {
            /* If the image is anything but JPEG, it gets converted to JPEG and sent in chunks */
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            ESP_LOGI(TAG, "Response Sent in chunks");
        }
    }
    /* To switch cameras */
    mcc_config.pic_done = true; 
    return res;
}

esp_err_t config_settings_post_handler(httpd_req_t *req)
{
    /** 
    * Buffer for recieving content/params, content is expected to be a JSON string
    * such as {saturation: 2, contrast: 0, brightness: 1, ...} 
    */
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
	

    JSON_config_set(content);
    mcc_config.sensor = esp_camera_sensor_get();
    camera_set_settings(mcc_config);

    /* Send a response to */
    const char resp[] = "Paramaters set";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;

    /* Sample cURL command is in http.h */
}

esp_err_t exit_handler(httpd_req_t *req){
    mcc_config.exit = 1;
    /* Send a response to */
    const char resp[] = "Exited safely";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
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

/* POST Handlers */
httpd_uri_t config_settings_post = {
    .uri      = "/config",
    .method   = HTTP_POST,
    .handler  = config_settings_post_handler,
    .user_ctx = NULL
};

httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 19520;

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        
        /* GET */
        httpd_register_uri_handler(server, &uri_get);   // simple handler for testing
        httpd_register_uri_handler(server, &pic_get);   // to get the image
        httpd_register_uri_handler(server, &exit_get);   // to get the image
        
        /* POST */
        httpd_register_uri_handler(server, &config_settings_post); // change camera settings
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}
