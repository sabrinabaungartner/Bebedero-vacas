#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H
#define ERROR_GET_INT_RTDB 99999
#define MAX_BACKUPS 10

void check_wifi();
void setup_wifi();
void set_current_water_level_value(int value);
void set_current_water_temperature_value(float value);
void set_current_date();
void set_NTP_server();
void backup_current_date();
void get_cattle_waterer_selected();

#endif