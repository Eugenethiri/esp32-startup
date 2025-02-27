/*
probe scanner

doent work after users connect to an ap, need to expand on the packets sniffed

*/

#include "WiFi.h"
#include "esp_wifi.h"

// Callback function to handle received packets
void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (type == WIFI_PKT_MGMT) { // Only process management packets
        const wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
        const uint8_t* payload = pkt->payload;

        // Check if the packet is a Probe Request (subtype 0x40) [need to dive into more packets and see what info i can get from there]
        if ((payload[0] & 0xF0) == 0x40) {
            Serial.println("Probe Request Detected!");

            // Extract and print the source MAC address
            Serial.print("Source MAC: ");
            for (int i = 10; i < 16; i++) { // Source MAC starts at byte 10
                Serial.printf("%02X", payload[i]);
                if (i < 15) Serial.print(":");
            }
            Serial.println();

            // Extract and print the SSID if present
            int ssidLength = payload[25]; // SSID length is at byte 25
            if (ssidLength > 0 && ssidLength <= 32) { // Valid SSID length
                Serial.print("Probed SSID: ");
                for (int i = 0; i < ssidLength; i++) {
                    Serial.print((char)payload[26 + i]); // SSID starts at byte 26
                }
                Serial.println();
            } else {
                Serial.println("Probed SSID: [Hidden or Empty]");
            }

            Serial.println("---------------------------------");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing Wi-Fi Sniffer...");

    // Set Wi-Fi mode to station (non-connected)
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();

    // Enable promiscuous mode and set the callback function
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&snifferCallback);

    Serial.println("Wi-Fi Sniffer Started!");
}

void loop() {
    // The main loop does not need to do anything
    // All processing happens in the snifferCallback
    delay(1000);
}

/* to do 
this only uses probe request, if the client is already connected this wont work. 
>> add other frames for sniffing the mac address, ssid and other information in the packet 
*/
