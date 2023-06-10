// --
// structs

// include guard
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

/* ------------------------------------------------------------------*
 *            general structs
 * ------------------------------------------------------------------*/

struct ValueLimit
{
  int max_value;
  int min_value;
};

struct QueueNode
{
  struct QueueNode *next;
  void *data;
};


struct Queue 
{
  struct QueueNode *front;
  struct QueueNode *back;
};


struct MemoryEntryPos
{
  unsigned char page;
  unsigned char entry;
  bool null_flag;
};

struct MemoryStartEndPage
{
  unsigned char start_page;
  unsigned char end_page;
};


struct TeleNr
{
  unsigned char id;
  unsigned char nr[16];
};


struct Thm
{
  int hou;
  int min;
};


struct Thms
{
  int hou;
  int min;
  int sec;
};


struct Tms
{
  int min;
  int sec;
};


struct RowColPos
{
 unsigned char row;
 unsigned char col;
};


/* ------------------------------------------------------------------*
 *            plant state structs
 * ------------------------------------------------------------------*/

struct PageState
{
  t_page page;
  struct Tms *page_time;
};


struct InputHandler 
{
  unsigned char float_sw_alarm;
};


struct Modem 
{
  unsigned char turned_on;
  unsigned char turn_on_state;
  unsigned char turn_on_error;
  int startup_delay;
  struct TeleNr *tele_nr1;
  struct TeleNr *tele_nr2;
  struct TeleNr *tele_nr_temp;
  unsigned char temp_digit_pos;
};


struct PortState
{
  bool buzzer_on;
  unsigned char valve_state;
  t_valve_action valve_action;
  bool valve_action_flag;
  t_valve_handling valve_handling;
  struct Queue *queue_valve_action;
  bool valve_init;
};


struct CompressorState
{
  int operation_hours;
  int cycle_o2_min;
  int old_min;
  unsigned char operation_sixty_min_count;
};


struct ErrorState
{
  t_page page;
  unsigned char error_code;
  unsigned char pending_err_code;
  bool error_reset_flag;
  t_error_op_state op_state;
  unsigned char error_counter[5];
  unsigned char cycle_error_code_record;
  unsigned char error_on_counter;
};


struct Backlight
{
  t_backlight_states state;
  unsigned int count;
};


struct FrameCounter
{
  unsigned int usv;
  unsigned int lcd_reset;
  unsigned int frame;
  unsigned int sixty_sec_counter;
  float fps;
  int delta_t;
};


struct MPXState
{
  unsigned char mpx_count;
  int mpx_values[10];
  unsigned char error_counter;
  int level_cal;
};


struct PhosphorState
{
  struct Tms *ph_tms;
  t_phosphor_states ph_state;
  bool init_flag;
};


struct InflowPumpState
{
  struct Thms *ip_thms;
  t_inflow_pump_states ip_state;
  unsigned char ip_active_pump_id;
  bool init_flag;
};


struct AirCircState
{
  struct Tms *air_tms;
};


struct CANState
{
  bool mcp2525_ok_flag;
  unsigned char rxb0_buffer[10];
  bool rxb0_data_av;
};


struct TWIState
{
  unsigned char twid_rx_buffer[10];
};


struct USARTState
{
  unsigned char rx_buffer[50];
  unsigned char pos;
};


struct SonicState
{
  t_sonic_app app_type;
  unsigned char software_version;
  unsigned char no_us_error_counter;
  bool no_us_flag;
  int level_cal;
  int d_mm;
  int d_mm_prev;
  int d_mm_max;
  int d_mm_min;
  int temp;
  int temp_max;
  int temp_min;
  unsigned char d_error;
  unsigned char read_tank_state;
  t_sonic_query_states query_state;
  unsigned char query_error_count;
  unsigned char record_position;
  unsigned char record_error_update;
};


struct TimeState
{
  unsigned char tic_dat;
  unsigned char tic_mon;
  unsigned char tic_yea;
  unsigned char tic_hou;
  unsigned char tic_min;
  unsigned char tic_sec;
  unsigned char old_sec;
  bool tic_sec_update_flag;
};


struct EEPROMState
{
  struct Thm time_manual_entry;
};


struct TouchState
{
  t_touch_state state;
  unsigned char x;
  unsigned char y;
  unsigned char chunk;
  int x_data[2];
  int y_data[2];
  bool init;
  unsigned char touched;
  unsigned char select;
  unsigned char var[5];
  int pin_touch;
  int *p_value_setting;
  struct ValueLimit *p_value_limit;
};


struct Settings
{
  struct SettingsCirculate *settings_circulate;
  struct SettingsAir *settings_air;
  struct SettingsSetDown *settings_set_down;
  struct SettingsPumpOff *settings_pump_off;
  struct SettingsMud *settings_mud;
  struct SettingsCompressor *settings_compressor;
  struct SettingsPhosphor *settings_phosphor;
  struct SettingsInflowPump *settings_inflow_pump;
  struct SettingsCalibration *settings_calibration;
  struct SettingsAlarm *settings_alarm;
  struct SettingsZone *settings_zone;
};


struct SettingsCirculate
{
  int on_min;
  int off_min;
  int time_min;
  unsigned char sensor_in_tank;
  struct ValueLimit val_lim_on_min;
  struct ValueLimit val_lim_off_min;
  struct ValueLimit val_lim_time_min;
};


struct SettingsAir
{
  int on_min;
  int off_min;
  int time_min;
  struct ValueLimit val_lim_on_min;
  struct ValueLimit val_lim_off_min;
  struct ValueLimit val_lim_time_min;
};


struct SettingsSetDown
{
  int time_min;
  struct ValueLimit val_lim_time_min;
};


struct SettingsPumpOff
{
  int on_min;
  unsigned char pump;
  struct ValueLimit val_lim_on_min;
};


struct SettingsMud
{
  int on_min;
  int on_sec;
  struct ValueLimit val_lim_on_min;
  struct ValueLimit val_lim_on_sec;
};


struct SettingsCompressor
{
  int min_pressure;
  int max_pressure;
  struct ValueLimit val_lim_min_pressure;
  struct ValueLimit val_lim_max_pressure;
};


struct SettingsPhosphor
{
  int on_min;
  int off_min;
  struct ValueLimit val_lim_on_min;
  struct ValueLimit val_lim_off_min;
};


struct SettingsInflowPump
{
  int on_min;
  int off_min;
  int off_hou;
  unsigned char pump;
  unsigned char sensor_out_tank;
  struct ValueLimit val_lim_on_min;
  struct ValueLimit val_lim_off_min;
  struct ValueLimit val_lim_off_hou;
};


struct SettingsCalibration
{
  unsigned char redo_on;
  int zero_offset_pressure;
  int tank_level_min_pressure;
  int tank_level_min_sonic;
};


struct SettingsAlarm
{
  unsigned char sensor;
  unsigned char compressor;
  int temp;
  struct ValueLimit val_lim_temp;
};


struct SettingsZone
{
  unsigned char sonic_on;
  int level_to_air;
  int level_to_set_down;
  struct ValueLimit val_lim_level_to_air;
  struct ValueLimit val_lim_level_to_set_down;
};


struct PlantState
{
  struct PageState *page_state;
  struct PageState *auto_save_page_state;
  struct PortState *port_state;
  struct CompressorState *compressor_state;
  struct Backlight *backlight;
  struct FrameCounter *frame_counter;
  struct ErrorState *error_state;
  struct MPXState *mpx_state;
  struct PhosphorState *phosphor_state;
  struct InflowPumpState *inflow_pump_state;
  struct AirCircState *air_circ_state;
  struct CANState *can_state;
  struct TWIState *twi_state;
  struct USARTState *usart_state;
  struct SonicState *sonic_state;
  struct InputHandler *input_handler;
  struct Modem *modem;
  struct TimeState *time_state;
  struct EEPROMState *eeprom_state;
  struct TouchState *touch_state;
  struct Settings *settings;
}; 

#endif