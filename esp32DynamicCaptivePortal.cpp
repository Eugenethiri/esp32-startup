/* issues:
no initial connection with any devices

no captive portal
research https://github.com/nthnn/DynaConfig/blob/main/src/dynaconfig.cpp >> had a whole new library for this 
Compiling issue ,for more read>>  https://github.com/me-no-dev/ESPAsyncWebServer/issues/464
might have to use previous libraries

error:
C:/Users/morar/AppData/Local/Arduino15/packages/esp32/tools/esp-x32/2405/bin/../lib/gcc/xtensa-esp-elf/13.2.0/../../../../xtensa-esp-elf/bin/ld.exe: C:\Users\morar\AppData\Local\arduino\cores\304856845982640bb0ab8356b5381a20\core.a(main.cpp.o):(.literal._Z8loopTaskPv+0xc): undefined reference to `setup()'
C:/Users/morar/AppData/Local/Arduino15/packages/esp32/tools/esp-x32/2405/bin/../lib/gcc/xtensa-esp-elf/13.2.0/../../../../xtensa-esp-elf/bin/ld.exe: C:\Users\morar\AppData\Local\arduino\cores\304856845982640bb0ab8356b5381a20\core.a(main.cpp.o): in function `loopTask(void*)':
C:\Users\morar\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.1.3\cores\esp32/main.cpp:56:(.text._Z8loopTaskPv+0x1c): undefined reference to `setup()'
collect2.exe: error: ld returned 1 exit status

exit status 1

Compilation error: exit status 1

 

this is gonna be annoying :(
*/


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
