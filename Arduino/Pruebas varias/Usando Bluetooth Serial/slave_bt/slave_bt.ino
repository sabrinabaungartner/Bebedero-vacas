// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Evandro Copercini - 2018
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// and also demonstrate that SerialBT have the same functionalities of a normal Serial
// Note: Pairing is authenticated automatically by this device

#include "slave_serial_bt.h"

void set_bluetooth_configuration() {
  SerialBT.register_callback(callback);
  SerialBT.begin(device_name);
  pinMode(LED_BT_BLUE, OUTPUT);
  digitalWrite(LED_BT_BLUE, LOW);
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void check_bluetooth_state() {
  while (SerialBT.available()) {
    char receivedByte = SerialBT.read();
    if (receivedByte == ESCAPE_CHARACTER) {
      SerialBT.write(data, dataLength);
    }
    Serial.print("Received byte: ");
    Serial.println(receivedByte);
  }
}

void setup() {
  Serial.begin(115200);
  set_bluetooth_configuration();
}

void loop() {
  check_bluetooth_state();
}