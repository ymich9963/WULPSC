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
    .exit =         false,
    .flash =        false,
    .sd_save =      SD_OK,
    .pic_poll =     false,
    .cam_switched = false,
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

void app_main(void)
{   
    esp_err_t ret; // variable for function returns

    httpd_handle_t server;

    // initialise non volatile storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init()); 
    }

    // Initialise SD card
    sd_init();

    // Read the WiFi credentials from the SD card
    read_wifi_credentials();
    
    // Checks if SD should be de-initialised
    sys_sd_save_check(sys_config, fb);

    /* Initialise WiFi */
    wifi_init_general();
    wifi_init_sta();    

    // Setup the flash LED
    setup_flash();

    /* Initialise the camera */
    init_camera();
    
    /* Set sensor, and set to default values */
    sys_config.sensor = esp_camera_sensor_get();
    camera_set_settings(sys_config);  

    /* Refresh picture to make sure the latest image is received */
    fb = fb_refresh(fb);

    ESP_LOGI(TAG, "Taking picture...");
    fb = sys_take_picture(sys_config); 
    if(fb){
        ESP_LOGI(TAG, "Picture taken!");
    }else{
        ESP_LOGW(TAG, "Picture not taken!!!");
    }

    // Check if SD should be saved
    sys_sd_save_check(sys_config, fb);

    /* Display settings to console for debugging */
    // camera_get_settings(sys_config);

    /* Display picture data for debugging */
    pic_data_output(fb);

    server = start_webserver();
    if(server == NULL){
        ESP_LOGE(TAG,"Error Server is NULL");
        return;
    } {
        ESP_LOGI(TAG, "Started server...");
    }

    // while(!server_ack){
        
    // }

    while(server){
        // ESP_LOGI(TAG, "Entered server loop");
        vTaskDelay(10/portTICK_PERIOD_MS);
        if(sys_config.exit){
            ESP_LOGI(TAG, "Server Stopping");
            stop_webserver(server);
            server = NULL;
        }
    }

    // Return frame buffer
    esp_camera_fb_return(fb);
    sd_deinit();
    ESP_LOGI(TAG, "Frame buffer returned before exit");

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}
