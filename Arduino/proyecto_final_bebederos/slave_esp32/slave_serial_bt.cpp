#include "slave_serial_bt.h"
#include "Arduino.h"
#include "package.h"
#include "water_level_sensor.h"

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

uint8_t received_array[SIZE_ARRAY]; 
uint8_t array_to_send[SIZE_ARRAY];

struct packet my_received_packet_struct;
struct packet my_packet_to_send;

void set_bluetooth_configuration() {
  SerialBT.register_callback(callback);
  SerialBT.begin(device_name);
  pinMode(LED_BT_BLUE, OUTPUT);
  digitalWrite(LED_BT_BLUE, LOW);
}

void assemble_package(uint8_t size, uint8_t reply, uint8_t water_level) {
  my_packet_to_send.length = size;
  my_packet_to_send.type_of_message = reply;
  my_packet_to_send.payload[0] = water_level;
}

void check_order() {
  switch(my_received_packet_struct.type_of_message) {
    case GET_WATER_LEVEL:
      assemble_package(SIZE_ARRAY, REPLY_WATER_LEVEL, read_water_level());
      break;
  }
}

void send_package() {
  packet_to_send(&my_packet_to_send, array_to_send);
  SerialBT.write(array_to_send, sizeof(array_to_send));
}

void bluetooth_SPP_TxHandler() {
  check_order();
  send_package();
}

void check_bluetooth_SPP_RxHandler() {
  uint8_t index = 0;
  while (SerialBT.available()) {
    received_array[index++] = SerialBT.read();
  }

  received_packet(&my_received_packet_struct, received_array);
  bluetooth_SPP_TxHandler();
}