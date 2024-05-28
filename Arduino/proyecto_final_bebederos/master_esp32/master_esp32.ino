#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"
#include "fnqueue.h"
#include "timer.h"

#define GET_WATER_LEVEL_AND_TEMPERATURE 1
#define REPLY_WATER_LEVEL_AND_TEMPERATURE 2
#define TURN_ON_PUMP 3
#define TURN_OFF_PUMP 4
#define REPLY_TURN_ON_PUMP 5
#define REPLY_TURN_OFF_PUMP 6
#define GET_WATER_LEVEL 7
#define REPLY_WATER_LEVEL 8
#define NULO 0

// Variables
int seconds = 0;
int seconds_filling_waterer = 0;
int water_level = 0;
int cattle_waterer_selected = 1;
int days_without_fill_waterer = 0;
int filling_waterer = 0;
float water_temperature = 0.0;
bool wifi_is_connected = false;
int max_water_level = 0;
int turn_off_water_pump = 0;
int fill_waterer = 0;

// Declaration of functions
void iniciar_timer();
void funcion_timer();
void update_status_cattle_waterer();
void request_and_receive_water_level_and_temperature();
void reset_values_after_filling_waterer();

void request_and_receive_water_level_and_temperature() {
  request_to_slave(GET_WATER_LEVEL_AND_TEMPERATURE);
  receive_from_slave();
  if (get_message_received() == REPLY_WATER_LEVEL_AND_TEMPERATURE) {
    water_level = receive_requested_water_level();
    water_temperature = receive_requested_water_temperature();
    Serial.print("water level value: ");
    Serial.println(water_level);
    Serial.print("water level temperature: ");
    Serial.println(water_temperature);
  }
  else {
    Serial.println("no recibi del slave");
    water_level = 0;
    water_temperature = 0.0;
  }
}

void reset_values_after_filling_waterer() {
  set_is_water_pump_enabled(0, cattle_waterer_selected); // Android app detects this change and use it to enable "rellenar bebedero" button
  set_fill_waterer(0, cattle_waterer_selected);
  set_last_filling_date(cattle_waterer_selected);
  set_days_without_filling(0, cattle_waterer_selected);
}

void update_status_cattle_waterer() {
  set_current_water_level_value(water_level, cattle_waterer_selected);
  set_current_water_temperature_value(water_temperature, cattle_waterer_selected);
  set_current_date(cattle_waterer_selected);
  update_days_without_filling(cattle_waterer_selected);
}

void funcion_timer() {
  if (!check_slave_is_connected()) {
    Serial.println("Serial BT communicaton is not seted!");
    connect_to_slave();
    seconds = 0;
  }

  else {
    if (seconds == 2) {
      fill_waterer = get_fill_waterer(cattle_waterer_selected);
      if ((fill_waterer == 1) && (filling_waterer == 0)) { // Check if it is necessary to refill the waterer and if it is not filling
          request_to_slave(TURN_ON_PUMP);
          receive_from_slave();
          if (get_message_received() == REPLY_TURN_ON_PUMP) {
            set_is_water_pump_enabled(1, cattle_waterer_selected); // Enable water pump in Firebase
            filling_waterer = 1;
            Serial.println("requeri prender la bomba al slave y el slave me respondio que la prendio");
          }
      }
    }

    if (seconds == 4) {
      if ((fill_waterer == 1) && (filling_waterer == 1) && !turn_off_water_pump) { // Check if it is necessary to fill waterer, if it is filling waterer and if not must to turn of the pump
        request_to_slave(GET_WATER_LEVEL);
        receive_from_slave();
        Serial.println("pedi y recibi nivel del agua");
        if (receive_requested_water_level() >= max_water_level) {
          turn_off_water_pump = 1;
          Serial.println("la bomab se tiene que apagar!");
        }
      }
    }

    if (seconds == 6) {
      if (fill_waterer == 1) { // Check if it is necessary to fill waterer and if the waterer is filling
        if (filling_waterer == 1) {
          if (turn_off_water_pump) {
            Serial.println("voy a pedir que se apague la bomba");
            request_to_slave(TURN_OFF_PUMP);
            Serial.println("ya mande que se apague la bomba");
            receive_from_slave();
            Serial.println("Pedi que se apague la bomba, voy a ver qye recbo");
            if (get_message_received() == REPLY_TURN_OFF_PUMP) {
              Serial.println("me respondieron que se apago la bomba");
              reset_values_after_filling_waterer();
              filling_waterer = 0;
              turn_off_water_pump = 0;
              fill_waterer = 0;
            }
          }
        }
        seconds = 0;
      }
    }

    if (seconds == 8) {
      request_and_receive_water_level_and_temperature();
    }

    if (seconds == 10) {
      update_status_cattle_waterer();
    }

    if (seconds == 12) {
      backup_current_data(cattle_waterer_selected);
      seconds = 0;
    }
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
  setup_bluetooth_configuration();
  fnqueue_init();
  iniciar_timer();
  set_last_filling_date(cattle_waterer_selected);
  //set last_check_filling date en cero
  max_water_level = get_max_water_level(cattle_waterer_selected);
  Serial.print("max water level: ");
  Serial.println(max_water_level);
}

void loop() {
  fnqueue_run();
}