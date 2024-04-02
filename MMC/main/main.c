#include <esp_log.h>
#include <string.h>

#include "camera.h"
#include "sd.h"
#include "wifi.h"
#include "http.h"
#include "mmc_config.h"

static const char *TAG = "WULPSC";

camera_fb_t* fb = NULL;
mmc_config_t mmc_config = {
    .exit           =   false,
    .flash          =   false,
    .sd_save        =   NO_SD,
    .pic_poll       =   false,
    .active_cam     =   0,
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
    esp_err_t wifi_ret;

    httpd_handle_t server;

    // initialise non volatile storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init()); 
    }

    // Initialise SD card
    sd_init();

#if SD_CRED
    // Read the WiFi credentials from the SD card
    read_wifi_credentials();
#endif
    // Checks if SD should be de-initialised
    sys_sd_save_check(&mmc_config, fb);

    /* Initialise WiFi */
    wifi_init_general();
    wifi_ret = wifi_init_sta();    

    /**
     * Setup the GPIO pin to change the value of the SEL pin of the MUXs, initialised to 0
     * */
    gpio_set_direction(SEL_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(SEL_PIN, 0);    
    
    // Setup the flash LED
    setup_flash();

    /* Initialise the camera */
    ret = init_camera();
    if(ret != ESP_OK){
        turn_on_flash(); // to signify error with cameras
        return;
    }
    
    /* Set sensor, and set to default values */
    mmc_config.sensor = esp_camera_sensor_get();
    camera_set_settings(mmc_config);  



    /* Refresh picture to make sure the latest image is received */
    fb = fb_refresh(fb);

    ESP_LOGI(TAG, "Taking picture...");
    fb = sys_take_picture(mmc_config); 
    if(fb){
        ESP_LOGI(TAG, "Picture taken!");
    }else{
        ESP_LOGW(TAG, "Picture NOT taken...");
    }

    /* Display settings to console for debugging */
    // camera_get_settings(mmc_config);

    /* Display picture data for debugging */
    pic_data_output(fb);

    // Check if image should be saved
    sys_sd_save_check(&mmc_config, fb);
        
    /* Only create a HTTP server if a WiFi connection was established */
    if(wifi_ret == ESP_OK){
        server = start_webserver();
        if(server == NULL){
            ESP_LOGE(TAG,"Error Server is NULL");
            return;
        } {
            ESP_LOGI(TAG, "Started server...");
        }
        while(server){
            // ESP_LOGI(TAG, "Entered server loop");
            vTaskDelay(10/portTICK_PERIOD_MS);
            if(mmc_config.exit){
                ESP_LOGI(TAG, "Server Stopping");
                stop_webserver(server);
                server = NULL;
            }
        }
    } else {
        /**
         * Save only one image if no WiFi so that some monitoring can still occur,
         * SD card will always be required due to the WiFi credentials
         * If SD saving is not enabled, enable it in order to allow for partial monitoring,
         * If SD saving is enabled, then image was saved from before.
         * */ 
        if(mmc_config.sd_save == NO_SD){

            // Setup SD saving
            mmc_config.sd_save = SD_SAVE;
            sys_sd_save_check(&mmc_config, fb);

            // Call the function again to save the image
            sys_sd_save_check(&mmc_config, fb);

            // // Set the active camera and switch
            // mmc_config.active_cam = 0;    
            // ret = sys_camera_switch(mmc_config);
            // if(ret != ESP_OK){
            //     ESP_LOGE(TAG, "Camera switch returned badly");
            // }

            // /* Get sensor data again and set the settings */
            // mmc_config.sensor = esp_camera_sensor_get();
            // camera_set_settings(mmc_config);  

            // /* Refresh picture to make sure the latest image is received */
            // fb = fb_refresh(fb);

            // /* Take a new image */
            // ESP_LOGI(TAG, "Taking picture...");
            // fb = sys_take_picture(mmc_config); 
            // if(fb){
            //     ESP_LOGI(TAG, "Picture taken!");
            // }else{
            //     ESP_LOGW(TAG, "Picture NOT taken...");
            // }

            // /* Save again */
            // sys_sd_save_check(&mmc_config, fb);
        }
    }

    // Return frame buffer
    esp_camera_fb_return(fb);
    esp_camera_deinit();
    ESP_LOGI(TAG, "Returned frame buffer and de-initialised camera before exit.");

    if(mmc_config.sd_save == SD_SAVING){
        sd_deinit();
    }

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}
