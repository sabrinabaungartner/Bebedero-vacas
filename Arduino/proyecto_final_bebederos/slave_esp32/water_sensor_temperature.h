#include "Arduino.h"
#include <stdint.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef WATER_LEVEL_TEMPERATURE_H
#define WATER_LEVEL_TEMPERATURE_H

#define SENSOR_PIN 5

/** initialize the DS18B20 sensor */
void setup_water_temperature_sensor();

/** Get temperature of water in ºC. Returns 0 in case of error */
float get_water_temperature();

#endif