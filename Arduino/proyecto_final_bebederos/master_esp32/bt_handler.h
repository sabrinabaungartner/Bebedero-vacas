#ifndef BT_HANDLER_H
#define BT_HANDLER_H
#include <Arduino.h>

void set_bluetooth_configuration();
void request_water_level();
uint8_t receive_requested_water_level();

#endif