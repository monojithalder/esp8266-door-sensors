
#include <stdlib.h>
#include <RestClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include<SoftwareSerial.h>

SoftwareSerial s(2,3);
ESP8266WebServer server(80);

const char* ssid     = "WEBTECHLINK";
const char* password = "web2013techlink";

IPAddress ip(192,168,1, 153);
IPAddress gateway(192,168,1,1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0);
int receiveddata =0;
int password_address = 20;

void setup() {
    EEPROM.begin(512);
  //eeWriteInt(2,30);
  int i =0;
  for(i=0;i<=10;i++) {
    eeWriteInt(i,0);
  }
  eeWriteInt(password_address,-1);
  
  WiFi.mode(WIFI_AP);
  //WiFi.softAPConfig(esp_ip, esp_ip, subnet);        // declared as: bool softAPConfig (IPAddress local_ip, IPAddress gateway, IPAddress subnet)
  WiFi.softAP("Ayan_Door_Sensor", "password", 7);
  
  server.on("/",indexPage);
  server.on("change-password",changePassword);
  server.begin();
  delay(500);
  
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  s.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void indexPage() {
  if(EEPROM.read(password_address) == -1) {
    server.sendHeader("Location", "change-password", true);
    server.send ( 302, "text/plain", "");
  }
  else {
    String html;
    html = "<html><head><title>Door Sensors</title></head><body>";
    html += "<div style=\"width:100%;margin-right:auto;margin-left:auto;\">";
    html += "<form action=\"/myaccount\" method=\"post\">";
    html += "<input type=\"password\" name=\"password\">";
    html += "<input type=\"submit\" value=\"Enter\">";
    html += "<div></form>";
    String password;
    password =eeGetInt(password_address);
    html += "<lable>"+password+"</lable>";
    html += "</body></html>";
    server.send(200, "text/html", html); 
  }
}

void changePassword() {
  String html;
  html = "<html><head><title>Door Sensors | Change Password</title></head><body>";
  html += "<div style=\"width:100%;margin-right:auto;margin-left:auto;\">";
  html += "<form action=\"/myaccount\" method=\"post\">";
  html += "<input type=\"password\" name=\"password\">";
  html += "<input type=\"submit\" value=\"Enter\">";
  html += "<div></form>";
  String password;
  password = EEPROM.read(password_address);
  html += "<lable>Change Password "+ password +"</lable>";
  html += "</body></html>";
  server.send(200, "text/html", html);   
}

void eeWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}
