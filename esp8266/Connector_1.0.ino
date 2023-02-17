#include <Ticker.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SocketIoClient.h>
#include <ESP8266WiFiMulti.h>

void sendSensorData ();

SocketIoClient webSocket;
ESP8266WiFiMulti wifiMulti;
Ticker SensorTimer(sendSensorData, 2000); 

#define USER_SERIAL Serial
int buttons[] = {0,0,0,0,0,0,0,0};
int temp ;
int humidity ;
int co ;
int ch;
//For Wifi Connection
const char* ssid = "ROOTRSK";
const char* pass = "12345678";
//For Connectiong with server
const char* username = "arduino-maga-2560";
const char* password = "dEwz4B63q6dOckO7Ao8q";
const char* room = "123";

void setup(){
  Serial.begin(115200);
//  searchWiFi();
  configureWiFi();
  connectWiFi();
  SensorTimer.start();
  webSocket.on("connect",connection);
  webSocket.on("switch-triggered",switchHandler);
  webSocket.on("sync",syncHandler);
  webSocket.on("req-arduino-status",sendArduinoStatus);
  webSocket.begin("mammoth-galvanized-fiber.glitch.me");
}

void loop() {
  webSocket.loop();
  SensorTimer.update();
  if(Serial.available()){
    String msg = "";
      while(Serial.available()){
        msg += char(Serial.read());
        delay(10);
      }
      if(msg.length()> 7){
        setSensorsReading(msg);
      }
  }
  if(wifiMulti.run() == WL_CONNECTED) {}
}

//Funtion to Scna newtorsk
void searchWiFi(){
  Serial.write("");
  Serial.write("Scanning Wifi Networks");
  int numberOfNetworks = WiFi.scanNetworks();
  for(int i=0; i<numberOfNetworks; i++){
     Serial.print("Network Name: ");
     Serial.println(WiFi.SSID(i));
     Serial.print("Signal Stringth:");
     Serial.println(WiFi.RSSI(i));
     Serial.println("----------------");
  }
}
void configureWiFi(){
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("ROOTRSK", "Rootrsk@7909");
  wifiMulti.addAP("Compay", "9933817194");
}
void connectWiFi(){
  Serial.println("Connecting To Network");
  if(wifiMulti.run() == WL_CONNECTED) {
//    Serial.println("");
//    Serial.print("Connected To :");
//    Serial.println(WiFi.SSID());
//    Serial.println("IP Address: ");
//    Serial.println(WiFi.localIP());
      Serial.println("WiFi-Connected");
  }
  
}
//void connectWiFi(){
//  Serial.write("Connecting To Known Network");
//  WiFi.begin(ssid,pass);
//  while(WiFi.status() != WL_CONNECTED){
//    USER_SERIAL.print(".");
//    delay(1000);
//  }
//  Serial.write("");
//  Serial.write("Connected To : ");
//  Serial.println(WiFi.SSID());
//  Serial.write("IP Address: ");
//  Serial.print(WiFi.localIP());
//}

void switchHandler (const char* message,size_t length){
  
  StaticJsonDocument<64> doc;
  deserializeJson(doc,message);
  int switch_no = doc["switch_no"];
  bool switch_status =   doc["status"]; 
  buttons[switch_no-1] = switch_status;
  
  StaticJsonDocument<256> doc2;
  JsonArray switches = doc2.createNestedArray("s");
  for(int i=0; i<8;i++){
    switches.add(buttons[i]);
  }
  serializeJson(doc2, Serial);
  
  
//  String datas = "{\"s_n\":\""+String(switch_no)+"\", \"s_s\":\""+ String(switch_status) +"\"}";
//  int len = datas.length()+1;
//  char x[len];
//  datas.toCharArray(x,len);
//  Serial.write(x);
}
void syncHandler (const char* message,size_t length){
  
  StaticJsonDocument<256> doc;
  deserializeJson(doc,message);

  JsonArray switchStatus = doc["switchStatus"].as<JsonArray>();
  int itr = 0;
  for(JsonVariant state : switchStatus) {
      buttons[itr] = state.as<int>();
      itr++;
  }
//  serializeJson(doc, Serial);
  StaticJsonDocument<256> doc2;
  JsonArray switches = doc2.createNestedArray("s");
  for(int i=0; i<8;i++){
    switches.add(buttons[i]);
  }
  serializeJson(doc2, Serial);

//  StaticJsonDocument<128> doc2;
//  for(int i=0; i<8;i++){
//    doc2.add(buttons[i]);
//  }
//  serializeJson(doc2, Serial);
}

void connection(const char* message,size_t length){
  String LoginDetails = "{\"username\":\""+String(username)+"\",\"password\":\""+String(password)+"\",\"room\":\""+String(room)+"\"}"; 
//  Serial.println(LoginDetails);
  int len = LoginDetails.length()+1;
  char loginCharDetails[len];
  LoginDetails.toCharArray(loginCharDetails,len);
  webSocket.emit("join",loginCharDetails);
//  Serial.write("Socket-Connected");
}

void sendSensorData () {
  String sensorDetails = "{\"temp\":\""+String(temp)+"\",\"humidity\":\""+String(humidity)+"\",\"co\":\""+String(co)+"\",\"ch\":\""+String(ch)+"\"}"; 
  int len = sensorDetails.length()+1;
  char sensorsDatas[len];
  sensorDetails.toCharArray(sensorsDatas,len);
  webSocket.emit("sensor-send",sensorsDatas);
}
void sendArduinoStatus(const char* message,size_t length){
  String btns = "";
  for(int i=0;i<8;i++){
    btns += String(buttons[i]);
    if(i<7) btns+= ",";
  }
  String switchStatus = "{\"switchStatus\" : ["+String(btns)+"]}"; 
  int len = switchStatus.length()+1;
  char switchStatusChar[len];
  switchStatus.toCharArray(switchStatusChar,len);
  webSocket.emit("sync-with-arduino",switchStatusChar);
//  Serial.println("Data Synced");
}

void setSensorsReading (String sensorsReading){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc,sensorsReading);
  temp = doc["t"];
  humidity=doc["h"];
  co = doc["co"];
  ch = doc["ch"];
}