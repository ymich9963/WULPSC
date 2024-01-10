#include <esp_log.h>
#include <string.h>

#include "camera.h"
#include "sd.h"

static const char *TAG = "WULPSC";

void app_main(void)
{   
    esp_err_t ret; // variable for function returns

    setup_flash();

    ret = init_camera();

    if(ret != ESP_OK) { // if camera is not initialised, return
        return;
    }
    
    // camera settings
    sensor_t *s = esp_camera_sensor_get();
    // s->set_gain_ctrl(s, 1);                       // auto gain on
    // s->set_exposure_ctrl(s, 1);                   // auto exposure on
    // s->set_awb_gain(s, 1);                        // Auto White Balance enable (0 or 1)
    // s->set_brightness(s, 1);                      // (-2 to 2) - set brightness
    // s->set_aec2(s,1);                             // auto exposure control on
    // s->set_exposure_ctrl(s,0);
    // s->set_aec2(s,0);
    // s->set_aec_value(s,1000);
    //s->reset(s);    

    ESP_LOGI(TAG, "Taking picture...");
    camera_fb_t *fb = esp_camera_fb_get();

    turn_on_flash();
    vTaskDelay(10 / portTICK_PERIOD_MS); //10 millisecond delay for WDT and flash
    turn_off_flash();
    pic_data_output(fb);

    ret = sd_init(fb);

    if(ret != ESP_OK) {
        return;
    }

    // return frame buffer
    esp_camera_fb_return(fb);
    ESP_LOGI(TAG, "Frame buffer returned");

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}