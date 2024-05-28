#include "slave_serial_bt.h"
#include "Arduino.h"
#include "package.h"

#define SUCCESS 1
#define FAILURE 0

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

void setup_bluetooth_configuration() {
  SerialBT.register_callback(callback);
  SerialBT.begin(device_name);
  pinMode(LED_BT_BLUE, OUTPUT);
  digitalWrite(LED_BT_BLUE, LOW);
}

void assemble_package_water_level(uint8_t size, uint8_t reply, int water_level, float water_temperature, uint8_t pump_status) {
  my_packet_to_send.length = size;
  my_packet_to_send.type_of_message = reply;

  for (size_t i = 0; i < sizeof(int); i++) { // i goes from 0 to 4
    my_packet_to_send.payload[i] = (water_level >> (i * 8)) & 0xFF;
  }

  Serial.print("nivel agua en clase bt: ");
  Serial.println(water_level);

  my_packet_to_send.payload[4] = water_temperature;
  my_packet_to_send.payload[5] = water_temperature;
  my_packet_to_send.payload[6] = pump_status;
}

void assemble_package_water_level_temperature(uint8_t size, uint8_t reply, int water_level, float water_temperature, uint8_t pump_status) {
  my_packet_to_send.length = size;
  my_packet_to_send.type_of_message = reply;

  for (size_t i = 0; i < sizeof(int); i++) { // i goes from 0 to 4
    my_packet_to_send.payload[i] = (water_level >> (i * 8)) & 0xFF;
  }

  Serial.print("nivel agua en clase bt: ");
  Serial.println(water_level);

  int water_temperature_aux = int(water_temperature);
  float decimal = water_temperature - water_temperature_aux;

  uint8_t water_temperature_aux_uint8 = static_cast<uint8_t>(water_temperature_aux);
  uint8_t decimal_uint8 = static_cast<uint8_t>(decimal * 100);

  my_packet_to_send.payload[4] = water_temperature_aux_uint8;
  my_packet_to_send.payload[5] = decimal_uint8;
  my_packet_to_send.payload[6] = pump_status;
}

void assemble_package_pump(uint8_t size, uint8_t reply, int water_level, float water_temperature, uint8_t pump_status) {
  my_packet_to_send.length = size;
  my_packet_to_send.type_of_message = reply;

  for (size_t i = 0; i < sizeof(int); i++) { // i goes from 0 to 4
    my_packet_to_send.payload[i] = water_level;
  }

  my_packet_to_send.payload[4] = water_temperature;
  my_packet_to_send.payload[5] = water_temperature;
  my_packet_to_send.payload[6] = pump_status;
}

void send_package() {
  packet_to_send(&my_packet_to_send, array_to_send);
  SerialBT.write(array_to_send, sizeof(array_to_send));
}

void bluetooth_SPP_TxHandler(int water_level, float water_temperature) {
  switch(my_received_packet_struct.type_of_message) {
    case GET_WATER_LEVEL_AND_TEMPERATURE:
      Serial.println("estoy por ensamblar nivel y temperatura");
      assemble_package_water_level_temperature(SIZE_ARRAY, REPLY_WATER_LEVEL_AND_TEMPERATURE, water_level, water_temperature, NULO);
      send_package();
      break;
    case GET_WATER_LEVEL:
      assemble_package_water_level(SIZE_ARRAY, REPLY_WATER_LEVEL, water_level, NULO, NULO);
      send_package();
      break;
    case TURN_ON_PUMP:
      assemble_package_pump(SIZE_ARRAY, REPLY_TURN_ON_PUMP, NULO, NULO, SUCCESS);
      send_package();
      break;
    case TURN_OFF_PUMP:
      assemble_package_pump(SIZE_ARRAY, REPLY_TURN_OFF_PUMP, NULO, NULO, SUCCESS);
      send_package();
      break;
  }
}

uint8_t bluetooth_SPP_RxHandler() {
  uint8_t index = 0;
  if (SerialBT.available() > 0) {
    while (SerialBT.available() && (index < SIZE_ARRAY)) {
      received_array[index++] = SerialBT.read();
    }

    received_packet(&my_received_packet_struct, received_array);
    return 1;
  }

  else {
    return 0;
  }
}

uint8_t get_message_type_of_request() {
  Serial.println(my_received_packet_struct.type_of_message);
  return my_received_packet_struct.type_of_message;
}