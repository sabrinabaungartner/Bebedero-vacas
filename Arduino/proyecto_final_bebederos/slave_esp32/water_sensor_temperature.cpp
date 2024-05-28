#include "water_sensor_temperature.h"

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float temp_c; // Temperature in Celsius

void setup_water_temperature_sensor() {
  DS18B20.begin();
}

float get_water_temperature() {
  DS18B20.requestTemperatures();
  temp_c = DS18B20.getTempCByIndex(0);
  return temp_c;
}