#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"

void setup() {
  Serial.begin(115200);
  setup_wifi();
  set_bluetooth_configuration();
}

void loop() {
  //check_wifi();
  //get_value_example();
  //delay(1000);
  request_water_level();
  uint8_t water_value = receive_requested_water_level();
  Serial.print("water level value: ");
  Serial.println(water_value);
  delay(1000);
}