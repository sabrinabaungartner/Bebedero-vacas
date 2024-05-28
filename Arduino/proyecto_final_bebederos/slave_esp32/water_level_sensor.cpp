#include "water_level_sensor.h"

int value = 0;

void setup_water_level_sensor(){
  pinMode(PIN_SIGNAL, INPUT);
  pinMode(POWER,OUTPUT);
  digitalWrite(POWER,HIGH);
}


int get_water_level() {
  value = analogRead(PIN_SIGNAL);
  Serial.print("valor nivel agua:");
  Serial.println(value);
  return value;
}