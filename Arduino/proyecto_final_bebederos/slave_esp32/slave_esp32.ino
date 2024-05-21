#include <Arduino.h>
#include "slave_serial_bt.h"
#include "water_level_sensor.h"
#include "water_sensor_temperature.h"
#include "fnqueue.h"
#include "timer.h"

// Variables
uint8_t seconds = 0;
uint8_t request_received = 0;
int water_level = 0;
float water_temperature = 0.0;
int reading_from_sensors = 0;

void read_from_sensors() {
  water_level = read_water_level();
  water_temperature = read_water_temperature();
}

void funcion_timer() {
  if (seconds == 1) {
    request_received = bluetooth_SPP_RxHandler();
    /*if (request_received) {
      read_from_sensors();
      bluetooth_SPP_TxHandler(water_level, water_temperature);
      request_received = 0;
    }*/
    //seconds = 0;
  }

  if (seconds == 2) {
    if (request_received) {
      read_from_sensors();
      reading_from_sensors = 1;
    }
  }

  if (seconds == 3) {
    if (request_received && reading_from_sensors) {
      request_received = 0;
      reading_from_sensors = 0;
      bluetooth_SPP_TxHandler(water_level, water_temperature);
    }

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
  set_bluetooth_configuration();
  set_water_level_parameters();
  set_water_temperature();
  fnqueue_init();
  iniciar_timer();
}

void loop() {
  fnqueue_run();
}