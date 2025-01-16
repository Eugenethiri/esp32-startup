#include <WiFi.h>

const char* ssid = "xxxxxxxx"; //max is 32 characters
const char* password = "12345678";  // this has to be 8 character else it resorts to default 

WiFiServer server(80);


void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);

  Serial.print("Setting AP:");
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  Serial.println("Server started");
}

void loop(){
  WiFiClient client = server.available();

  if (client){
    Serial.println("new client connected");

    String request = client.readString();
    Serial.println(request);

    // Send an HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    // Send a simple HTML page
    client.println("<html><body>");
    client.println("<h1>Hello from MArs!</h1>");
    client.println("<p>Welcome to my ESP32 Access Point.</p>");
    client.println("</body></html>");

    // Close the client connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
