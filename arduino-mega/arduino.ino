
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTPIN 22     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 22  (AM2302)

int incomingByte = 0;
bool sts;

const int s1 = 3;
const int s2 = 4;
const int s3 = 5;
const int s4 = 6;
const int s5 = 7;
const int s6 = 8;
const int s7 = 9;
const int s8 = 10;
const int MqD = 24;
const int buzzer =  26;
const int led = 28;
const int mqA = A0;

float R0 = 0.95; //Resistance in fresh Air
float sensor_volt;//Sensor volt
float m = -0.47; //Slope 
float b = 1.31; //Y-Intercept 
float RS_gas; // Get value of RS in a GAS
float ratio; // Get ratio RS_GAS/RS_air
String t = "20";
String h = "20";
String co = "40";
String ch = "60";

DHT dht(DHTPIN, DHTTYPE);
void setup() {
   Serial.begin(9600);
   dht.begin();
   Serial3.begin(115200);
   
   pinMode(s1,OUTPUT);
   pinMode(s2,OUTPUT);
   pinMode(s3,OUTPUT);
   pinMode(s4,OUTPUT);
   pinMode(s5,OUTPUT);
   pinMode(s6,OUTPUT);
   pinMode(s7,OUTPUT);
   pinMode(s8,OUTPUT);
   pinMode(led,OUTPUT);
   pinMode(buzzer,OUTPUT);
   pinMode(MqD,INPUT);
   
   Serial.println("Starting Serial Port");
   while(!Serial3.available()){
    Serial.print(".");
    delay(1000);
   }
   Serial.println("");
   Serial.println("Wifi Module Started");
}

void loop() {
    int sensorValue = analogRead(A0);
    int sensorStatus = digitalRead(MqD);
    Serial.print(sensorStatus);
    
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL
    ratio = RS_gas/R0;  // ratio = RS/R0 
    double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
    double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
    
    co=ppm;
    ch=sensor_volt;
    h = dht.readHumidity();
    t = dht.readTemperature();
    if(t.toInt()>50 || h.toInt()>90 || sensorStatus==0){
      digitalWrite(buzzer,HIGH);
    }else{
      digitalWrite(buzzer,LOW);
    }
    sendSenorsReadingToEsp();
    if(Serial3.available()){
      String msg = "";
      while(Serial3.available()){
        msg += char(Serial3.read());
        delay(10);
      }
      Serial.println(msg.length());
      relayHandler(msg);
    }
    delay(500);
} 
void relayHandler (String message){
  
  Serial.print(message);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc,message);
  int  switch_no = doc["s_n"];
  int   states =  doc["s_s"];
  sts = states == 1 ? HIGH : LOW;
  Serial.println("");
  Serial.println(switch_no);
  Serial.println(sts);
  if(switch_no){
    digitalWrite(switch_no+2,sts);
  }
}
 void sendSenorsReadingToEsp() {
  String sensorDetails = "{\"t\":\""+String(t)+"\",\"h\":\""+String(h)+"\",\"co\":\""+String(co)+"\",\"ch\":\""+String(ch)+"\"}"; 
  int len = sensorDetails.length()+1;
  char sensorsDatas[len];
  sensorDetails.toCharArray(sensorsDatas,len);
  Serial3.write(sensorsDatas);
}
                                                                                                          
