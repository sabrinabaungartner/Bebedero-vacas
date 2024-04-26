#include <Arduino.h>
#include "slave_serial_bt.h"
#include "water_level_sensor.h"
#include "water_sensor_temperature.h"
#include "fnqueue.h"
#include "timer.h"

// Variables
int seconds = 0;

void funcion_timer() {
  if (seconds == 2) {
    check_bluetooth_SPP_RxHandler();
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
  //check_bluetooth_SPP_RxHandler();
  //delay(2000);
  fnqueue_run();
}