#include <WiFi.h> // connect to wifi
#include <WiFiClientSecure.h> //im guessing the secure https connection
#include <ArduinoJson.h> // im guessing for arduino 

// Telegram declarations

String botToken = "ndnd";
const char* telegramHost = "api.telegram.org";


// https client
WiFiClientSecure client;

String lastUpdateId= "0";

unsigned long lastUpdate = 0;

// send message function
void sendMessage(long chat_id, String text) {
  if (!client.connect(telegramHost, 443)) return;

  String url = "/bot" + botToken +
               "/sendMessage?chat_id=" + String(chat_id) +
               "&text=" + text;

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + telegramHost + "\r\n" +
               "Connection: close\r\n\r\n");
}

// updates function
void getUpdates() {
  if (!client.connect(telegramHost, 443)) {
    Serial.println("Connection failed");
    return;
  }

  // build Telegram API URL
  String url = "/bot" + botToken + "/getUpdates?offset=" + lastUpdateId;

  // send HTTPS GET request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + telegramHost + "\r\n" +
               "Connection: close\r\n\r\n");

  // read headers until blank line
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  // read JSON body
  String payload;
  while (client.available()) {
    payload += client.readString();
  }

  Serial.println(payload);

  // parse JSON
  parseMessage(payload);
}

void parseMessage(String payload) {
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, payload);

  JsonArray result = doc["result"];

  // loop through each update
  for (JsonObject msg : result) {

    // update offset for next read
    lastUpdateId = String(msg["update_id"].as<int>() + 1);

    long chat_id = msg["message"]["chat"]["id"];
    String text = msg["message"]["text"];

    Serial.printf("Message from %ld: %s\n", chat_id, text.c_str());

    // simple command reactions
    if (text == "/on")  sendMessage(chat_id, "Turning ON!");
    if (text == "/off") sendMessage(chat_id, "Turning OFF!");
  }
}



void setup() {
  Serial.begin(115200);

  WiFi.begin("yourmom","yourmom@123");
  while(WiFi.status() != WL_CONNECTED) delay(1000);

  client.setInsecure();
}

void loop() {
  if (millis() - lastUpdate > 4000){
    getUpdates();
    lastUpdate = millis();
  }
}
