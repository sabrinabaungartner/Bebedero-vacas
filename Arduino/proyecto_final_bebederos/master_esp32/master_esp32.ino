#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"
#include "fnqueue.h"
#include "timer.h"

// Variables
int seconds = 0;
int seconds_filling_waterer = 0;
int water_value = 0;
int cattle_waterer_selected = 1;
int days_without_fill_waterer = 0;
int filling_waterer = 0;
float water_temperature = 0.0;
bool wifi_is_connected = false;

// Declaration of functions
void iniciar_timer();
void funcion_timer();
void update_status_cattle_waterer();

void update_status_cattle_waterer() {
  set_current_water_level_value(water_value, cattle_waterer_selected);
  set_current_water_temperature_value(water_temperature, cattle_waterer_selected);
  set_current_date(cattle_waterer_selected);
  update_days_without_filling(cattle_waterer_selected);
}

void funcion_timer() {
  if (seconds == 2) {
    //wifi_is_connected = check_wifi();
    //cattle_waterer_selected = get_cattle_waterer_selected();
  }

  if (seconds == 5) {
    if (get_fill_waterer(cattle_waterer_selected) == 1) {
      if (seconds_filling_waterer == 0) {
        set_is_water_pump_enabled(1, cattle_waterer_selected);
        filling_waterer = 1;
      }

      if (seconds_filling_waterer == 1) {
        seconds_filling_waterer = 0;
        set_is_water_pump_enabled(0, cattle_waterer_selected); // Android app detects this change and use it to enable "rellenar bebedero" button
        set_fill_waterer(0, cattle_waterer_selected);
        set_last_filling_date(cattle_waterer_selected);
        set_days_without_filling(0, cattle_waterer_selected);
        filling_waterer = 0;
      }
      
      seconds_filling_waterer += 1;
      seconds = 0;
    }
  }

  if ((seconds == 10) && !filling_waterer) {
    request_water_level_temperature();
    receive_water_level_temperature();
    water_value = receive_requested_water_level();
    water_temperature = receive_requested_water_temperature();
    Serial.print("water level value: ");
    Serial.println(water_value);
    Serial.print("water level temperature: ");
    Serial.println(water_temperature);
  }

  if ((seconds == 15) && !filling_waterer) {
    update_status_cattle_waterer();
  }

  if ((seconds == 20) && !filling_waterer) {
    backup_current_data(cattle_waterer_selected);
    seconds = 0;
  }

  seconds += 1;
}

void iniciar_timer(){
  timer_init(&funcion_timer, 1000000);
  timer_start();
}

void setup() {
  Serial.begin(115200);
  setup_wifi_firebase();
  set_NTP_server();
  set_bluetooth_configuration();
  fnqueue_init();
  iniciar_timer();
}

void loop() {
  fnqueue_run();
}