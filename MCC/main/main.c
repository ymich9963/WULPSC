#include "camera.h"
#include "sd.h"
#include "wifi.h"
#include "http.h"
#include "mcc_config.h"

/* TAG to be used when outputting logs to terminal */
static const char *TAG = "WULPSC";

/* Empty camera buffer to be used throughout the project */
camera_fb_t* fb = NULL;

/* System configuration default settings. The values set here are for normal room light level. */
mcc_config_t mcc_config = {
    .exit           =   false,
    .flash          =   false,
    .sd_save        =   NO_SD_SAVE,
    .pic_done       =   false,
    .sel_status     =   0,
    .camera = {
        .brightness     = 0,    // from -2 to 2            
        .contrast       = 0,    // from -2 to 2
        .saturation     = 0,    // from -2 to 2
        .sharpness      = 0,    // from -2 to 2, NOT SUPPORTED
        .denoise        = 0,    // from 0 to 255, NOT SUPPORTED
        .special_effect = 0,    // from 0 to 6 
        .wb_mode        = 3,    // from 0 to 4
        .ae_level       = 0,    // from -2 to 2
        .aec_value      = 20,   // from 0 to 1200
        .agc_gain       = 2,    // from 0 to 30
        .gainceiling    = 1,    // from 0 to 6
        .lenc           = true,
        .agc            = false,
        .aec            = false,
        .hmirror        = false,
        .vflip          = false,
        .aec2           = true, 
        .bpc            = true,
        .wpc            = true
    }
};

void app_main(void)
{   
    esp_err_t ret; // variable for function returns
    esp_err_t wifi_ret;
    httpd_handle_t server;

    /* Initialise the camera */
    ret = init_camera();
    if(ret != ESP_OK) return;

    /* Initialise NVS for WiFi*/
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init()); 
    }

    /**
     * Setup the GPIO pin to change the value of the SEL pin of the MUXs, initialised to 0
     * */
    gpio_set_direction(SEL_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(SEL_PIN, 0);    
    vTaskDelay(10/portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "SEL pin set to %d", SEL_PIN);

/* Set SD_CRED definition to 0 to use the hardcoded credentials in wifi.h */
#if SD_CRED
    /* Initialise SD card */
    sd_init();

    /* Read the WiFi credentials from the SD card */
    read_wifi_credentials();

    if (mcc_config.sd_save == NO_SD_SAVE) {
        sd_deinit();
    }
#endif

    /* Initialise WiFi */
    wifi_init_general();
    wifi_ret = wifi_init_sta();    

    /* Only create a HTTP server if a WiFi connection was established */
    if (wifi_ret == ESP_OK) {
        server = start_webserver();
        if (server == NULL) {
            ESP_LOGE(TAG,"Error Server is NULL");
            return;
        } else {
            ESP_LOGI(TAG, "Started server...");
        }

        while(server) {
            vTaskDelay(10/portTICK_PERIOD_MS);
            if (mcc_config.exit) {
                ESP_LOGI(TAG, "Server Stopping");
                stop_webserver(server);
                server = NULL;
            }
            if (mcc_config.sd_save == SD_SETUP) {
                sd_init();
                sys_sd_var_setup();
                mcc_config.sd_save = SD_SAVE;
            } 
            if (mcc_config.pic_done) {
                if (mcc_config.sd_save == SD_SAVE) {
                    sys_sd_save(fb);
                }   

                /* Return frame buffer and make sure it is empty */
                esp_camera_fb_return(fb);
                fb = NULL;
                sys_camera_switch();
                
                /* Reset for next picture */
                mcc_config.pic_done = false;
                ESP_LOGI(TAG, "SWITCHED");
            }
        }
    } else {
        /**
         * Save only one image if no WiFi so that some monitoring can still occur,
         * SD card will always be required due to the WiFi credentials
         * If SD saving is not enabled, enable it in order to allow for partial monitoring
         * */ 

        fb = fb_refresh(fb);
        fb = sys_take_picture();

        if(!mcc_config.sd_save){
            sys_sd_var_setup();
        }
        sys_sd_save(fb);
    }

    /* Return frame buffer */
    esp_camera_fb_return(fb);
    esp_camera_deinit();
    ESP_LOGI(TAG, "Returned frame buffer and de-initialised camera before exit.");

    /* Check if SD is initialised to de-initialise it*/
    if(mcc_config.sd_save == SD_SAVE){
        sd_deinit();
    }

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}
