#ifndef BT_HANDLER_H
#define BT_HANDLER_H
#include <Arduino.h>

void setup_bluetooth_configuration();
//void request_water_level_temperature();
void receive_from_slave();
//void request_set_fill_waterer();
//void request_water_pump_status();
int receive_requested_water_level();
float receive_requested_water_temperature();
bool connect_to_slave();
bool check_slave_is_connected();
int receive_water_pump_status();
void request_to_slave(uint8_t request);
uint8_t get_message_received();
#endif