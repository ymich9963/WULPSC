# WiFi Ultra-Low Power Stereo Camera (WULPSC) - HTTP Functionality
To individually test the http protocol of the system, please use this branch which isolates this functionality. WiFi credentials are required and can be setup in ``wifi.h``.  In case no image capture is required then set the ``#define`` directive of ``CAMERA_CAPTURE`` to be 0. This will simply send a string to the server instead of an image. 

## Setup Environment
To setup environment for this, please look at the main branch.