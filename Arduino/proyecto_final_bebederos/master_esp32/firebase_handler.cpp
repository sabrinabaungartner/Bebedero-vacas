#include "firebase_handler.h"
#include <WiFi.h>
//#include <FirebaseESP32.h>
#include <Firebase_ESP_Client.h>
#include "credentials_firebase.h"
#include "addons/TokenHelper.h" // Provide the token generation process info.
#include "addons/RTDBHelper.h" // Provide the RTDB payload printing info and other helper functions.
#include "time.h"
#include <ArduinoJson.h>
#include <Arduino.h>

FirebaseData fbdo; // Firebase Data object
FirebaseAuth auth;
FirebaseConfig config;

unsigned long last_connection_attempt = 0;
const unsigned long connection_interval = 10000; // 1000ms attempt to reconnect
bool is_connected = false;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 0;

int next_backup = 0;

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

void set_current_water_level_value(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    //Firebase.setInt(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/current_data/water_level", value);
    Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/water_level", value);
  }
}

void set_current_water_temperature_value(float value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    //Firebase.setFloat(fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_1/current_data/water_temperature", value);
    Firebase.RTDB.setFloat(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/water_temperature", value);
  }
}

void set_current_date(int cattle_waterer_selected) {
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
    Firebase.RTDB.setString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/date", datetime_str);
  }
}

int get_next_backup_struct(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/next_backup_to_modify"))
      if(fbdo.dataType()=="int"){
        return fbdo.intData();
      }
    else
      Serial.println("error get");
  }
  
  return 0;
}

void set_next_backup_to_modify(int value, int cattle_waterer_selected) {
  Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/next_backup_to_modify", value);
}

void backup_current_date(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    next_backup = get_next_backup_struct(cattle_waterer_selected);

    String path = "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/backup_" + String(next_backup);
    Serial.print("path: ");
    Serial.println(path);

    // Crear un objeto JSON para almacenar los datos recibidos
    FirebaseJson json;
    
    // Check if the path exists, create it if it doesn't
    /*if (!Firebase.get(fbdo, path)) {
      // Path does not exist, create it
      Firebase.set(fbdo, path, nullptr);
    }*/
    
    // Save current data intp backup
    if (Firebase.RTDB.getJSON(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data")) {
      if (fbdo.dataType() == "json") {
        if (json.setJsonData(fbdo.payload())) {
          if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
            Serial.println("Datos JSON respaldados correctamente.");
          } else {
            Serial.println("Error al respaldar los datos JSON.");
          }
        }
      }
    }

    next_backup = (next_backup + 1) % MAX_BACKUPS;
    set_next_backup_to_modify(next_backup, cattle_waterer_selected);
  }
}

int get_cattle_waterer_selected() {
  if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_selected")) {
    if(fbdo.dataType()=="int"){
      return fbdo.intData();
      //cattle_waterer_selected = fbdo.to<String>();
    } else {
      Serial.println("error get_cattle_waterer_selected");
    }
  }

  return 0;
}