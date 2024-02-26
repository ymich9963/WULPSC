# WiFi Ultra-Low Power Stereo Camera (WULPSC) - HTTP Functionality
To individually test the http protocol of the system, please use this branch which isolates this functionality. WiFi credentials are required and can be setup in ``wifi.h``. Also used to test the JSON parsing of the POST request data.

## Setup Environment
To setup environment for this, please look at the main branch.

## Observations
It is very sensitive to network and if connection is unstable it won't work. It requires an access point with port forwarding port 19520 and connecting to it via the open IP address. Can't really handle the JPEG getting sent at once therefore chunking was used. Example command to send the JSON file ``camera_settings.json`` is ``curl -ContentType 'application/json' -Body camera_settings.json  -Method Post http://192.168.124.112:19520/config``.

 For WiFi to work in some cases port forwarding or placing the IP address into the DMZ may be required if errors occur. PLease advise your internet service provider for doing so.
