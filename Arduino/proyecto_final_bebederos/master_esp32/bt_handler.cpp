#include "bt_handler.h"
#include <Arduino.h>
#include "BluetoothSerial.h"
#include "package.h"

uint8_t rx_tx_buf[SIZE_ARRAY];
struct packet pac_to_send, my_received_packet_struct;
uint8_t received_array[SIZE_ARRAY]; 

#define USE_NAME // Comment this to use MAC address instead of a slave_name
#define LED_BT_BLUE 2 // LED onboard
#define LED_WHITE 5 // LED that lights up when a word is received from the slave

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

#ifdef USE_NAME
  String slave_name = "ESP32-BT-Slave"; // Name of slave BT device
#else
  uint8_t address[6]  = {0x26, 0x6F, 0x28, 0x17, 0x9F, 0x80}; // Slave MAC address 24:6F:28:17:9F:80
#endif

String master_name = "ESP32-BT-Master";
bool slave_is_connected = false;

// Callback function for Bluetooth
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){ // it works when passing data
  if (event == ESP_SPP_OPEN_EVT) {
    Serial.println("estoy en ESP_SPP_OPEN_EVT");
    digitalWrite(LED_BT_BLUE, HIGH);
    slave_is_connected = true;
  }

  if (event == ESP_SPP_CLOSE_EVT) {
    digitalWrite(LED_BT_BLUE, LOW);
    slave_is_connected = false;
  }
}

void set_bluetooth_configuration() {
  pinMode(LED_BT_BLUE, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  SerialBT.register_callback(callback);

  SerialBT.begin(master_name, true);

  #ifndef USE_NAME
    SerialBT.setPin(pin);
  #endif
}

bool connect_to_slave() {
  #ifdef USE_NAME
    slave_is_connected = SerialBT.connect(slave_name);
  #else
    slave_is_connected = SerialBT.connect(address);
  #endif
  return slave_is_connected;
}

void send_via_bt(uint8_t array[], uint8_t size_array) {
  SerialBT.write(array, size_array);
}

void send_package() {
  packet_to_send(&pac_to_send, rx_tx_buf);
  send_via_bt(rx_tx_buf, sizeof(rx_tx_buf));
}

void create_package_to_send(uint8_t size, uint8_t message, uint8_t water_level) {
  pac_to_send.length = size;
  pac_to_send.type_of_message = message;
  pac_to_send.payload[0] = water_level;
}

void request_water_level() {
  if (!check_slave_is_connected()) {
    while (!connect_to_slave()) {
      delay(1000);
    }
  }
  else {
    create_package_to_send(SIZE_ARRAY, GET_WATER_LEVEL, NULO);
    send_package();
  }
}

uint8_t receive_requested_water_level() {
  uint8_t index = 0;
  while (SerialBT.available()) {
    received_array[index++] = SerialBT.read();
  }

  received_packet(&my_received_packet_struct, received_array);

  return my_received_packet_struct.payload[0];
}



