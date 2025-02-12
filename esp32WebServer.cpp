#include <WiFi.h>
#include <DNSServer.h>

// ;LISTEN UP!!  
const char* ssid = "xxxxxxxx"; //max is 32 characters
const char* password = "12345678";  // this has to be 8 character else it resorts to default 

WiFiServer server(80);
DNSServer dnsServer;  // Creates a DNS server instance yk domain name "no" server

void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  Serial.print("Setting AP:");
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(53, "*", WiFi.softAPIP()); //dns hijack to port 53, all web querrys, to the esps ip address

  server.begin();
  Serial.println("Server started");
}


void loop() {
    dnsServer.processNextRequest();  // Handle DNS requests

    WiFiClient client = server.available();
    if (client) {
        Serial.println("New client connected");

        String request = client.readString();
        Serial.println(request);

      /*
      TO DO
      1: would be to just send a black page telling thje captive portal url ther is no wifi yk you request you dont get the "success" s
      2: or redirect it like so (might add the second option as a pocket knife when i cant reload or outta bullets)

          client.println("HTTP/1.1 302 Found");
          client.println("Location: http://192.168.4.1/");
          client.println();

          problem is the 302 redirection may not be followed by sone devices
      3: TRY implementing SPIFF, i dont like how im adding the whole html on this script gotta add it to /spiff 
      orr
      3.5: memory card module way better 
      speaking of memory esp to listen to music :D
      
      
      */

        // Handle Captive Portal Detection (Apple, Android, Windows)
        if (request.indexOf("generate_204") > 0 || 
            request.indexOf("hotspot-detect.html") > 0 || 
            request.indexOf("ncsi.txt") > 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Length: 0");
            client.println();
            client.stop();
            return;
        }

        // Serve Warning Page
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html"); // just empty with no content 
        client.println("Connection: close");
        client.println("Cache-Control: no-cache, no-store, must-revalidate");
        client.println("Pragma: no-cache");
        client.println("Expires: 0");
        client.println();

  
        // Security Warning Page (Your Custom HTML)
        client.println("<html><head><style>");
        client.println("body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; padding: 20px; }");
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
        client.println("<p><mark>Protected by your friendly neighborhood friend TK<sup><3</sup></mark></p>");
        client.println("</body></html>");

        client.stop();
        Serial.println("Client disconnected");
    }
}
