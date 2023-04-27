// --
// enumeration types

// include guard
#ifndef ENUMS_H
#define ENUMS_H


/* ------------------------------------------------------------------*
 *            backlight states
 * ------------------------------------------------------------------*/

typedef enum
{
  _bl_off, _bl_on, _bl_error
}t_backlight_states;
/* ------------------------------------------------------------------*
 *            phosphor states
 * ------------------------------------------------------------------*/

typedef enum
{ _ph_off, _ph_on, _ph_disabled
}t_phosphor_states;


/* ------------------------------------------------------------------*
 *            inflow pump states
 * ------------------------------------------------------------------*/

typedef enum
{ _ip_off, _ip_on, _ip_disabled
}t_inflow_pump_states;


/* ------------------------------------------------------------------*
 *            function commands
 * ------------------------------------------------------------------*/

typedef enum
{ _clean, _exe, _init, _reset, _add, _write, _saveValue, _off, _on, _count, _new, _save,
  _set, _state, _sym, _error, _disabled, _clear, _read, _read1, _read2, _ready, _write1, 
  _write2, _shot, _shot1, _temp, _temp1, _ton, _ovent, _cvent, _notav, _mbar, _mmbar, 
  _right, _op, _telnr, _wrong, _debug, _entry, _dec, _check, _noData, _noUS, _noBoot, 
  _ok, _success, _mcp_fail, _autotext
}t_FuncCmd;


/* ------------------------------------------------------------------*
 *            font types
 * ------------------------------------------------------------------*/

typedef enum
{
  f_6x8_p, f_6x8_n, f_4x6_p, f_4x6_n, f_8x16_p, f_8x16_n
}t_font_type;


/* ------------------------------------------------------------------*
 *            symbols
 * ------------------------------------------------------------------*/

typedef enum
{
  s_35x23, s_29x17, s_19x19, s_34x21, s_39x16, s_logo_hecs, s_logo_purator
}t_symbol_type;


typedef enum
{
  // 35 x 23 [8]
  n_pumpOff, n_mud, n_inflowPump, n_pump2, p_pumpOff, p_mud, p_inflowPump, p_pump2,

  // 29 x 17 [20]
  n_setDown, n_alarm, n_air, n_sensor, n_watch, n_compressor, n_circulate, n_cal, n_zone, n_level,
  p_setDown, p_alarm, p_air, p_sensor, p_watch, p_compressor, p_circulate, p_cal, p_zone, p_level,

  // 19 x 19 [23]
  n_phosphor, n_pump, n_esc, n_plus, n_minus, n_arrowUp, n_arrowDown, n_ok, n_grad, n_sonic, n_arrowRedo,
  p_phosphor, p_pump, p_esc, p_plus, p_minus, p_arrowUp, p_arrowDown, p_ok, p_line, p_grad, p_sonic, p_arrowRedo,

  // 34 x 21 [6]
  frame, p_escape, p_del, black, n_escape, n_del,

  // 39 x 16 [2]
  n_text_frame, p_text_frame,

  // hecs [1]
  logo_hecs,

  // purator [1]
  logo_purator

}t_any_symbol;


/* ------------------------------------------------------------------*
 *            text in buttons enum
 * ------------------------------------------------------------------*/

typedef enum
{
  Auto, Manual, Setup, Data, Sonic, Shot, OpenV, Boot, Read, Write
}t_text_buttons;


/* ------------------------------------------------------------------*
 *            setup page symbols
 * ------------------------------------------------------------------*/

typedef enum
{
  sn_circulate, sn_air, sn_setDown, sn_pumpOff,
  sn_mud, sn_compressor, sn_phosphor, sn_inflowPump,
  sn_cal, sn_alarm, sn_watch, sn_zone,
  sp_circulate, sp_air, sp_setDown, sp_pumpOff,
  sp_mud, sp_compressor, sp_phosphor, sp_inflowPump,
  sp_cal, sp_alarm, sp_watch, sp_zone,
}t_SetupSym;


/* ------------------------------------------------------------------*
 *            control buttons
 * ------------------------------------------------------------------*/

typedef enum
{
  sn_plus, sn_minus, sn_esc, sn_ok,
  sp_plus, sp_minus, sp_esc, sp_ok
}t_CtrlButtons;


/* ------------------------------------------------------------------*
 *            Date Time Page
 * ------------------------------------------------------------------*/

typedef enum
{
  n_h, n_min, n_day, n_month, n_year,
  p_h, p_min, p_day, p_month, p_year
}t_DateTime;


/* ------------------------------------------------------------------*
 *            EEPROM enum for telephone number
 * ------------------------------------------------------------------*/

typedef enum
{
  TEL1_0, TEL1_1, TEL1_2, TEL1_3,
  TEL1_4, TEL1_5, TEL1_6, TEL1_7,
  TEL1_8, TEL1_9, TEL1_A, TEL1_B,
  TEL1_C, TEL1_D, TEL1_E, TEL1_F,
  TEL2_0, TEL2_1, TEL2_2, TEL2_3,
  TEL2_4, TEL2_5, TEL2_6, TEL2_7,
  TEL2_8, TEL2_9, TEL2_A, TEL2_B,
  TEL2_C, TEL2_D, TEL2_E, TEL2_F,
}t_EEvar;


/* ------------------------------------------------------------------*
 *            can commands
 * ------------------------------------------------------------------*/

typedef enum
{
  CAN_CMD_sonic_set_can_address = 0, 
  CAN_CMD_sonic_one_shot = 1, 
  CAN_CMD_sonic_start_5shots = 2, 
  CAN_CMD_sonic_start_temp = 3, 
  CAN_CMD_sonic_read_dist = 4, 
  CAN_CMD_sonic_read_temp = 5,
  CAN_CMD_sonic_read_USSREG = 6, 
  CAN_CMD_sonic_read_USCREG1 = 7, 
  CAN_CMD_sonic_read_USCREG2 = 8, 
  CAN_CMD_sonic_write_USSREG = 9, 
  CAN_CMD_sonic_write_USCREG1 = 10,
  CAN_CMD_sonic_write_USCREG2 = 11,
  CAN_CMD_sonic_read_sversion = 12, 
  CAN_CMD_sonic_working = 13, 
  CAN_CMD_sonic_ack = 14, 
  CAN_CMD_sonic_wait = 15, 
  CAN_CMD_sonic_boot = 16, 
  CAN_CMD_sonic_app = 17, 
  CAN_CMD_sonic_program = 18, 
  CAN_CMD_sonic_read_program = 19,
  CAN_CMD_sonic_end_mark = 20
}t_can_cmd;


/* ------------------------------------------------------------------*
 *            sonic query states
 * ------------------------------------------------------------------*/

typedef enum
{
  _usWait = 0,
  _usDistReq = 1,
  _usTempReq = 2,
  _usDistAckOK = 3,
  _usTempAckOK = 4,
  _usDistAv = 5,
  _usTempAv = 6,
  _usDistSuccess = 7,
  _usTempSuccess = 8,
  _usErrTimeout1 = 10,
  _usErrTimeout2 = 11,
  _usErrTimeout3 = 12,
  _usErrWrongReq = 13
}t_sonic_query_states;


/* ------------------------------------------------------------------*
 *            sonic application program
 * ------------------------------------------------------------------*/

typedef enum
{
  SONIC_APP_75kHz = 0,
  SONIC_APP_125kHz = 1,
  SONIC_APP_boot = 2,
  SONIC_APP_none = 128
}t_sonic_app;


/* ------------------------------------------------------------------*
 *            sonic states (fixed)
 * ------------------------------------------------------------------*/

typedef enum
{
  SONIC_TANK_listen = 0,
  SONIC_TANK_timer_init = 1
}t_sonic_tank_states;


/* ------------------------------------------------------------------*
 *            pages
 * ------------------------------------------------------------------*/

typedef enum
{ 
  AutoPage, ManualPage, SetupPage, DataPage, PinManual, PinSetup, PinModem, AutoZone, AutoSetDown, AutoMud,
  AutoCircOn, AutoCircOff, AutoPumpOff, AutoAirOn, AutoAirOff, SetupMain, SetupCirculate, SetupAir, SetupSetDown, SetupPumpOff,
  SetupMud, SetupCompressor, SetupPhosphor, SetupInflowPump, SetupCal, SetupCalPressure, SetupAlarm, SetupWatch, SetupZone, ManualMain,
  ManualPumpOff_On, ManualCircOn, ManualCircOff, ManualAir, ManualSetDown, ManualPumpOff, ManualMud, ManualCompressor, ManualPhosphor, ManualInflowPump,
  DataMain, DataAuto, DataManual, DataSetup, DataSonic, DataSonicAuto, DataSonicBoot, DataSonicBootR, DataSonicBootW, ErrorMPX,
  ErrorTreat, NoPage
}t_page;


/* ------------------------------------------------------------------*
 *            memory enums
 * ------------------------------------------------------------------*/

typedef enum
{ 
  ON_circ, OFF_circ, TIME_H_circ, TIME_L_circ,
  ON_air, OFF_air, TIME_H_air, TIME_L_air,
  TIME_setDown,
  ON_pumpOff, PUMP_pumpOff,
  ON_MIN_mud, ON_SEC_mud,
  MIN_H_druck, MIN_L_druck, MAX_H_druck, MAX_L_druck,
  ON_phosphor, OFF_phosphor, ON_inflowPump, OFF_inflowPump,
  PUMP_inflowPump, T_IP_off_h, SENSOR_inTank, SENSOR_outTank,
  ALARM_temp, ALARM_comp,  ALARM_sensor,
  CAL_H_druck, CAL_L_druck, CAL_Redo_on,
  SONIC_H_LV, SONIC_L_LV, SONIC_on,
  TANK_H_Circ, TANK_L_Circ,
  TANK_H_O2, TANK_L_O2,
  TANK_H_MinP, TANK_L_MinP,
  TOUCH_X_max, TOUCH_Y_max,
  TOUCH_X_min, TOUCH_Y_min
}t_var;

typedef enum
{ 
  DATA_day, DATA_month, DATA_year, DATA_hour, DATA_minute, DATA_H_O2, DATA_L_O2, DATA_ERROR
}t_data;

typedef enum
{ 
  Write_o2, Write_Error, Write_Entry
}t_auto_entry;


/* ------------------------------------------------------------------*
 *            port valve enum
 * ------------------------------------------------------------------*/

typedef enum
{ 
  OPEN_Reserve, CLOSE_Reserve, OPEN_MudPump, CLOSE_MudPump,
  OPEN_Air, CLOSE_Air, OPEN_ClearWater, CLOSE_ClearWater, CLOSE_IPAir
}t_valve;

#endif