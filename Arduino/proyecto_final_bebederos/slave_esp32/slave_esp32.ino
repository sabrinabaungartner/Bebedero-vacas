#include <Arduino.h>
#include "slave_serial_bt.h"
#include "water_level_sensor.h"
#include "water_sensor_temperature.h"
#include "fnqueue.h"
#include "timer.h"
#include "water_pump.h"

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
uint8_t seconds = 0;
uint8_t request_received = 0;
uint8_t request = 0;
int water_level = 0;
float water_temperature = 0.0;

void iniciar_timer();
void funcion_timer();
void check_request();
void determine_request();
void response_request();
void read_water_temperatura_from_sensor();
void read_water_level_from_sensor();

void setup() {
  Serial.begin(115200);
  setup_bluetooth_configuration();
  setup_water_level_sensor();
  setup_water_temperature_sensor();
  setup_water_pump();
  fnqueue_init();
  iniciar_timer();
}

void loop() {
  fnqueue_run();
}

void iniciar_timer(){
  timer_init(&funcion_timer, 1000000);
  timer_start();
}

void funcion_timer() {
  if (seconds == 1) {
    check_request();
  }

  if (seconds == 2) {
    if (request_received) {
      Serial.println("recibi algo!");
      determine_request();
    }
  }

  if (seconds == 3) {
    if (request_received) {
      response_request();
    }
    seconds = 0;
  }

  seconds += 1;
}

void check_request() {
  request_received = bluetooth_SPP_RxHandler();
}

void determine_request() {
  request = get_message_type_of_request();

  switch(request) {
    case GET_WATER_LEVEL_AND_TEMPERATURE:
      Serial.println("Me pidieron nivel y temperatura");
      read_water_level_from_sensor();
      read_water_temperature_from_sensor();
      break;
    case GET_WATER_LEVEL:
      Serial.println("Me pidieron nivel del agua");
      read_water_level_from_sensor();
      break;
    case TURN_ON_PUMP:
      Serial.println("Me pidieron prender la bomba");
      turn_on_water_pump();
      break;
    case TURN_OFF_PUMP:
      turn_off_water_pump();
      break;
  }
}

void read_water_level_from_sensor() {
  water_level = get_water_level();
}

void read_water_temperature_from_sensor() {
  water_temperature = get_water_temperature();
}

void response_request() {
  switch(request) {
    case GET_WATER_LEVEL_AND_TEMPERATURE:
      Serial.println("Voy a enviar nivel y temperatura");
      bluetooth_SPP_TxHandler(water_level, water_temperature);
      break;
    case GET_WATER_LEVEL:
      bluetooth_SPP_TxHandler(water_level, NULO);
      break;
    case TURN_ON_PUMP:
      bluetooth_SPP_TxHandler(NULO, NULO);
      break;
    case TURN_OFF_PUMP:
      bluetooth_SPP_TxHandler(NULO, NULO);
      break;
  }
}
