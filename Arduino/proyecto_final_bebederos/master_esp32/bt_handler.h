#ifndef BT_HANDLER_H
#define BT_HANDLER_H
#include <Arduino.h>

void set_bluetooth_configuration();
//void request_water_level();
void request_water_level_temperature();
void receive_package();
uint8_t receive_requested_water_level();
float receive_requested_water_temperature();

#endif