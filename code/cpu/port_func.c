// --
// port functions

#include "port_func.h"
#include "config.h"
#include "lcd_driver.h"
#include "memory_app.h"
#include "mcp9800_driver.h"
#include "adc_func.h"
#include "modem_driver.h"
#include "error_func.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void PORT_Init(void)
{
  // inputs
  P_OPTO.DIRCLR = PIN3_bm | OC1 | OC2 | OC3 | OC4;

  // outputs
  P_VALVE.DIR = 0xFF;
  P_RELAIS.DIR = 0xFF;

  // Pins PULL UP
  PORTCFG.MPCMASK = 0xFF;
  P_OPTO.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;

  // FirmwareUpdate PullUp
  PORTD.PIN5CTRL = PORT_OPC_WIREDANDPULL_gc;

  // buzzer output
  BUZZER_DIR;
  BACKLIGHT_DIR;
}


/* ------------------------------------------------------------------*
 *            bootloader
 * ------------------------------------------------------------------*/

void PORT_Bootloader(void)
{
  if(!(PORTD.IN & PIN5_bm))
  {
    LCD_Clean();
    LCD_WriteAnyStringFont(f_6x8_p, 1, 1, "Bootloader-Modus");
    asm volatile("jmp 0x20000");
  }
}


/* ------------------------------------------------------------------*
 *            buzzer
 * ------------------------------------------------------------------*/

void PORT_Buzzer_Update(struct PlantState *ps)
{
  // error sound
  if(ps->port_state->buzzer_on)
  {
    if(ps->frame_counter->sixty_sec_counter % 3){ BUZZER_ON; }
    else{ BUZZER_OFF; }
  }

  // off state
  else{ BUZZER_OFF; }
}


/*-------------------------------------------------------------------*
 *  LCD backlight on / off / reset
 * ------------------------------------------------------------------*/

void PORT_Backlight_On(struct Backlight *backlight)
{
  BACKLIGHT_ON;
  backlight->count = 0;
  backlight->state = _bl_on;
}

void PORT_Backlight_Off(struct Backlight *backlight)
{
  BACKLIGHT_OFF;
  backlight->count = 0;
  backlight->state = _bl_off;
}

void PORT_Backlight_Error(struct Backlight *backlight)
{
  backlight->count = 0;
  backlight->state = _bl_error;
}


/*-------------------------------------------------------------------*
 *  LCD backlight update
 * ------------------------------------------------------------------*/

void PORT_Backlight_Update(struct Backlight *b)
{
  //***LightAlwaysOn-Debug
  if(DEBUG){ return; }

  // light is on
  if(b->state == _bl_on)
  {
    b->count++;
    if(b->count > BACKLIGHT_TON_FRAMES){ PORT_Backlight_Off(b); }
  }

  // error
  else if(b->state == _bl_error)
  {
    b->count++;
    if(b->count > BACKLIGHT_ERROR_ON_FRAMES){ BACKLIGHT_OFF; }
    if(b->count > BACKLIGHT_ERROR_OFF_FRAMES){ b->count = 0; BACKLIGHT_ON; }
  }
}


/* ------------------------------------------------------------------*
 *            ventilator
 * ------------------------------------------------------------------*/

void PORT_Ventilator(struct PlantState *ps)
{
  // temperature
  unsigned char temp = MCP9800_PlusTemp(ps->twi_state);
  unsigned char alarm_temp = ps->settings->settings_alarm->temp;

  // hysteresis
  unsigned char hystOn = (alarm_temp - 15);
  unsigned char hystOff = (alarm_temp - 20);

  if(!(temp & 0x80))
  {
    if(temp > hystOn){ PORT_RelaisSet(R_VENTILATOR); }
    if(temp < hystOff){ PORT_RelaisClr(R_VENTILATOR); }
  }
  else{ PORT_RelaisClr(R_VENTILATOR); }
}


/* ------------------------------------------------------------------*
 *            relais
 * ------------------------------------------------------------------*/

void PORT_RelaisSet(unsigned char relais)
{
  P_RELAIS.OUTSET = relais;
}

void PORT_RelaisClr(unsigned char relais)
{
  P_RELAIS.OUTCLR = relais;
}


/* ------------------------------------------------------------------*
 *            run time functions
 * ------------------------------------------------------------------*/

void PORT_Auto_RunTime(struct PlantState *ps)
{ 
  // once per minute check
  if(ps->frame_counter->sixty_sec_counter == 20)
  {
    PORT_Ventilator(ps);

    // Floating switch alarm
    if(IN_FLOAT_S3 && !ps->input_handler->float_sw_alarm)
    {
      if(ps->settings->settings_alarm->sensor)
      {
        Modem_Alert(ps, "Error: floating switch");
        Error_On(ps);
      }
      ps->input_handler->float_sw_alarm = 1;
    }
    else if(!IN_FLOAT_S3 && ps->input_handler->float_sw_alarm)
    {
      if(ps->settings->settings_alarm->sensor)
      {
        Error_Off(ps);
      }
      ps->input_handler->float_sw_alarm = 0;
    }

    //*** debug USVCheckVoltageSupply
    if(!DEBUG)
    {
      ADC_USV_Check(ps);
    }
  }
}


/* ------------------------------------------------------------------*
 *            debug
 * ------------------------------------------------------------------*/

void PORT_Debug(struct PlantState *ps)
{ 
  unsigned char y_pos_r = 19;
  unsigned char y_pos_v = 21;

  if(ps->time_state->tic_sec_update_flag)
  {
    // print register names
    if(ps->frame_counter->sixty_sec_counter % 2)
    {
      LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 0, "R: I1 I2 Ph Ex Co Cl Al Ve");
      LCD_WriteAnyStringFont(f_6x8_n, y_pos_v, 0, "V: Ai Mu Cl Re");
    }
    
    // print register states
    else
    {
      // relays
      for(int relais = 0; relais < 8; relais++)
      {
        if(P_RELAIS.OUT & (1 << relais)){ LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 18 + 18 * relais, "1 "); }
        else{ LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 18 + 18 * relais, "0 "); }
      }

      // valves
      for(int valve = 0; valve < 4; valve++)
      {
        if(ps->port_state->valve_state & (1 << valve)){ LCD_WriteAnyStringFont(f_6x8_n, y_pos_v, 18 + 18 * valve, "1 "); }
        else{ LCD_WriteAnyStringFont(f_6x8_n, y_pos_v, 18 + 18 * valve, "0 "); }
      }
    }
  }
}
