#include "firebase_handler.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "credentials_firebase.h"
#include <addons/TokenHelper.h> // Provide the token generation process info.
#include <addons/RTDBHelper.h> // Provide the RTDB payload printing info and other helper functions.
#include "time.h"
#include <ArduinoJson.h>

FirebaseData fbdo; // Firebase Data object
FirebaseAuth auth;
FirebaseConfig config;

unsigned long last_connection_attempt = 0;
const unsigned long connection_interval = 10000; // 1000ms attempt to reconnect
bool is_connected = false;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 0;

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

void set_NTP_server() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void set_current_water_level_value(int value) {
  if (Firebase.ready()) {
    Firebase.setInt(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/current_data/water_level", value);
  }
}

void set_current_date() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char datetime_str[100]; // String to store date and time

  sprintf(datetime_str, "%02d/%02d/%04d %02d:%02d:%02d",
          timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  Serial.println("Fecha y hora:");
  Serial.println(datetime_str);

  if (Firebase.ready()) {
    Firebase.setString(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/current_data/date", datetime_str);
  }
}

int get_last_backup_modified() {
  Firebase.getInt(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/backup_data/last_backup_modified");
  int last_modified_backup_value = ERROR_GET_RTDB;
  if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_integer) {
      last_modified_backup_value = fbdo.to<int>();
  }
  return last_modified_backup_value;
}

void set_last_backup_modified(int value) {
  Firebase.setInt(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/backup_data/last_backup_modified", value);
}

void backup_current_date() {
  if (Firebase.ready()) {
    int last_backup = get_last_backup_modified();

    last_backup = (last_backup + 1) % 10;

    String backup_node = "backup_" + String(last_backup);
    
    // Save current data intp backup
    if (Firebase.getJSON(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/current_data")) {
      if (fbdo.dataType() == "json") {
        FirebaseJson json;
        if (json.setJsonData(fbdo.payload())) {
          Firebase.setJSON(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/backup_data/" + backup_node, json);
        }
      }
    }

    set_last_backup_modified(last_backup);
  }
}