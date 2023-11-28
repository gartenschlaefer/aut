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

void PORT_Init(struct PlantState *ps)
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

  // buzzer
  BUZZER_DIR;
  BUZZER_OFF;
  ps->port_state->buzzer_on = false;

  // ventilator off
  PORT_RelaisClr(R_VENTILATOR);
  ps->port_state->ventilator_on_flag = false;

  // backlight
  BACKLIGHT_DIR;
  ps->port_state->f_backlight_update = &PORT_Nope;
  PORT_Backlight_On(ps->backlight);
}


/* ------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void PORT_Update(struct PlantState *ps)
{
  PORT_Buzzer_Update(ps);
  ps->port_state->f_backlight_update(ps);
  PORT_Bootloader();
  PORT_Ventilator_Update(ps);

  // once per minute check
  if(ps->frame_counter->sixty_sec_counter == 20)
  {
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
 *            change page
 * ------------------------------------------------------------------*/

void PORT_ChangePage(struct PlantState *ps, t_page new_page)
{
  //***LightAlwaysOn-Debug
  if(DEBUG){ ps->port_state->f_backlight_update = &PORT_Nope; return; }

  // page dependent handling
  switch(new_page)
  {
    // auto pages
    case AutoSetDown: case AutoMud: case AutoPumpOff: case AutoZone: case AutoCirc: case AutoAir: 

    // manual pages
    case ManualMain: case ManualPumpOff_On: case ManualCirc: case ManualAir: case ManualSetDown: case ManualPumpOff:
    case ManualMud: case ManualCompressor: case ManualPhosphor: case ManualInflowPump: 
      ps->port_state->f_backlight_update = &PORT_Backlight_Update;
      break;

    default: ps->port_state->f_backlight_update = &PORT_Nope; break;  
  }
}

void PORT_Nope(struct PlantState *ps){ ; }


/* ------------------------------------------------------------------*
 *            bootloader
 * ------------------------------------------------------------------*/

void PORT_Bootloader(void)
{
  if(!(PORTD.IN & PIN5_bm))
  {
    LCD_Clean();
    LCD_WriteAnyStringFont(_f_6x8, 1, 1, "Bootloader-Mode", false);
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
    if(ps->frame_counter->sixty_sec_counter & 3){ BUZZER_ON; }
    else{ BUZZER_OFF; }
  }
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

void PORT_Backlight_Update(struct PlantState *ps)
{
  // light is on
  if(ps->backlight->state == _bl_on)
  {
    ps->backlight->count++;
    if(ps->backlight->count > BACKLIGHT_TON_FRAMES){ PORT_Backlight_Off(ps->backlight); }
  }

  // error
  else if(ps->backlight->state == _bl_error)
  {
    if(ps->frame_counter->sixty_sec_counter & 3){ BACKLIGHT_OFF; }
    else{ BACKLIGHT_ON; }
    // b->count++;
    // if(b->count > BACKLIGHT_ERROR_ON_FRAMES){ BACKLIGHT_OFF; }
    // if(b->count > BACKLIGHT_ERROR_OFF_FRAMES){ b->count = 0; BACKLIGHT_ON; }
  }
}


/* ------------------------------------------------------------------*
 *            ventilator
 * ------------------------------------------------------------------*/

void PORT_Ventilator_Update(struct PlantState *ps)
{
  // no new temp
  if(!ps->temp_sensor->new_temp_flag){ return; }

  // temperature
  char temp = ps->temp_sensor->actual_temp;
  char alarm_temp = (char)ps->settings->settings_alarm->temp;

  // ventilator
  if(ps->port_state->ventilator_on_flag)
  {
    char hyst_off = (alarm_temp - 20);
    if(temp < hyst_off)
    { 
      PORT_RelaisClr(R_VENTILATOR);
      ps->port_state->ventilator_on_flag = false;
    }
  }
  else
  {
    char hyst_on = (alarm_temp - 15);
    if(temp > hyst_on)
    { 
      PORT_RelaisSet(R_VENTILATOR);
      ps->port_state->ventilator_on_flag = true;
    }
  }

}


/* ------------------------------------------------------------------*
 *            relais
 * ------------------------------------------------------------------*/

void PORT_RelaisSet(unsigned char relais){ P_RELAIS.OUTSET = relais; }
void PORT_RelaisClr(unsigned char relais){ P_RELAIS.OUTCLR = relais; }


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
      LCD_WriteAnyStringFont(_f_6x8, y_pos_r, 0, "R: I1 I2 Ph Ex Co Cl Al Ve", true);
      LCD_WriteAnyStringFont(_f_6x8, y_pos_v, 0, "V: Ai Mu Cl Re", true);
    }
    
    // print register states
    else
    {
      // relays
      for(int relais = 0; relais < 8; relais++)
      {
        if(P_RELAIS.OUT & (1 << relais)){ LCD_WriteAnyStringFont(_f_6x8, y_pos_r, 18 + 18 * relais, "1 ", true); }
        else{ LCD_WriteAnyStringFont(_f_6x8, y_pos_r, 18 + 18 * relais, "0 ", true); }
      }

      // valves
      for(int valve = 0; valve < 4; valve++)
      {
        if(ps->port_state->valve_state & (1 << valve)){ LCD_WriteAnyStringFont(_f_6x8, y_pos_v, 18 + 18 * valve, "1 ", true); }
        else{ LCD_WriteAnyStringFont(_f_6x8, y_pos_v, 18 + 18 * valve, "0 ", true); }
      }
    }
  }
}