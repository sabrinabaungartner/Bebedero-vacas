// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Evandro Copercini - 2018
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// and also demonstrate that SerialBT have the same functionalities of a normal Serial
// Note: Pairing is authenticated automatically by this device

#include "BluetoothSerial.h"

String device_name = "ESP32-BT-Slave";
#define LED_BT_BLUE 2
#define ESCAPE_CHARACTER '/'
uint8_t data [] = {'H', 'e', 'l', 'l', 'o', ESCAPE_CHARACTER};
size_t dataLength = sizeof(data);

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

// Callback function for Bluetooth
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if (event == ESP_SPP_CLOSE_EVT) {
    Serial.println("A SPP connection is closed");
    digitalWrite(LED_BT_BLUE, LOW);
  }

  if (event == ESP_SPP_SRV_OPEN_EVT) { 
    Serial.println("A client connects to the SPP server");
    digitalWrite(LED_BT_BLUE, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.register_callback(callback);
  SerialBT.begin(device_name);
  pinMode(LED_BT_BLUE, OUTPUT);
  digitalWrite(LED_BT_BLUE, LOW);
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void loop() {
  while (SerialBT.available()) {
    char receivedByte = SerialBT.read();
    if (receivedByte == ESCAPE_CHARACTER) {
      SerialBT.write(data, dataLength);
    }
    Serial.print("Received byte: ");
    Serial.println(receivedByte);
  }
}