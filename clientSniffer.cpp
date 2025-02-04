#include "WiFi.h"

void packetSniffer(void* buf, wifi_promiscuous_pkt_type_t type){ 
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;

if (type == WIFI_PKT_MGMT){
  Serial.println("Management frame captured"); //4 testing lets scan everything
  Serial.print("packet data");

  for (int i = 0; i< pkt->payload_len; i++){
    Serial.print(pkt->payload[i], HEX);
    Serial.print(" "); //some space for organisation
    }
    Serial.println();
    }
}

void setup(){
  Serial.begin(115200); //listen here to the serial monitor 
  WiFi.mode(WIFI_STA);  //station
  WiFi.disconnect();
  delay(1000); //short break coz everyone needs them

//Setting up the listener settings
  esp_wifi_set_promiscuous(true);
  wifi_promiscuous_set_rx_cb(packetSniffer); //scan specific frame (management frames only);
}

void loop(){
  delay (1000); //short delay in ms to keep it running 
}

