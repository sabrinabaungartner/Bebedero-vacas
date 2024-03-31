// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Victor Tchistiak - 2019
//
// This example demonstrates master mode Bluetooth connection to a slave BT device
// defined either by String "slave_name" by default "ESP32-BT-Slave" or by MAC address
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// This is an extension of the SerialToSerialBT example by Evandro Copercini - 2018
//
// DO NOT try to connect to phone or laptop - they are master
// devices, same as the ESP using this code - you will be able
// to pair, but the serial communication will NOT work!
//
// You can try to flash a second ESP32 with the example SerialToSerialBT - it should
// automatically pair with ESP32 running this code
// Note: Pairing is authenticated automatically by this device
#include "BluetoothSerial.h"

#define USE_NAME // Comment this to use MAC address instead of a slave_name
#define LED_BT_BLUE 2 // LED onboard
#define LED_WHITE 5 // LED that lights up when a word is received from the slave
#define WORD_TO_TRIGGER_LED "Hello" // Word that the master expects to receive from the slave
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

#ifdef USE_NAME
  String slave_name = "ESP32-BT-Slave"; // Name of slave BT device
#else
  String mac_address = "24:6F:28:17:9F:80"; // This only for printing
  uint8_t address[6]  = {0x26, 0x6F, 0x28, 0x17, 0x9F, 0x80}; // Slave MAC address
#endif

String master_name = "ESP32-BT-Master";
bool slave_is_connected = false;

// Callback function for Bluetooth
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if (event == ESP_SPP_OPEN_EVT) {
    Serial.println("A SPP client opens a connection");
    digitalWrite(LED_BT_BLUE, HIGH);
    slave_is_connected = true;
  }

  if (event == ESP_SPP_CLOSE_EVT) {
    Serial.println("A SPP connection is closed");
    digitalWrite(LED_BT_BLUE, LOW);
    slave_is_connected = false;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BT_BLUE, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  SerialBT.register_callback(callback);

  SerialBT.begin(master_name, true);
  Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", master_name.c_str());

  #ifndef USE_NAME
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif

  if (!connect_to_slave()) {
    Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
  }
}

void loop() {
  if (!slave_is_connected) {
    while (!connect_to_slave()) {
      digitalWrite(LED_BT_BLUE, HIGH);
      delay(1000);
      digitalWrite(LED_BT_BLUE, LOW);
      Serial.println("Trying to reconnect...");
    }
  }

  else {
    delay(3000);
    SerialBT.write(data, dataLength);

    while (SerialBT.available()) {
      char receivedByte = SerialBT.read();
      if (receivedByte == '/') {
        digitalWrite(LED_WHITE, HIGH);
        delay(1000);
        digitalWrite(LED_WHITE, LOW);
      }
    }
  }
}

bool connect_to_slave() {
  #ifdef USE_NAME
    slave_is_connected = SerialBT.connect(slave_name);
    Serial.printf("Connecting to slave BT device named \"%s\"\n", slave_name.c_str());
  #else
    slave_is_connected = SerialBT.connect(address);
    Serial.print("Connecting to slave BT device with MAC "); Serial.println(mac_address);
  #endif
  return slave_is_connected;
}

