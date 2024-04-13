#include <Arduino.h>
#include "firebase_handler.h"
#include "bt_handler.h"
#include "fnqueue.h"
#include "timer.h"

// Variables
int seconds = 0;

// Function declarations
void iniciar_timer();
void funcion_timer();

void funcion_timer() {
  if (seconds == 5) {
    Serial.println("Entrando en if de funcion_timer");
    check_wifi();
    get_value_example();
    request_water_level();
    uint8_t water_value = receive_requested_water_level();
    Serial.print("water level value: ");
    Serial.println(water_value);
    seconds = 0;
    Serial.println("Saliendo de if de funcion_timer");
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
  set_bluetooth_configuration();
  fnqueue_init();
  iniciar_timer();
}

void loop() {
  fnqueue_run();
  //Serial.println("En loop");
}