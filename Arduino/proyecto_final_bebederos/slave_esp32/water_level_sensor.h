#include "Arduino.h"
#include <stdint.h>

#ifndef WATER_LEVEL_SENSOR_H
#define WATER_LEVEL_SENSOR_H

#define POWER 19 //33
#define PIN_SIGNAL 33 //32

/** Set pins as output or input modes. Set power in HIGH mode */
void setup_water_level_sensor();

/** Get level of water. Returns 0 in case of read failure */
int get_water_level();

#endif