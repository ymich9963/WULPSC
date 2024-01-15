#ifndef WIFI_H
#define WIFI_H

// can change from menuconfig
#define ESP_WIFI_SSID      "SSID"
#define ESP_WIFI_PASS      "PASSWORD"

#define ESP_MAXIMUM_RETRY  5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define DEFAULT_SCAN_LIST_SIZE 5



void wifi_init_general();



void print_auth_mode(int authmode);


void wifi_init_sta(void);



void wifi_scan(void);


#endif