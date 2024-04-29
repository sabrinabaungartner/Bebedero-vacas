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
void backup_current_data(int cattle_waterer_selected);
//void get_user_selected();
int get_cattle_waterer_selected();
//void set_days_without_fill(int cattle_waterer_selected);
String get_last_backup_date(int cattle_waterer_selected);
//void check_and_delete_old_backups(int cattle_waterer_selected);
void update_days_without_filling(int cattle_waterer_selected);

#endif