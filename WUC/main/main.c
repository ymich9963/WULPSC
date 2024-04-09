#include "wifi.h"
#include "http.h"
#include "wuc_nvs.h"
#include "wuc_config.h"

static const char *TAG = "WULPSC - Wake Up Module";

wuc_config_t wuc_config = {
    .exit               = false,
    .sleep_time_sec     = 30,
    .active_time_sec    = 60,
    .ssid               = "",
    .pswd               = "",
    .stored_creds       = false,
};

void app_main(void)
{   
    ESP_LOGI(TAG, "------WUC Booted------");

    /* Variable for function returns */
    esp_err_t ret;
    esp_err_t wifi_ret;

    /* HTTP Server handle */
    httpd_handle_t server;

    /* Setup deep sleep */
    deep_sleep_setup();

    /* Initialise the non volatile storage */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        if(ret != ESP_OK) {
            ESP_LOGE(TAG, "Error with NVS initialisation");
            return;
        }
    }

    /* Check the NVS for credentials */
    check_nvs();

/* Used for testing the WUC, set the credentials in wifi.h*/
#if TEST_CRED
    memcpy(wuc_config.ssid, ESP_WIFI_SSID, sizeof(ESP_WIFI_SSID));
    memcpy(wuc_config.pswd, ESP_WIFI_PASS, sizeof(ESP_WIFI_PASS));
    wuc_config.stored_creds = true;
#endif

    
    /**
     *  If stored credentials are detected, initialise the WiFi with them,
     * else start smart config to get the credentials
     * */
    if(wuc_config.stored_creds){
        wifi_ret = wifi_init();
    } else {
        wifi_ret = wifi_init_sc();
        
        /* Poll for smart config to finish */
        while(strcmp(wuc_config.ssid, "") == 0 && strcmp(wuc_config.pswd, "") == 0){vTaskDelay(10/ portTICK_PERIOD_MS);}

        /* Store the new credentials */
        store_creds_to_nvs();
    }

    /* Setup GPIO pin for MMC power-up */
    setup_mcc_power_switch();

    /* If WiFi initialisation and connection was succesful */
    if(wifi_ret == ESP_OK){ 
        /* Power-up MCC */
        mcc_powerup();
        
        /* Start the HTTP server */
        server = start_webserver();
        if(server == NULL){
            ESP_LOGE(TAG,"Error Server is NULL");
            return;
        }

        ESP_LOGI(TAG, "Entering webserver");
        while(server){
            vTaskDelay(10/portTICK_PERIOD_MS);
            if(wuc_config.exit){
                ESP_LOGI(TAG, "Server Stopping");
                stop_webserver(server);
                server = NULL;
            }
        }
    } else {
        /* If no WiFi connection, still allow the MCC to power up for partial monitoring */
        ESP_LOGW(TAG, "WiFi not succesfully connected, turn on MCC based on the active time...");
        mcc_powerup();
        vTaskDelay(wuc_config.active_time_sec*1000/portTICK_PERIOD_MS);
    }
    
    /* Power down the MCC */
    mcc_powerdown();
     
    /* Enter deep sleep */   
    ESP_LOGI(TAG,"Entering deep sleep\n");
    esp_deep_sleep_start();

    ESP_LOGI(TAG, "Ended app_main()");
    return;
}