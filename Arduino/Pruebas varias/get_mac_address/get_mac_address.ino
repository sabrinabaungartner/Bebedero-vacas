// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/

#include "WiFi.h"

void setup(){
  Serial.begin(115200);
  
}
 
void loop(){
  Serial.println("looking for address....");
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
  delay(2000);
}
