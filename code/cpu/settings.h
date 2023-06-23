// --
// basic functions, such as init, watchdog, clock

// include guard
#ifndef SETTINGS_H   
#define SETTINGS_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

struct Settings *Settings_New(void);
void Settings_Init(struct Settings *settings);
void Settings_Read_Circulate(struct SettingsCirculate *settings_circulate);
void Settings_Save_Circulate(struct SettingsCirculate *settings_circulate);
void Settings_Read_Air(struct SettingsAir *settings_air);
void Settings_Save_Air(struct SettingsAir *settings_air);
void Settings_Read_SetDown(struct SettingsSetDown *settings_set_down);
void Settings_Save_SetDown(struct SettingsSetDown *settings_set_down);
void Settings_Read_PumpOff(struct SettingsPumpOff *settings_pump_off);
void Settings_Save_PumpOff(struct SettingsPumpOff *settings_pump_off);
void Settings_Read_Mud(struct SettingsMud *settings_mud);
void Settings_Save_Mud(struct SettingsMud *settings_mud);
void Settings_Read_Compressor(struct SettingsCompressor *settings_compressor);
void Settings_Save_Compressor(struct SettingsCompressor *settings_compressor);
void Settings_Read_Phosphor(struct SettingsPhosphor *settings_phosphor);
void Settings_Save_Phosphor(struct SettingsPhosphor *settings_phosphor);
void Settings_Read_InflowPump(struct SettingsInflowPump *settings_inflow_pump);
void Settings_Save_InflowPump(struct SettingsInflowPump *settings_inflow_pump);
void Settings_Read_Calibration(struct SettingsCalibration *settings_calibration);
void Settings_Save_Calibration(struct SettingsCalibration *settings_calibration);
void Settings_Read_Alarm(struct SettingsAlarm *settings_alarm);
void Settings_Save_Alarm(struct SettingsAlarm *settings_alarm);
void Settings_Read_Zone(struct SettingsZone *settings_zone);
void Settings_Save_Zone(struct PlantState *ps);
void Settings_Destroy(struct Settings *settings);


#endif