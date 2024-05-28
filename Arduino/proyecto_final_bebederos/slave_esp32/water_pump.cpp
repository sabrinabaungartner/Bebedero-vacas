#include "water_pump.h"

void setup_water_pump() {
  pinMode(PIN_RELAY, OUTPUT); // Configure relay pin as output
  turn_off_water_pump();
}

void turn_on_water_pump() {
  digitalWrite(PIN_RELAY, LOW);
}

void turn_off_water_pump() {
  digitalWrite(PIN_RELAY, HIGH);
}
