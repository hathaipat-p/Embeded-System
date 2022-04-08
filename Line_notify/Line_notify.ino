//ESP32 Temperature Sensor notify via Line application

#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D
#include <TridentTD_LineNotify.h>
#include <WiFi.h>

#define LINE_TOKEN  "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"   // token line notify

char ssid[] = "xxxx";   // wifi SSID (name) 
char pass[] = "xxxx";   // wifi password


float temperatureC;

void setup() {
  delay(5000);
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200); 
  Serial.println(LINE.getVersion());

  WiFi.mode(WIFI_STA);
  
  Serial.print("Connecting... ");
  while(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pass); 
    Serial.print(".");
    delay(5000);     
  } 
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());

  LINE.setToken(LINE_TOKEN);
}

void loop() {
  temperatureC = ReadTemp();
  Serial.printf("Temperature : %f degree celcius\n", temperatureC);
  
  LINE.notify("temperature "+String(temperatureC)+" C");
  if (temperatureC <= 30){
    Serial.println("Cold");
    LINE.notify("Cold");
  }
  else if (temperatureC >= 36) {
    Serial.println("Hot");
    LINE.notify("Hot");
  }
  else{
    Serial.println("Warm");
    LINE.notify("Warm");
  }
  Serial.println("--------------------------------");
  delay(60000);
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
