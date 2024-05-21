#include "water_level_sensor.h"

int value = 0;

void set_water_level_parameters(){
  pinMode(PIN_SIGNAL, INPUT);
  pinMode(POWER,OUTPUT);
  digitalWrite(POWER,HIGH);
}


int read_water_level() {
  //digitalWrite(POWER,HIGH);
  //delay(10);
  value = analogRead(PIN_SIGNAL);
  //delay(10);
  //digitalWrite(POWER,LOW);
  Serial.print("valor nivel agua:");
  Serial.println(value);
  return value;
}