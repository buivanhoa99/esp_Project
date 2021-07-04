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

class PcInfo{
  public : 
    int Id;
    String Name;
    String MacAddress; 
    PcInfo(){};
    PcInfo(int id,String name,String MAC){
      Id= id;
      Name = name;
      MacAddress = MAC;
    }
};
#define COUNT 4
PcInfo listPC [COUNT] ={
    PcInfo(0,"Hoá","4C:ED:FB:43:61:9D"),
    PcInfo(1,"Lên","E0:D5:5E:D1:DA:BC"),
    PcInfo(2,"Nguyễn","E0:D5:5E:D1:DA:BC"),
    PcInfo(3,"Khải","E0:D5:5E:D1:DA:BC")
  };

void wakeMyPC(String MAC) {
    WOL.setRepeat(3, 100);
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());

    WOL.sendMagicPacket(MAC);
    WOL.sendMagicPacket(MAC,7);
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
        String ONE = "<!DOCTYPE html>\
              <html lang='en'>\
              <head>\
                  <meta charset='UTF-8'>\
                  <meta >\
                  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
                  <link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css'>\
                  <link rel='stylesheet' href='https://pro.fontawesome.com/releases/v5.10.0/css/all.css' />\
                  <title>2CS Webpage</title>\
              </head>\
              <body>\
                  <h1 style='text-align: center;'>List of 2CS's computers</h1>\
                  <div class='w-50 mx-auto'>\
                  <table class='table table-hover'>\
                      <thead>\
                        <tr>\
                          <th>#</th>\
                          <th scope='col'>Name</th>\
                          <th scope='col'>MAC Address</th>\
                          <th scope='col'>Status</th>\
                          <th></th>\
                        </tr>\
                      </thead>\
                      <tbody>";
          String TWO="";
          for (int i=0;i<COUNT;i++)
              TWO = TWO + "<tr>\
                <th scope='row'>"+String(i+1)+"</th>\
                <td>"+listPC[i].Name+"</td>\
                <td>"+listPC[i].MacAddress+"</td>\
                <td>\
                    <a href='/ON?id="+listPC[i].MacAddress+"'><i style='font-size: 40px;' class='fas fa-power-off'></i>\
                    </a>\
                </td>\
              </tr>";
   
          String THREE = "</tbody>\
                </table>\
              </div>\
          </body>\
          </html>";
         server.send(200,"text/html",ONE+TWO+THREE);
    });


  server.on("/ON",[]{
        String id_string = server.arg("id");
        int id = id_string.toInt();
        wakeMyPC(listPC[id].MacAddress);
        server.send(200,"text/html","<h1>Power on "+ listPC[id].Name + "successfully</h1>"  );
    });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
