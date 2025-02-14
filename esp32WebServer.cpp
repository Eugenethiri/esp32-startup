#include <WiFi.h>

// ;LISTEN UP!!  
const char* ssid = "xxxxxxxx"; //max is 32 characters
const char* password = "12345678";  // this has to be 8 character else it resorts to default 

WiFiServer server(80);

void setup(){
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    Serial.print("Setting up Ap ;D");
    
    Serial.println("Ap Setup done ;D ;D ;D");
    Serial.print("Ap ip address:");
    Serial.println(WiFi.softAPIP());
    
    server.begin();
}

void loop(){
    WiFiClient client = server.available(); //wait for connection tobe made 2 ap

    if (client) {
        Serial.println("New victim connected");

        //wait for request
        String request = "" ;
        while (client.available()){
            char c = client.read();
            request += c;
        }
        Serial.print(request);
        
   /* Simple HTML to warn any connected users, if it's an open network that is :D, and public networks. Educates incase of evil portal attack
   to be tested in the public, nxt is the captive portal with this implementation warning

   Now we send a response to allow the html file 
    */
          client.println(R"rawliteral(
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
</body></html>)rawliteral");

        client.stop(); // manually close the connection 
        Serial.println("Disconnected lil bro");
    }
}
