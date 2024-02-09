#include <esp_log.h>
#include <string.h>

#include "camera.h"
#include "sd.h"
#include "wifi.h"
#include "http.h"
#include "config.h"

static const char *TAG = "WULPSC";

camera_fb_t * fb = NULL;
system_config_t sys_config = {
    .done = 0,
    .flash = 0,
    .sd_save = 0,
};

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
    wifi_scan();
    wifi_init_sta();    

    ret = init_camera();
    if(ret != ESP_OK){ 
        return; // if camera is not initialised, return 
    }
    
    // camera settings
    sys_config.sensor = esp_camera_sensor_get();

    // ret = sys_config.sensor->reset(sys_config.sensor);
    // if(ret != ESP_OK){ 
    //     return; 
    // }

    // sys_config.sensor->set_saturation(sys_config.sensor,0);
    // camera_set_settings(sys_config.camera, sys_config);

    if (sys_config.flash){
        setup_flash();
        turn_on_flash();
    }

    ESP_LOGI(TAG, "Taking picture...");
    fb = esp_camera_fb_get();
    vTaskDelay(10 / portTICK_PERIOD_MS); // 10 millisecond delay for WDT and flash


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

    while(server){
        vTaskDelay(100/portTICK_PERIOD_MS);
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