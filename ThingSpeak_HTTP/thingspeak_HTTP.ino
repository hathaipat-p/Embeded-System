#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D

#include <WiFi.h>
#include <HTTPClient.h>

char ssid[] = "ssid";          // network SSID (name) 
char pass[] = "pass";   // network password

WiFiClient client;
HTTPClient http;

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
String APIKey = "N4O5I6EVETMLHVBN";

float temperatureC;
int light;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  temperatureC = ReadTemp();
  light = ReadLight();
  
  Serial.println("--------------------------------");
  Serial.printf("Temperature : %f degree celcius\n", temperatureC);
  Serial.printf("Light : %d \n", light);
  
  if(WiFi.status()== WL_CONNECTED){
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    // payload = api_key=APIKEY&field1=Temp&feild2=Light
    String httpRequestData = "api_key=" + APIKey + "&field1=" + String(temperatureC) + "&field2=" + String(light);     
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
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
