// --
// structs

// include guard
#ifndef STRUCTS_H
#define STRUCTS_H


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


struct ErrTreat
{
  t_page page;
  unsigned char err_code;
  unsigned char err_reset_flag;
};


struct LcdBacklight
{
  t_FuncCmd state;
  unsigned int count;
};
//lcd_backlight_default = { .state = _off, .count = 0 };


struct PlantState
{
  t_page page;
  struct LcdBacklight lcd_backlight;
}; 
//plant_state_default = { .page = DataPage, .lcd_backlight = lcd_backlight_default };

#endif