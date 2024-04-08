#include <esp_log.h>
#include <string.h>

#include "camera.h"
#include "sd.h"
#include "wifi.h"
#include "http.h"
#include "mcc_config.h"

static const char *TAG = "WULPSC";

camera_fb_t* fb = NULL;
mcc_config_t mcc_config = {
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

    /**
     * Setup the GPIO pin to change the value of the SEL pin of the MUXs, initialised to 0
     * */
    gpio_set_direction(SEL_PIN, GPIO_MODE_OUTPUT);
    mcc_config.active_cam = 0;
    gpio_set_level(SEL_PIN, 0);    
    vTaskDelay(10/portTICK_PERIOD_MS);

    /* Initialise the camera */
    ret = init_camera();
    if(ret != ESP_OK){
        // turn_on_flash(); // to signify error with cameras
        return;
    }
    
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
    // sys_sd_save_check(&mcc_config, fb);
    sd_deinit();

    /* Initialise WiFi */
    wifi_init_general();
    wifi_ret = wifi_init_sta();    


    /* Only create a HTTP server if a WiFi connection was established */
    if(wifi_ret == ESP_OK){

        server = start_webserver();
        if(server == NULL){
            ESP_LOGE(TAG,"Error Server is NULL");
            return;
        } else {
            ESP_LOGI(TAG, "Started server...");
        }

        while(server){
            // ESP_LOGI(TAG, "Entered server loop");
            vTaskDelay(10/portTICK_PERIOD_MS);
            if(mcc_config.exit){
                ESP_LOGI(TAG, "Server Stopping");
                stop_webserver(server);
                server = NULL;
            }
        }

        // de-initialise 
        ret = esp_camera_deinit();
        if(ret != ESP_OK){
            ESP_LOGW(TAG, "De-init returned badly");
        } else {
            ESP_LOGW(TAG, "De-init OK");
        }

        // power down?
        gpio_set_level(CAM_PIN_PWDN, 0);
        if(ret != ESP_OK){
            ESP_LOGW(TAG, "GPIO set level returned badly");
        } else {
            ESP_LOGW(TAG, "GPIO set level OK. PWDN is 0");
        }

        mcc_config.active_cam = 1;
        mcc_config.exit = 0;
        gpio_set_level(SEL_PIN, 1);
        vTaskDelay(1000/portTICK_PERIOD_MS);

        // power up!
        gpio_set_level(CAM_PIN_PWDN, 1);
        if(ret != ESP_OK){
            ESP_LOGW(TAG, "GPIO set level returned badly");
        } else {
            ESP_LOGW(TAG, "GPIO set level OK. PSWDN is 1.");
        }

        // initialise
        ret = init_camera();
        if(ret != ESP_OK){
            ESP_LOGW(TAG, "Camera init returned badly");
        } else {
            ESP_LOGW(TAG, "Camera init OK");
        }

        server = start_webserver();
        if(server == NULL){
            ESP_LOGE(TAG,"Error Server is NULL");
            return;
        } else {
            ESP_LOGI(TAG, "Started server...");
        }
        
        while(server){
            // ESP_LOGI(TAG, "Entered server loop");
            vTaskDelay(10/portTICK_PERIOD_MS);
            if(mcc_config.exit){
                ESP_LOGI(TAG, "Server Stopping");
                stop_webserver(server);
                server = NULL;
            }
        }
        
        if(mcc_config.sd_save == SD_SAVE){
            // sys_sd_save_check(&mcc_config, fb);
        }
    } else {
        /**
         * Save only one image if no WiFi so that some monitoring can still occur,
         * SD card will always be required due to the WiFi credentials
         * If SD saving is not enabled, enable it in order to allow for partial monitoring,
         * If SD saving is enabled, then image was saved from before.
         * */ 
        if(mcc_config.sd_save == NO_SD){

            // Setup SD saving
            mcc_config.sd_save = SD_SAVE;
            sys_sd_save_check(&mcc_config, fb);

            // Call the function again to save the image
            sys_sd_save_check(&mcc_config, fb);
        }
    }

    // Return frame buffer
    esp_camera_fb_return(fb);
    esp_camera_deinit();
    ESP_LOGI(TAG, "Returned frame buffer and de-initialised camera before exit.");

    if(mcc_config.sd_save == SD_SAVING){
        sd_deinit();
    }

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}
