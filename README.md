# WiFi Ultra-Low Power Stereo Camera (WULPSC)

## Setup Environment
To develop for this project there some steps to setup the environment for building and flashing. VSCode is the chosen IDE.

  - Step 1. Donwload ESP-IDF Toolchain [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html) and install normally.

 - Step 2. Make note of the installed `IDF_PATH` and `IDF_TOOLS_PATH` , they should be listed in the final screen of the installer.

 - Step 3. Download [Python](https://www.python.org/downloads/) (any 3.8+ version) and the newest version of [Git](https://git-scm.com/downloads) (using 2.43.0). This to handle some issues with IDF donwloading it's own Python and Git versions.

 - Step 4. Setup the [VSCode Extension for ESP32-IDF]((https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md)) and select `Existing Setup`. Put the corresponding paths in the text boxes and install. If any issues arise relating to Python/Pip or Git, delete the `C:\Espressif\tools\idf-python` or `C:\Espressif\tools\idf-git` folders and retry.

 - Step 5. Clone the repo `git clone https://github.com/ymich9963/WULPSC`

 - Step 6. Open the project folder in VSCode, press `CTRL + SHIFT + P` and type `>Add vscode configuration folder`. This is to add your own paths to the tools to build and flash the project.

 - Step 7. Have fun.