#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <WakeOnLan.h>

WiFiUDP UDP;
WakeOnLan WOL(UDP);
#ifndef STASSID
#define STASSID "2Click Solutions_2.4G"
#define STAPSK  "DevOps735"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);


void wakeMyPC() {
    WOL.setRepeat(3, 100);
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());

    
    const char *MACAddress = "4C:ED:FB:43:61:9D";
    WOL.sendMagicPacket(MACAddress);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", []{
        String html = "<!DOCTYPE html>\
          <html lang='en'>\
          <head>\
              <meta charset='UTF-8'>\
              <meta http-equiv='X-UA-Compatible' content='IE=edge'>\
              <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
              <title>\Document</title>\
          </head>\
          <body>\
              <h1>\Hello bui van</h1>\
              <a href ='/ON'>\<button>\Hoa</button>\</a>\
          </body>\
          </html>";
         server.send(200,"text/html",html);
         
    });


  server.on("/ON",[]{
        wakeMyPC();
        server.send(200,"text/html","Successful");
    });



  server.onNotFound(handleNotFound);

 

  // Hook examples
  /////////////////////////////////////////////////////////

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
