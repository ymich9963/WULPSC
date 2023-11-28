# WiFi Ultra-Low Power Stereo Camera (WULPSC) - SD Card Functionality
To isolate the SD card feature and to further document development, a new branch just for it was created. This can be used for testing, for future reference, or for any other projects out there that want this feature in their ESP32s (mainly ESP32-CAM)

## Setup Environment
To setup environment for this, please look at the main branch.


## Observations
Some observations when testing are that if fast read/write times are required, the image should me stored in DRAM of the ESP and not in PSRAM.