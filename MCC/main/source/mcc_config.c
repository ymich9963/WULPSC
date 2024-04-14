#include "mcc_config.h"

const char * TAG = "WULPSC - Config";

extern mcc_config_t mcc_config;

/* 16-bit random number */
uint16_t rnd;

/* String var to store 16-bit random number (up to 65536,  i.e. 5 character bytes + 1 for null terminator)  */
char rnd_str[6] = "\0";

/* 19+1 bytes max length file path */
char path[20] = "\0";

esp_err_t camera_set_settings()
{
    /* Set the values to the system config */
    mcc_config.sensor->set_brightness(mcc_config.sensor, mcc_config.camera.brightness);            // from -2 to 2
    mcc_config.sensor->set_contrast(mcc_config.sensor, mcc_config.camera.contrast);                // from -2 to 2
    mcc_config.sensor->set_saturation(mcc_config.sensor, mcc_config.camera.saturation);            // from -2 to 2
    // Not supported by OV2640
    // mcc_config.sensor->set_sharpness(mcc_config.sensor, mcc_config.camera.sharpness);              // from -2 to 2 NOT SUPPORTED
    // mcc_config.sensor->set_denoise(mcc_config.sensor, mcc_config.camera.denoise);                  // from 0 to 255 NOT SUPPORTED
    mcc_config.sensor->set_special_effect(mcc_config.sensor, mcc_config.camera.special_effect);    // from 0 to 6
    mcc_config.sensor->set_wb_mode(mcc_config.sensor, mcc_config.camera.wb_mode);                  // from 0 to 4, white balance mode, 0 - sunny, 1 - cloudy, 2 - office, 3 - home 

    mcc_config.sensor->set_ae_level(mcc_config.sensor, mcc_config.camera.ae_level);                // from -2 to 2, valid when set_exposure_control is ON
    mcc_config.sensor->set_aec_value(mcc_config.sensor, mcc_config.camera.aec_value);              // from 0 to 1200, value for automatic exposure control
    mcc_config.sensor->set_agc_gain(mcc_config.sensor, mcc_config.camera.agc_gain);                // from 0 to 30, automatic gain control
    mcc_config.sensor->set_gainceiling(mcc_config.sensor, mcc_config.camera.gainceiling);          // from 0 to 6, upper limit of gain
    
    /* Settings with 1/0 enable/disable */
    mcc_config.sensor->set_lenc(mcc_config.sensor, mcc_config.camera.lenc);                         // lens correction
    mcc_config.sensor->set_gain_ctrl(mcc_config.sensor, mcc_config.camera.agc);                     // auto gain control on/off    
    mcc_config.sensor->set_exposure_ctrl(mcc_config.sensor, mcc_config.camera.aec);                 /* auto exposure control on/off, 
                                                                                                       it is ON by default. If this ON state alone is insufficient for the exposure value, it is a good idea to also turn set_aec2 ON.     */
    mcc_config.sensor->set_hmirror(mcc_config.sensor, mcc_config.camera.hmirror);                   // mirror horizontally        
    mcc_config.sensor->set_vflip(mcc_config.sensor, mcc_config.camera.vflip);                       // flip vertically        
    mcc_config.sensor->set_aec2(mcc_config.sensor, mcc_config.camera.aec2);                         /* auto exposure control 2, auto exposure control with DSP of camera          
                                                                                                       It seems that it will not take effect unless set_exposure_ctrl is turned ON. I think if you turn on both set_exposure_ctrl and set_aec2, 
                                                                                                       you can get an acceptable exposure for automatic exposure control.    */
    mcc_config.sensor->set_bpc(mcc_config.sensor, mcc_config.camera.bpc);                           // black pixel correction        
    mcc_config.sensor->set_wpc(mcc_config.sensor, mcc_config.camera.wpc);                           // white pixel correction        

    ESP_LOGI(TAG, "Set camera settings...");
    return ESP_OK;
}

void camera_get_settings()
{    
    ESP_LOGI(TAG, "---SENSOR--DATA---");
    ESP_LOGI(TAG, "Brightness: %d", mcc_config.sensor->status.brightness);
    ESP_LOGI(TAG, "Contrast: %d", mcc_config.sensor->status.contrast);
    ESP_LOGI(TAG, "Saturation: %d", mcc_config.sensor->status.saturation);
    ESP_LOGI(TAG, "Special Effect: %d", mcc_config.sensor->status.special_effect);
    ESP_LOGI(TAG, "WB Mode: %d", mcc_config.sensor->status.wb_mode);
    ESP_LOGI(TAG, "AE Level: %d", mcc_config.sensor->status.ae_level);
    ESP_LOGI(TAG, "AEC Value: %d", mcc_config.sensor->status.aec_value);
    ESP_LOGI(TAG, "AGC Gain: %d", mcc_config.sensor->status.agc_gain);
    ESP_LOGI(TAG, "Gainceiling: %d", mcc_config.sensor->status.gainceiling);
    ESP_LOGI(TAG, "Lenc: %d", mcc_config.sensor->status.lenc);
    ESP_LOGI(TAG, "AGC: %d", mcc_config.sensor->status.agc);
    ESP_LOGI(TAG, "AEC: %d", mcc_config.sensor->status.aec);
    ESP_LOGI(TAG, "HMirror: %d", mcc_config.sensor->status.hmirror);
    ESP_LOGI(TAG, "VFlip: %d", mcc_config.sensor->status.vflip);
    ESP_LOGI(TAG, "AEC2: %d", mcc_config.sensor->status.aec2);
    ESP_LOGI(TAG, "BPC: %d", mcc_config.sensor->status.bpc);
    ESP_LOGI(TAG, "WPC: %d", mcc_config.sensor->status.wpc);
    return;
}

esp_err_t JSON_config_set(char* content)
{
    /* Use the cJSON API to parse the content to a JSON object*/
    cJSON *root = cJSON_Parse(content);
    ESP_LOGI(TAG, "Changing settings...");

    /**
     * Look for the string corresponding to the correct value in the system configuration. 
     * Happens for every variable the user can change through HTTP.
    */
    if (cJSON_GetObjectItem(root, "brightness")) {
		int8_t _brightness = cJSON_GetObjectItem(root,"brightness")->valueint;
        mcc_config.camera.brightness =  _brightness;
		ESP_LOGI(TAG, "Brightness=%d",_brightness);
	}
    if (cJSON_GetObjectItem(root, "contrast")) {
		int8_t _contrast = cJSON_GetObjectItem(root,"contrast")->valueint;
        mcc_config.camera.contrast = _contrast;
		ESP_LOGI(TAG, "Contrast=%d",_contrast);
	}
    if (cJSON_GetObjectItem(root, "saturation")) {
		int8_t _saturation = cJSON_GetObjectItem(root,"saturation")->valueint;
        mcc_config.camera.saturation = _saturation;
		ESP_LOGI(TAG, "Saturation=%d",_saturation);
	}
    if (cJSON_GetObjectItem(root, "sharpness")) {
		int8_t _sharpness = cJSON_GetObjectItem(root,"sharpness")->valueint;
        mcc_config.camera.sharpness = _sharpness;
		ESP_LOGI(TAG, "Sharpness=%d",_sharpness);
	}
    if (cJSON_GetObjectItem(root, "denoise")) {
		uint8_t _denoise = cJSON_GetObjectItem(root,"denoise")->valueint;
        mcc_config.camera.denoise = _denoise;
		ESP_LOGI(TAG, "Denoise=%d",_denoise);
	}
    if (cJSON_GetObjectItem(root, "special_effect")) {
		uint8_t _special_effect = cJSON_GetObjectItem(root,"special_effect")->valueint;
        mcc_config.camera.special_effect = _special_effect;
		ESP_LOGI(TAG, "Special Effect=%d",_special_effect);
	}
    if (cJSON_GetObjectItem(root, "wb_mode")) {
		uint8_t _wb_mode = cJSON_GetObjectItem(root,"wb_mode")->valueint;
        mcc_config.camera.wb_mode = _wb_mode;
		ESP_LOGI(TAG, "WB Mode=%d",_wb_mode);
	}
    if (cJSON_GetObjectItem(root, "ae_level")) {
		int8_t _ae_level = cJSON_GetObjectItem(root,"ae_level")->valueint;
        mcc_config.camera.ae_level = _ae_level;
		ESP_LOGI(TAG, "AE Level=%d",_ae_level);
	}
    if (cJSON_GetObjectItem(root, "aec_value")) {
		int16_t _aec_value = cJSON_GetObjectItem(root,"aec_value")->valueint;
        mcc_config.camera.aec_value = _aec_value;
		ESP_LOGI(TAG, "AEC Value=%d",_aec_value);
	}
    if (cJSON_GetObjectItem(root, "agc_gain")) {
		uint8_t _agc_gain = cJSON_GetObjectItem(root,"agc_gain")->valueint;
        mcc_config.camera.agc_gain = _agc_gain;
		ESP_LOGI(TAG, "AGC Gain=%d",_agc_gain);
	}
    if (cJSON_GetObjectItem(root, "gainceiling")) {
		uint8_t _gainceiling = cJSON_GetObjectItem(root,"gainceiling")->valueint;
        mcc_config.camera.gainceiling = _gainceiling;
		ESP_LOGI(TAG, "Gainceiling=%d",_gainceiling);
	}
    if (cJSON_GetObjectItem(root, "lenc")) {
		bool _lenc = cJSON_GetObjectItem(root,"lenc")->valueint;
        mcc_config.camera.lenc = _lenc;
		ESP_LOGI(TAG, "LenC=%d",_lenc);
	}
    if (cJSON_GetObjectItem(root, "agc")) {
		bool _agc = cJSON_GetObjectItem(root,"agc")->valueint;
        mcc_config.camera.agc = _agc;
		ESP_LOGI(TAG, "AGC=%d",_agc);
	}
    if (cJSON_GetObjectItem(root, "aec")) {
		bool _aec = cJSON_GetObjectItem(root,"aec")->valueint;
        mcc_config.camera.aec = _aec;
		ESP_LOGI(TAG, "AEC=%d",_aec);
	}
    if (cJSON_GetObjectItem(root, "hmirror")) {
		bool _hmirror = cJSON_GetObjectItem(root,"hmirror")->valueint;
        mcc_config.camera.hmirror = _hmirror;
		ESP_LOGI(TAG, "HMirror=%d",_hmirror);
	}
    if (cJSON_GetObjectItem(root, "vflip")) {
		bool _vflip = cJSON_GetObjectItem(root,"vflip")->valueint;
        mcc_config.camera.vflip = _vflip;
		ESP_LOGI(TAG, "VFlip=%d",_vflip);
	}
    if (cJSON_GetObjectItem(root, "aec2")) {
		bool _aec2 = cJSON_GetObjectItem(root,"aec2")->valueint;
        mcc_config.camera.aec2 = _aec2;
		ESP_LOGI(TAG, "AEC2=%d",_aec2);
	}
    if (cJSON_GetObjectItem(root, "bpc")) {
		bool _bpc = cJSON_GetObjectItem(root,"bpc")->valueint;
        mcc_config.camera.bpc = _bpc;
		ESP_LOGI(TAG, "BPC=%d",_bpc);
	}
    if (cJSON_GetObjectItem(root, "wpc")) {
		bool _wpc = cJSON_GetObjectItem(root,"wpc")->valueint;
        mcc_config.camera.wpc = _wpc;
		ESP_LOGI(TAG, "WPC=%d",_wpc);
	}
    if (cJSON_GetObjectItem(root, "sd_save")) {
        bool _sd_save = cJSON_GetObjectItem(root, "sd_save")->valueint;
        mcc_config.sd_save = _sd_save;
        ESP_LOGI(TAG, "SD_SAVE=%d", _sd_save);
    } 
	cJSON_Delete(root);    
    return ESP_OK;     
}


camera_fb_t* sys_take_picture()
{
    camera_fb_t* fb;

    /* Apply settings at each picture. Required due to initialisation and de-initialisation */
    mcc_config.sensor = esp_camera_sensor_get();
    camera_set_settings(mcc_config);  
    vTaskDelay(10/portTICK_PERIOD_MS);

    /* Not implemented, but if a flash LED was used, it would get turned on based on the .flash member state */
    switch (mcc_config.flash){
    case false:
        fb = esp_camera_fb_get();
        return fb;
        break;
    case true:
        turn_on_flash();
        fb = esp_camera_fb_get();
        turn_off_flash();
        return fb;
        break;
    default:
        fb = NULL;
        ESP_LOGW(TAG, "Problem with flash value, no image taken...");
        return fb;
        break;
    }
}

esp_err_t sys_camera_switch()
{
    esp_err_t ret;
 
    /* De-initialise */ 
    ret = esp_camera_deinit();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "De-init returned badly");
    } else {
        ESP_LOGW(TAG, "De-init OK");
    }
    vTaskDelay(100/portTICK_PERIOD_MS);

    /* Power down for safe de-initialisation */
    gpio_set_level(CAM_PIN_PWDN, 0);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "GPIO set level returned badly");
    } else {
        ESP_LOGW(TAG, "GPIO set level OK. PWDN is 0");
    }
    vTaskDelay(100/portTICK_PERIOD_MS);

    /* Using an XOR gate to flip the SEL pin for the MUXs*/
    mcc_config.sel_status ^= 0x1;
    gpio_set_level(SEL_PIN, mcc_config.sel_status);
    ESP_LOGI(TAG, "SEL Status: %d at %d", mcc_config.sel_status, SEL_PIN);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    /* Power up! */
    gpio_set_level(CAM_PIN_PWDN, 1);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "GPIO set level returned badly");
    } else {
        ESP_LOGW(TAG, "GPIO set level OK. PSWDN is 1.");
    }
    vTaskDelay(100/portTICK_PERIOD_MS);

    /* Initialise */
    ret = init_camera();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Camera init returned badly");
    } else {
        ESP_LOGW(TAG, "Camera init OK");
    }
    vTaskDelay(100/portTICK_PERIOD_MS);

    return ESP_OK;
}

esp_err_t sys_sd_var_setup()
{
    /* To reduce file size when saving */
    change_pixformat_to_jpeg();

    esp_fill_random(&rnd, sizeof(rnd));
    sprintf(rnd_str,"%u", rnd);

    ESP_LOGI(TAG, "SD Ready for saving!");
    return ESP_OK;
}

esp_err_t sys_sd_save(camera_fb_t* fb){
    esp_err_t ret;

    /* Set the first part of the path which is the mount point '/sdcard' followed by another '/' */
    strcpy(path,MOUNT_POINT"/"); 

    /* Attach the random string obtained from the setup function */
    strcat(path, rnd_str);

    /* Based on the SEL pin status, attach the L or R as a suffix to separate the two images, as well as the file extension */
    switch (mcc_config.sel_status) {
    case 0x0:
        strcat(path, "L.jpg");
        break;
    case 0x1:
        strcat(path, "R.jpg");
    default:
        break;
    }

    /* Write frame buffer to SD card */
    ret = sd_write_arr(path, fb);

    /* Error check if file was not written correctly */
    if (ret != ESP_OK){
        ESP_LOGW(TAG, "File not written correctly");
        return ESP_FAIL;
    }

    return ESP_OK;
}