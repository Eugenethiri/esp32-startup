/* 
ref:// https://github.com/justcallmekoko/ESP32BeaconSpam/blob/master/ESP32BeaconSpam.ino#L15
To do 
Untested till i get home or whenever i wanna test 

*/


//libraries
#include "WiFi.h"

extern "C"{
#include "esp_wifi.h"
 esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
  esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

static const char alphanum[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

//beacon frame

uint8_t my_packet[128] = { 0x80, 0x00, 0x00, 0x00, 
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                /*22*/  0xc0, 0x6c, 
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 
                /*32*/  0x64, 0x00, 
                /*34*/  0x01, 0x04, 
                /* SSID */
                /*36*/  0x00, 0x12, 0x72, 0x72, 0x72, 0x72, 0x72, 0x5f, 0x52, 0x65, 0x64, 0x54, 0x65, 0x61, 0x6d, 0x5f, 0x46, 0x61, 0x6b, 0x65,
                        0x01, 0x08, 0x82, 0x84,
                        0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 
                /*56*/  0x04};                       

// barebones packet
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, //Frame Control, Duration
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                /*22*/  0xc0, 0x6c, //Seq-ctl
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                /*32*/  0x64, 0x00, //Beacon interval
                /*34*/  0x01, 0x04, //Capability info
                /* SSID */
                /*36*/  0x00
                };

void setup(){
  Serial.begin(115200); //baudrate
  Serial.setTimeout(100);
  Serial.println("PREPARING THE STUFF( -_•) ▄︻デ══━一💥");
  WiFi.mode(WIFI_AP_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(82);

void broadcastSSID() {
    int result;
    
    // Select a random channel and set it
    channel = random(1, 13);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    delay(100);  // Check if this delay is needed or can be reduced/eliminated

    // Randomize SRC MAC using a loop (applies to both sets of addresses)
    for (int i = 0; i < 6; i++) {
        uint8_t rnd = random(256);
        packet[10 + i] = rnd;
        packet[16 + i] = rnd;
    }

    // Generate a random length for the SSID prefix
    int prefixLenRandom = random(6, 10);  // random length between 6 and 9 bytes
    int tagLen = strlen(prefix);           // length of your fixed tag
    int fullSSIDLen = prefixLenRandom + tagLen;
    packet[37] = fullSSIDLen;

    // Insert the random prefix (using loop)
    for (int i = 0; i < prefixLenRandom; i++) {
        packet[38 + i] = alfa[random(65)];
    }
    // Insert the fixed tag (using memcpy)
    memcpy(&packet[38 + prefixLenRandom], prefix, tagLen);

    // Insert the channel in the proper location
    packet[50 + fullSSIDLen] = channel;

    // Append the post-SSID data (note: original array is 13 bytes but only 12 bytes were copied)
    const uint8_t postSSID[12] = {
        0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
        0x03, 0x01  // copying 12 bytes; if 13 is needed, adjust accordingly
    };
    memcpy(&packet[38 + fullSSIDLen], postSSID, sizeof(postSSID));

    // Transmit the packet 5 times in a loop
    for (int i = 0; i < 5; i++) {
        result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
        // Optionally, check 'result' for errors here
    }
}

void loop(){
  broadcastSSID()
  delay(1);
  
} 
