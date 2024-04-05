#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"

void setup() {
  Serial.begin(115200);
  setup_wifi();
  set_bluetooth_configuration();
}

void loop() {
  check_wifi();
  delay(1000);
  get_value_example();
  check_bluetooth_state();
}