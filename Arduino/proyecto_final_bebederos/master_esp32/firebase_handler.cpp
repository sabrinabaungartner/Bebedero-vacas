#include <Arduino.h>
#include "firebase_handler.h"
#include <WiFi.h>
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

  //Firebase.reconnectWiFi(true); /* deprecated */
  Firebase.reconnectNetwork(true);
}

void set_NTP_server() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void set_current_water_level_value(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (!Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/water_level", value)) {
      Serial.println("in function set_current_water_level_value: failed to set data in firebase function");
    }
  } else { Serial.println("in function set_current_water_level_value: Firebase not ready"); }
}

void set_current_water_temperature_value(float value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (!Firebase.RTDB.setFloat(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/water_temperature", value)) {
      Serial.println("in function set_current_water_temperature_value: failed to set data in firebase function");
    }
  } else { Serial.println("in function set_current_water_temperature_value: Firebase not ready"); }
}

void set_current_date(int cattle_waterer_selected) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  char datetime_str[100];

  sprintf(datetime_str, "%02d/%02d/%04d %02d:%02d:%02d",
          timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  Serial.println("Fecha y hora:");
  Serial.println(datetime_str);

  if (Firebase.ready()) {
    if (!Firebase.RTDB.setString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/date", datetime_str)) {
      Serial.println("in function set_current_date: failed to set data in firebase function");
    }
  } 
  else { Serial.println("in function set_current_date: Firebase not ready"); }
}

int get_next_backup_struct(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/next_backup_to_modify")) {
      if (fbdo.dataType() == "int") {
        return fbdo.intData();
      } else { Serial.println("in function get_next_backup_struct: failed in dataType function"); } 
    } else { Serial.println("in function get_next_backup_struct: failed to get data from firebase function"); }
  } else { Serial.println("in function get_next_backup_struct: Firebase not ready"); }

  return 0;
}

void set_current_data_into_backup(int year, int month, int day, int hour, int minute, int second, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    char datetime_str[20];
    sprintf(datetime_str, "%04d%02d%02d_%02d%02d%02d",
            year, month, day,
            hour, minute, second);

    String firebasePath = "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/last_backup_modified";
    if (Firebase.RTDB.setString(&fbdo, firebasePath.c_str(), datetime_str)) {
      Serial.println("Current data stored in Firebase at " + firebasePath);
    } else { Serial.println("in function set_current_data_into_backup: failed to set data in Firebase function"); }
  } else { Serial.println("in function set_current_data_into_backup: Firebase not ready"); }
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

    if (!Firebase.RTDB.setString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_filling_date", last_filling_date_value)) {
      Serial.println("in function set_last_filling_date: failed to set data in Firebase function");
    }
  } else { Serial.println("in function set_last_filling_date: Firebase not ready"); }
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

              if (Firebase.RTDB.setJSON(&fbdo, backupPath, &json)) {
                Serial.println("JSON data backed up successfully in: " + String(backupPath)); // Save current data to backup path

              } else { Serial.println("in function backup_current_data: Error backing up JSON data"); }
            } else { Serial.println("in function backup_current_data: failed to set JSON data object function"); }
          } else { Serial.println("in function backup_current_data: failed jSON data type function"); }
        } else { Serial.println("in function backup_current_data: failed to get JSON data from Firebase function"); }
      } else { Serial.println("in function backup_current_data: failed data type function"); }
    } else { Serial.println("in function backup_current_data: failed to get data from Firebase function"); }
  } else { Serial.println("in function backup_current_data: Firebase not ready"); }
}

int get_cattle_waterer_selected() {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_selected")) {
      if (fbdo.dataType() == "int") {
        return fbdo.intData();
      } else { Serial.println("in function get_cattle_waterer_selected: failed data type function"); }
    } else { Serial.println("in function get_cattle_waterer_selected: failed to get data from Firebase function"); }
  } else { Serial.println("in function get_cattle_waterer_selected: Firebase not ready"); }

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

void update_days_without_filling_in_firebase(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling")){
      if (fbdo.dataType() == "int") {
        int days_without_filling = fbdo.intData();
        days_without_filling += 1;

        if (!Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling", days_without_filling)) { // Actualizar days_without_filling en Firebase
          Serial.println("in function update_days_without_filling_in_firebase: SUCCESS UPDATING DAYS_WITHOUT_FILLING");
        } else { Serial.println("in function update_days_without_filling_in_firebase: failed to update days"); }
      } else { Serial.println("in function update_days_without_filling_in_firebase: failed data type function"); }
    } else { Serial.println("in function update_days_without_filling_in_firebase: failed to get from firebase function"); }
  } else { Serial.println("in function update_days_without_filling_in_firebase: Firebase not ready"); }
}

void update_days_without_filling(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }

    // Retrieve the last filled date from Firebase
    if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_filling_date")) {
      const char* last_filled_date = fbdo.stringData().c_str();

      int year, month, day, hour, minute, second;
      sscanf(last_filled_date, "%4d%2d%2d_%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);

      // Create the time structure for the last filled date
      struct tm lastFilledTime = { 0 };
      lastFilledTime.tm_year = year - 1900;  // Adjust the year
      lastFilledTime.tm_mon = month - 1;     // Adjust the month
      lastFilledTime.tm_mday = day;
      lastFilledTime.tm_hour = hour;
      lastFilledTime.tm_min = minute;
      lastFilledTime.tm_sec = second;

      // Get date from last_check_filling_date from Firebase
      if (Firebase.RTDB.getString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_check_filling_date")) {
        const char* last_check_date = fbdo.stringData().c_str();  // Get last check date as const char*

        int checkYear, checkMonth, checkDay, checkHour, checkMinute, checkSecond;
        sscanf(last_check_date, "%4d%2d%2d_%2d%2d%2d", &checkYear, &checkMonth, &checkDay, &checkHour, &checkMinute, &checkSecond);

        // // Create the time structure for the last filled check date
        struct tm lastCheckTime = { 0 };
        lastCheckTime.tm_year = checkYear - 1900;  // Adjust the year
        lastCheckTime.tm_mon = checkMonth - 1;     // Adjust the month
        lastCheckTime.tm_mday = checkDay;
        lastCheckTime.tm_hour = checkHour;
        lastCheckTime.tm_min = checkMinute;
        lastCheckTime.tm_sec = checkSecond;

        // Check if 20 minutes have passed since last_filling_date
        if (difftime(mktime(&timeinfo), mktime(&lastFilledTime)) >= 1200) { // 20 minutes in seconds (20 * 60 = 1200)
          // If last_check_filling_date is not set (== 0) or 20 minutes have passed since last check
          if (lastCheckTime.tm_year == 0 || difftime(mktime(&timeinfo), mktime(&lastCheckTime)) >= 1200) {
            // Update days_without_filling in Firebase
            update_days_without_filling_in_firebase(cattle_waterer_selected);

            // Update last_check_filling_date with current date actual in Firebase
            char current_date_time[20];
            sprintf(current_date_time, "%04d%02d%02d_%02d%02d%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

            if (Firebase.RTDB.setString(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/backup_data/last_check_filling_date", current_date_time)) {
              Serial.println("in function update_days_without_filling_in_firebase: updated last_check_filling_date in Firebase");
            } else { Serial.println("in function update_days_without_filling: failed to set data last_check_filling_date in Firebase function"); }
          }
        }
      } else { Serial.println("in function update_days_without_filling_in_firebase: failed to get data last_check_filling_date from firebase function"); }
    } else { Serial.println("in function update_days_without_filling_in_firebase: failed to get data last_filling_date from firebase function"); }
  } Serial.println("in function update_days_without_filling: Firebase not ready");
}

int get_fill_waterer(int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/fill_waterer")) {
      if (fbdo.dataType() == "int") {
        Serial.println("Success on getting value of fill_waterer");
        return fbdo.intData();
      } else { Serial.println("in function get_fill_waterer: failed to get data type from firebase function"); }
    } else { Serial.println("in function get_fill_waterer: failed to get data from firebase function"); }
  } else { Serial.println("in function get_fill_waterer: Firebase not ready"); }
}

void set_fill_waterer(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/fill_waterer", value)) {
      Serial.println("in function set_fill_waterer: SUCCESS on setting value of fill_waterer function");
    } else { Serial.println("in function set_fill_waterer: failed to set data in firebase function"); }
  } else { Serial.println("in function set_fill_waterer: Firebase not ready"); }
}

void set_is_water_pump_enabled(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/is_water_pump_enabled", value)) {
      Serial.println("in function set_is_water_pump_enabled: SUCCESS to set value of is_water_pump_enabled in firebase function");
    } else { Serial.println("in function set_is_water_pump_enabled: failed to set data in firebase function"); }
  } else { Serial.println("in function set_is_water_pump_enabled: Firebase not ready"); }
}

void set_days_without_filling(int value, int cattle_waterer_selected) {
  if (Firebase.ready()) {
    if (Firebase.RTDB.setInt(&fbdo, "UsersData/zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2/cattle_waterer_" + String(cattle_waterer_selected) + "/current_data/days_without_filling", value)) {
      Serial.println("in function set_days_without_filling: SUCCESS to set valuf of days_without_filling in firebase function");
    } else { Serial.println("in function set_days_without_filling: failed to set data in firebase function"); }
  } else { Serial.println("in function set_days_without_filling: Firebase not ready"); }
}