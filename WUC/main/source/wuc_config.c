#include "wuc_config.h"

static const char *TAG = "WUC - System Config";
static RTC_DATA_ATTR struct timeval sleep_enter_time;

extern wuc_config_t wuc_config;

esp_err_t setup_mcc_power_switch(){
    return gpio_set_direction(MCC_PIN, GPIO_MODE_OUTPUT);
}

esp_err_t mcc_powerup(){
    ESP_LOGI(TAG, "Powering up MCC...");
    return gpio_set_level(MCC_PIN, 1);        
}

esp_err_t mcc_powerdown(){
    ESP_LOGI(TAG, "Powering down MCC...");
    return gpio_set_level(MCC_PIN, 0);        
}

esp_err_t deep_sleep_setup(){
    /* Register RTC timer */
    ESP_LOGI(TAG,"Enabling timer wakeup, %ds", wuc_config.sleep_time_sec);

    /* Enable RTC timer wakeup */    
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wuc_config.sleep_time_sec / 1000000));

    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TIMER: {
            ESP_LOGI(TAG,"Wake up from timer. Time spent in deep sleep: %dms", sleep_time_ms);
            break;
        }
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            ESP_LOGI(TAG,"Not a deep sleep reset");
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);

    // Isolate GPIO12 pin from external circuits. This is needed for modules
    // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
    // to minimize current consumption.
    rtc_gpio_isolate(GPIO_NUM_12);

    // get deep sleep enter time
    gettimeofday(&sleep_enter_time, NULL);

    return ESP_OK;
}