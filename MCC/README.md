# WiFi Ultra-Low Power Stereo Camera (WULPSC) - Main Camera Controller Module
This is the code for the Main Camera Controller (MCC) module. Uses the ``esp-camera`` module to setup the CSI with the OV2640. Connects to WiFi by reading the credentials from the SD card and then creates an HTTP server to send the pictures to a remote server.