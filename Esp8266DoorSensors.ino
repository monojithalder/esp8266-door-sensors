
#include <stdlib.h>
#include <RestClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include<SoftwareSerial.h>

//SoftwareSerial s(2,3);
ESP8266WebServer server(80);

const char* ssid     = "WEBTECHLINK";
const char* password = "web2013techlink";

IPAddress ip(192,168,1, 153);
IPAddress gateway(192,168,1,1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0);
int receiveddata =0;
int password_address = 20;
int login = 0;
int buzzer_address = 30;
int wifi_name_address= 50;
int wifi_password_address=100;
void setup() {
    EEPROM.begin(512);
  //eeWriteInt(2,30);
  int i =0;
  /*for(i=0;i<=10;i++) {
    eeWriteInt(i,0);
  }*/
  //eeWriteInt(password_address,-1); 
  /*String wifi_name = "TESTINGY";
  byte wifi_name_byte[wifi_name.length()+1];
  wifi_name.getBytes(wifi_name_byte, wifi_name.length()+1);
  for(i=0;i<=wifi_name.length()+1; i++) {
    EEPROM.write(50+i,wifi_name_byte[i]);
  }*/
  
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);  
  WiFi.mode(WIFI_AP);
  //WiFi.softAPConfig(esp_ip, esp_ip, subnet);        // declared as: bool softAPConfig (IPAddress local_ip, IPAddress gateway, IPAddress subnet)
  WiFi.softAP("TESTING", "password", 7);
  
  server.on("/",indexPage);
  server.on("/change-password",changePassword);
  server.on("/do-change-password",doChangePassword);
  server.on("/do-login",doLogin);
  server.on("/myaccount",myAccount);
  server.on("/on-buzzer",onBuzzer);
  server.on("/off-buzzer",offBuzzer);
  server.on("/logout",logout);
  server.on("/reboot",reboot);
  server.on("/wifi-settings",wifiSettings);
  server.on("/do-wifi-settings",doWifiSettings);
  server.on("/reset-all-memory",resetAllMemory);
  server.begin();
  delay(500);
  
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

}

void indexPage() {
  digitalWrite(2,HIGH);
  if(eeGetInt(password_address) == 0) {
    server.sendHeader("Location", "change-password", true);
    server.send ( 302, "text/plain", "");
  }
  else {
    String html;
    html = "<html><head><title>Door Sensors</title></head><body>";
    html += "<div style=\"width:100%;margin-right:auto;margin-left:auto;\">";
    html += "<form action=\"/do-login\" method=\"post\">";
    html += "<input type=\"password\" name=\"password\">";
    html += "<input type=\"submit\" value=\"Enter\">";
    html += "<div></form>";
    String password;
    byte wifi_name_byte[11];
    int i =0;
    for(i=0;i<11;i++) {
      wifi_name_byte[i] = EEPROM.read(50+i);
    }
    password = String((char *)wifi_name_byte);
    //password =eeGetInt(password_address);
    html += "<lable>"+password+"</lable>";
    html += "</body></html>";
    server.send(200, "text/html", html); 
  }
}

void changePassword() {
  digitalWrite(2,LOW);
  String html;
  html = "<html><head><title>Door Sensors | Change Password</title></head><body>";
  html += "<div style=\"width:100%;margin-right:auto;margin-left:auto;\">";
  html += "<form action=\"/do-change-password\" method=\"post\">";
  html += "<input type=\"password\" name=\"password\">";
  html += "<input type=\"submit\" value=\"Enter\">";
  html += "<div></form>";
  String password;
  password = EEPROM.read(password_address);
  html += "<lable>Change Password "+ password +"</lable>";
  html += "</body></html>";
  server.send(200, "text/html", html);   
}

void doChangePassword() {
  String password_string;
  char password_char[100];
  int password_int = 0;
  if (server.arg("password")== ""){     //Parameter not found
    server.sendHeader("Location", "change-password", true);
    server.send ( 302, "text/plain", "");
  }
  else{ 
    password_string = server.arg("password");
    password_string.toCharArray(password_char,100);
    password_int = atoi(password_char);
    eeWriteInt(password_address,password_int);
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "");
  }
}

void doLogin() {
  String password_string;
  char password_char[100];
  int password_int = 0;
  int store_password = 0;
  if (server.arg("password")== ""){     //Parameter not found
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "");
  }
  else{ 
    password_string = server.arg("password");
    password_string.toCharArray(password_char,100);
    password_int = atoi(password_char);
    store_password = eeGetInt(password_address);
    if(store_password == password_int) {
      login = 1;
      server.sendHeader("Location", "/myaccount", true);
      server.send ( 302, "text/plain", ""); 
    }
    else {
      server.sendHeader("Location", "/", true);
      server.send ( 302, "text/plain", "");
    }
  }
}

void myAccount() {
  if(login == 1) {
    String html;
    int buzzer_status = eeGetInt(buzzer_address);
    html = "<html><head><title>Door Sensors | My Account</title></head>";
    html += "<body><h1>This is My Account Page</h1>";
    html += "<a href=\"/logout\">Logout</a>";
    if(buzzer_status == 0) {
      html += "<a href=\"/on-buzzer\">ON</a>";
    }
    else {
      html += "<a href=\"/off-buzzer\">OFF</a>";
    }
    server.send(200, "text/html", html);
  }
  else {
     server.sendHeader("Location", "/", true);
     server.send ( 302, "text/plain", "");
  }
}

void onBuzzer() {
  if(login == 1) {
    eeWriteInt(buzzer_address,1);
    server.sendHeader("Location", "/myaccount", true);
    server.send ( 302, "text/plain", "");
  }
  else {
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "");
  }
}

void offBuzzer() {
  if(login == 1) {
    eeWriteInt(buzzer_address,0);
    server.sendHeader("Location", "/myaccount", true);
    server.send ( 302, "text/plain", "");
  }
  else {
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "");
  }
}
void logout() {
  login = 0;
  server.sendHeader("Location", "/", true);
  server.send ( 302, "text/plain", "");
}

void reboot() {
  ESP.restart();
  server.sendHeader("Location", "/", true);
  server.send ( 302, "text/plain", "");
}

void wifiSettings() {
  if(login == 1) {
    String html;
    String wifi_name,wifi_password;
    byte wifi_name_byte[20],wifi_password_byte[20];
    int i =0;
    for(i=0;i<20;i++) {
      wifi_name_byte[i] = EEPROM.read(wifi_name_address+i);
    }
    for(i=0;i<20;i++) {
      wifi_password_byte[i] = EEPROM.read(wifi_password_address+i);
    }
    
    wifi_name = String((char *)wifi_name_byte);
    wifi_password = String((char *)wifi_password_byte);
    
    html = "<html><head><title>Door Sensors | Change Password</title></head><body>";
    html += "<div style=\"width:100%;margin-right:auto;margin-left:auto;\">";
    html += "<form action=\"/do-wifi-settings\" method=\"post\">";
    html += "<lable>Wifi Name</lable>";
    html += "<input type=\"text\" name=\"wifi_name\" value=\""+wifi_name+"\">";
    html += "<lable>Wifi Password</lable>";
    html += "<input type=\"password\" name=\"wifi_password\">";
    html += "<input type=\"submit\" value=\"Save\" value=\""+wifi_password+"\">";
    html += "<div></form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  }
  else {
    server.sendHeader("Location", "/", true);
    server.send ( 302, "text/plain", "");
  }
}
void doWifiSettings() {
  String wifi_name,wifi_password;
  wifi_name = server.arg("wifi_name");
  wifi_password = server.arg("wifi_password");
  byte wifi_name_byte[wifi_name.length()+1];
  byte wifi_password_byte[wifi_password.length()+1];
  int i = 0;
  wifi_password.getBytes(wifi_password_byte,wifi_password.length()+1);
  wifi_name.getBytes(wifi_name_byte, wifi_name.length()+1);
  
  for(i=0;i<=wifi_name.length()+1; i++) {
    EEPROM.write(wifi_name_address + i,wifi_name_byte[i]);
  }
  for(i=0;i<=wifi_password.length()+1; i++) {
    EEPROM.write(wifi_password_address + i,wifi_password_byte[i]);
  }
  server.sendHeader("Location", "/wifi-settings", true);
  server.send ( 302, "text/plain", "");  
}

void resetAllMemory() {
  int i=0;
  for(i=0; i< 512; i++) {
    EEPROM.write(i,0);
  }
  String html;
  html = "<html><head><title>Door Sensors | Reset Memory</title></head><body>";
  html += "<div style=\"width:100%;margin-right:auto;margin-left:auto;\">";
  html += "<h1>Reset All memory done</h1>";
  html += "</div></body></html>";
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
