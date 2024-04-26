#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H
#define ERROR_GET_INT_RTDB 99999
#define MAX_BACKUPS 10

void setup_wifi();
void check_wifi();
void set_current_water_level_value(int value, int cattle_waterer_selected);
void set_current_water_temperature_value(float value, int cattle_waterer_selected);
void set_current_date(int cattle_waterer_selected);
void set_NTP_server();
void backup_current_date(int cattle_waterer_selected);
//void get_user_selected();
int get_cattle_waterer_selected();

#endif