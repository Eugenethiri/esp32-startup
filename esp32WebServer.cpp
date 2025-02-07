#include <WiFi.h>

// ;LISTEN UP!!  
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
    
   // Simple HTML to warn any connected users, if it's an open network that is :D, and public networks. Educates incase of evil portal attack
  // to be tested in the public, hopefully it makes an impact
    
    client.println("<html><head><style>");
    client.println("body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; padding: 20px;}");
    client.println("h1 { color: red; }");
    client.println("ul { list-style-type: none; padding: 0; }");
    client.println("li { margin: 10px 0; }");
    client.println("strong { color: #444; }");
    client.println("em { font-style: italic; color: #555; }");
    client.println("</style></head><body>");
    client.println("<h1>Warning: Public WiFi is Risky!</h1>");
    client.println("<p><strong>Security Risks of Using Public WiFi:</strong></p>");
    client.println("<ul>");
    client.println("<li><strong>Man-in-the-middle attacks:</strong> Hackers can intercept your communications.</li>");
    client.println("<li><strong>Data theft:</strong> Personal and sensitive data can be easily stolen.</li>");
    client.println("<li><strong>Fake networks:</strong> Attackers can set up rogue WiFi hotspots to impersonate legitimate networks.</li>");
    client.println("<li><strong>Malware:</strong> Connecting to unsecured networks can expose you to malware or ransomware attacks.</li>");
    client.println("</ul>");
    client.println("<p><strong>What can you do to stay safe?</strong></p>");
    client.println("<ul>");
    client.println("<li>Use a Virtual Private Network (VPN) to encrypt your connection.</li>");
    client.println("<li>Avoid accessing sensitive information (e.g., banking sites) on public WiFi.</li>");
    client.println("<li>Verify the authenticity of the WiFi network before connecting.</li>");
    client.println("<li>Enable two-factor authentication on your online accounts for added security.</li>");
    client.println("</ul>");
    client.println("<p><em>Stay Safe, Stay Secure!</em></p>");
    client.println("<p> <mark> Protected by your friendly neighboorhood friend TK<sup><3</sup> </mark> </p>");
    client.println("</body></html>");

    // Close the client connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
