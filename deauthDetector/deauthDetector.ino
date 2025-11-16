/*
Deauth sniffer & detector  

*/

#include <WiFi.h>
#include <esp_wifi.h>

//Custom headers for tracking packets
#include <set> 
#include <map>

//String2MAC
String macToStr(const uint8_t *mac){
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

//Define header information
typedef struct {
  uint16_t frame_ctrl; //type 0f frame
  uint16_t duration_id;
  uint8_t addr1[6]; // Destination 
  uint8_t addr2[6]; // sender
  uint8_t addr3[6]; //AP BSSID
  uint16_t seq_ctrl;
} __attribute__((packed)) ieee80211_mac_hdr_t;

// deauth frame

typedef struct {
  uint16_t reason_code;
} __attribute__((packed)) wifi_ieee80211_deauth_t;

typedef struct {
  ieee80211_mac_hdr_t header;
  wifi_ieee80211_deauth_t deauth;
} __attribute__((packed)) wifi_ieee80211_deauth_frame_t;

// Deauth tracking
struct DeauthSrcInfo {
  int count = 0;
  unsigned long lastSeen = 0;
  std::set<String> victims; //unique clients hit
};

std::map<String, DeauthSrcInfo> attackerDB;


// Evil twin setting

std::map<String, std::set<int>> bssidChannels; // BSSID -> channels seen

void checkEvilTwin(String bssid, int channel) {
  if (bssidChannels.count(bssid) == 0){ 
    bssidChannels[bssid].insert(channel); 
    return;
  }

  auto &channelsSeen = bssidChannels[bssid];

    if(channelsSeen.find(channel) == channelsSeen.end()){ //track and stop when duplicate ssid operate on the same channel

    Serial.println("\n Possible Evil Twin Detected!");
    Serial.print("BSSID: "); Serial.println(bssid);

    Serial.println("Previously seen on channels: ");
    for (int ch : channelsSeen) {
      Serial.print(ch);
      Serial.print(" ");
    }

    Serial.print("\nNow also seen on: ");
    Serial.println(channel);
    Serial.print(" ");


      channelsSeen.insert(channel);
  }
}

//callback function to handle all the processing
void bigNose(void* buf, wifi_promiscuous_pkt_type_t  type){

  if (type != WIFI_PKT_MGMT) return; 

  wifi_promiscuous_pkt_t* packet =(wifi_promiscuous_pkt_t*)buf; 
  if(!packet) return; // error checker if not null

  uint8_t* raw_payload = packet->payload; // parase contents as it is
  int rssi = packet->rx_ctrl.rssi; // power signal 2 be used later in the evil twin attack, twin essids ap with different rssi
  int chan = packet->rx_ctrl.channel; // channel might be used in the output but my embedded deauths on all channels without discrimination hahahaha
  int len = packet->rx_ctrl.sig_len; // parase real mgt frames and save memory
  if (len < sizeof(wifi_ieee80211_deauth_frame_t)) return;

  ieee80211_mac_hdr_t* hdr = (ieee80211_mac_hdr_t*) raw_payload;
  
  // Extract bssid  for EvilTwin detection
  String bssid = macToStr(hdr->addr3);

  checkEvilTwin(bssid, chan);
  

  // extract frame  control fields focusing on deauthentication 
  uint16_t fc = hdr->frame_ctrl;
  uint8_t subtype = (fc >> 4) & 0xF;


  // Frame control 
  if (subtype == 12) { //  0nly deauth frames

    String src = macToStr(hdr->addr2);
    String dst = macToStr(hdr->addr1);

    unsigned long now = millis();

    DeauthSrcInfo &info = attackerDB[src];

  
    info.count++;
    info.lastSeen = now;
    if (dst != "FF:FF:FF:FF:FF:FF" ) info.victims.insert(dst);

    wifi_ieee80211_deauth_frame_t* f = (wifi_ieee80211_deauth_frame_t*) raw_payload;
    uint16_t reasonCode = (f->deauth.reason_code);

    //Codes 1 & 0  being just unreasonable and obvious
    if (reasonCode == 0 || reasonCode == 1){
      Serial.println("\n Suspicious deauth reason(0/1) code:");
      Serial.print(reasonCode);
    }

    //Detect deauth flood
    // Detection logic
    if (info.count > 20 || info.victims.size() > 5) {
      Serial.println("\n DEAUTH FLOOD DETECTED!");
      Serial.print("spoofed attacker: "); Serial.println(src);
      Serial.print("Frames: "); Serial.println(info.count);
      Serial.print("RSSI: "); Serial.println(packet->rx_ctrl.rssi);
      Serial.print("Unique victims: "); Serial.println(info.victims.size());

      Serial.print("\n Victim list: ");
      for(const auto &v : info.victims) {
        Serial.print(" - ");
        Serial.println(v);
      }
    }
  } 
}
  
void setup(){
  Serial.begin(115200);
  Serial.println("General BigNose sniffing ^ܠ^ ");

  WiFi.mode(WIFI_MODE_STA); //station mode 
  WiFi.disconnect(); //if connected it will x itself from the network
  
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&bigNose);  
}

void loop(){
  for(int i = 1; i <=13; i++){
    esp_wifi_set_channel(i, WIFI_SECOND_CHAN_NONE);      
    Serial.print("CH:");
    Serial.println(i);
    delay(750);
  }
}