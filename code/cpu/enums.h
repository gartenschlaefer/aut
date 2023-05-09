// --
// enumeration types

// include guard
#ifndef ENUMS_H
#define ENUMS_H


/* ------------------------------------------------------------------*
 *            touch states
 * ------------------------------------------------------------------*/

typedef enum
{
  _touch_clean, 
  _touch_setup_x, _touch_read_x,
  _touch_setup_y, _touch_read_y,
  _touch_ready
}t_touch_state;

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
 *            touch states
 * ------------------------------------------------------------------*/

typedef enum
{
  _error_op_ok, _error_op_close_start, _error_op_close_process,
  _error_op_stop_close_start_open, _error_op_open_process, _error_op_open_process_spring, _error_op_open_stop
}t_error_op_state;


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
  _n_pumpOff, _n_mud, _n_inflowPump, _n_pump2, _p_pumpOff, _p_mud, _p_inflowPump, _p_pump2,

  // 29 x 17 [20]
  _n_setDown, _n_alarm, _n_air, _n_sensor, _n_watch, _n_compressor, _n_circulate, _n_cal, _n_zone, _n_level,
  _p_setDown, _p_alarm, _p_air, _p_sensor, _p_watch, _p_compressor, _p_circulate, _p_cal, _p_zone, _p_level,

  // 19 x 19 [23]
  _n_phosphor, _n_pump, _n_esc, _n_plus, _n_minus, _n_arrow_up, _n_arrow_down, _n_ok, _n_grad, _n_sonic, _n_arrow_redo,
  _p_phosphor, _p_pump, _p_esc, _p_plus, _p_minus, _p_arrow_up, _p_arrow_down, _p_ok, _p_line, _p_grad, _p_sonic, _p_arrow_redo,

  // 34 x 21 [6]
  _frame, _p_escape, _p_del, _black, _n_escape, _n_del,

  // 39 x 16 [2]
  _n_text_frame, _p_text_frame,

  // hecs [1]
  _logo_hecs,

  // purator [1]
  _logo_purator

}t_any_symbol;


/* ------------------------------------------------------------------*
 *            text in buttons enum
 * ------------------------------------------------------------------*/

typedef enum
{
  TEXT_BUTTON_auto = 0,
  TEXT_BUTTON_manual = 1,
  TEXT_BUTTON_setup = 2,
  TEXT_BUTTON_data = 3,
  TEXT_BUTTON_sonic = 4,
  TEXT_BUTTON_shot = 5,
  TEXT_BUTTON_open_ventil = 6,
  TEXT_BUTTON_boot = 7,
  TEXT_BUTTON_read = 8,
  TEXT_BUTTON_write = 9
}t_text_buttons;


/* ------------------------------------------------------------------*
 *            setup page symbols
 * ------------------------------------------------------------------*/

typedef enum
{
  _setup_neg_sym_circulate, _setup_neg_sym_air, _setup_neg_sym_setDown, _setup_neg_sym_pumpOff,
  _setup_neg_sym_mud, _setup_neg_sym_compressor, _setup_neg_sym_phosphor, _setup_neg_sym_inflowPump,
  _setup_neg_sym_cal, _setup_neg_sym_alarm, _setup_neg_sym_watch, _setup_neg_sym_zone,
  _setup_pos_sym_circulate, _setup_pos_sym_air, _setup_pos_sym_setDown, _setup_pos_sym_pumpOff,
  _setup_pos_sym_mud, _setup_pos_sym_compressor, _setup_pos_sym_phosphor, _setup_pos_sym_inflowPump,
  _setup_pos_sym_cal, _setup_pos_sym_alarm, _setup_pos_sym_watch, _setup_pos_sym_zone,
}t_setup_sym;


/* ------------------------------------------------------------------*
 *            control buttons
 * ------------------------------------------------------------------*/

typedef enum
{
  _setup_neg_sym_plus, _setup_neg_sym_minus, _setup_neg_sym_esc, _setup_neg_sym_ok,
  _setup_pos_sym_plus, _setup_pos_sym_minus, _setup_pos_sym_esc, _setup_pos_sym_ok
}t_CtrlButtons;


/* ------------------------------------------------------------------*
 *            Date Time Page
 * ------------------------------------------------------------------*/

typedef enum
{
  _n_h, _n_min, _n_day, _n_month, _n_year,
  _p_h, _p_min, _p_day, _p_month, _p_year
}t_DateTime;


/* ------------------------------------------------------------------*
 *            EEPROM enum for telephone number
 * ------------------------------------------------------------------*/

typedef enum
{
  TEL1_0 = 0,
  TEL1_1 = 1,
  TEL1_2 = 2,
  TEL1_3 = 3,
  TEL1_4 = 4,
  TEL1_5 = 5,
  TEL1_6 = 6,
  TEL1_7 = 7,
  TEL1_8 = 8,
  TEL1_9 = 9,
  TEL1_A = 10,
  TEL1_B = 11,
  TEL1_C = 12,
  TEL1_D = 13,
  TEL1_E = 14,
  TEL1_F = 15,
  TEL2_0 = 16,
  TEL2_1 = 17,
  TEL2_2 = 18,
  TEL2_3 = 19,
  TEL2_4 = 20,
  TEL2_5 = 21,
  TEL2_6 = 22,
  TEL2_7 = 23,
  TEL2_8 = 24,
  TEL2_9 = 25,
  TEL2_A = 26,
  TEL2_B = 27,
  TEL2_C = 28,
  TEL2_D = 29,
  TEL2_E = 30,
  TEL2_F = 31
}t_at24c_eeprom_var;


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
  ON_circ = 0, 
  OFF_circ = 1, 
  TIME_H_circ = 2, 
  TIME_L_circ = 3,
  ON_air = 4, 
  OFF_air = 5, 
  TIME_H_air = 6, 
  TIME_L_air = 7,
  TIME_setDown = 8,
  ON_pumpOff = 9, 
  PUMP_pumpOff = 10,
  ON_MIN_mud = 11, 
  ON_SEC_mud = 12,
  MIN_H_druck = 13, 
  MIN_L_druck = 14, 
  MAX_H_druck = 15, 
  MAX_L_druck = 16,
  ON_phosphor = 17, 
  OFF_phosphor = 18, 
  ON_inflowPump = 19, 
  OFF_inflowPump = 20,
  PUMP_inflowPump = 21, 
  T_IP_off_h = 22, 
  SENSOR_inTank = 23, 
  SENSOR_outTank = 24,
  ALARM_temp = 25, 
  ALARM_comp = 26,  
  ALARM_sensor = 27,
  CAL_H_druck = 28, 
  CAL_L_druck = 29, 
  CAL_Redo_on = 30,
  SONIC_H_LV = 31, 
  SONIC_L_LV = 32, 
  SONIC_on = 33,
  TANK_H_Circ = 34, 
  TANK_L_Circ = 35,
  TANK_H_O2 = 36, 
  TANK_L_O2 = 37,
  TANK_H_MinP = 38, 
  TANK_L_MinP = 39,
  TOUCH_X_max = 40, 
  TOUCH_Y_max = 41,
  TOUCH_X_min = 42, 
  TOUCH_Y_min = 43
}t_eeprom_var;


/* ------------------------------------------------------------------*
 *            data request
 * ------------------------------------------------------------------*/

typedef enum
{ 
  DATA_day, DATA_month, DATA_year, DATA_hour, DATA_minute, DATA_H_O2, DATA_L_O2, DATA_ERROR
}t_data;


/* ------------------------------------------------------------------*
 *            port valve enum
 * ------------------------------------------------------------------*/

typedef enum
{ 
  OPEN_Reserve, CLOSE_Reserve, OPEN_MudPump, CLOSE_MudPump,
  OPEN_Air, CLOSE_Air, OPEN_ClearWater, CLOSE_ClearWater, CLOSE_IPAir
}t_valve;

#endif