#include "system_config.h"

const char * TAG = "WULPSC - Config";

esp_err_t camera_set_settings(system_config_t sys_config){

    // settings with values
    sys_config.sensor->set_brightness(sys_config.sensor, sys_config.camera.brightness);            // from -2 to 2
    sys_config.sensor->set_contrast(sys_config.sensor, sys_config.camera.contrast);                // from -2 to 2
    sys_config.sensor->set_saturation(sys_config.sensor, sys_config.camera.saturation);            // from -2 to 2
    // sys_config.sensor->set_sharpness(sys_config.sensor, sys_config.camera.sharpness);              // from -2 to 2 NOT SUPPORTED
    // sys_config.sensor->set_denoise(sys_config.sensor, sys_config.camera.denoise);                  // from 0 to 255 NOT SUPPORTED
    sys_config.sensor->set_special_effect(sys_config.sensor, sys_config.camera.special_effect);    // from 0 to 6
    sys_config.sensor->set_wb_mode(sys_config.sensor, sys_config.camera.wb_mode);                  // from 0 to 4, white balance mode
    sys_config.sensor->set_ae_level(sys_config.sensor, sys_config.camera.ae_level);                // from -2 to 2, valid when set_exposure is ON
    sys_config.sensor->set_aec_value(sys_config.sensor, sys_config.camera.aec_value);              // from 0 to 1200, value for automatic exposure control
    sys_config.sensor->set_agc_gain(sys_config.sensor, sys_config.camera.agc_gain);                // from 0 to 30, automatic gain control
    sys_config.sensor->set_gainceiling(sys_config.sensor, sys_config.camera.gainceiling);          // from 0 to 6, upper limit of gain
    
    // settings with 1/0 enable/disable
    // sys_config.sensor->set_lenc(sys_config.sensor, sys_config.camera.lenc);                         // lens correction
    sys_config.sensor->set_gain_ctrl(sys_config.sensor, sys_config.camera.agc);                     // auto gain control on/off    
    sys_config.sensor->set_exposure_ctrl(sys_config.sensor, sys_config.camera.aec);                 /* auto exposure control on/off, variable to use is awb, 
                                                                                            it is ON by default. If this ON state alone is insufficient for the exposure value, it is a good idea to also turn set_aec2 ON.     */
    sys_config.sensor->set_hmirror(sys_config.sensor, sys_config.camera.hmirror);                   // mirror horizontally        
    sys_config.sensor->set_vflip(sys_config.sensor, sys_config.camera.vflip);                       // flip vertically        
    sys_config.sensor->set_aec2(sys_config.sensor, sys_config.camera.aec2);                         /* auto exposure control 2, auto exposure control with DSP of camera          
                                                                                            It seems that it will not take effect unless set_exposure_ctrl is turned ON. I think if you turn on both set_exposure_ctrl and set_aec2, 
                                                                                            you can get an acceptable exposure for automatic exposure control.    */
    sys_config.sensor->set_bpc(sys_config.sensor, sys_config.camera.bpc);                           // black pixel correction        
    sys_config.sensor->set_wpc(sys_config.sensor, sys_config.camera.wpc);                           // white pixel correction        

    return ESP_OK;
}

system_config_t JSON_config_set(char* content, system_config_t sys_config){
    cJSON *root = cJSON_Parse(content);
    ESP_LOGI(TAG, "Changing settings...");
    if (cJSON_GetObjectItem(root, "brightness")) {
		int8_t _brightness = cJSON_GetObjectItem(root,"brightness")->valueint;
        sys_config.camera.brightness =  _brightness;
		ESP_LOGI(TAG, "Brightness=%d",_brightness);
	}
    if (cJSON_GetObjectItem(root, "contrast")) {
		int8_t _contrast = cJSON_GetObjectItem(root,"contrast")->valueint;
        sys_config.camera.contrast = _contrast;
		ESP_LOGI(TAG, "Contrast=%d",_contrast);
	}
    if (cJSON_GetObjectItem(root, "saturation")) {
		int8_t _saturation = cJSON_GetObjectItem(root,"saturation")->valueint;
        sys_config.camera.saturation = _saturation;
		ESP_LOGI(TAG, "Saturation=%d",_saturation);
	}
    if (cJSON_GetObjectItem(root, "sharpness")) {
		int8_t _sharpness = cJSON_GetObjectItem(root,"sharpness")->valueint;
        sys_config.camera.sharpness = _sharpness;
		ESP_LOGI(TAG, "Sharpness=%d",_sharpness);
	}
    if (cJSON_GetObjectItem(root, "denoise")) {
		uint8_t _denoise = cJSON_GetObjectItem(root,"denoise")->valueint;
        sys_config.camera.denoise = _denoise;
		ESP_LOGI(TAG, "Denoise=%d",_denoise);
	}
    if (cJSON_GetObjectItem(root, "special_effect")) {
		uint8_t _special_effect = cJSON_GetObjectItem(root,"special_effect")->valueint;
        sys_config.camera.special_effect = _special_effect;
		ESP_LOGI(TAG, "Special Effect=%d",_special_effect);
	}
    if (cJSON_GetObjectItem(root, "wb_mode")) {
		uint8_t _wb_mode = cJSON_GetObjectItem(root,"wb_mode")->valueint;
        sys_config.camera.wb_mode = _wb_mode;
		ESP_LOGI(TAG, "WB Mode=%d",_wb_mode);
	}
    if (cJSON_GetObjectItem(root, "ae_level")) {
		int8_t _ae_level = cJSON_GetObjectItem(root,"ae_level")->valueint;
        sys_config.camera.ae_level = _ae_level;
		ESP_LOGI(TAG, "AE Level=%d",_ae_level);
	}
    if (cJSON_GetObjectItem(root, "aec_value")) {
		int16_t _aec_value = cJSON_GetObjectItem(root,"aec_value")->valueint;
        sys_config.camera.aec_value = _aec_value;
		ESP_LOGI(TAG, "AEC Value=%d",_aec_value);
	}
    if (cJSON_GetObjectItem(root, "agc_gain")) {
		uint8_t _agc_gain = cJSON_GetObjectItem(root,"agc_gain")->valueint;
        sys_config.camera.agc_gain = _agc_gain;
		ESP_LOGI(TAG, "AGC Gain=%d",_agc_gain);
	}
    if (cJSON_GetObjectItem(root, "gainceiling")) {
		uint8_t _gainceiling = cJSON_GetObjectItem(root,"gainceiling")->valueint;
        sys_config.camera.gainceiling = _gainceiling;
		ESP_LOGI(TAG, "Gainceiling=%d",_gainceiling);
	}
    if (cJSON_GetObjectItem(root, "lenc")) {
		bool _lenc = cJSON_GetObjectItem(root,"lenc")->valueint;
        sys_config.camera.lenc = _lenc;
		ESP_LOGI(TAG, "LenC=%d",_lenc);
	}
    if (cJSON_GetObjectItem(root, "agc")) {
		bool _agc = cJSON_GetObjectItem(root,"agc")->valueint;
        sys_config.camera.agc = _agc;
		ESP_LOGI(TAG, "AGC=%d",_agc);
	}
    if (cJSON_GetObjectItem(root, "aec")) {
		bool _aec = cJSON_GetObjectItem(root,"aec")->valueint;
        sys_config.camera.aec = _aec;
		ESP_LOGI(TAG, "AEC=%d",_aec);
	}
    if (cJSON_GetObjectItem(root, "hmirror")) {
		bool _hmirror = cJSON_GetObjectItem(root,"hmirror")->valueint;
        sys_config.camera.hmirror = _hmirror;
		ESP_LOGI(TAG, "HMirror=%d",_hmirror);
	}
    if (cJSON_GetObjectItem(root, "vflip")) {
		bool _vflip = cJSON_GetObjectItem(root,"vflip")->valueint;
        sys_config.camera.vflip = _vflip;
		ESP_LOGI(TAG, "VFlip=%d",_vflip);
	}
    if (cJSON_GetObjectItem(root, "aec2")) {
		bool _aec2 = cJSON_GetObjectItem(root,"aec2")->valueint;
        sys_config.camera.aec2 = _aec2;
		ESP_LOGI(TAG, "AEC2=%d",_aec2);
	}
    if (cJSON_GetObjectItem(root, "bpc")) {
		bool _bpc = cJSON_GetObjectItem(root,"bpc")->valueint;
        sys_config.camera.bpc = _bpc;
		ESP_LOGI(TAG, "BPC=%d",_bpc);
	}
    if (cJSON_GetObjectItem(root, "wpc")) {
		bool _wpc = cJSON_GetObjectItem(root,"wpc")->valueint;
        sys_config.camera.wpc = _wpc;
		ESP_LOGI(TAG, "WPC=%d",_wpc);
	}

	cJSON_Delete(root);    

    return sys_config;     
}

