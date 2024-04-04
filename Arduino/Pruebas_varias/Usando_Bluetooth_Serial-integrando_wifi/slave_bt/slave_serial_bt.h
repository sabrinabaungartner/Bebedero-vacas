#ifndef SLAVE_BT_H
#define SLAVE_BT_H

#include "BluetoothSerial.h"

#define LED_BT_BLUE 2
#define ESCAPE_CHARACTER '/'

extern String device_name;
extern uint8_t data [];
extern size_t dataLength;

extern BluetoothSerial SerialBT;

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

// Callback function for Bluetooth
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
#endif