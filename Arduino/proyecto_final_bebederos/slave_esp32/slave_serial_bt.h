#ifndef SLAVE_BT_H
#define SLAVE_BT_H

#include "BluetoothSerial.h"

#define LED_BT_BLUE 2
#define ESCAPE_CHARACTER '/'

extern String device_name;
extern int data [];
extern size_t dataLength;

extern BluetoothSerial SerialBT;

/** Check if Bluetooth is available */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

/** Check Serial Port Profile */
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

/** Callback function for bluetooth */
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

/** Set initial parameters and configuration for the bluetooth connection */
void setup_bluetooth_configuration();

/** Checks if data was received and stores it for later processing */
uint8_t bluetooth_SPP_RxHandler();

/** Sends a response to the request */
void bluetooth_SPP_TxHandler(int water_level, float water_temperature);

/** Return type of message requested */
uint8_t get_message_type_of_request();

#endif