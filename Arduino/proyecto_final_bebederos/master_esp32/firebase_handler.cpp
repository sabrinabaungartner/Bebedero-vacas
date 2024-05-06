#include <Arduino.h>
#include "firebase_handler.h"
#include <WiFi.h>
//#include <FirebaseESP32.h>
#include <Firebase_ESP_Client.h>
#include <TimeLib.h>  // Para makeTime y now
#include "credentials_firebase.h"
#include "addons/TokenHelper.h" // Provide the token generation process info.
#include "addons/RTDBHelper.h" // Provide the RTDB payload printing info and other helper functions.
#include "time.h"
#include <ArduinoJson.h>
#include <vector>

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
    Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/water_level", value);
  }
}

void set_current_water_temperature_value(float value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
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

void set_current_data_into_backup(int year, int month, int day, int hour, int minute, int second, int cattle_waterer_selected) {
  if (Firebase.ready()) {
      char datetime_str[20];
      sprintf(datetime_str, "%04d%02d%02d_%02d%02d%02d",
              year, month, day,
              hour, minute, second);

      String firebasePath = "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/last_backup_modified";
      Firebase.RTDB.setString(&fbdo, firebasePath.c_str(), datetime_str);
      Serial.println("Último backup modificado almacenado en Firebase: " + firebasePath);
  }
}

void set_last_filling_date(int cattle_waterer_selected) {
  if (Firebase.ready()) {
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
          Serial.println("Failed to obtain time");
          return;
      }

      // Store date and time
      int year = timeinfo.tm_year + 1900;
      int month = timeinfo.tm_mon + 1;
      int day = timeinfo.tm_mday;
      int hour = timeinfo.tm_hour;
      int minute = timeinfo.tm_min;
      int second = timeinfo.tm_sec;

      // Make backup string with date and time 
      char last_filling_date_value[100];
      sprintf(last_filling_date_value, "%04d%02d%02d_%02d%02d%02d",
              year, month, day,
              hour, minute, second);

    if (Firebase.RTDB.setString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_filling_date", last_filling_date_value)) {
      Serial.println("Success on setting value of last_filling_date");
    } else { 
        Serial.println("Error setting value of last_filling_date");
      }
  } else {
      Serial.println("Failed to set last_filling_date value in Firebase (Firebase not ready)");
    }
}

void backup_current_data(int cattle_waterer_selected) {
  if (Firebase.ready()) {
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
          Serial.println("Failed to obtain time");
          return;
      }

      // Store date and time
      int year = timeinfo.tm_year + 1900;
      int month = timeinfo.tm_mon + 1;
      int day = timeinfo.tm_mday;
      int hour = timeinfo.tm_hour;
      int minute = timeinfo.tm_min;
      int second = timeinfo.tm_sec;

      // Make backup string with date and time 
      char backupPath[100];
      sprintf(backupPath, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_%d/backup_data/backup_%04d%02d%02d_%02d%02d%02d",
              cattle_waterer_selected,
              year, month, day,
              hour, minute, second);

      // JSON object to save current params
      FirebaseJson json;
      if (Firebase.RTDB.getJSON(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data")) {
          if (fbdo.dataType() == "json") {
              if (json.setJsonData(fbdo.payload())) {
                  if (Firebase.RTDB.setJSON(&fbdo, backupPath, &json)) {
                      Serial.println("Datos JSON respaldados correctamente en: " + String(backupPath));
                      // Store date and time in Firebase backup
                      set_current_data_into_backup(year, month, day, hour, minute, second, cattle_waterer_selected);
                  } else {
                      Serial.println("Error al respaldar los datos JSON.");
                  }
              }
          }
      }
  }
}

int get_cattle_waterer_selected() {
  if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_selected")) {
    if(fbdo.dataType()=="int"){
      return fbdo.intData();
    } else {
      Serial.println("error get_cattle_waterer_selected");
    }
  }

  return 0;
}

/*void check_and_delete_old_backups(int cattle_waterer_selected) {
  if (Firebase.ready()) {
      
    String backupPath = "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data";

    FirebaseJson backups;
    if (Firebase.RTDB.getJSON(&fbdo, backupPath)) {
      if (fbdo.dataType() == "json") {
        if (backups.setJsonData(fbdo.payload())) {
          size_t backupCount = backups.iteratorBegin();
          
          for (size_t i = 0; i < backupCount; ++i) {
              // Obtener el valor del backup en la posición actual
              FirebaseJson::IteratorValue backup = backups.valueAt(i);
              
              // Obtener la clave (nombre del backup) y el valor (datos del backup)
              String backupKey = backup.key.c_str();
              Serial.print("backupkey: ");
              Serial.println(backupKey);*/
              //FirebaseJson backupData = backup.value;
              
              /*String date = backupData["date"].to<String>();
              Serial.print("Backup Key: ");
              Serial.println(backupKey);
              Serial.print("Date: ");
              Serial.println(date);*/
              
              // Por ejemplo, puedes implementar lógica para eliminar backups antiguos
              // if (daysWithoutFilling > 7) {
              //     Firebase.RTDB.deleteNode(backupPath + "/" + backupKey);
              // }
         /* }
          
          backups.iteratorEnd(); // Liberar memoria utilizada durante la iteración
        } else {Serial.println("!backups.setJsonData in check_and_delete_old_backups");} 
      } else {Serial.println("!dataType in check_and_delete_old_backups");}
    } else {Serial.println("!Firebase.RTDB.getJSON in check_and_delete_old_backups");}
  } else {Serial.println("!firebase.ready in check_and_delete_old_backups");}
}*/

// Función para calcular si han pasado 20 minutos entre dos fechas
bool hasElapsedTwentyMinutes(struct tm previousTime, struct tm currentTime) {
  // Convertir las fechas a tiempo en segundos desde el Epoch
  time_t prevTimeSeconds = mktime(&previousTime);
  time_t currTimeSeconds = mktime(&currentTime);

  // Calcular el tiempo transcurrido en segundos
  double elapsedTimeSeconds = difftime(currTimeSeconds, prevTimeSeconds);

  // Convertir el tiempo transcurrido a minutos
  int elapsedTimeMinutes = elapsedTimeSeconds / 60;

  // Verificar si han pasado al menos 20 minutos
  return (elapsedTimeMinutes >= 20);
}

void update_days_without_filling(int cattle_waterer_selected) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Retrieve the last filled date from Firebase
  if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_filling_date")) {
    const char* last_filled_date = fbdo.stringData().c_str();  // Obtiene el último dato llenado como const char*

    int year, month, day, hour, minute, second;
    sscanf(last_filled_date, "%4d%2d%2d_%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);

    Serial.print("Year: ");
    Serial.println(year);
    Serial.print("Month: ");
    Serial.println(month);
    Serial.print("Day: ");
    Serial.println(day);
    Serial.print("Hour: ");
    Serial.println(hour);
    Serial.print("Minute: ");
    Serial.println(minute);
    Serial.print("Second: ");
    Serial.println(second);

    // Crear la estructura de tiempo para la última fecha registrada
    struct tm lastFilledTime = {0};
    lastFilledTime.tm_year = year - 1900;  // Ajustar el año
    lastFilledTime.tm_mon = month - 1;     // Ajustar el mes
    lastFilledTime.tm_mday = day;
    lastFilledTime.tm_hour = hour;
    lastFilledTime.tm_min = minute;
    lastFilledTime.tm_sec = second;

    if (hasElapsedTwentyMinutes(lastFilledTime, timeinfo)) {
      if (Firebase.ready()) {
        if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/last_backup_modified")) {
          String last_backup = fbdo.stringData();
          Serial.print("Last backup modified: ");
          Serial.println(last_backup);
          if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/backup_" + last_backup + "/days_without_filling")) {
            int days_without_filling = fbdo.intData();
            Serial.print("Days without filling from backup: ");
            Serial.println(days_without_filling);
            days_without_filling += 1;
            if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling", days_without_filling)) {
              Serial.print("Updated days without filling in Firebase. Days: ");
              Serial.println(days_without_filling);
            } else {Serial.println("error 3");}
            
          } else {Serial.println("error 1");}
        } else {Serial.println("error");}
        
      }
    }
  } else {
    Serial.println("Failed to retrieve last filled date from Firebase");
  }
}

int get_fill_waterer(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/fill_waterer")) {
      Serial.println("Success on getting value of fill_waterer");
      if(fbdo.dataType()=="int"){
        return fbdo.intData();
      } else {
        Serial.println("error get_fill_waterer datatype");
      }
    } else { 
        Serial.println("Error getting value of fill_waterer");
    }
  } else {
    Serial.println("Failed to retrieve fill_waterer value from Firebase (Firebase not ready)");
  }
}

void set_fill_waterer(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/fill_waterer", value)) {
      Serial.println("Success on setting value of fill_waterer");
    } else { 
        Serial.println("Error setting value of fill_waterer");
      }
  } else {
    Serial.println("Failed to set fill_waterer value in Firebase (Firebase not ready)");
  }
}

void set_is_water_pump_enabled(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/is_water_pump_enabled", value)) {
      Serial.println("Success on setting value of is_water_pump_enabled");
    } else { 
        Serial.println("Error setting value of is_water_pump_enabled");
      }
  } else {
    Serial.println("Failed to set is_water_pump_enabled value in Firebase (Firebase not ready)");
  }
}

void reset_days_without_filling(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling", 0)) {
      Serial.println("Success on setting value of days_without_filling");
    } else { 
        Serial.println("Error setting value of days_without_filling");
      }
  } else {
    Serial.println("Failed to set days_without_filling value in Firebase (Firebase not ready)");
  }
}