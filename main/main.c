#include <esp_log.h>
#include <string.h>

#include "camera.h"
#include "sd.h"
#include "wifi.h"
#include "http.h"
#include "system_config.h"

static const char *TAG = "WULPSC";

camera_fb_t* fb = NULL;
system_config_t sys_config = {
    .done =         0,
    .flash =        0,
    .sd_save =      0,
    .pic_taken =    0,
    .cam_switched = 0,
    .camera = {
        .brightness     = 0,    // from -2 to 2            
        .contrast       = 0,    // from -2 to 2
        .saturation     = 0,    // from -2 to 2
        .sharpness      = 0,    // from -2 to 2, NOT SUPPORTED
        .denoise        = 0,    // from 0 to 255, NOT SUPPORTED
        .special_effect = 0,    // from 0 to 6 
        .wb_mode        = 3,    // from 0 to 4
        .ae_level       = 0,    // from -2 to 2
        .aec_value      = 100,  // from 0 to 1200
        .agc_gain       = 3,    // from 0 to 30
        .gainceiling    = 1,    // from 0 to 6
        .lenc           = true,
        .agc            = false,
        .aec            = false,
        .hmirror        = false,
        .vflip          = false,
        .aec2           = false, 
        .bpc            = true,
        .wpc            = true
    }
};

// system_config_t* _sys_config = &sys_config;

void app_main(void)
{   
    esp_err_t ret; // variable for function returns

    httpd_handle_t server;

    // initialise non volatile storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if(ret != ESP_OK){
        return;
    }

    wifi_init_general();
    // wifi_scan();
    wifi_init_sta();    

    ret = init_camera();
    if(ret != ESP_OK){ 
        return; // if camera is not initialised, return 
    }
    
    if (sys_config.flash){
        setup_flash();
        turn_on_flash();
    }


    // set sensor, and set to default values
    sys_config.sensor = esp_camera_sensor_get();
    camera_set_settings(sys_config);  

    fb = fb_refresh(fb);

    ESP_LOGI(TAG, "Taking picture...");
    fb = esp_camera_fb_get();

    camera_get_settings(sys_config);

    vTaskDelay(10 / portTICK_PERIOD_MS); // 10 ms delay for WDT and flash

    if(sys_config.flash){
        turn_off_flash();
    }

    pic_data_output(fb); // for logging

    if(sys_config.sd_save){
        ret = sd_init(fb);
    }

    if(ret != ESP_OK){
        ESP_LOGE(TAG,"Error with SD init");
        return;
    }

    server = start_webserver();
    if(server == NULL){
        ESP_LOGE(TAG,"Error Server is NULL");
        return;
    }

    // while(!server_ack){
        
    // }

    while(server){
        // ESP_LOGI(TAG, "Entered server loop");
        vTaskDelay(10/portTICK_PERIOD_MS);
        if(sys_config.done){
            ESP_LOGI(TAG, "Server Stopping");
            stop_webserver(server);
            server = NULL;
        }
    }

    // return frame buffer
    esp_camera_fb_return(fb);
    ESP_LOGI(TAG, "Frame buffer returned before exit");

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}
