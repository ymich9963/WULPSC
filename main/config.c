#include "config.h"

esp_err_t camera_set_settings(camera_status_t camera, system_config_t sys_config){
    // s = esp_camera_sensor_get();
    // s.sensor = esp_camera_sensor_get();

    // settings with values
    sys_config.sensor->set_brightness(sys_config.sensor, camera.brightness);            // from -2 to 2
    sys_config.sensor->set_contrast(sys_config.sensor, camera.contrast);                // from -2 to 2
    sys_config.sensor->set_saturation(sys_config.sensor, camera.saturation);            // from -2 to 2
    sys_config.sensor->set_sharpness(sys_config.sensor, camera.sharpness);              // from -2 to 2
    sys_config.sensor->set_denoise(sys_config.sensor, camera.denoise);                  // from 0 to 255
    sys_config.sensor->set_special_effect(sys_config.sensor, camera.special_effect);    // from 0 to 6
    sys_config.sensor->set_wb_mode(sys_config.sensor, camera.wb_mode);                  // from 0 to 4, white balance mode
    sys_config.sensor->set_ae_level(sys_config.sensor, camera.ae_level);                // from -2 to 2, valid when set_exposure is ON
    sys_config.sensor->set_aec_value(sys_config.sensor, camera.aec_value);              // from 0 to 1200, value for automatic exposure control
    sys_config.sensor->set_agc_gain(sys_config.sensor, camera.agc_gain);                // from 0 to 30, automatic gain control
    sys_config.sensor->set_gainceiling(sys_config.sensor, camera.gainceiling);          // from 0 to 6, upper limit of gain
    
    // settings with 0/1 enable/disable
    sys_config.sensor->set_lenc(sys_config.sensor, camera.lenc);                         // lens correction
    sys_config.sensor->set_gain_ctrl(sys_config.sensor, camera.agc);                     // auto gain control on/off    
    sys_config.sensor->set_exposure_ctrl(sys_config.sensor, camera.awb);                 /* auto exposure control on/off, variable to use is awb, 
                                                                                            it is ON by default. If this ON state alone is insufficient for the exposure value, it is a good idea to also turn set_aec2 ON.     */
    sys_config.sensor->set_hmirror(sys_config.sensor, camera.hmirror);                   // mirror horizontally        
    sys_config.sensor->set_vflip(sys_config.sensor, camera.vflip);                       // flip vertically        
    sys_config.sensor->set_aec2(sys_config.sensor, camera.aec2);                         /* auto exposure control 2, auto exposure control with DSP of camera          
                                                                                            It seems that it will not take effect unless set_exposure_ctrl is turned ON. I think if you turn on both set_exposure_ctrl and set_aec2, 
                                                                                            you can get an acceptable exposure for automatic exposure control.    */
    sys_config.sensor->set_bpc(sys_config.sensor, camera.bpc);                           // black pixel correction        
    sys_config.sensor->set_wpc(sys_config.sensor, camera.wpc);                           // white pixel correction        

    return ESP_OK;
}
