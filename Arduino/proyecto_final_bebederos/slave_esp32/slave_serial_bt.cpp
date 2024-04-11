#include "slave_serial_bt.h"

String device_name = "ESP32-BT-Slave";

BluetoothSerial SerialBT;

// Callback function for Bluetooth
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if (event == ESP_SPP_CLOSE_EVT) {
    digitalWrite(LED_BT_BLUE, LOW);
  }

  if (event == ESP_SPP_SRV_OPEN_EVT) { 
    digitalWrite(LED_BT_BLUE, HIGH);
  }
}