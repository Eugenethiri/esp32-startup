Each folder in this repo contains a .ino that performs a straight forwad task (i.e set up a webserver ). I use this a a quick start up point when i develop any ESP related task. 

# After purchase setup:
* Arduino IDE, go to File> Preferences ... Enter the following into the “Additional Board Manager URLs” field:
    https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
* CP2102 chip, get the VCOM port drivers from silicons own site as windows doesnt come preinstalled with the divers @ https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
* Open the Boards Manager. Go to Tools > Board > Boards Manager… gethe esp32 from Espressif download & DOIT ESP32 DEVKIT1

## Resourcres:
    https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino
great place to learn more about your arduino :)
