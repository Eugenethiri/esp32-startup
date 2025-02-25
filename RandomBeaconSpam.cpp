/* 
ref:// https://github.com/justcallmekoko/ESP32BeaconSpam/blob/master/ESP32BeaconSpam.ino#L15
To do 
Untested till i get home or whenever i wanna test 
*/

// Libraries
#include "WiFi.h"

extern "C" {
#include "esp_wifi.h"
    esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
    esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

// Character set for random SSID generation
static const char alphanum[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

// Predefined SSID prefix
const char* prefix = "YourTag";

// Packet Buffer
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, // Frame Control, Duration
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination address 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source address - overwritten later
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID - overwritten later
                /*22*/  0xc0, 0x6c, // Seq-ctl
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
                /*32*/  0x64, 0x00, // Beacon interval
                /*34*/  0x01, 0x04, // Capability info
                /*36*/  0x00 // Placeholder for SSID Length
                };

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);
    Serial.println("PREPARING THE STUFF( -_•) ▄︻デ══━一💥");

    WiFi.mode(WIFI_AP_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_max_tx_power(82);
}

void broadcastSSID() {
    int result;
    uint8_t channel = random(1, 13); // Declare channel variable
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    delay(10);  // Reduce delay to improve efficiency

    // Randomize Source MAC and BSSID
    for (int i = 0; i < 6; i++) {
        uint8_t rnd = random(256);
        packet[10 + i] = rnd;
        packet[16 + i] = rnd;
    }

    // Generate a random SSID prefix
    int prefixLenRandom = random(6, 10);  // Random length between 6 and 9 bytes
    int tagLen = strlen(prefix);          // Fixed tag length
    int fullSSIDLen = prefixLenRandom + tagLen;
    packet[37] = fullSSIDLen; // Set SSID length

    // Insert random prefix
    for (int i = 0; i < prefixLenRandom; i++) {
        packet[38 + i] = alphanum[random(sizeof(alphanum) - 1)];
    }

    // Insert fixed tag (using memcpy)
    memcpy(&packet[38 + prefixLenRandom], prefix, tagLen);

    // Insert channel info
    packet[50 + fullSSIDLen] = channel;

    // Append post-SSID data
    const uint8_t postSSID[12] = {
        0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
        0x03, 0x01
    };
    memcpy(&packet[38 + fullSSIDLen], postSSID, sizeof(postSSID));

    // Transmit the beacon 5 times
    for (int i = 0; i < 5; i++) {
        result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
    }
}

void loop() {
    broadcastSSID();
    delay(1);
}
