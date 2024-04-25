#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"
#include "fnqueue.h"
#include "timer.h"

// Variables
int seconds = 0;
uint8_t water_value = 0;
float water_temperature = 0.0;

// Declaration of functions
void iniciar_timer();
void funcion_timer();
void update_status_cattle_waterer();

void update_status_cattle_waterer() {
  set_current_water_level_value((int) water_value);
  set_current_water_temperature_value(water_temperature);
  set_current_date();
}

void funcion_timer() {
  if (seconds == 5) {
    check_wifi();
  }

  if (seconds == 10) {
    backup_current_date();
  }

  if (seconds == 15) {
    //get_user_selected();
    //get_cattle_waterer_selected();
    request_water_level_temperature();
    receive_water_level_temperature();
    water_value = receive_requested_water_level();
    water_temperature = receive_requested_water_temperature();
    Serial.print("water level value: ");
    Serial.println(water_value);
    Serial.print("water level temperature: ");
    Serial.println(water_temperature);
  }

  if (seconds == 20) {
    update_status_cattle_waterer();
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
  setup_wifi();
  set_NTP_server();
  set_bluetooth_configuration();
  fnqueue_init();
  iniciar_timer();
}

void loop() {
  fnqueue_run();
}