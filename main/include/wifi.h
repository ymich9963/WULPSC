#ifndef WIFI_H
#define WIFI_H

// can change from menuconfig
// #define ESP_WIFI_SSID      "EE-Hub-c5Jc"
// #define ESP_WIFI_PASS      "clock-lob-CLAD"
#define ESP_WIFI_SSID      "plus"
#define ESP_WIFI_PASS      "abcde123"

#define ESP_MAXIMUM_RETRY  5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define DEFAULT_SCAN_LIST_SIZE 5


/**
 * @brief General required calls to initialise WiFi functionality
*/
void wifi_init_general();


/**
 * @brief Print the WiFi Authorisation Mode
*/
void print_auth_mode(int authmode);

/**
 * @brief Used to set the CONNECTED or FAIL bits for the event
*/
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Initialise Station Mode
*/
void wifi_init_sta(void);


/**
 * @brief Scan for nearby networks
*/
void wifi_scan(void);


#endif