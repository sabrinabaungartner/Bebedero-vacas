// This example code is in the Public Domain (or CC0 licensed, at your option.)
// By Evandro Copercini - 2018
//
// This example creates a bridge between Serial and Classical Bluetooth (SPP)
// and also demonstrate that SerialBT have the same functionalities of a normal Serial
// Note: Pairing is authenticated automatically by this device

#include "BluetoothSerial.h"

String device_name = "ESP32-BT-Slave";
#define LED_BT_BLUE 2

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
  Serial.println("----- START CALLBACK FUNCTION -----");
  if (event == ESP_SPP_INIT_EVT) {
    Serial.println("SPP mode is initialized");
  }

  if (event == ESP_SPP_UNINIT_EVT) {
    Serial.println("SPP mode is deinitialized");
  }

  if (event == ESP_SPP_DISCOVERY_COMP_EVT) {
    Serial.println("Service discovery is complete");
  }

  if (event == ESP_SPP_OPEN_EVT) {
    Serial.println("A SPP client opens a connection");
    digitalWrite(LED_BT_BLUE, HIGH);
  }

  if (event == ESP_SPP_CLOSE_EVT) {
    Serial.println("A SPP connection is closed");
    digitalWrite(LED_BT_BLUE, LOW);
  }

  if (event == ESP_SPP_START_EVT) {
    Serial.println("the SPP server is initialized");
  }

  if (event == ESP_SPP_CL_INIT_EVT) {
    Serial.println("A SPP client initializes a connection");
  }

  if (event == ESP_SPP_SRV_OPEN_EVT) { 
    Serial.println("a client connects to the SPP server");
    digitalWrite(LED_BT_BLUE, HIGH);
  }

  if (event == ESP_SPP_SRV_STOP_EVT) {
    Serial.println("the SPP server stops");
  }

  Serial.println("----- END CALLBACK FUNCTION -----");
}

void setup() {
  Serial.begin(115200);
  SerialBT.register_callback(callback);
  SerialBT.begin(device_name); //Bluetooth device name
  pinMode(LED_BT_BLUE, OUTPUT);
  digitalWrite(LED_BT_BLUE, LOW); // Inicialmente apaga el LED azul
  //SerialBT.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void loop() {
  /*if (Serial.available()) {
    SerialBT.write(Serial.read());
  }*/
  if (SerialBT.available()) {
    //Serial.write(SerialBT.read());
    String receivedData = SerialBT.readString();
    Serial.println("Received from master: " + receivedData); // Imprime lo recibido del maestro
    if (receivedData == "Hello") { // Si lo recibido es "Hello"
      SerialBT.print("Hello"); // Envía "Hello" al maestro
    }
  }
}