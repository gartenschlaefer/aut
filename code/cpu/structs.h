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


struct PlantState
{
  unsigned char init;
  struct PageState *page_state;
  struct PageState *prev_page_state;
  struct LcdBacklight *lcd_backlight;
  struct FrameCounter *frame_counter;
  struct ErrorState *error_state;
}; 

#endif