// from esp32-camera example
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"

// from SD card example
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#include "esp_http_server.h"

// Camera definitions for ESP32-CAM
#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
#define CAM_PIN_FLASH 4

// Folder to mount at
#define MOUNT_POINT "/sdcard"

// SD card pins used in SPI communication
//based on sdspi example
#define SD_DATA0 2
#define SD_DATA3 13
#define SD_CLK 14
#define SD_CMD 15

// defined to be used as SPI pins
#define PIN_NUM_MISO SD_DATA0
#define PIN_NUM_CS SD_DATA3
#define PIN_NUM_CLK SD_CLK
#define PIN_NUM_MOSI SD_CMD 

static const char *TAG = "WULPSC";

// declare the camera configuration
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_VGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 2, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
    .fb_location = CAMERA_FB_IN_DRAM,
};

static esp_err_t init_camera(void)
{
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

static esp_err_t init_sd(void){
    
    return ESP_OK;
}

static esp_err_t write_arr(const char *path, camera_fb_t *fb){
    size_t ret;
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    ret = fwrite(fb->buf, sizeof(char), fb->len, f);
    ESP_LOGI(TAG,"wrote %zu elements out of %d requested", ret, fb->len);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    return ESP_OK;
}

void setup_flash(){
    gpio_set_pull_mode(CAM_PIN_FLASH,GPIO_PULLDOWN_ONLY);
    gpio_set_direction(CAM_PIN_FLASH,GPIO_MODE_OUTPUT);
    gpio_set_level(CAM_PIN_FLASH,1);
}

void turn_off_flash(){
    gpio_set_level(CAM_PIN_FLASH,0);
}

void app_main(void)
{   
    
    // if camera is not initialised, return
    if(ESP_OK != init_camera()) {
        return;
    }

    
    // camera settings
    //sensor_t *s = esp_camera_sensor_get();
    // s->set_gain_ctrl(s, 1);                       // auto gain on
    // s->set_exposure_ctrl(s, 1);                   // auto exposure on
    // s->set_awb_gain(s, 1);                        // Auto White Balance enable (0 or 1)
    // s->set_brightness(s, 1);                      // (-2 to 2) - set brightness
    // s->set_aec2(s,1);                             // auto exposure control on
    //s->reset(s);

    setup_flash();

    ESP_LOGI(TAG, "Taking picture...");
    camera_fb_t *fb = esp_camera_fb_get();

    turn_off_flash();
    vTaskDelay(10 / portTICK_PERIOD_MS); //10 millisecond delay for WDT and flash

    ESP_LOGI(TAG, "-----------------------");
    ESP_LOGI(TAG, "Picture taken!");
    ESP_LOGI(TAG, "Size: %zu bytes", fb->len);
    ESP_LOGI(TAG, "Height: %zu", fb->height);
    ESP_LOGI(TAG, "Width: %zu", fb->width);
    ESP_LOGI(TAG, "Format: %d", fb->format); // see sensor.h for format
    ESP_LOGI(TAG, "-----------------------"); 

    // variable for function returns
    esp_err_t ret;

    // initialise sd card
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    // Default structure initializer for SD over SPI driver
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    // initialise spi bus config
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1, // change these for more data pins?
        .quadhd_io_num = -1,
        .max_transfer_sz = 4092,
    };

    // initialise and check if succesful
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // initializes the slot without card detect (CD) and write protect (WP) signals...what does that mean?
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT(); // uses GPIO13 for CS, how does it know?
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Use POSIX and C standard library functions to work with files.

    //inspect the first 6 values of buf
    ESP_LOGI(TAG,"Buffer header details: %d ,%d, %d, %d ", fb->buf[0], fb->buf[1], fb->buf[2], fb->buf[3]);

    // First create a file.
    const char *file = MOUNT_POINT"/img.jpg";
    ESP_LOGI(TAG,"File created");


    ret = write_arr(file, fb);


    // error check if file was not written correctly
    if (ret != ESP_OK) {
        return;
    }

    // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    // return frame buffer
    esp_camera_fb_return(fb);
    ESP_LOGI(TAG, "Frame buffer returned");


    //deinitialize the bus after all devices are removed
    spi_bus_free(host.slot);

    ESP_LOGI(TAG,"DONE!!!!!!!!!!!");
    return;
}