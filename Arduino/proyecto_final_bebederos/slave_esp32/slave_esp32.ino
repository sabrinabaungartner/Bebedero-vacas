#include "slave_serial_bt.h"

void setup() {
  Serial.begin(115200);
  set_bluetooth_configuration();
}

void loop() {
  check_bluetooth_SPP_RxHandler();
  delay(2000);
}