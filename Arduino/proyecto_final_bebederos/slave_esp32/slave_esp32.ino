#include "slave_serial_bt.h"
#include "water_level_sensor.h"

void setup() {
  Serial.begin(115200);
  set_bluetooth_configuration();
  set_water_level_parameters();
}

void loop() {
  check_bluetooth_SPP_RxHandler();
  delay(2000);
}