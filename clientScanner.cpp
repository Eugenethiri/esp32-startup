//simple frame/packet sniffer  [Untested]

#include <WiFi.h>  //standard 4 wifi operations
#include <esp_wifi.h> // Promiscuous mode

//callback function to handle all the processing
void bigNose(void* buf, wifi_promiscuous_pkt_type_t  type){
  wifi_promiscuous_pkt_t* packet =(wifi_promiscuous_pkt_t*)buf; //gives the packet a packet structure 
  uint8_t* payload = packet->payload; //allows us to manipulate or view different parts of the packet
  int rssi = packet->rx_ctrl.rssi; // extract the power signal in packet

  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.print(" | Frame Type: ");

  
  uint8_t frameType = payload[0] & 0xF0; // need better understanding on using & for filtering
  if (frameType == 0x00){
    Serial.print("Management | ");
  }
  else if (frameType == 0x04 || frameType == 0x05){
    Serial.print("Control | ");
  }
  else if (frameType == 0x08){
    Serial.print("Data | ");
  } 
  else {
    Serial.print("++Unknown++");
  }

  /*extract MAC (dst) 
  get bytes from payload[4-9]
  else the other payload[10-15]
  */
  Serial.print(" | Destination Mac: ");
  for(int i = 4; i<10; i++){
    Serial.printf("%02X", payload[i]); //%02X makes the hexadecimal format, 02>> hexaformat
    if(i<9) Serial.print(":");
  }
  
  Serial.print(" | Source Mac: ");
  for(int i = 10; i<16; i++){
    Serial.printf("%02X", payload[i]);
    if (i<15) Serial.print(":");
  }
  
    // Extract SSID (only for Management Frames like Beacons & Probe Requests) the rest we only get macs 
  if (frameType == 0x00) { 
      int ssidOffset = 36; // SSID starts at byte 36
      int ssidLength = payload[ssidOffset]; // First byte at offset gives SSID length

      Serial.print(" | SSID: ");
      if (ssidLength > 0 && ssidLength <= 32) { 
          for (int i = 0; i < ssidLength; i++) {
              Serial.print((char)payload[ssidOffset + 1 + i]); 
          }
      } 
      else { 
          Serial.print("--Hidden--"); 
      }
  }

  Serial.println(); 
}
  
  
void setup(){
  Serial.begin(115200);
  Serial.println("General BigNose sniffing ^ܠ^ ");

  WiFi.mode(WIFI_MODE_STA); //station mode 
  WiFi.disconnect(); //if connected it will x itself from the network
  
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&bigNose);  
}

void loop(){
  for(int i = 1; i <=13; i++){
    esp_wifi_set_channel(i, WIFI_SECOND_CHAN_NONE);      
    Serial.print("Switching to channel:");
    Serial.println(i);
    delay(200);
  }
}
