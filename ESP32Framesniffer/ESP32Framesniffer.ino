/*Promiscuous packet sniffer grouping frames
  Foundation for deeper sniffing& analyzing  
 */

#include <WiFi.h>  //standard 4 wifi operations
#include <esp_wifi.h> // Promiscuous mode

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



//callback function to handle all the processing
void bigNose(void* buf, wifi_promiscuous_pkt_type_t  type){
  wifi_promiscuous_pkt_t* packet =(wifi_promiscuous_pkt_t*)buf; //pointer storing packet fields

  if(!packet) return; // error checker if not null
  uint8_t* raw_payload = packet->payload; // parase contents as it is
  int rssi = packet->rx_ctrl.rssi; // power signal 
  int chan = packet->rx_ctrl.channel; // channel

  ieee80211_mac_hdr_t* hdr = (ieee80211_mac_hdr_t*) raw_payload;

  // extract frame  control fields
  uint16_t fc = hdr->frame_ctrl;
  uint8_t type_field = (fc >> 2) & 0x3; 
  uint8_t subtype = (fc >> 4) & 0xF;
  bool toDS = fc & (1 << 8); // to AP
  bool fromDS = fc & (1 << 9); //from AP

  //Type field 
  const char* typeStr;
  switch (type_field){
    case 0 : 
    typeStr = "Mgt";
    break;

    case 1 : 
    typeStr = "Ctrl"; 
    break;

    case 2 :
    typeStr = "Data"; 
    break;

    default : 
    typeStr = "RSVD"; 
    break;
  }
  String subtypeStr = String(subtype); // fallback to numeric string
  if (type_field == 0) { // Management frame
    switch (subtype) {
      case 0:  subtypeStr = "Assoc Req";  break;
      case 1:  subtypeStr = "Assoc Resp"; break;
      case 2:  subtypeStr = "Reassoc Req";break;
      case 3:  subtypeStr = "Reassoc Resp";break;
      case 4:  subtypeStr = "Probe Req";  break;
      case 5:  subtypeStr = "Probe Resp"; break;
      case 8:  subtypeStr = "Beacon";     break;
      case 10: subtypeStr = "Auth";       break;
      case 11: subtypeStr = "Deauth? check"; break; // common to be 12, leave fallback
      case 12: subtypeStr = "Deauth";     break;
      default: subtypeStr = "Mgmt Other"; break;
    }
  }
 

  Serial.printf("[CH %02d] RSSI: %d dBm | Type: %s Subtype: %s | toDS:%d fromDS:%d\n",
              chan, rssi, typeStr, subtypeStr.c_str(), toDS, fromDS);

  Serial.printf("  DA:%s  SA:%s  BSSID:%s\n",
                macToStr(hdr->addr1).c_str(),
                macToStr(hdr->addr2).c_str(),
                macToStr(hdr->addr3).c_str());
  Serial.println();
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
    delay(500);
  }
}