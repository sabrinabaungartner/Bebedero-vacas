#include "firebase_handler.h"
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

void setup_wifi() {
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  config.api_key = API_KEY; //Assign the api key (required)

  auth.user.email = USER_EMAIL; //Assign the user sign in credentials
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL; //Assign the RTDB URL (required)

  Firebase.reconnectNetwork(true); //Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager

  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);
  Firebase.begin(&config, &auth);
}

void get_value_example() {
  if (Firebase.ready()) {
    Serial.printf("Get water_level backup_1... %s\n", Firebase.getInt(fbdo, F("UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/backup_data/backup_1/water_level")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
 }
}
