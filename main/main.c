#include <esp_log.h>
#include <string.h>
#include <nvs_flash.h>
#include "camera.h"
#include "wifi.h"
#include "esp_timer.h"
#include "http.h"

static const char *TAG = "WULPSC - Stereo Test";
camera_fb_t * fb = NULL;
bool pic = false;
bool cam_switched = false;

void app_main(void)
{   
    // Variable for function returns
    esp_err_t ret;

    httpd_handle_t server;

    //// Wi-Fi stuff
    //initialise the non volatile storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if(ret != ESP_OK) 
    {
        return;
    }

    wifi_init_general();
    wifi_scan();
    wifi_init_sta();

    server = start_webserver();

    if(server == NULL)
    {
        ESP_LOGE(TAG,"Error Server is NULL");
        return;
    }

    ret = init_camera();
    if(ret != ESP_OK) // if camera is not initialised, return 
    { 
        return;
    }

    ESP_LOGI(TAG, "Taking picture...");
    fb = esp_camera_fb_get();
    pic_data_output(fb);
    vTaskDelay(10 / portTICK_PERIOD_MS); // 10 ms delay for WDT and flash

    while(server)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
        //ESP_LOGI(TAG,"In the loop");
    }

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}