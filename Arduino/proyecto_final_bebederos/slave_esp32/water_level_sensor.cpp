#include "water_level_sensor.h"

//int water_pin = 33;  // Define el número del pin D15 (GPIO15)
//int water_pin = 34;
uint8_t value = 0;

void set_water_level_parameters(){
  pinMode(PIN_SIGNAL, INPUT);
  pinMode(POWER,OUTPUT);
  digitalWrite(POWER,LOW);
}


uint8_t read_water_level() {
  digitalWrite(POWER,HIGH);
  delay(10);
  value = analogRead(PIN_SIGNAL);
  delay(10);
  digitalWrite(POWER,LOW);
  Serial.print("water level: ");
  Serial.println(value);
  return value;
}