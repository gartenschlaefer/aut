// --
// structs

// include guard
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>


/* ------------------------------------------------------------------*
 *            structs
 * ------------------------------------------------------------------*/

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


struct TelNr
{
  unsigned char id;
  char tel;
  unsigned char pos;
};


struct ErrorState
{
  t_page page;
  unsigned char err_code;
  unsigned char pending_err_code;
  unsigned char err_reset_flag;
};


struct LcdBacklight
{
  t_FuncCmd state;
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


struct PageState
{
  t_page page;
  struct Tms *page_time;
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
  unsigned char rxb0_buffer[10];
  bool rxb0_data_av;
};


struct SonicState
{
  t_sonic_app app_type;
  unsigned char software_version;
  bool no_us_flag;
  int level_cal;
  int d_mm;
  int d_mm_prev;
  int temp;
  unsigned char d_error;
  unsigned char read_tank_state;
  t_sonic_query_states query_state;
  unsigned char query_error_count;
};


struct PlantState
{
  unsigned char init;
  struct PageState *page_state;
  struct PageState *auto_save_page_state;
  struct LcdBacklight *lcd_backlight;
  struct FrameCounter *frame_counter;
  struct ErrorState *error_state;
  struct MPXState *mpx_state;
  struct PhosphorState *phosphor_state;
  struct InflowPumpState *inflow_pump_state;
  struct AirCircState *air_circ_state;
  struct CANState *can_state;
  struct SonicState *sonic_state;
  struct InputHandler *input_handler;
  struct Modem *modem;
}; 

#endif