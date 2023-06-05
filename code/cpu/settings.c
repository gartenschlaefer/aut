// --
// settings

#include <stdlib.h>
#include "settings.h"
#include "memory_app.h"


/* ------------------------------------------------------------------*
 *            create settings
 * ------------------------------------------------------------------*/

struct Settings *Settings_New(void)
{
  // init main settings
  struct Settings *settings = malloc(sizeof(*settings));
  if(settings == NULL) { return NULL; }

  // create sub settings
  settings->settings_circulate = malloc(sizeof(settings->settings_circulate));
  settings->settings_air = malloc(sizeof(settings->settings_air));
  settings->settings_set_down = malloc(sizeof(settings->settings_set_down));
  settings->settings_pump_off = malloc(sizeof(settings->settings_pump_off));
  settings->settings_mud = malloc(sizeof(settings->settings_mud));
  settings->settings_compressor = malloc(sizeof(settings->settings_compressor));
  settings->settings_phosphor = malloc(sizeof(settings->settings_phosphor));
  settings->settings_inflow_pump = malloc(sizeof(settings->settings_inflow_pump));
  settings->settings_calibration = malloc(sizeof(settings->settings_calibration));
  settings->settings_alarm = malloc(sizeof(settings->settings_alarm));
  settings->settings_zone = malloc(sizeof(settings->settings_zone));

  return settings;
}


/* ------------------------------------------------------------------*
 *            init all settings
 * ------------------------------------------------------------------*/

void Settings_Init(struct Settings *settings)
{
  Settings_Read_Circulate(settings->settings_circulate);
  Settings_Read_Air(settings->settings_air);
  Settings_Read_SetDown(settings->settings_set_down);
  Settings_Read_PumpOff(settings->settings_pump_off);
  Settings_Read_Mud(settings->settings_mud);
  Settings_Read_Compressor(settings->settings_compressor);
  Settings_Read_Phosphor(settings->settings_phosphor);
  Settings_Read_InflowPump(settings->settings_inflow_pump);
  Settings_Read_Calibration(settings->settings_calibration);
  Settings_Read_Alarm(settings->settings_alarm);
  Settings_Read_Zone(settings->settings_zone);
}


/* ------------------------------------------------------------------*
 *            circulate settings
 * ------------------------------------------------------------------*/

void Settings_Read_Circulate(struct SettingsCirculate *settings_circulate)
{
  settings_circulate->on_min = MEM_EEPROM_ReadVar(ON_circ);
  settings_circulate->off_min = MEM_EEPROM_ReadVar(OFF_circ);
  settings_circulate->time_min = ((MEM_EEPROM_ReadVar(TIME_H_circ) << 8) | MEM_EEPROM_ReadVar(TIME_L_circ));
  settings_circulate->sensor_in_tank = MEM_EEPROM_ReadVar(SENSOR_inTank);
  settings_circulate->val_lim_on_min = (struct ValueLimit){ .max_value = 30, .min_value = 2 };
  settings_circulate->val_lim_off_min = (struct ValueLimit){ .max_value = 60, .min_value = 5 };
  settings_circulate->val_lim_time_min = (struct ValueLimit){ .max_value = 999, .min_value = 0 };
}

void Settings_Save_Circulate(struct SettingsCirculate *settings_circulate)
{
  MEM_EEPROM_WriteVar(ON_circ, settings_circulate->on_min);
  MEM_EEPROM_WriteVar(OFF_circ, settings_circulate->off_min);
  MEM_EEPROM_WriteVar(TIME_L_circ, (unsigned char)(settings_circulate->time_min & 0x00FF));
  MEM_EEPROM_WriteVar(TIME_H_circ, (unsigned char)((settings_circulate->time_min & 0xFF00) >> 8));
  MEM_EEPROM_WriteVar(SENSOR_inTank, settings_circulate->sensor_in_tank);
}


/* ------------------------------------------------------------------*
 *            air settings
 * ------------------------------------------------------------------*/

void Settings_Read_Air(struct SettingsAir *settings_air)
{
  settings_air->on_min = MEM_EEPROM_ReadVar(ON_air);
  settings_air->off_min = MEM_EEPROM_ReadVar(OFF_air);
  settings_air->time_min = ((MEM_EEPROM_ReadVar(TIME_H_air) << 8) | MEM_EEPROM_ReadVar(TIME_L_air));
  settings_air->val_lim_on_min = (struct ValueLimit){ .max_value = 60, .min_value = 10 };
  settings_air->val_lim_off_min = (struct ValueLimit){ .max_value = 55, .min_value = 5 };
  settings_air->val_lim_time_min = (struct ValueLimit){ .max_value = 999, .min_value = 0 };
}

void Settings_Save_Air(struct SettingsAir *settings_air)
{
  MEM_EEPROM_WriteVar(ON_air, settings_air->on_min);
  MEM_EEPROM_WriteVar(OFF_air, settings_air->off_min);
  MEM_EEPROM_WriteVar(TIME_L_air, (unsigned char)(settings_air->time_min & 0x00FF));
  MEM_EEPROM_WriteVar(TIME_H_air, (unsigned char)((settings_air->time_min & 0xFF00) >> 8));
}


/* ------------------------------------------------------------------*
 *            set down settings
 * ------------------------------------------------------------------*/

void Settings_Read_SetDown(struct SettingsSetDown *settings_set_down)
{
  settings_set_down->time_min = MEM_EEPROM_ReadVar(TIME_setDown);
  settings_set_down->val_lim_time_min = (struct ValueLimit){ .max_value = 90, .min_value = 50 };
}

void Settings_Save_SetDown(struct SettingsSetDown *settings_set_down)
{
  MEM_EEPROM_WriteVar(TIME_setDown, settings_set_down->time_min);
}


/* ------------------------------------------------------------------*
 *            pump off setting
 * ------------------------------------------------------------------*/

void Settings_Read_PumpOff(struct SettingsPumpOff *settings_pump_off)
{
  settings_pump_off->on_min = MEM_EEPROM_ReadVar(ON_pumpOff);
  settings_pump_off->pump = MEM_EEPROM_ReadVar(PUMP_pumpOff);
  settings_pump_off->val_lim_on_min = (struct ValueLimit){ .max_value = 60, .min_value = 5 };
}

void Settings_Save_PumpOff(struct SettingsPumpOff *settings_pump_off)
{
  MEM_EEPROM_WriteVar(ON_pumpOff, settings_pump_off->on_min);
  MEM_EEPROM_WriteVar(PUMP_pumpOff, settings_pump_off->pump);
}


/* ------------------------------------------------------------------*
 *            mud setting
 * ------------------------------------------------------------------*/

void Settings_Read_Mud(struct SettingsMud *settings_mud)
{
  settings_mud->on_min = MEM_EEPROM_ReadVar(ON_MIN_mud);
  settings_mud->on_sec = MEM_EEPROM_ReadVar(ON_SEC_mud);
  settings_mud->val_lim_on_min = (struct ValueLimit){ .max_value = 20, .min_value = 0 };
  settings_mud->val_lim_on_sec = (struct ValueLimit){ .max_value = 59, .min_value = 0 };
}

void Settings_Save_Mud(struct SettingsMud *settings_mud)
{
  MEM_EEPROM_WriteVar(ON_MIN_mud, settings_mud->on_min);
  MEM_EEPROM_WriteVar(ON_SEC_mud, settings_mud->on_sec);
}


/* ------------------------------------------------------------------*
 *            compressor setting
 * ------------------------------------------------------------------*/

void Settings_Read_Compressor(struct SettingsCompressor *settings_compressor)
{
  settings_compressor->min_pressure = ((MEM_EEPROM_ReadVar(MIN_H_pressure) << 8) | MEM_EEPROM_ReadVar(MIN_L_pressure));
  settings_compressor->max_pressure = ((MEM_EEPROM_ReadVar(MAX_H_pressure) << 8) | MEM_EEPROM_ReadVar(MAX_L_pressure));
  settings_compressor->val_lim_min_pressure = (struct ValueLimit){ .max_value = 999, .min_value = 0 };
  settings_compressor->val_lim_max_pressure = (struct ValueLimit){ .max_value = 999, .min_value = 0 };
}

void Settings_Save_Compressor(struct SettingsCompressor *settings_compressor)
{
  MEM_EEPROM_WriteVar(MIN_L_pressure, (unsigned char)(settings_compressor->min_pressure & 0x00FF));
  MEM_EEPROM_WriteVar(MIN_H_pressure, (unsigned char)((settings_compressor->min_pressure & 0xFF00) >> 8));
  MEM_EEPROM_WriteVar(MAX_L_pressure, (unsigned char)(settings_compressor->max_pressure & 0x00FF));
  MEM_EEPROM_WriteVar(MAX_H_pressure, (unsigned char)((settings_compressor->max_pressure & 0xFF00) >> 8));
}


/* ------------------------------------------------------------------*
 *            phosphor settings
 * ------------------------------------------------------------------*/

void Settings_Read_Phosphor(struct SettingsPhosphor *settings_phosphor)
{
  settings_phosphor->on_min = MEM_EEPROM_ReadVar(ON_phosphor);
  settings_phosphor->off_min = MEM_EEPROM_ReadVar(OFF_phosphor);
  settings_phosphor->val_lim_on_min = (struct ValueLimit){ .max_value = 60, .min_value = 0 };
  settings_phosphor->val_lim_off_min = (struct ValueLimit){ .max_value = 60, .min_value = 0 };
}

void Settings_Save_Phosphor(struct SettingsPhosphor *settings_phosphor)
{
  MEM_EEPROM_WriteVar(ON_phosphor, settings_phosphor->on_min);
  MEM_EEPROM_WriteVar(OFF_phosphor, settings_phosphor->off_min);
}


/* ------------------------------------------------------------------*
 *            inflow pump settings
 * ------------------------------------------------------------------*/

void Settings_Read_InflowPump(struct SettingsInflowPump *settings_inflow_pump)
{
  settings_inflow_pump->on_min = MEM_EEPROM_ReadVar(ON_inflowPump);
  settings_inflow_pump->off_min = MEM_EEPROM_ReadVar(OFF_inflowPump);
  settings_inflow_pump->off_hou = MEM_EEPROM_ReadVar(T_IP_off_h);
  settings_inflow_pump->pump = MEM_EEPROM_ReadVar(PUMP_inflowPump);
  settings_inflow_pump->sensor_out_tank = MEM_EEPROM_ReadVar(SENSOR_outTank);
  settings_inflow_pump->val_lim_on_min = (struct ValueLimit){ .max_value = 60, .min_value = 0 };
  settings_inflow_pump->val_lim_off_min = (struct ValueLimit){ .max_value = 59, .min_value = 0 };
  settings_inflow_pump->val_lim_off_hou = (struct ValueLimit){ .max_value = 99, .min_value = 0 };
}

void Settings_Save_InflowPump(struct SettingsInflowPump *settings_inflow_pump)
{
  MEM_EEPROM_WriteVar(ON_inflowPump, settings_inflow_pump->on_min);
  MEM_EEPROM_WriteVar(OFF_inflowPump, settings_inflow_pump->off_min);
  MEM_EEPROM_WriteVar(T_IP_off_h, settings_inflow_pump->off_hou);
  MEM_EEPROM_WriteVar(PUMP_inflowPump, settings_inflow_pump->pump);
  MEM_EEPROM_WriteVar(SENSOR_outTank, settings_inflow_pump->sensor_out_tank);
}


/* ------------------------------------------------------------------*
 *            calibration settings
 * ------------------------------------------------------------------*/

void Settings_Read_Calibration(struct SettingsCalibration *settings_calibration)
{
  settings_calibration->redo_on = MEM_EEPROM_ReadVar(CAL_Redo_on);
  settings_calibration->zero_offset_pressure = ((MEM_EEPROM_ReadVar(CAL_ZeroOffsetPressure_H) << 8) | MEM_EEPROM_ReadVar(CAL_ZeroOffsetPressure_L));
  settings_calibration->tank_level_min_pressure = ((MEM_EEPROM_ReadVar(TANK_LV_MinPressure_H) << 8) | MEM_EEPROM_ReadVar(TANK_LV_MinPressure_L));
  settings_calibration->tank_level_min_sonic = ((MEM_EEPROM_ReadVar(TANK_LV_Sonic_H) << 8) | MEM_EEPROM_ReadVar(TANK_LV_Sonic_L));
}

void Settings_Save_Calibration(struct SettingsCalibration *settings_calibration)
{
  MEM_EEPROM_WriteVar(CAL_Redo_on, settings_calibration->redo_on);
  MEM_EEPROM_WriteVar(CAL_ZeroOffsetPressure_L, (unsigned char)(settings_calibration->tank_level_min_pressure & 0x00FF));
  MEM_EEPROM_WriteVar(CAL_ZeroOffsetPressure_H, (unsigned char)((settings_calibration->tank_level_min_pressure & 0xFF00) >> 8));
  MEM_EEPROM_WriteVar(TANK_LV_MinPressure_L, (unsigned char)(settings_calibration->tank_level_min_pressure & 0x00FF));
  MEM_EEPROM_WriteVar(TANK_LV_MinPressure_H, (unsigned char)((settings_calibration->tank_level_min_pressure & 0xFF00) >> 8));
  MEM_EEPROM_WriteVar(TANK_LV_Sonic_L, (unsigned char)(settings_calibration->tank_level_min_sonic & 0x00FF));
  MEM_EEPROM_WriteVar(TANK_LV_Sonic_H, (unsigned char)((settings_calibration->tank_level_min_sonic & 0xFF00) >> 8));
}


/* ------------------------------------------------------------------*
 *            alarm settings
 * ------------------------------------------------------------------*/

void Settings_Read_Alarm(struct SettingsAlarm *settings_alarm)
{
  settings_alarm->sensor = MEM_EEPROM_ReadVar(ALARM_sensor);
  settings_alarm->compressor = MEM_EEPROM_ReadVar(ALARM_comp);
  settings_alarm->temp = MEM_EEPROM_ReadVar(ALARM_temp);
  settings_alarm->val_lim_temp = (struct ValueLimit){ .max_value = 99, .min_value = 15 };
}

void Settings_Save_Alarm(struct SettingsAlarm *settings_alarm)
{
  MEM_EEPROM_WriteVar(ALARM_sensor, settings_alarm->sensor);
  MEM_EEPROM_WriteVar(ALARM_comp, settings_alarm->compressor);
  MEM_EEPROM_WriteVar(ALARM_temp, (unsigned char)settings_alarm->temp);
}


/* ------------------------------------------------------------------*
 *            zone settings
 * ------------------------------------------------------------------*/

void Settings_Read_Zone(struct SettingsZone *settings_zone)
{
  settings_zone->sonic_on = MEM_EEPROM_ReadVar(SONIC_on);
  settings_zone->level_to_air = ((MEM_EEPROM_ReadVar(TANK_LV_LevelToAir_H) << 8) | MEM_EEPROM_ReadVar(TANK_LV_LevelToAir_L));
  settings_zone->level_to_set_down = ((MEM_EEPROM_ReadVar(TANK_LV_LevelToSetDown_H) << 8) | MEM_EEPROM_ReadVar(TANK_LV_LevelToSetDown_L));
  settings_zone->val_lim_level_to_air = (struct ValueLimit){ .max_value = 999, .min_value = 0 };
  settings_zone->val_lim_level_to_set_down = (struct ValueLimit){ .max_value = 999, .min_value = 0 };
}

void Settings_Save_Zone(struct SettingsZone *settings_zone)
{
  MEM_EEPROM_WriteVar(SONIC_on, settings_zone->sonic_on);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToAir_L, (unsigned char)(settings_zone->level_to_air & 0x00FF));
  MEM_EEPROM_WriteVar(TANK_LV_LevelToAir_H, (unsigned char)((settings_zone->level_to_air & 0xFF00) >> 8));
  MEM_EEPROM_WriteVar(TANK_LV_LevelToSetDown_L, (unsigned char)(settings_zone->level_to_set_down & 0x00FF));
  MEM_EEPROM_WriteVar(TANK_LV_LevelToSetDown_H, (unsigned char)((settings_zone->level_to_set_down & 0xFF00) >> 8));
}


/* ------------------------------------------------------------------*
 *            destroy settings
 * ------------------------------------------------------------------*/

void Settings_Destroy(struct Settings *settings)
{
  if(settings == NULL){ return; }

  // free sub settings
  free(settings->settings_circulate);
  free(settings->settings_air);
  free(settings->settings_set_down);
  free(settings->settings_pump_off);
  free(settings->settings_mud);
  free(settings->settings_compressor);
  free(settings->settings_phosphor);
  free(settings->settings_inflow_pump);
  free(settings->settings_calibration);
  free(settings->settings_alarm);
  free(settings->settings_zone);

  // free at last
  free(settings);
}