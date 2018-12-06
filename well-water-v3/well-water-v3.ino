#include <SoftwareSerial.h>
#define RX 2
#define TX 3

#include "connectionvars.h"

// The below are defined in connectionvars.h:
//String AP = "**********";       // CHANGE ME
//String PASS = "**********"; // CHANGE ME
//String API = "**********";   // CHANGE ME
//String HOST = "api.thingspeak.com";
//String PORT = "80";

String fieldBars = "field1"; // Pressure bars
String fieldPascals = "field2"; // Pressure pascals
String fieldVolume = "field3"; // Water volume in liters
String fieldVoltage = "field4"; // Pressure sensor voltage

#define pressurePin A0
float pressure_bar;
float pressure_pascal;
float voltage;
float liters;
int volume;

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
//int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}
void loop() {
  
 getSensorData();
 String getData = "GET /update?api_key="+ API +"&"+ fieldBars +"="+String(pressure_bar) +"&"+ fieldVoltage +"="+String(voltage) +"&"+ fieldVolume +"="+String(volume);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 delay(2000);
}

void getSensorData(){

  int sensorVal = analogRead(pressurePin);
  Serial.print("Sensor Value: ");
  Serial.print(sensorVal);

  voltage = (sensorVal*4.68)/1024.0;
  Serial.print(".  Volts: ");
  Serial.print(voltage);
   
  pressure_pascal = (3.0*((float)voltage-0.47))*1000000.0;
  Serial.print(".  Pressure Pa = ");
  Serial.print(pressure_pascal);
  
  pressure_bar = pressure_pascal/10e5;
  Serial.print("  Pressure Bar = ");
  Serial.println(pressure_bar);

  int bars_int = pressure_bar * 100;
  Serial.print("Bars int: ");
  Serial.print(bars_int);
  volume = map(bars_int, 0, 60, 0, 100); // map to percents
  Serial.print(" Tank volume %: ");
  Serial.println(volume);
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
