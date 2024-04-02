#include "mmc_config.h"

const char * TAG = "WULPSC - Config";

/* 8-bit random number */
uint16_t rnd;

/* String var to store 16-bit random number (up to 65536,  i.e. 6 bytes + 1)  */
char rnd_str[6] = "\0";

/* 22+1 bytes max length file path */
char path[20] = "\0";

esp_err_t camera_set_settings(mmc_config_t mmc_config){

    // settings with values
    mmc_config.sensor->set_brightness(mmc_config.sensor, mmc_config.camera.brightness);            // from -2 to 2
    mmc_config.sensor->set_contrast(mmc_config.sensor, mmc_config.camera.contrast);                // from -2 to 2
    mmc_config.sensor->set_saturation(mmc_config.sensor, mmc_config.camera.saturation);            // from -2 to 2
    // mmc_config.sensor->set_sharpness(mmc_config.sensor, mmc_config.camera.sharpness);              // from -2 to 2 NOT SUPPORTED
    // mmc_config.sensor->set_denoise(mmc_config.sensor, mmc_config.camera.denoise);                  // from 0 to 255 NOT SUPPORTED
    mmc_config.sensor->set_special_effect(mmc_config.sensor, mmc_config.camera.special_effect);    // from 0 to 6
    mmc_config.sensor->set_wb_mode(mmc_config.sensor, mmc_config.camera.wb_mode);                  // from 0 to 4, white balance mode, 0 - sunny, 1 - cloudy, 2 - office, 3 - home 

    mmc_config.sensor->set_ae_level(mmc_config.sensor, mmc_config.camera.ae_level);                // from -2 to 2, valid when set_exposure_control is ON
    mmc_config.sensor->set_aec_value(mmc_config.sensor, mmc_config.camera.aec_value);              // from 0 to 1200, value for automatic exposure control
    mmc_config.sensor->set_agc_gain(mmc_config.sensor, mmc_config.camera.agc_gain);                // from 0 to 30, automatic gain control
    mmc_config.sensor->set_gainceiling(mmc_config.sensor, mmc_config.camera.gainceiling);          // from 0 to 6, upper limit of gain
    
    // settings with 1/0 enable/disable
    mmc_config.sensor->set_lenc(mmc_config.sensor, mmc_config.camera.lenc);                         // lens correction
    mmc_config.sensor->set_gain_ctrl(mmc_config.sensor, mmc_config.camera.agc);                     // auto gain control on/off    
    mmc_config.sensor->set_exposure_ctrl(mmc_config.sensor, mmc_config.camera.aec);                 /* auto exposure control on/off, 
                                                                                                       it is ON by default. If this ON state alone is insufficient for the exposure value, it is a good idea to also turn set_aec2 ON.     */
    mmc_config.sensor->set_hmirror(mmc_config.sensor, mmc_config.camera.hmirror);                   // mirror horizontally        
    mmc_config.sensor->set_vflip(mmc_config.sensor, mmc_config.camera.vflip);                       // flip vertically        
    mmc_config.sensor->set_aec2(mmc_config.sensor, mmc_config.camera.aec2);                         /* auto exposure control 2, auto exposure control with DSP of camera          
                                                                                                       It seems that it will not take effect unless set_exposure_ctrl is turned ON. I think if you turn on both set_exposure_ctrl and set_aec2, 
                                                                                                       you can get an acceptable exposure for automatic exposure control.    */
    mmc_config.sensor->set_bpc(mmc_config.sensor, mmc_config.camera.bpc);                           // black pixel correction        
    mmc_config.sensor->set_wpc(mmc_config.sensor, mmc_config.camera.wpc);                           // white pixel correction        

    ESP_LOGI(TAG, "Set camera settings...");
    return ESP_OK;
}

void camera_get_settings(mmc_config_t mmc_config){
    
    ESP_LOGI(TAG, "---SENSOR--DATA---");
    ESP_LOGI(TAG, "Brightness: %d", mmc_config.sensor->status.brightness);
    ESP_LOGI(TAG, "Contrast: %d", mmc_config.sensor->status.contrast);
    ESP_LOGI(TAG, "Saturation: %d", mmc_config.sensor->status.saturation);
    ESP_LOGI(TAG, "Special Effect: %d", mmc_config.sensor->status.special_effect);
    ESP_LOGI(TAG, "WB Mode: %d", mmc_config.sensor->status.wb_mode);
    ESP_LOGI(TAG, "AE Level: %d", mmc_config.sensor->status.ae_level);
    ESP_LOGI(TAG, "AEC Value: %d", mmc_config.sensor->status.aec_value);
    ESP_LOGI(TAG, "AGC Gain: %d", mmc_config.sensor->status.agc_gain);
    ESP_LOGI(TAG, "Gainceiling: %d", mmc_config.sensor->status.gainceiling);
    ESP_LOGI(TAG, "Lenc: %d", mmc_config.sensor->status.lenc);
    ESP_LOGI(TAG, "AGC: %d", mmc_config.sensor->status.agc);
    ESP_LOGI(TAG, "AEC: %d", mmc_config.sensor->status.aec);
    ESP_LOGI(TAG, "HMirror: %d", mmc_config.sensor->status.hmirror);
    ESP_LOGI(TAG, "VFlip: %d", mmc_config.sensor->status.vflip);
    ESP_LOGI(TAG, "AEC2: %d", mmc_config.sensor->status.aec2);
    ESP_LOGI(TAG, "BPC: %d", mmc_config.sensor->status.bpc);
    ESP_LOGI(TAG, "WPC: %d", mmc_config.sensor->status.wpc);
}

mmc_config_t JSON_config_set(char* content, mmc_config_t mmc_config){
    cJSON *root = cJSON_Parse(content);
    ESP_LOGI(TAG, "Changing settings...");
    if (cJSON_GetObjectItem(root, "brightness")) {
		int8_t _brightness = cJSON_GetObjectItem(root,"brightness")->valueint;
        mmc_config.camera.brightness =  _brightness;
		ESP_LOGI(TAG, "Brightness=%d",_brightness);
	}
    if (cJSON_GetObjectItem(root, "contrast")) {
		int8_t _contrast = cJSON_GetObjectItem(root,"contrast")->valueint;
        mmc_config.camera.contrast = _contrast;
		ESP_LOGI(TAG, "Contrast=%d",_contrast);
	}
    if (cJSON_GetObjectItem(root, "saturation")) {
		int8_t _saturation = cJSON_GetObjectItem(root,"saturation")->valueint;
        mmc_config.camera.saturation = _saturation;
		ESP_LOGI(TAG, "Saturation=%d",_saturation);
	}
    if (cJSON_GetObjectItem(root, "sharpness")) {
		int8_t _sharpness = cJSON_GetObjectItem(root,"sharpness")->valueint;
        mmc_config.camera.sharpness = _sharpness;
		ESP_LOGI(TAG, "Sharpness=%d",_sharpness);
	}
    if (cJSON_GetObjectItem(root, "denoise")) {
		uint8_t _denoise = cJSON_GetObjectItem(root,"denoise")->valueint;
        mmc_config.camera.denoise = _denoise;
		ESP_LOGI(TAG, "Denoise=%d",_denoise);
	}
    if (cJSON_GetObjectItem(root, "special_effect")) {
		uint8_t _special_effect = cJSON_GetObjectItem(root,"special_effect")->valueint;
        mmc_config.camera.special_effect = _special_effect;
		ESP_LOGI(TAG, "Special Effect=%d",_special_effect);
	}
    if (cJSON_GetObjectItem(root, "wb_mode")) {
		uint8_t _wb_mode = cJSON_GetObjectItem(root,"wb_mode")->valueint;
        mmc_config.camera.wb_mode = _wb_mode;
		ESP_LOGI(TAG, "WB Mode=%d",_wb_mode);
	}
    if (cJSON_GetObjectItem(root, "ae_level")) {
		int8_t _ae_level = cJSON_GetObjectItem(root,"ae_level")->valueint;
        mmc_config.camera.ae_level = _ae_level;
		ESP_LOGI(TAG, "AE Level=%d",_ae_level);
	}
    if (cJSON_GetObjectItem(root, "aec_value")) {
		int16_t _aec_value = cJSON_GetObjectItem(root,"aec_value")->valueint;
        mmc_config.camera.aec_value = _aec_value;
		ESP_LOGI(TAG, "AEC Value=%d",_aec_value);
	}
    if (cJSON_GetObjectItem(root, "agc_gain")) {
		uint8_t _agc_gain = cJSON_GetObjectItem(root,"agc_gain")->valueint;
        mmc_config.camera.agc_gain = _agc_gain;
		ESP_LOGI(TAG, "AGC Gain=%d",_agc_gain);
	}
    if (cJSON_GetObjectItem(root, "gainceiling")) {
		uint8_t _gainceiling = cJSON_GetObjectItem(root,"gainceiling")->valueint;
        mmc_config.camera.gainceiling = _gainceiling;
		ESP_LOGI(TAG, "Gainceiling=%d",_gainceiling);
	}
    if (cJSON_GetObjectItem(root, "lenc")) {
		bool _lenc = cJSON_GetObjectItem(root,"lenc")->valueint;
        mmc_config.camera.lenc = _lenc;
		ESP_LOGI(TAG, "LenC=%d",_lenc);
	}
    if (cJSON_GetObjectItem(root, "agc")) {
		bool _agc = cJSON_GetObjectItem(root,"agc")->valueint;
        mmc_config.camera.agc = _agc;
		ESP_LOGI(TAG, "AGC=%d",_agc);
	}
    if (cJSON_GetObjectItem(root, "aec")) {
		bool _aec = cJSON_GetObjectItem(root,"aec")->valueint;
        mmc_config.camera.aec = _aec;
		ESP_LOGI(TAG, "AEC=%d",_aec);
	}
    if (cJSON_GetObjectItem(root, "hmirror")) {
		bool _hmirror = cJSON_GetObjectItem(root,"hmirror")->valueint;
        mmc_config.camera.hmirror = _hmirror;
		ESP_LOGI(TAG, "HMirror=%d",_hmirror);
	}
    if (cJSON_GetObjectItem(root, "vflip")) {
		bool _vflip = cJSON_GetObjectItem(root,"vflip")->valueint;
        mmc_config.camera.vflip = _vflip;
		ESP_LOGI(TAG, "VFlip=%d",_vflip);
	}
    if (cJSON_GetObjectItem(root, "aec2")) {
		bool _aec2 = cJSON_GetObjectItem(root,"aec2")->valueint;
        mmc_config.camera.aec2 = _aec2;
		ESP_LOGI(TAG, "AEC2=%d",_aec2);
	}
    if (cJSON_GetObjectItem(root, "bpc")) {
		bool _bpc = cJSON_GetObjectItem(root,"bpc")->valueint;
        mmc_config.camera.bpc = _bpc;
		ESP_LOGI(TAG, "BPC=%d",_bpc);
	}
    if (cJSON_GetObjectItem(root, "wpc")) {
		bool _wpc = cJSON_GetObjectItem(root,"wpc")->valueint;
        mmc_config.camera.wpc = _wpc;
		ESP_LOGI(TAG, "WPC=%d",_wpc);
	}

	cJSON_Delete(root);    

    return mmc_config;     
}

esp_err_t sys_sd_save_check(mmc_config_t* mmc_config, camera_fb_t* fb){
    esp_err_t ret;
    switch (mmc_config->sd_save){
    case NO_SD:
        ret = sd_deinit();

        if(ret != ESP_OK){
            ESP_LOGW(TAG, "Error with SD de-init");
            return ESP_FAIL;
        }

        return ESP_OK;
        break;
    case SD_SAVE:
        mmc_config->sd_save = SD_SAVING;

        // to reduce file size when saving
        change_pixformat_to_jpeg();

        esp_fill_random(&rnd, sizeof(rnd));
        sprintf(rnd_str,"%u", rnd);

        ESP_LOGI(TAG, "SD Ready for saving!");
        return ESP_OK;
        break;
    case SD_SAVING:
        strcpy(path,MOUNT_POINT"/");

        strcat(path, rnd_str);

        switch (mmc_config->active_cam)
        {
        case 0:
            strcat(path, "L.jpg");
            break;
        case 1:
            strcat(path, "R.jpg");
        default:
            break;
        }
        ret = sd_write_arr(path, fb);

        // error check if file was not written correctly
        if (ret != ESP_OK){
            ESP_LOGW(TAG, "File not written correctly");
            return ESP_FAIL;
        }

        return ESP_OK;
        break;
    default:
        return ESP_FAIL;
        break;
    } 
}

camera_fb_t* sys_take_picture(mmc_config_t mmc_config){
    camera_fb_t* fb;
    switch (mmc_config.flash){
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
        return fb;
        break;
    }
}

esp_err_t sys_camera_switch(mmc_config_t mmc_config){
    esp_err_t ret;
    
    // de-initialise 
    ret = esp_camera_deinit();
    if(ret != ESP_OK){
        ESP_LOGW(TAG, "De-init returned badly");
    }

    // power down?
    gpio_set_level(CAM_PIN_PWDN, 0);
    if(ret != ESP_OK){
        ESP_LOGW(TAG, "GPIO set level returned badly");
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
    
    switch (mmc_config.active_cam){
    case 0:
        gpio_set_level(SEL_PIN, 1);
        break;
    case 1:
        gpio_set_level(SEL_PIN, 0);
        break;
    default:
        ESP_LOGW(TAG,"Entered default case in cam_switched(). Reset to 0");
        mmc_config.active_cam = 0;
        break;
    }

    // power up!
    gpio_set_level(CAM_PIN_PWDN, 1);
    if(ret != ESP_OK){
        ESP_LOGW(TAG, "GPIO set level returned badly");
    }
    vTaskDelay(10/portTICK_PERIOD_MS);

    // initialise
    ret = init_camera();
    if(ret != ESP_OK){
        ESP_LOGW(TAG, "Camera init returned badly");
    }

    // set settings again
    mmc_config.sensor = esp_camera_sensor_get();
    camera_set_settings(mmc_config);

    return ESP_OK;
}
