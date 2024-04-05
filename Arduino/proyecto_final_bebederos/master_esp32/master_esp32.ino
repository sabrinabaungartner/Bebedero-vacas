#include <Arduino.h>
#include "firebase_handler.h"

void setup() {
  Serial.begin(115200);

  setup_wifi();
}

void loop() {
  check_wifi();
  delay(1000);
  get_value_example();
}