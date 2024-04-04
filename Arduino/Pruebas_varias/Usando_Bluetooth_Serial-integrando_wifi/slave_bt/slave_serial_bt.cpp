#include "slave_serial_bt.h"

String device_name = "ESP32-BT-Slave";
uint8_t data [] = {'H', 'e', 'l', 'l', 'o', ESCAPE_CHARACTER};
size_t dataLength = sizeof(data);

BluetoothSerial SerialBT;

// Callback function for Bluetooth
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if (event == ESP_SPP_CLOSE_EVT) {
    //Serial.println("A SPP connection is closed");
    digitalWrite(LED_BT_BLUE, LOW);
  }

  if (event == ESP_SPP_SRV_OPEN_EVT) { 
    //Serial.println("A client connects to the SPP server");
    digitalWrite(LED_BT_BLUE, HIGH);
  }
}