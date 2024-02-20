# WiFi Ultra-Low Power Stereo Camera (WULPSC) - Stereo Camera Test
This branch is used to test the stereo functionality of the cameras. It uses two HTTP GET requests to take both pictures. One request takes a picture from camera 1, sends it, and switches to camera2. The second handler polls for the picture for camera 1 to be sent and then takes a picture to send over.

## Setup Environment
To setup environment for this, please look at the main branch.

## Observations
Due to the hardware configuration of the connector, the cameras should be switched off and on as well as initialised and de-initialised. Code is very similar to the http-test branch but with no POST handlers.