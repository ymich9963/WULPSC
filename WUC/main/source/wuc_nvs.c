#include "wuc_nvs.h"

static const char* TAG = "WUC - NVS";
extern wuc_config_t wuc_config;
size_t ssid_required_size = 0;  // value will default to 0, if not set yet in NVS
size_t pswd_required_size = 0;  

esp_err_t get_nvs_sizes(){
    ESP_LOGI(TAG, "Getting the credential sizes from the NVS...");

    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {ESP_LOGE(TAG, "Error opening NVS."); return err; } 

    // Read SSID
    err = nvs_get_str(my_handle, "ssid", NULL, &ssid_required_size);
    ESP_LOGI(TAG, "SSID required size: %d", ssid_required_size);

    // NVS not found error is okay!
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error getting SSID size."); 
        return err; 
    }
    
    // Read password
    err = nvs_get_str(my_handle, "pswd", NULL, &pswd_required_size);
    ESP_LOGI(TAG, "Password required size: %d", pswd_required_size);

    // NVS not found error is okay!
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error getting Password size."); 
        return err; 
    }

    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t check_nvs(){

    ESP_LOGI(TAG, "Checking NVS for the credentials...");

    nvs_handle_t my_handle;
    esp_err_t err;

    get_nvs_sizes();

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {ESP_LOGE(TAG, "Error opening NVS."); return err; } 

    // If either of them is 0
    if(ssid_required_size == 0 || pswd_required_size == 0){
        // do smart config to get credentials
        wuc_config.stored_creds = false;
    } else {
        // Read stored SSID
        err = nvs_get_str(my_handle, "ssid", wuc_config.ssid, &ssid_required_size);
        ESP_LOGI(TAG, "Read SSID from NVS: %s", wuc_config.ssid);

        // NVS not found error is okay!
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error getting string. Error: %s", esp_err_to_name(err)); 
            return err; 
        }
        
        // Read stored password
        err = nvs_get_str(my_handle, "pswd", wuc_config.pswd, &pswd_required_size);
        ESP_LOGI(TAG, "Read Password from NVS: %s", wuc_config.pswd);

        // NVS not found error is okay!
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error getting string."); 
            return err; 
        }
        wuc_config.stored_creds = true;
    }

    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t store_creds_to_nvs(){
    ESP_LOGI(TAG, "Saving credentials to NVS...");
    nvs_handle_t my_handle;
    esp_err_t err;

    get_nvs_sizes();

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Save the credentials
    err = nvs_set_str(my_handle, "ssid", wuc_config.ssid);
    if (err != ESP_OK) return err;

    err = nvs_set_str(my_handle, "pswd", wuc_config.pswd);
    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK;

}