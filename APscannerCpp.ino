//scanner
#include "WiFi.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("scan start ");
  Serial.println("   ⋆༺𓆩☠︎︎𓆪༻⋆");
  Serial.println(" ");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n==0){
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found.");
    for (int i = 0; i<n; ++i){
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" | MAC address: ");
      Serial.print(WiFi.BSSIDstr(i));
      Serial.print(" | AP STRENGTH: ");
      Serial.print("(");
      Serial.print(WiFi.RSSI(i));
      Serial.print(") | ");
      Serial.print("Channel: ");
      Serial.print(WiFi.channel(i));
      Serial.print(" | "); //more organised 

      if (WiFi.SSID(i).length() == 0 ) {
        Serial.print("  ++HIDDEN SSID++");
       }

      switch (WiFi.encryptionType(i)){
          case WIFI_AUTH_OPEN:
              Serial.println("**[0PEN N3TWORK]**");
              break;
          case WIFI_AUTH_WEP:
              Serial.println("**[WEP PROTECTION] ULTRA W3AK**");
              break;
          case WIFI_AUTH_WPA_PSK:
              Serial.println("**[WPA PROTECTION]**");
              break;
          case WIFI_AUTH_WPA2_PSK:
              Serial.println("**[WPA2 PR0TECTI0N]**");
              break;
          case WIFI_AUTH_WPA_WPA2_PSK:
              Serial.println("**[WPA/WPA2 PR0TECTION]**");
              break;
          case WIFI_AUTH_WPA2_ENTERPRISE:
             Serial.println("**[WPA2-3NTERPRISE PROTECTION]**");
             break;
          case WIFI_AUTH_WPA3_PSK:
             Serial.println("**[WPA3 PROTECTION]**");
             break;
          default:
             Serial.println("**[UNKN0WN]**");
             break;
      }
      delay(10);
    }
  }
  Serial.println("");
  delay(5000);
}
