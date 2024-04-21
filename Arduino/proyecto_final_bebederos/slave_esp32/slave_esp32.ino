#include "slave_serial_bt.h"
#include "water_level_sensor.h"
#include "water_sensor_temperature.h"

void setup() {
  Serial.begin(115200);
  set_bluetooth_configuration();
  set_water_level_parameters();
  set_water_temperature();
}

void loop() {
  check_bluetooth_SPP_RxHandler();
  delay(2000);
}