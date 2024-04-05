#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "credentials_firebase.h"

#include <addons/TokenHelper.h> // Provide the token generation process info.

#include <addons/RTDBHelper.h> // Provide the RTDB payload printing info and other helper functions.

FirebaseData fbdo; // Firebase Data object
FirebaseAuth auth;
FirebaseConfig config;

unsigned long last_connection_attempt = 0;
const unsigned long connection_interval = 10000; // 1000ms attempt to reconnect
bool is_connected = false;

void check_wifi() {
  if (WiFi.status() != WL_CONNECTED && is_connected) { is_connected = false;
  } else if (WiFi.status() == WL_CONNECTED && !is_connected) { is_connected = true; }

  if (!is_connected) {
    unsigned long current_millis = millis();
    if (current_millis - last_connection_attempt >= connection_interval) {
      last_connection_attempt = current_millis;
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(wifi_ssid, wifi_password);
    }
  }
}

void setup() {

  Serial.begin(115200);

  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY; //Assign the api key (required)

  auth.user.email = USER_EMAIL; //Assign the user sign in credentials
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL; //Assign the RTDB URL (required)

  Firebase.reconnectNetwork(true); //Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  String uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

 if (Firebase.ready()) {
    Serial.printf("Get water_level backup_1... %s\n", Firebase.getInt(fbdo, F("UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/backup_data/backup_1/water_level")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
 }
}

void loop() {
  check_wifi();

  delay(1000);
  if (Firebase.ready()) {
    Serial.printf("Get water_level backup_1... %s\n", Firebase.getInt(fbdo, F("UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/backup_data/backup_1/water_level")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
 }
}