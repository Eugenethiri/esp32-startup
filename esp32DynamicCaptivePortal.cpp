//dynamic captive portal

//Define libraries & variables
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

// server related variables
DNSServer dnsServer;
AsyncWebServer server(80);

String user_name;
String proficiency;
bool name_received = false;
bool proficiency_received = false;

//store the HTML in flash memory instead of ram, y? because its huge & to save RAM
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Dynamic Captive Portal ;Demo</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>Captive Portal ;D</h3>
  <br><br>
  <form action="/get">
    <br>
    Name: <input type="text" name="name">
    <br>
    ESP32 Proficiency: 
    <select name = "proficiency">
      <option value=Beginner>Beginner</option>
      <option value=Advanced>Advanced</option>
      <option value=Pro>Pro</option>
    </select>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

//handle requests
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); //send_P is used instead of send, y coz its a big enough html page
  }
};

void setupServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html); 
      Serial.println("Client Connected");
  });

  //recive and send information
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String inputMessage;
      String inputParam;
  
      if (request->hasParam("name")) {
        inputMessage = request->getParam("name")->value();
        inputParam = "name";
        user_name = inputMessage;
        Serial.println(inputMessage);
        name_received = true;
      }

      if (request->hasParam("proficiency")) {
        inputMessage = request->getParam("proficiency")->value();
        inputParam = "proficiency";
        proficiency = inputMessage;
        Serial.println(inputMessage);
        proficiency_received = true;
      }
      request->send(200, "text/html", "The values entered by you have been successfully sent to the device <br><a href=\"/\">Return to Home Page</a>");
  });
}

void setup(){
  //your other setup stuff...
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("youreCook3d2", "12345678");
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
  if(name_received && proficiency_received){
      Serial.print("Victim captured: ");Serial.println(user_name);
      Serial.print("You have stated your proficiency to be ");Serial.println(proficiency);
      name_received = false;
      proficiency_received = false;
      Serial.println("We'll wait for the next client now: ");
    }
}

/* without espasync server (cant solve the library issue so we try killing the rat using poison instead)

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h> // 50/50 if i need to include this for my web capture 
#include  "FS.h"
#include  "SPIFFS.h"

WebServer server(80);
StaticJsonBuffer<234> jsonBuffer;

//creds
const char* ssidAPConfig = "youreC00ked";
const char* passAPConfig = "12345678";

//leaving this blank untill i make my own, what about yk hiw a page redirects you to another. ill need that now that i think about it there is no need to serve the html here if ill store it in the spiff, or the main page here and the sub other where, nah i prefer the first storing both at spiff. what do yo think gpt?
const char index_html[] PROGMEM = R"rawliteral( )rawliteral";)

//redirection
server.onNotFound([]() {
    server.sendHeader("Location", "http://your-captive-portal.local/", true);
    server.send(302, "text/plain", "");
});

//this only serves, do we need to listen? 
server.on("/", HTTP_GET, []() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
});

unfinished because there is no more red bull

void setup(){
  
}

void loop(){

}
 


*/
