// file contains all camera related function definitions and default camera configuration

#include "camera.h"

static const char *TAG = "WULPSC - Camera";

// declare the camera configuration
camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_YUV422 , //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_VGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 4,    //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_LATEST, // grab latest to get current pictures
    .fb_location = CAMERA_FB_IN_PSRAM,
};

esp_err_t init_camera(){
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }
    return ESP_OK;
}

esp_err_t setup_flash(){
    gpio_set_level(CAM_PIN_FLASH,0); //initialise to 0 so to not blink twice
    return gpio_set_direction(CAM_PIN_FLASH,GPIO_MODE_OUTPUT);;
}

esp_err_t turn_on_flash(){
    return gpio_set_level(CAM_PIN_FLASH,1); // returns ESP_OK if succesful
}

esp_err_t turn_off_flash(){
    return gpio_set_level(CAM_PIN_FLASH,0);
}

void pic_data_output(camera_fb_t *fb){
    ESP_LOGI(TAG, "-----------------------");
    ESP_LOGI(TAG, "Picture taken!");
    ESP_LOGI(TAG, "Size: %zu bytes", fb->len);
    ESP_LOGI(TAG, "Height: %zu", fb->height);
    ESP_LOGI(TAG, "Width: %zu", fb->width);
    ESP_LOGI(TAG, "Using JPEG Quality: %d", camera_config.jpeg_quality);
    ESP_LOGI(TAG, "Format: %d", fb->format); // see sensor.h for format
    ESP_LOGI(TAG, "-----------------------"); 
}

esp_err_t camera_settings(int enable){
    sensor_t * sensor = esp_camera_sensor_get();

    // sensor->reset;
    // sensor->init_status(sensor);
    // sensor->set_pixformat(sensor,);
    // sensor->set_framesize(sensor,);
    // sensor->set_contrast(sensor,);
    // sensor->set_brightness(sensor,);
    // sensor->set_saturation(sensor,);

    // sensor->set_quality(sensor,);
    // sensor->set_colorbar(sensor,);

    // sensor->set_gainceiling(sensor,);
    // sensor->set_gain_ctrl(sensor,);
    // sensor->set_exposure_ctrl(sensor,);
    // sensor->set_hmirror(sensor,);
    sensor->set_vflip(sensor, enable);

    // sensor->set_whitebal(sensor,);
    // sensor->set_aec2(sensor,);
    // sensor->set_aec_value(sensor,);
    // sensor->set_special_effect(sensor,);
    // sensor->set_wb_mode(sensor,);
    // sensor->set_ae_level(sensor,);

    // sensor->set_dcw(sensor,);
    // sensor->set_bpc(sensor,);
    // sensor->set_wpc(sensor,);
    // sensor->set_awb_gain(sensor,);
    // sensor->set_agc_gain(sensor,);

    // sensor->set_raw_gma(sensor,);
    // sensor->set_lenc(sensor,);

    // nnot supported
    // sensor->set_sharpness = set_sharpness;
    // sensor->set_denoise = set_denoise;

    // what do these do?
    // sensor->get_reg(sensor,);
    // sensor->set_reg(sensor,);
    // sensor->set_res_raw(sensor,);
    // sensor->set_pll(sensor,);
    // sensor->set_xclk(sensor,);

    return ESP_OK;
}