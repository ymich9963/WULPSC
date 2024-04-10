#include "wifi.h"

static const char *TAG = "WULSC - Wi-Fi";

/* Variable to store amount of retries */
static int s_retry_num = 0;

/* FreeRTOS event group to signal when connected */
static EventGroupHandle_t s_wifi_event_group;
static esp_netif_t *sta_netif;
static wifi_init_config_t cfg;

/* Variables for the string credentials defined in sd.c */
extern char sd_ssid[MAX_CHAR_LINE];
extern char sd_pswd[MAX_CHAR_LINE];

void wifi_init_general() 
{
    /* Standard initialisations to enable WiFi on an ESP32 */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    cfg = (wifi_init_config_t) WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    /* Check for each WiFi event and act accordingly */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        /* Retry as many times as defined by the directive */
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_init_sta(void)
{
    esp_err_t ret;

    /* Initialise events for STA mode */
    s_wifi_event_group = xEventGroupCreate();
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

/* Directive set to use credentials from the SD card. */
#if SD_CRED
    wifi_config_t wifi_config = {
        .sta = {
            .ssid       = "",       // Initialise the values with empty string due to a bug 
            .password   = ""
        }
    };

    /* Copy the strings from the text file to the config */
    strcpy((char*)wifi_config.sta.ssid, sd_ssid);
    strcpy((char*)wifi_config.sta.password, sd_pswd);

/* If set to 0 use the directives. Mainly used for testing. */
#else
    wifi_config_t wifi_config = {
        .sta = {
            .ssid       = ESP_WIFI_SSID, // this works due to initialisation and not value assignment
            .password   = ESP_WIFI_PASS
        }
    };
#endif


    ESP_LOGI(TAG, "Detected SSID: %s", wifi_config.sta.ssid);
    ESP_LOGI(TAG, "Detected Password: %s", wifi_config.sta.password);

    /* Set credentials and try to connect. It also tries to connect in the event_handler() */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ret = esp_wifi_connect();

    /* Error checking */
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error with Wi-Fi connection");
        return ESP_FAIL;
    }

    /**
     * Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) 
    */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);


    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to Access Point SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGW(TAG, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        return ESP_FAIL;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        return ESP_FAIL;
    }
}
