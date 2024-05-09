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

void read_from_sensors() {
  water_level = read_water_level();
  water_temperature = read_water_temperature();
}

void funcion_timer() {
  if (seconds == 2) {
    request_received = bluetooth_SPP_RxHandler();
    if (request_received) {
      read_from_sensors();
      bluetooth_SPP_TxHandler(water_level, water_temperature);
      request_received = 0;
    }
    seconds = 0;
  }

  /*if (seconds == 3 && request_received) {
    read_from_sensors();
  }

  if (seconds == 4 && request_received) {
    request_received = 0;
    seconds = 0;
    bluetooth_SPP_TxHandler(water_level, water_temperature);
  }*/

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