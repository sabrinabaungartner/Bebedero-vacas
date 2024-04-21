#include "Arduino.h"
#include <stdint.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef WATER_LEVEL_TEMPERATURE_H
#define WATER_LEVEL_TEMPERATURE_H

#define SENSOR_PIN 5

void set_water_temperature();
float read_water_temperature();

#endif