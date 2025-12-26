//ΛPscanner
#include "WiFi.h"

//define the auth types WiFi use
const char* wifi_auth_types[] = {

      "[0PEN]",
      "[WEP] V-W3AK",
      "[WPA]",
      "[WPA2]",
      "[WPA/WPA2]",
      "[WPA2-ENT]",
      "[WPA3]"
      };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  Serial.println("scan start ");
  Serial.println("   ⋆༺𓆩☠︎︎𓆪༻⋆ \n");
  
  int n = WiFi.scanNetworks(); // we cant use this to discover hidden networks hence the if statement is useless, i wonder how to scan for hidden networks? listen to prob requests ? 
  Serial.println("scan done");
  
  if (n==0){
    Serial.println("no networks found");
  }
  else {
    Serial.printf("𒉭%d networks found𒉭.\n " , n);
    
    for (int i = 0; i < n; i++){
      uint8_t auth_type= WiFi.encryptionType(i);
      const char* auth_type_str = (auth_type <= WIFI_AUTH_WPA3_PSK) ? wifi_auth_types[auth_type] : "[UNKN0WN]";

      Serial.printf("%d: %s ♱MAC: %s ♱RSSI: %d ♱CH: %d ♱Auth: %s ♱ \n",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.BSSIDstr(i).c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    auth_type_str);
      delay(10);
    }
  } 
  Serial.println(" "); // 0rganisation
  delay(5000);
}


