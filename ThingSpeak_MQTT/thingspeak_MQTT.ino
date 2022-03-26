#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D

#include "PubSubClient.h"
#include <WiFi.h>  

const char* ssid = "POM";
const char* password = "0800778528";
//char* topic="channels/<channelID/publish/<channelAPI>
char* topic = "channels/1680083/publish/N4O5I6EVETMLHVBN"; 
char* server = "mqtt.thingspeak.com";
String clientName="ESP-Thingspeak";

WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);

float temperatureC;
int light;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqttconnect();
}

void loop() {
  temperatureC = ReadTemp();
  light = ReadLight();
  
  String payload="field1=";
  payload+=temperatureC;
  payload+="&field2=";
  payload+=light;
  payload+="&status=MQTTPUBLISH";

  Serial.println(temperatureC);
  Serial.println(light);

  client.loop();
  
  if (!client.connected()){
    mqttconnect();
  }
  else {
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  Serial.println("---------------------------------------------");
  delay(5000);
}

void mqttconnect(){
  if (client.connect((char*) clientName.c_str())) {
    if (client.publish(topic, "hello from ESP8266")) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
}

float ReadTemp(){
  float temp = 0.0;
  byte Temp_data[2];
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(0x4D, 2);
  Temp_data[1] = Wire.read();
  Temp_data[0] = Wire.read();
  
  if (Temp_data[1]&0b01000000) temp += 128.0;
  if (Temp_data[1]&0b00100000) temp += 64;
  if (Temp_data[1]&0b00010000) temp += 32;
  if (Temp_data[1]&0b00001000) temp += 16;
  if (Temp_data[1]&0b00000100) temp += 8;
  if (Temp_data[1]&0b00000010) temp += 4;
  if (Temp_data[1]&0b00000001) temp += 2;

  if (Temp_data[0]&0b10000000) temp += 1;
  if (Temp_data[0]&0b01000000) temp += 0.5;
  if (Temp_data[0]&0b00100000) temp += 0.25;
  if (Temp_data[0]&0b00010000) temp += 0.125;
  if (Temp_data[0]&0b00001000) temp += 0.0625;
  if (Temp_data[0]&0b00000100) temp += 0.03125;
  
  if (Temp_data[1]&0b10000000) temp *= -1.0;
  
  return temp;
}

int ReadLight(){
  int analog_value = analogRead(36);
  return analog_value;
}
