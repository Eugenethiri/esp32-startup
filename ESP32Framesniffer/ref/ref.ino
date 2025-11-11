#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"

// Helper: format MAC to string
String macToStr(const uint8_t *mac){
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

// Minimal 802.11 MAC header (packed)
typedef struct {
  uint16_t frame_ctrl; // type of frame, gonna need this to output type if data or managememnt and what type of management
  uint16_t duration_id;  // lifespan
  uint8_t addr1[6]; //sender
  uint8_t addr2[6];  // ap address
  uint8_t addr3[6];  // ap bssid same as above
  uint16_t seq_ctrl;
} __attribute__((packed)) ieee80211_mac_hdr_t; // intresting bit is the packed 

// Promiscuous callback (safe parsing)
void bigNose(void* buf, wifi_promiscuous_pkt_type_t type){
  // buf is actually a pointer to wifi_promiscuous_pkt_t (intresting what does this mean stores the data into memory location)
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*) buf;
  if (!pkt) return; // is this an error checker?

  // Pointer to raw 802.11 frame start
  uint8_t* raw = pkt->payload; // we say variable raw is the contents of payload, also what size is uint8_t 
  // Some SDK variants provide a length in rx_ctrl.sig_len or pkt->rx_ctrl.sig_len
  int rssi = pkt->rx_ctrl.rssi;
  int chan = pkt->rx_ctrl.channel;
  int sig_len = pkt->rx_ctrl.sig_len; // might be 0 on some SDKs

  // Quick sanity: ensure we at least have space for a MAC header
  // Note: we can't reliably know the available payload length from all SDKs,
  // so keep parsing minimal and defensive.
  if (!raw) return;

  // Cast to header (safe only if enough bytes; we assume typical frames)
  ieee80211_mac_hdr_t* hdr = (ieee80211_mac_hdr_t*) raw;

  // Extract frame control fields (little-endian). Use bit masks.
  uint16_t fc = hdr->frame_ctrl;
  uint8_t type_field = (fc >> 2) & 0x3;
  uint8_t subtype = (fc >> 4) & 0xF;
  bool toDS = fc & (1 << 8);
  bool fromDS = fc & (1 << 9);

  Serial.printf("[CH %02d] RSSI: %d dBm | Type: %u Subtype: %u | toDS:%d fromDS:%d\n",
                chan, rssi, type_field, subtype, toDS, fromDS);

  // Print MAC addresses (addr1 = DA, addr2 = SA, addr3 = BSSID for many frames)
  Serial.printf("  DA:%s  SA:%s  BSSID:%s\n",
                macToStr(hdr->addr1).c_str(),
                macToStr(hdr->addr2).c_str(),
                macToStr(hdr->addr3).c_str());

  // Sequence control
  uint16_t seq_ctrl = hdr->seq_ctrl;
  uint16_t seq_num = (seq_ctrl >> 4);
  uint8_t frag_num = seq_ctrl & 0xF;
  Serial.printf("  Seq:%u Frag:%u SigLen:%d\n", seq_num, frag_num, sig_len);

  // Example: if management beacon (type=0, subtype=8) try to read SSID IE safely
  if (type_field == 0 && subtype == 8) {
    // Management body usually starts immediately after the MAC header.
    // The MAC header above is 24 bytes; ensure not to access dangerously far.
    const size_t hdr_len = sizeof(ieee80211_mac_hdr_t);
    uint8_t* mgmt = raw + hdr_len;
    // Beacon fixed fields: timestamp (8) + beacon interval (2) + cap info (2) = 12 bytes
    uint8_t* ies = mgmt + 12;

    // We cannot trust an SDK-provided length always, so limit scan to a sane window:
    const int MAX_IE_SCAN = 200; // bytes
    int pos = 0;
    while (pos + 2 < MAX_IE_SCAN) {
      uint8_t id = ies[pos];
      uint8_t len = ies[pos + 1];
      if (id == 0) { // SSID IE
        if (len == 0) {
          Serial.println("  SSID: <hidden>");
        } else {
          char ssid[33] = {0};
          int copy = min((int)len, 32);
          memcpy(ssid, &ies[pos + 2], copy);
          Serial.printf("  SSID: %s\n", ssid);
        }
        break;
      }
      // Move to next IE. Guard against insane lengths.
      if (len > 200) break;
      pos += 2 + len;
    }
  }

  Serial.println(); // spacer for readability
}

// ---------- Setup and loop (Arduino IDE) ----------
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Starting ESP32 promiscuous sniffer...");

  // Ensure WiFi is in STA mode (promiscuous works well in STA)
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(); // don't connect to anything

  // Disable WiFi power save so radio remains active
  esp_wifi_set_ps(WIFI_PS_NONE);

  // Register callback first, then enable promiscuous mode
  esp_wifi_set_promiscuous_rx_cb(&bigNose);

  // Enable promiscuous mode
  esp_wifi_set_promiscuous(true);

  // Start on channel 1 (you can change)
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.println("Promiscuous mode enabled. Channel hopper running in loop().");
}

void loop() {
  // Simple channel hopper to capture across channels
  static uint8_t channel = 1;
  static unsigned long last = 0;
  const unsigned long CH_HOP_MS = 500; // channel dwell in ms

  unsigned long now = millis();
  if (now - last >= CH_HOP_MS) {
    last = now;
    channel++;
    if (channel > 13) channel = 1; // 1..13 for most regions; adjust if you need 1..11
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    // small debug print
    Serial.printf("Switched to channel %u\n", channel);
  }

  // Keep loop responsive; nothing else needed here.
  delay(10);
}

/*for deauthentication 
ref
// --- DEAUTH handling: parse reason code from payload if present ---
if (type_field == 0 && subtype == 12) { // Deauthentication (Mgmt subtype 12)
  // compute header length (this minimal header layout is 24 bytes)
  const size_t header_len = sizeof(ieee80211_mac_hdr_t); // typically 24
  int payload_len = sig_len - header_len;
  if (payload_len >= 2) {
    uint8_t* body = raw + header_len; // start of frame body
    // reason is 2 bytes (little-endian)
    uint16_t reason = (uint16_t)body[0] | ((uint16_t)body[1] << 8);
    Serial.printf("  DEAUTH reason: %u\n", reason);

    // Optional: act on certain reason codes (example numeric checks)
    if (reason == 1) {
      Serial.println("   -> Reason 1: Unspecified (common)");
    }

*/
