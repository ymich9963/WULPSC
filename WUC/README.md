# WiFi Ultra-Low Power Stereo Camera (WULPSC) - Wake Up Controller Module
This is the code used in the Wake Up Controller (WUC) module. Uses the NVS to store the credentials after deep sleep, and uses smart config with ESPTouch to collect the correct credentials at the initial boot. HTTP server is then started which is used to communicate with the remote server.

## Setup Environment
To setup environment for this, please look at the main page.

## Observations
ESPTouch can take it's time to connect. Only a network with a 2.4 GHz connection can be used!!!