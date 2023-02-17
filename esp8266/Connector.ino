#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SocketIoClient.h>
#include "Ticker.h"
#define USER_SERIAL Serial
void sendSensorData ();
Ticker SensorTimer(sendSensorData, 2000); 
SocketIoClient webSocket;

bool s_1= false;
bool s_2= false;
bool s_3= false;
bool s_4= false;
bool s_5= false;
bool s_6= false;
bool s_7= false;
bool s_8= false;

int temp ;
int humidity ;
int co ;
int ch;

const char* ssid = "ROOTRSK";
const char* pass = "7909@1234";

void setup(){
  Serial.begin(115200);
  searchWiFi();
  connectWiFi();
  SensorTimer.start();
  webSocket.on("connect",connection);
  webSocket.on("switch-triggered",relayHandler);
  webSocket.on("req-arduino-status",sendButtonStatus);
  webSocket.begin("rootrsk-home-automation-api.herokuapp.com");
//  webSocket.begin("192.168.43.192",3001);
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
}

//Funtion to Scna newtorsk
void searchWiFi(){
  Serial.write("");
  Serial.write("Scanning Wifi Networks");
  
  int numberOfNetworks = WiFi.scanNetworks();
  for(int i=0; i<numberOfNetworks; i++){
     USER_SERIAL.write("Network Name: ");
     USER_SERIAL.print(WiFi.SSID(i));
     USER_SERIAL.write("   Signal Stringth: ");
     USER_SERIAL.println(WiFi.RSSI(i));
     Serial.println("----------------");
  }
  Serial.write("");
}
void connectWiFi(){
  Serial.write("Connecting To Known Network");
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED){
    USER_SERIAL.print(".");
    delay(1000);
  }
  Serial.write("");
  Serial.write("Connected To : ");
  Serial.println(WiFi.SSID());
  Serial.write("IP Address: ");
  Serial.print(WiFi.localIP());
}

void relayHandler (const char* message,size_t length){
  
  DynamicJsonDocument doc(1024);
  deserializeJson(doc,message);
  int  switch_no = doc["switch_no"];
  bool   state =   doc["status"]; 
  handleLocalButtonStatus(switch_no,state);
  String datas = "{\"s_n\":\""+String(switch_no)+"\", \"s_s\":\""+ String(state) +"\"}";
  int len = datas.length()+1;
  char x[len];
  datas.toCharArray(x,len);
  Serial.write(x);
}

void connection(const char* message,size_t length){
  USER_SERIAL.println("Connected To Socket");
  String LoginDetails = "{\"username\":\"arduino\",\"password\":\"rootrsk\",\"room\":\"123\"}"; 
  int len = LoginDetails.length()+1;
  char loginCharDetails[len];
  LoginDetails.toCharArray(loginCharDetails,len);
  webSocket.emit("join",loginCharDetails);
  Serial.write(message);
}

void sendSensorData () {
  String sensorDetails = "{\"temp\":\""+String(temp)+"\",\"humidity\":\""+String(humidity)+"\",\"co\":\""+String(co)+"\",\"ch\":\""+String(ch)+"\"}"; 
  int len = sensorDetails.length()+1;
  char sensorsDatas[len];
  sensorDetails.toCharArray(sensorsDatas,len);
  webSocket.emit("sensor-send",sensorsDatas);
}

void setSensorsReading (String sensorsReading){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc,sensorsReading);
  temp = doc["t"];
  humidity=doc["h"];
  co = doc["co"];
  ch = doc["ch"];
}

void sendButtonStatus(const char* message,size_t length){
  String buttonStatus = "{\"s_1\":\""+String(s_1)+"\",\"s_2\":\""+String(s_2)+"\",\"s_3\":\""+String(s_3)+"\",\"s_4\":\""+String(s_4)+"\",\"s_5\":\""+String(s_5)+"\",\"s_6\":\""+String(s_6)+"\",\"s_7\":\""+String(s_7)+"\",\"s_8\":\""+String(s_8)+"\"}";
  int len = buttonStatus.length()+1;
  char buttonSts[len];
  buttonStatus.toCharArray(buttonSts,len);
  webSocket.emit("arduino-status",buttonSts);
}
void handleLocalButtonStatus(int switch_no,bool sts){
  switch(switch_no){
    case 1 :
        s_1 = sts;
        break;
     case 2 :
        s_2 = sts;
        break;
     case 3 :
        s_3 = sts;
        break;
     case 4 :
        s_4 = sts;
        break;
      case 5 :
        s_5 = sts;
        break;
     case 6 :
        s_6 = sts;
        break;
     case 7 :
        s_7 = sts;
        break;
     case 8 :
        s_8 = sts;
        break;
  }
}
