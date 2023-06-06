// --
//  internal memory applications

#include "memory_app.h"

#include "memory_func.h"
#include "mcp7941_driver.h"


/* ------------------------------------------------------------------*
 *            write var default - normal
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault(void)
{
  MEM_EEPROM_WriteVar(ON_circ, 5);
  MEM_EEPROM_WriteVar(OFF_circ, 25);
  MEM_EEPROM_WriteVar(ON_air, 30);
  MEM_EEPROM_WriteVar(OFF_air, 30);
  MEM_EEPROM_WriteVar(TIME_setDown, 60);
  MEM_EEPROM_WriteVar(ON_pumpOff, 20);
  MEM_EEPROM_WriteVar(PUMP_pumpOff, 0);
  MEM_EEPROM_WriteVar(ON_MIN_mud, 0);
  MEM_EEPROM_WriteVar(ON_SEC_mud, 30);
  MEM_EEPROM_WriteVar(ON_phosphor, 0);
  MEM_EEPROM_WriteVar(OFF_phosphor, 55);
  MEM_EEPROM_WriteVar(ON_inflowPump, 0);
  MEM_EEPROM_WriteVar(OFF_inflowPump, 30);
  MEM_EEPROM_WriteVar(PUMP_inflowPump, 0);
  MEM_EEPROM_WriteVar(SENSOR_inTank, 0);
  MEM_EEPROM_WriteVar(SENSOR_outTank, 0);
  MEM_EEPROM_WriteVar(TIME_H_circ, 0);
  MEM_EEPROM_WriteVar(TIME_L_circ, 240);
  MEM_EEPROM_WriteVar(TIME_H_air, 0);
  MEM_EEPROM_WriteVar(TIME_L_air, 240);
  MEM_EEPROM_WriteVar(MIN_H_pressure, 0);
  MEM_EEPROM_WriteVar(MIN_L_pressure, 35);
  MEM_EEPROM_WriteVar(MAX_H_pressure, 0);
  MEM_EEPROM_WriteVar(MAX_L_pressure, 250);
  MEM_EEPROM_WriteVar(ALARM_temp, 60);
  MEM_EEPROM_WriteVar(ALARM_comp, 1);
  MEM_EEPROM_WriteVar(ALARM_sensor, 1);
  MEM_EEPROM_WriteVar(CAL_ZeroOffsetPressure_H, 0);
  MEM_EEPROM_WriteVar(CAL_ZeroOffsetPressure_L, 0);
  MEM_EEPROM_WriteVar(TANK_LV_Sonic_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_Sonic_L, 0);
  MEM_EEPROM_WriteVar(SONIC_on, 0);

  MEM_EEPROM_WriteVar(CAL_Redo_on, 0);
  MEM_EEPROM_WriteVar(T_IP_off_h, 0);

  MEM_EEPROM_WriteVar(TANK_LV_MinPressure_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_MinPressure_L, 120);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToAir_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToAir_L, 10);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToSetDown_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToSetDown_L, 15);
}


/* ------------------------------------------------------------------*
 *            write var default - short
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault_Short(void)
{
  MEM_EEPROM_WriteVar(ON_circ, 1);
  MEM_EEPROM_WriteVar(OFF_circ, 1);
  MEM_EEPROM_WriteVar(ON_air, 1);
  MEM_EEPROM_WriteVar(OFF_air, 1);
  MEM_EEPROM_WriteVar(TIME_setDown, 1);
  MEM_EEPROM_WriteVar(ON_pumpOff, 1);
  MEM_EEPROM_WriteVar(PUMP_pumpOff, 0);
  MEM_EEPROM_WriteVar(ON_MIN_mud, 0);
  MEM_EEPROM_WriteVar(ON_SEC_mud, 20);
  MEM_EEPROM_WriteVar(ON_phosphor, 1);
  MEM_EEPROM_WriteVar(OFF_phosphor, 1);
  MEM_EEPROM_WriteVar(ON_inflowPump, 2);
  MEM_EEPROM_WriteVar(OFF_inflowPump, 2);
  MEM_EEPROM_WriteVar(PUMP_inflowPump, 0);
  MEM_EEPROM_WriteVar(SENSOR_inTank, 0);
  MEM_EEPROM_WriteVar(SENSOR_outTank, 0);
  MEM_EEPROM_WriteVar(TIME_H_circ, 0);
  MEM_EEPROM_WriteVar(TIME_L_circ, 5);
  MEM_EEPROM_WriteVar(TIME_H_air, 0);
  MEM_EEPROM_WriteVar(TIME_L_air, 3);
  MEM_EEPROM_WriteVar(MIN_H_pressure, 0);
  MEM_EEPROM_WriteVar(MIN_L_pressure, 20);
  MEM_EEPROM_WriteVar(MAX_H_pressure, 0);
  MEM_EEPROM_WriteVar(MAX_L_pressure, 250);
  MEM_EEPROM_WriteVar(ALARM_temp, 60);
  MEM_EEPROM_WriteVar(ALARM_comp, 1);
  MEM_EEPROM_WriteVar(ALARM_sensor, 1);
  MEM_EEPROM_WriteVar(CAL_ZeroOffsetPressure_H, 0);
  MEM_EEPROM_WriteVar(CAL_ZeroOffsetPressure_L, 0);
  MEM_EEPROM_WriteVar(TANK_LV_Sonic_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_Sonic_L, 0);
  MEM_EEPROM_WriteVar(SONIC_on, 0);

  MEM_EEPROM_WriteVar(CAL_Redo_on, 0);
  MEM_EEPROM_WriteVar(T_IP_off_h, 0);

  MEM_EEPROM_WriteVar(TANK_LV_MinPressure_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_MinPressure_L, 120);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToAir_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToAir_L, 10);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToSetDown_H, 0);
  MEM_EEPROM_WriteVar(TANK_LV_LevelToSetDown_L, 15);
}


/*-------------------------------------------------------------------*
 *  reads variable from EEPROM page size: 32bytes
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_ReadVar(t_eeprom_var var)
{
  switch(var)
  {
    case ON_circ: return (MEM_EEPROM_Read(1, 0));
    case OFF_circ: return (MEM_EEPROM_Read(1, 1));
    case ON_air: return (MEM_EEPROM_Read(1, 2));
    case OFF_air: return (MEM_EEPROM_Read(1, 3));
    case TIME_setDown: return (MEM_EEPROM_Read(1, 4));
    case ON_pumpOff: return (MEM_EEPROM_Read(1, 5));
    case PUMP_pumpOff: return (MEM_EEPROM_Read(1, 6));
    case ON_MIN_mud: return (MEM_EEPROM_Read(1, 7));
    case ON_SEC_mud: return (MEM_EEPROM_Read(1, 8));
    case ON_phosphor: return (MEM_EEPROM_Read(1, 9));
    case OFF_phosphor: return (MEM_EEPROM_Read(1, 10));
    case ON_inflowPump: return (MEM_EEPROM_Read(1, 11));
    case OFF_inflowPump: return (MEM_EEPROM_Read(1, 12));
    case PUMP_inflowPump: return (MEM_EEPROM_Read(1, 13));
    case SENSOR_inTank: return (MEM_EEPROM_Read(1, 14));
    case SENSOR_outTank: return (MEM_EEPROM_Read(1, 15));
    case TIME_H_circ: return (MEM_EEPROM_Read(1, 16));
    case TIME_L_circ: return (MEM_EEPROM_Read(1, 17));
    case TIME_H_air: return (MEM_EEPROM_Read(1, 18));
    case TIME_L_air: return (MEM_EEPROM_Read(1, 19));
    case MIN_H_pressure: return (MEM_EEPROM_Read(1, 20));
    case MIN_L_pressure: return (MEM_EEPROM_Read(1, 21));
    case MAX_H_pressure: return (MEM_EEPROM_Read(1, 22));
    case MAX_L_pressure: return (MEM_EEPROM_Read(1, 23));
    case ALARM_temp: return (MEM_EEPROM_Read(1, 24));
    case ALARM_comp: return (MEM_EEPROM_Read(1, 25));
    case ALARM_sensor: return (MEM_EEPROM_Read(1, 26));
    case CAL_ZeroOffsetPressure_H: return (MEM_EEPROM_Read(1, 27));
    case CAL_ZeroOffsetPressure_L: return (MEM_EEPROM_Read(1, 28));
    case TANK_LV_Sonic_H: return (MEM_EEPROM_Read(1, 29));
    case TANK_LV_Sonic_L: return (MEM_EEPROM_Read(1, 30));
    case SONIC_on: return (MEM_EEPROM_Read(1, 31));

    case TOUCH_X_max: return (MEM_EEPROM_Read(0, 0));
    case TOUCH_Y_max: return (MEM_EEPROM_Read(0, 1));
    case TOUCH_X_min: return (MEM_EEPROM_Read(0, 2));
    case TOUCH_Y_min: return (MEM_EEPROM_Read(0, 3));
    case TANK_LV_MinPressure_H: return (MEM_EEPROM_Read(0, 4));
    case TANK_LV_MinPressure_L: return (MEM_EEPROM_Read(0, 5));
    case TANK_LV_LevelToAir_H: return (MEM_EEPROM_Read(0, 6));
    case TANK_LV_LevelToAir_L: return (MEM_EEPROM_Read(0, 7));
    case TANK_LV_LevelToSetDown_H: return (MEM_EEPROM_Read(0, 8));
    case TANK_LV_LevelToSetDown_L: return (MEM_EEPROM_Read(0, 9));
    case CAL_Redo_on: return (MEM_EEPROM_Read(0, 10));
    case T_IP_off_h: return (MEM_EEPROM_Read(0, 11));

    default: break;
  }
  return 0;
}


/*-------------------------------------------------------------------*
 *            writes variable to EEPROM, configure memory space
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVar(t_eeprom_var var, unsigned char eeData)
{
  switch(var)
  {
    case ON_circ:       MEM_EEPROM_WriteByte(1, 0, eeData); break;
    case OFF_circ:      MEM_EEPROM_WriteByte(1, 1, eeData); break;
    case ON_air:        MEM_EEPROM_WriteByte(1, 2, eeData); break;
    case OFF_air:       MEM_EEPROM_WriteByte(1, 3, eeData); break;
    case TIME_setDown:  MEM_EEPROM_WriteByte(1, 4, eeData); break;
    case ON_pumpOff:    MEM_EEPROM_WriteByte(1, 5, eeData); break;
    case PUMP_pumpOff:  MEM_EEPROM_WriteByte(1, 6, eeData); break;
    case ON_MIN_mud:    MEM_EEPROM_WriteByte(1, 7, eeData); break;
    case ON_SEC_mud:    MEM_EEPROM_WriteByte(1, 8, eeData); break;
    case ON_phosphor:   MEM_EEPROM_WriteByte(1, 9, eeData); break;
    case OFF_phosphor:    MEM_EEPROM_WriteByte(1, 10, eeData); break;
    case ON_inflowPump:   MEM_EEPROM_WriteByte(1, 11, eeData); break;
    case OFF_inflowPump:  MEM_EEPROM_WriteByte(1, 12, eeData); break;
    case PUMP_inflowPump: MEM_EEPROM_WriteByte(1, 13, eeData); break;
    case SENSOR_inTank:   MEM_EEPROM_WriteByte(1, 14, eeData); break;
    case SENSOR_outTank:  MEM_EEPROM_WriteByte(1, 15, eeData); break;
    case TIME_H_circ:   MEM_EEPROM_WriteByte(1, 16, eeData); break;
    case TIME_L_circ:   MEM_EEPROM_WriteByte(1, 17, eeData); break;
    case TIME_H_air:    MEM_EEPROM_WriteByte(1, 18, eeData); break;
    case TIME_L_air:    MEM_EEPROM_WriteByte(1, 19, eeData); break;
    case MIN_H_pressure:   MEM_EEPROM_WriteByte(1, 20, eeData); break;
    case MIN_L_pressure:   MEM_EEPROM_WriteByte(1, 21, eeData); break;
    case MAX_H_pressure:   MEM_EEPROM_WriteByte(1, 22, eeData); break;
    case MAX_L_pressure:   MEM_EEPROM_WriteByte(1, 23, eeData); break;
    case ALARM_temp:    MEM_EEPROM_WriteByte(1, 24, eeData); break;
    case ALARM_comp:    MEM_EEPROM_WriteByte(1, 25, eeData); break;
    case ALARM_sensor:  MEM_EEPROM_WriteByte(1, 26, eeData); break;
    case CAL_ZeroOffsetPressure_H:   MEM_EEPROM_WriteByte(1, 27, eeData); break;
    case CAL_ZeroOffsetPressure_L:   MEM_EEPROM_WriteByte(1, 28, eeData); break;
    case TANK_LV_Sonic_H:    MEM_EEPROM_WriteByte(1, 29, eeData); break;
    case TANK_LV_Sonic_L:    MEM_EEPROM_WriteByte(1, 30, eeData); break;
    case SONIC_on:      MEM_EEPROM_WriteByte(1, 31, eeData); break;

    case TOUCH_X_max:   MEM_EEPROM_WriteByte(0, 0, eeData); break;
    case TOUCH_Y_max:   MEM_EEPROM_WriteByte(0, 1, eeData); break;
    case TOUCH_X_min:   MEM_EEPROM_WriteByte(0, 2, eeData); break;
    case TOUCH_Y_min:   MEM_EEPROM_WriteByte(0, 3, eeData); break;
    case TANK_LV_MinPressure_H:   MEM_EEPROM_WriteByte(0, 4, eeData); break;
    case TANK_LV_MinPressure_L:   MEM_EEPROM_WriteByte(0, 5, eeData); break;
    case TANK_LV_LevelToAir_H:   MEM_EEPROM_WriteByte(0, 6, eeData); break;
    case TANK_LV_LevelToAir_L:   MEM_EEPROM_WriteByte(0, 7, eeData); break;
    case TANK_LV_LevelToSetDown_H:     MEM_EEPROM_WriteByte(0, 8, eeData); break;
    case TANK_LV_LevelToSetDown_L:     MEM_EEPROM_WriteByte(0, 9, eeData); break;
    case CAL_Redo_on:   MEM_EEPROM_WriteByte(0, 10, eeData); break;
    case T_IP_off_h:    MEM_EEPROM_WriteByte(0, 11, eeData); break;
    default:                                                break;
  }
}


/*-------------------------------------------------------------------*
 *            reads variable from EEPROM page size: 32bytes
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_ReadData(unsigned char page, unsigned char entry, t_data var)
{
  if(entry > 4){ entry = 4; }
  switch(var)
  {
    case DATA_day: return (MEM_EEPROM_Read(page, (0 + (entry * 8))));
    case DATA_month: return (MEM_EEPROM_Read(page, (1 + (entry * 8))));
    case DATA_year: return (MEM_EEPROM_Read(page, (2 + (entry * 8))));
    case DATA_hour: return (MEM_EEPROM_Read(page, (3 + (entry * 8))));
    case DATA_minute: return (MEM_EEPROM_Read(page, (4 + (entry * 8))));
    case DATA_H_O2: return (MEM_EEPROM_Read(page, (5 + (entry * 8))));
    case DATA_L_O2: return (MEM_EEPROM_Read(page, (6 + (entry * 8))));
    case DATA_ERROR: return (MEM_EEPROM_Read(page, (7 + (entry * 8))));
    default: break;
  }
  return 0;
}


/*-------------------------------------------------------------------*
 *            saves variables or write auto entry to EEPROM
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteAutoEntry(struct PlantState *ps)
{
  unsigned char data[8] = {0x00};
  struct MemoryEntryPos no_entry_pos = MEM_FindNoEntry(TEXT_BUTTON_auto);
  struct MemoryEntryPos old = MEM_FindOldestEntry(TEXT_BUTTON_auto);

  // time
  data[0] = ps->time_state->tic_dat;
  data[1] = ps->time_state->tic_mon;
  data[2] = ps->time_state->tic_yea;
  data[3] = ps->time_state->tic_hou;
  data[4] = ps->time_state->tic_min;

  // o2 low / high
  data[6] = (ps->compressor_state->cycle_o2_min & 0x00FF);
  data[5] = ((ps->compressor_state->cycle_o2_min >> 8) & 0x00FF);

  // error code
  data[7] = ps->error_state->cycle_error_code_record;
  ps->error_state->cycle_error_code_record = 0;

  // update pointer
  unsigned char page = (no_entry_pos.null_flag ? no_entry_pos.page : old.page);
  unsigned char entry = (no_entry_pos.null_flag ? no_entry_pos.entry : old.entry);

  // write protection
  if(page < MEM_AUTO_START_SECTION){ page = MEM_AUTO_START_SECTION; }

  // write entry
  for(unsigned char i = 0; i < 8; i++){ MEM_EEPROM_LoadData(entry, i, data[i]); }
  MEM_EEPROM_PageEraseWrite(page);
}


/* ------------------------------------------------------------------*
 *            manual entry
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteManualEntry(struct PlantState *ps)
{
  unsigned char data[7] = {0x00};
  struct MemoryEntryPos no_entry_pos = MEM_FindNoEntry(TEXT_BUTTON_manual);
  struct MemoryEntryPos old = MEM_FindOldestEntry(TEXT_BUTTON_manual);

  // time and o2
  data[0] = ps->time_state->tic_dat;
  data[1] = ps->time_state->tic_mon;
  data[2] = ps->time_state->tic_yea;
  data[3] = ps->eeprom_state->time_manual_entry.hou;
  data[4] = ps->eeprom_state->time_manual_entry.min;
  data[5] = ps->time_state->tic_hou;
  data[6] = ps->time_state->tic_min;

  // update pointer
  unsigned char page = (no_entry_pos.null_flag ? no_entry_pos.page : old.page);
  unsigned char entry = (no_entry_pos.null_flag ? no_entry_pos.entry : old.entry);

  // write protection
  if(page < MEM_MANUAL_START_SECTION){ page = MEM_MANUAL_START_SECTION; }

  // write entry
  for(unsigned char i = 0; i < 7; i++){ MEM_EEPROM_LoadData(entry, i, data[i]); }
  MEM_EEPROM_PageEraseWrite(page);
}


/* ------------------------------------------------------------------*
 *            setup entry
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteSetupEntry(struct PlantState *ps)
{
  unsigned char data[7] = {0x00};
  struct MemoryEntryPos no_entry_pos = MEM_FindNoEntry(TEXT_BUTTON_setup);
  struct MemoryEntryPos old = MEM_FindOldestEntry(TEXT_BUTTON_setup);

  // time
  data[0] = ps->time_state->tic_dat;
  data[1] = ps->time_state->tic_mon;
  data[2] = ps->time_state->tic_yea;
  data[3] = ps->time_state->tic_hou;
  data[4] = ps->time_state->tic_min;

  // update pointer
  unsigned char page = (no_entry_pos.null_flag ? no_entry_pos.page : old.page);
  unsigned char entry = (no_entry_pos.null_flag ? no_entry_pos.entry : old.entry);

  // write Protection
  if(page < MEM_SETUP_START_SECTION){ page = MEM_SETUP_START_SECTION; }

  // write entry
  for(unsigned char i = 0; i < 6; i++){ MEM_EEPROM_LoadData(entry, i, data[i]); }
  MEM_EEPROM_PageEraseWrite(page);
}


/*-------------------------------------------------------------------*
 *            writes all EEPROM data 0x00
 * ------------------------------------------------------------------*/

void MEM_EEPROM_SetZero(void)
{
  unsigned char data[8] = { 0x00 };

  // pages
  for(unsigned char eep = MEM_DATA_START_SECTION; eep <= MEM_DATA_END_SECTION; eep++)
  {
    // entries
    for(unsigned char e = 0; e < 4; e++)
    {
      // bytes
      for(unsigned char i = 0; i < 8; i++){ MEM_EEPROM_LoadData(e, i, data[i]); }
    }
    MEM_EEPROM_PageEraseWrite(eep);
  }
}


/*-------------------------------------------------------------------*
 *  MEM_EEPROM_LoadData
 * --------------------------------------------------------------
 *  Load eeData to EEPROM buffer, entry select one of 4 DataSections
 *  8Byte data byte == 1 display output page
 * ------------------------------------------------------------------*/

void MEM_EEPROM_LoadData(unsigned char entry, t_data byte, unsigned char eeData)
{
  switch(entry)
  {
    case 0:
      switch(byte)
      {
        case DATA_day:    MEM_EEPROM_LoadPageBuffer(0, eeData); break;
        case DATA_month:  MEM_EEPROM_LoadPageBuffer(1, eeData); break;
        case DATA_year:   MEM_EEPROM_LoadPageBuffer(2, eeData); break;
        case DATA_hour:   MEM_EEPROM_LoadPageBuffer(3, eeData); break;
        case DATA_minute: MEM_EEPROM_LoadPageBuffer(4, eeData); break;
        case DATA_H_O2:   MEM_EEPROM_LoadPageBuffer(5, eeData); break;
        case DATA_L_O2:   MEM_EEPROM_LoadPageBuffer(6, eeData); break;
        case DATA_ERROR:  MEM_EEPROM_LoadPageBuffer(7, eeData); break;
        default: break;
      } break;

    case 1:
      switch(byte)
      {
        case DATA_day:    MEM_EEPROM_LoadPageBuffer(8, eeData); break;
        case DATA_month:  MEM_EEPROM_LoadPageBuffer(9, eeData); break;
        case DATA_year:   MEM_EEPROM_LoadPageBuffer(10, eeData); break;
        case DATA_hour:   MEM_EEPROM_LoadPageBuffer(11, eeData); break;
        case DATA_minute: MEM_EEPROM_LoadPageBuffer(12, eeData); break;
        case DATA_H_O2:   MEM_EEPROM_LoadPageBuffer(13, eeData); break;
        case DATA_L_O2:   MEM_EEPROM_LoadPageBuffer(14, eeData); break;
        case DATA_ERROR:  MEM_EEPROM_LoadPageBuffer(15, eeData); break;
        default: break;
      } break;

    case 2:
      switch(byte)
      {
        case DATA_day:    MEM_EEPROM_LoadPageBuffer(16, eeData); break;
        case DATA_month:  MEM_EEPROM_LoadPageBuffer(17, eeData); break;
        case DATA_year:   MEM_EEPROM_LoadPageBuffer(18, eeData); break;
        case DATA_hour:   MEM_EEPROM_LoadPageBuffer(19, eeData); break;
        case DATA_minute: MEM_EEPROM_LoadPageBuffer(20, eeData); break;
        case DATA_H_O2:   MEM_EEPROM_LoadPageBuffer(21, eeData); break;
        case DATA_L_O2:   MEM_EEPROM_LoadPageBuffer(22, eeData); break;
        case DATA_ERROR:  MEM_EEPROM_LoadPageBuffer(23, eeData); break;
        default: break;
      } break;

    case 3:
      switch(byte)
      {
        case DATA_day:    MEM_EEPROM_LoadPageBuffer(24, eeData); break;
        case DATA_month:  MEM_EEPROM_LoadPageBuffer(25, eeData); break;
        case DATA_year:   MEM_EEPROM_LoadPageBuffer(26, eeData); break;
        case DATA_hour:   MEM_EEPROM_LoadPageBuffer(27, eeData); break;
        case DATA_minute: MEM_EEPROM_LoadPageBuffer(28, eeData); break;
        case DATA_H_O2:   MEM_EEPROM_LoadPageBuffer(29, eeData); break;
        case DATA_L_O2:   MEM_EEPROM_LoadPageBuffer(30, eeData); break;
        case DATA_ERROR:  MEM_EEPROM_LoadPageBuffer(31, eeData); break;
        default: break;
      } break;
  }
}


/* ------------------------------------------------------------------*
 *            finds out the the placement of a no entry, return position
 * ------------------------------------------------------------------*/

struct MemoryEntryPos MEM_FindNoEntry(t_text_buttons data)
{
  struct MemoryEntryPos no_entry_pos = { .page = MEM_AUTO_START_SECTION, .entry = 0, .null_flag = false };
  unsigned char stop = 0;

  // get start end page
  struct MemoryStartEndPage msep = MEM_GetStartEndPage(data);

  // start page
  no_entry_pos.page = msep.start_page;

  // pages
  for(unsigned char eep = msep.start_page; eep <= msep.end_page; eep++)
  {
    // update page
    no_entry_pos.page = eep;

    // entries
    for(unsigned char i = 0; i < 4; i++)
    {
      no_entry_pos.entry = i;
      if(!(MEM_EEPROM_ReadData(eep, i, DATA_day)))
      {
        stop = 1;
        break;
      }
    }

    // loop termination if null found
    if(stop)
    {
      // null indicator
      no_entry_pos.null_flag = true;
      break;
    }
  }

  return no_entry_pos;
}


/* ---------------------------------------------------------------*
 *          old entry
 * ---------------------------------------------------------------*/

struct MemoryEntryPos MEM_FindOldestEntry(t_text_buttons data)
{
  struct MemoryEntryPos old = { .page = MEM_AUTO_START_SECTION, .entry = 0, .null_flag = false };

  unsigned char day = 31;
  unsigned char month = 12;
  unsigned char year = 60;
  unsigned char h = 23;
  unsigned char min = 59;

  // get start end page
  struct MemoryStartEndPage msep = MEM_GetStartEndPage(data);

  // start page
  old.page = msep.start_page;

  // pages
  for(unsigned char eep = msep.start_page; eep <= msep.end_page; eep++)
  {
    // entries
    for(unsigned char i = 0; i < 4; i++)
    {
      unsigned char rDay = MEM_EEPROM_ReadData(eep, i, DATA_day);
      unsigned char rMonth = MEM_EEPROM_ReadData(eep, i, DATA_month);
      unsigned char rYear = MEM_EEPROM_ReadData(eep, i, DATA_year);
      unsigned char rH = MEM_EEPROM_ReadData(eep, i, DATA_hour);
      unsigned char rMin = MEM_EEPROM_ReadData(eep, i, DATA_minute);

      if((rDay) && ((rYear  < year) || (rMonth < month && rYear <= year) || (rDay < day && rMonth <= month && rYear <= year) || (rH   < h && rDay <= day && rMonth <= month && rYear <= year) || (rMin <= min  && rH <= h   && rDay <= day && rMonth <= month && rYear <= year)))
      {
        year = rYear;
        month = rMonth;
        day = rDay;
        h = rH;
        min = rMin;
        old.page = eep;
        old.entry = i;    
      }
    }
  }

  return old;
}


/* ---------------------------------------------------------------*
 *          latest entry
 * ---------------------------------------------------------------*/

struct MemoryEntryPos MEM_FindLatestEntry(t_text_buttons data)
{
  struct MemoryEntryPos latest = { .page = MEM_AUTO_START_SECTION, .entry = 0, .null_flag = false };

  unsigned char day = 0;
  unsigned char month = 0;
  unsigned char year = 0;
  unsigned char h = 0;
  unsigned char min = 0;

  // get start end page
  struct MemoryStartEndPage msep = MEM_GetStartEndPage(data);

  // start page
  latest.page = msep.start_page;

  // pages
  for(unsigned char eep = msep.start_page; eep <= msep.end_page; eep++)
  {
    // entries
    for(unsigned char i = 0; i < 4; i++)
    {
      unsigned char rDay = MEM_EEPROM_ReadData(eep, i, DATA_day);
      unsigned char rMonth = MEM_EEPROM_ReadData(eep, i, DATA_month);
      unsigned char rYear = MEM_EEPROM_ReadData(eep, i, DATA_year);
      unsigned char rH = MEM_EEPROM_ReadData(eep, i, DATA_hour);
      unsigned char rMin = MEM_EEPROM_ReadData(eep, i, DATA_minute);

      if((rDay) && ((rYear  > year) || (rMonth > month && rYear >= year) || (rDay > day && rMonth >= month && rYear >= year) || (rH   > h && rDay >= day && rMonth >= month && rYear >= year) || (rMin >= min  && rH >= h && rDay >= day && rMonth >= month && rYear >= year)))
      {
        year = rYear;
        month = rMonth;
        day = rDay;
        h = rH;
        min = rMin;
        latest.page = eep;
        latest.entry = i;
      }
    }
  }
  return latest;
}


/* ---------------------------------------------------------------*
 *          start end page
 * ---------------------------------------------------------------*/

struct MemoryStartEndPage MEM_GetStartEndPage(t_text_buttons data)
{
  struct MemoryStartEndPage msep = { .start_page = 0, .end_page = 0 };

  // determine start and end page
  switch(data)
  {
    case TEXT_BUTTON_auto: msep.start_page = MEM_AUTO_START_SECTION; msep.end_page = MEM_AUTO_END_SECTION; break;
    case TEXT_BUTTON_manual: msep.start_page = MEM_MANUAL_START_SECTION; msep.end_page = MEM_MANUAL_END_SECTION; break;
    case TEXT_BUTTON_setup: msep.start_page = MEM_SETUP_START_SECTION; msep.end_page = MEM_SETUP_END_SECTION; break;
    default: break;
  }
  return msep;
}