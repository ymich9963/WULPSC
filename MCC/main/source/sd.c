#include "sd.h"

const static char* TAG = "WULPSC - SD";

/* Default structure initializer for SD over SPI driver */
sdmmc_host_t host = SDSPI_HOST_DEFAULT();

/* Variable to store SD card information */
sdmmc_card_t *card;

/* Directory to mount to */
const char mount_point[] = MOUNT_POINT;

/* Initialise SSID and Password variables of size defined by MAX_CHAR_LINE */
char sd_ssid[MAX_CHAR_LINE];
char sd_pswd[MAX_CHAR_LINE];

esp_err_t sd_write_arr(const char *path, camera_fb_t *fb)
{
    size_t ret;
    ESP_LOGI(TAG, "Opening file |%s|", path);

    /* Open or create file located in the path string. Use binary write to write bytes instead of characters*/
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }

    /* Write the frame biffer to file */
    ret = fwrite(fb->buf, sizeof(char), fb->len, f);
    ESP_LOGI(TAG,"Wrote %zu elements out of %d requested", ret, fb->len);
    
    /* Close the file */
    fclose(f);
    ESP_LOGI(TAG, "File written");

    return ESP_OK;
}

esp_err_t sd_init()
{
    esp_err_t ret; 

    /* File system configuration to use for the SD card */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 10,
        .allocation_unit_size = 16 * 1024   // for 16 GB
    };
    
    ESP_LOGI(TAG, "Initializing SD card using SPI peripheral");
    /* Initialise SPI bus config */
    spi_bus_config_t spi_bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1, 
        .quadhd_io_num = -1,
        .max_transfer_sz = 4092,
    };

    /* Initialise and check if succesful */
    ret = spi_bus_initialize(host.slot, &spi_bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ret;
    }

    /* Initializes the slot without card detect (CD) and write protect (WP) signals */
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT(); // uses GPIO13 for CS
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    /* Finally mount the SD card using the declared SPI bus*/
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
        return ret;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    /* Card has been initialized, print its properties */
    sdmmc_card_print_info(stdout, card);
   
    return ret;
}

esp_err_t sd_deinit()
{
    /* All done, unmount partition and disable SPI peripheral */
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    /* Deinitialize the bus after all devices are removed */
    spi_bus_free(host.slot);
    return ESP_OK; 
}

esp_err_t read_wifi_credentials(){

    /* Initialise arrays with ACII null escape character '\0' */
    memset(sd_ssid, '\0', sizeof(sd_ssid));
    memset(sd_pswd, '\0', sizeof(sd_pswd));

    /* Read the wifi credentials text file */
    const char *path = WIFICRED_FILE_NAME;
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    /**
     * Get the two lines from the file. First line is used for the SSID,
     * second line is used for the Password. 
    */
    fgets(sd_ssid, sizeof(sd_ssid), f);
    fgets(sd_pswd, sizeof(sd_pswd), f);
    fclose(f);

    /**
     * Pointer to the character locations. 
     * Used to strip the '\n' and '\r' in the strings. 
     * If at the pos pointer location an '\r' or '\n' is detected, it is replaced by the 
     * null terminator. 
    */
    char *pos;
    pos = strchr(sd_ssid, '\r');
    if (pos) {
        *pos = '\0';
    }
    pos = strchr(sd_ssid, '\n');
    if (pos) {
        *pos = '\0';
    }
    pos = strchr(sd_pswd, '\r');
    if (pos) {
        *pos = '\0';
    }
    pos = strchr(sd_pswd, '\n');
    if (pos) {
        *pos = '\0';
    }

    ESP_LOGI(TAG, "SSID from file: '%s'", sd_ssid);
    ESP_LOGI(TAG, "Pswd from file: '%s'", sd_pswd);

    return ESP_OK;
}