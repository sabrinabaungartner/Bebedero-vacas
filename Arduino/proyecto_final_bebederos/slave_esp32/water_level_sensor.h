#include "Arduino.h"
#include <stdint.h>

#ifndef WATER_LEVEL_SENSOR_H
#define WATER_LEVEL_SENSOR_H

#define POWER 19 //33
#define PIN_SIGNAL 15 //32

void set_water_level_parameters();

int read_water_level();

#endif