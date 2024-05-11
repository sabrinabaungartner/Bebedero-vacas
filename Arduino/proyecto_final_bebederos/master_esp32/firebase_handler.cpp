#include <Arduino.h>
#include "firebase_handler.h"
#include <WiFi.h>
//#include <FirebaseESP32.h>
#include <Firebase_ESP_Client.h>
#include <TimeLib.h>  // Para makeTime y now
#include "credentials_firebase.h"
#include "addons/TokenHelper.h"  // Provide the token generation process info.
#include "addons/RTDBHelper.h"   // Provide the RTDB payload printing info and other helper functions.
#include "time.h"
#include <ArduinoJson.h>
#include <vector>

FirebaseData fbdo;  // Firebase Data object
FirebaseAuth auth;
FirebaseConfig config;

bool is_connected = false;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

int next_backup = 0;

bool check_wifi() {
  is_connected = (WiFi.status() == WL_CONNECTED);

  if (!is_connected) {
      WiFi.disconnect();
      WiFi.begin(wifi_ssid, wifi_password);
  }

  return is_connected;
}

void setup_wifi_firebase() {
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  config.api_key = API_KEY;  //Assign the api key (required)

  auth.user.email = USER_EMAIL;  //Assign the user sign in credentials
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;  //Assign the RTDB URL (required)

  Firebase.reconnectNetwork(true);  //Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager

  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);
  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);
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
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  char datetime_str[100];  // String to store date and time

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
      if (fbdo.dataType() == "int") {
        return fbdo.intData();
      } else
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
    FirebaseJson json;  // JSON object to save current params

    // Obtain date string from Firebase
    if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/date")) {
      if (fbdo.dataType() == "string") { // Check if data type is string
        String date = fbdo.stringData();

        // Parse the date string to extract year, month, day, hour, minute, second
        int year, month, day, hour, minute, second;
        sscanf(date.c_str(), "%d/%d/%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second);

        // Obtain current data JSON from Firebase
        if (Firebase.RTDB.getJSON(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data")) {
          if (fbdo.dataType() == "json") {
            if (json.setJsonData(fbdo.payload())) {
              // Make backup path with parsed date and time
              char backupPath[100];
              sprintf(backupPath, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_%d/backup_data/backup_%04d%02d%02d_%02d%02d%02d",
                      cattle_waterer_selected,
                      year, month, day,
                      hour, minute, second);

              // Save current data to backup path
              if (Firebase.RTDB.setJSON(&fbdo, backupPath, &json)) {
                Serial.println("JSON data backed up successfully in: " + String(backupPath));
              } else {
                Serial.println("Error backing up JSON data.");
              }
            } else {
              Serial.println("Error setting data to JSON Firebase object");
            }
          } else {
            Serial.println("Error data type is not JSON");
          }
        } else {
          Serial.println("Failed to obtain JSON data from current data path");
        }
      } else {
        Serial.println("Failed to obtain date string from current data");
      }
    } else {
      Serial.println("Failed to retrieve date string from current data path");
    }
  } else {
    Serial.println("Failed to set backup of current data in Firebase (Firebase not ready)");
  }
}

int get_cattle_waterer_selected() {
  if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_selected")) {
    if (fbdo.dataType() == "int") {
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

/*void update_days_without_filling(int cattle_waterer_selected) {
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

    // Crear la estructura de tiempo para la última fecha registrada
    struct tm lastFilledTime = { 0 };
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
            } else {
              Serial.println("error 3");
            }

          } else {
            Serial.println("error 1");
          }
        } else {
          Serial.println("error");
        }
      }
    }
  } else {
    Serial.println("Failed to retrieve last filled date from Firebase");
  }
}*/

void update_days_without_filling_in_firebase(int cattle_waterer_selected) {
  // Obtener y actualizar days_without_filling en Firebase
  if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling")){  // Obtener el valor actual de days_without_filling desde Firebase
    if (fbdo.dataType() == "int") {
      int days_without_filling = fbdo.intData();
      days_without_filling += 1;

      // Actualizar el valor de days_without_filling en Firebase
      if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling", days_without_filling)) { // Actualizar days_without_filling en Firebase
        Serial.println("Updated days_without_filling in Firebase");
      } else {
        Serial.println("Failed to update days_without_filling in Firebase");
      }
    }
  }
}

void update_days_without_filling(int cattle_waterer_selected) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Retrieve current time with time_t
  //time_t currentTime = mktime(&timeinfo);

  // Retrieve the last filled date from Firebase
  if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_filling_date")) {
    const char* last_filled_date = fbdo.stringData().c_str();

    int year, month, day, hour, minute, second;
    sscanf(last_filled_date, "%4d%2d%2d_%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);

    // Crear la estructura de tiempo para la última fecha llenada
    struct tm lastFilledTime = { 0 };
    lastFilledTime.tm_year = year - 1900;  // Ajustar el año
    lastFilledTime.tm_mon = month - 1;     // Ajustar el mes
    lastFilledTime.tm_mday = day;
    lastFilledTime.tm_hour = hour;
    lastFilledTime.tm_min = minute;
    lastFilledTime.tm_sec = second;

    // Obtener la fecha de last_check_filling_date desde Firebase
    if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_check_filling_date")) {
      const char* last_check_date = fbdo.stringData().c_str();  // Obtener la última fecha de verificación como const char*

      int checkYear, checkMonth, checkDay, checkHour, checkMinute, checkSecond;
      sscanf(last_check_date, "%4d%2d%2d_%2d%2d%2d", &checkYear, &checkMonth, &checkDay, &checkHour, &checkMinute, &checkSecond);

      // Crear la estructura de tiempo para la última fecha de verificación
      struct tm lastCheckTime = { 0 };
      lastCheckTime.tm_year = checkYear - 1900;  // Ajustar el año
      lastCheckTime.tm_mon = checkMonth - 1;     // Ajustar el mes
      lastCheckTime.tm_mday = checkDay;
      lastCheckTime.tm_hour = checkHour;
      lastCheckTime.tm_min = checkMinute;
      lastCheckTime.tm_sec = checkSecond;

      // Verificar si han pasado 20 minutos desde last_filling_Date
      if (difftime(mktime(&timeinfo), mktime(&lastFilledTime)) >= 1200) { // 20 minutos en segundos (20 * 60 = 1200)
        // Si last_check_filling_date no está establecido o han pasado 20 minutos desde la última verificación
        if (lastCheckTime.tm_year == 0 || difftime(mktime(&timeinfo), mktime(&lastCheckTime)) >= 1200) {
          // Actualizar days_without_filling en Firebase
          update_days_without_filling_in_firebase(cattle_waterer_selected);

          // Actualizar last_check_filling_date con la fecha y hora actual en Firebase
          char current_date_time[20];
          sprintf(current_date_time, "%04d%02d%02d_%02d%02d%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

          if (Firebase.RTDB.setString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_check_filling_date", current_date_time)) {
            Serial.println("Updated last_check_filling_date in Firebase");
          } else {
            Serial.println("Failed to update last_check_filling_date in Firebase");
          }
        }
      }
    } else {
      Serial.println("Failed to retrieve last_check_filling_date from Firebase");
    }
  } else {
    Serial.println("Failed to retrieve last_filling_date from Firebase");
  }
}

int get_fill_waterer(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/fill_waterer")) {
      Serial.println("Success on getting value of fill_waterer");
      if (fbdo.dataType() == "int") {
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

void set_days_without_filling(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling", value)) {
      Serial.println("Success on setting value of days_without_filling");
    } else {
      Serial.println("Error setting value of days_without_filling");
    }
  } else {
    Serial.println("Failed to set days_without_filling value in Firebase (Firebase not ready)");
  }
}