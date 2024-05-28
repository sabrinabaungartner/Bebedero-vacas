#include "Arduino.h"
#include <stdint.h>

#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#define PIN_RELAY 21

void setup_water_pump();
void turn_on_water_pump();
void turn_off_water_pump();

#endif