// --
// structs

// include guard
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

/* ------------------------------------------------------------------*
 *            general structs
 * ------------------------------------------------------------------*/

struct TelNr
{
  unsigned char id;
  char tel;
  unsigned char pos;
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
};


struct PortState
{
  bool buzzer_on;
  unsigned char valve_state;
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
  unsigned char err_code;
  unsigned char pending_err_code;
  unsigned char err_reset_flag;
  unsigned char op_state;
  unsigned char error_counter[5];
  unsigned char cycle_error_code_record;
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
  int ph_count;
  struct Tms *ph_tms;
  t_phosphor_states ph_state;
};


struct InflowPumpState
{
  int ip_count;
  struct Thms *ip_thms;
  t_inflow_pump_states ip_state;
  unsigned char ip_active_pump_id;
};


struct AirCircState
{
  int old_sec;
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


struct PlantState
{
  unsigned char init;
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
}; 

#endif