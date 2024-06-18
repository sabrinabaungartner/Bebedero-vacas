#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H
//#define ERROR_GET_INT_RTDB 99999
#define MAX_BACKUPS 10

/** Set wifi and Firebase connection */
void setup_wifi_firebase();

/** Set NTP server */
void set_NTP_server();

/** Set the current water level value in Firebase database */
void set_current_water_level_value(int value, int cattle_waterer_selected);

/** Set the current water temperature value in Firebase database */
void set_current_water_temperature_value(float value, int cattle_waterer_selected);

/** Set the current date in Firebawe database */
void set_current_date(int cattle_waterer_selected);

/** Backup current data in Firebase database */
void backup_current_data(int cattle_waterer_selected);

/** Get current cattle waterer selected */
int get_cattle_waterer_selected();

/** Get from Firebase database if it is necessary to fill waterer */
int get_fill_waterer(int cattle_waterer_selected);

/** Get max water level from Firebase database */
int get_max_water_level(int cattle_waterer_selected);

/** Set to Firebase database if it neccesary to fill waterer (1) or not (0) */
void set_fill_waterer(int value, int cattle_waterer_selected);

/** Set to Firebase database if water pump is on */
void set_is_water_pump_enabled(int value, int cattle_waterer_selected);

/** Set to Firebase database the last filling date */
void set_last_filling_date(int cattle_waterer_selected);

/** Set to Firebase database the last filling date */
void reset_last_check_filling_date(int cattle_waterer_selected);

/** Get from Firebase database the date of the last backup */
String get_last_backup_date(int cattle_waterer_selected);

/** Update to Firebase database the days without filling the cattle waterer*/
void update_days_without_filling(int cattle_waterer_selected);

/** Set to Firebase database days without filling the cattle waterer in current data */
void set_days_without_filling(int value, int cattle_waterer_selected);

#endif