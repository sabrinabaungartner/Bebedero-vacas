#include "water_sensor_temperature.h"

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float temp_c; // Temperature in Celsius

void set_water_temperature() {
  DS18B20.begin();    // initialize the DS18B20 sensor
}

float read_water_temperature() {
  DS18B20.requestTemperatures();       // Send the command to get temperatures
  temp_c = DS18B20.getTempCByIndex(0);  // Read temperature in °C
  return temp_c;
}