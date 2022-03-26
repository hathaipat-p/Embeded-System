#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D

#include <WiFi.h>
#include "ThingSpeak.h"

char ssid[] = "POM";   // network SSID (name) 
char pass[] = "0800778528";   // network password
WiFiClient  client;

unsigned long myChannelNumber = 1680083;
const char * myWriteAPIKey = "N4O5I6EVETMLHVBN";

float temperatureC;
int light;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  temperatureC = ReadTemp();
  light = ReadLight();
  
  Serial.println("--------------------------------");
  Serial.printf("Temperature : %f degree celcius\n", temperatureC);
  Serial.printf("Light : %d lux\n", light);
  
//   Connect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Connecting... ");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); 
      Serial.print(".");
      delay(5000);     
    } 
    Serial.print("Wifi connected, IP address: ");
    Serial.println(WiFi.localIP());
  }

  // set the fields with the values
  ThingSpeak.setField(1, temperatureC);
  ThingSpeak.setField(2, light);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(5000);
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
