#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"
#include "fnqueue.h"
#include "timer.h"

// Variables
int seconds = 0;

// Declaration of functions
void iniciar_timer();
void funcion_timer();
void update_status_cattle_waterer(int water_value, float water_temperature);

void update_status_cattle_waterer(int water_value, float water_temperature) {
  backup_current_date();
  set_current_water_level_value(water_value);
  set_current_water_temperature_value(water_temperature);
  set_current_date();
}

void funcion_timer() {
  if (seconds == 5) {
    check_wifi();
    //request_water_level();
    request_water_level_temperature();
    receive_package();
    //array[3] water_lectures = receive_requested_water_level_temperature();
    uint8_t water_value = receive_requested_water_level();
    float water_temperature = receive_requested_water_temperature();
    Serial.print("water level value: ");
    Serial.println(water_value);
    Serial.print("water level temperature: ");
    Serial.println(water_temperature);
    update_status_cattle_waterer((int) water_value, water_temperature);
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