//Ref: https://iotespresso.com/create-captive-portal-using-esp32/ ;) smart 

/*
  To Do 
  client status handling function "client connected, disconnected etc" done but now i want a counter for victims
  https://github.com/CDFER/Captive-Portal-ESP32/blob/main/src/main.cpp  >> will upgrade to bros later when i get better exp on  platform.ino
  
    
  */
 
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
HTTP/1.1 200 OK
Content-Type: text/html
Connection: close

<html><head><style>
body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; padding: 20px; }
h1 { color: red; }
ul { list-style-type: none; padding: 0; }
li { margin: 10px 0; }
strong { color: #444; }
em { font-style: italic; color: #555; }
</style></head><body>
<h1>Warning: Public WiFi is Risky!</h1>
<p><strong>Security Risks of Using Public WiFi:</strong></p>
<ul>
<li><strong>Man-in-the-middle attacks:</strong> Hackers can intercept your communications.</li>
<li><strong>Data theft:</strong> Personal and sensitive data can be easily stolen.</li>
<li><strong>Fake networks:</strong> Attackers can set up rogue WiFi hotspots to impersonate legitimate networks.</li>
<li><strong>Malware:</strong> Connecting to unsecured networks can expose you to malware or ransomware attacks.</li>
</ul>
<p><strong>What can you do to stay safe?</strong></p>
<ul>
<li>Use a Virtual Private Network (VPN) to encrypt your connection.</li>
<li>Avoid accessing sensitive information (e.g., banking sites) on public WiFi.</li>
<li>Verify the authenticity of the WiFi network before connecting.</li>
<li>Enable two-factor authentication on your online accounts for added security.</li>
</ul>
<p><em>Stay Safe, Stay Secure!</em></p>
<p><mark> Protected by your friendly neighborhood friend TK<sup><3</sup> </mark></p>
</body></html>)rawliteral";

/* 

** UNDER CONSTRACTION **
std::vector<String> connectedMACs; // Store MAC addresses

void checkNewClients() {
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList); // Get list of connected stations

    for (int i = 0; i < stationList.num; i++) {
        // Extract MAC address
        uint8_t *mac = stationList.sta[i].mac;
        char macStr[18];
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        String macAddress = String(macStr);

        // Check if it's a new connection
        if (std::find(connectedMACs.begin(), connectedMACs.end(), macAddress) == connectedMACs.end()) {
            connectedMACs.push_back(macAddress);
            Serial.print("Victim Connected: ");
            Serial.println(macAddress); // Print new MAC address
        }
    }
}


*/

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler(){}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  }
};

void setupServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html); 
      Serial.println("Victim Connected"); 
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
    Serial.println("Victim Connected");
    
  });
}

void setup(){
  //your other setup stuff...
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("youreCooked","12345678" );
  Serial.print("AP IP address: ");Serial.println(WiFi.softAPIP());
  Serial.println("Setting up Async WebServer");
  setupServer();
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
  Serial.println("All Done!");
}

  
void loop(){
  dnsServer.processNextRequest();
}
