// --
// port functions

//#include <avr/interrupt.h>

#include "port_func.h"

#include "config.h"
#include "lcd_driver.h"
#include "memory_app.h"
#include "mcp9800_driver.h"
#include "adc_func.h"
#include "tc_func.h"
#include "modem_driver.h"
#include "error_func.h"
#include "basic_func.h"
#include "port_func.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void PORT_Init(void)
{
  // inputs
  P_OPTO.DIRCLR = PIN3_bm | OC1 | OC2 | OC3 | OC4;

  // outputs
  P_VALVE.DIR =  0xFF;
  P_RELAIS.DIR =  0xFF;

  // Pins PULL UP
  PORTCFG.MPCMASK = 0xFF;
  P_OPTO.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;

  // FirmwareUpdate PullUp
  PORTD.PIN5CTRL= PORT_OPC_WIREDANDPULL_gc;

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

  // hysteresis
  unsigned char hystOn = (MEM_EEPROM_ReadVar(ALARM_temp) - 15);
  unsigned char hystOff = (MEM_EEPROM_ReadVar(ALARM_temp) - 20);

  if(!(temp & 0x80))
  {
    if(temp > hystOn){ PORT_RelaisSet(R_VENTILATOR); }
    if(temp < hystOff){ PORT_RelaisClr(R_VENTILATOR); }
  }
  else{ PORT_RelaisClr(R_VENTILATOR); }
}


/* ------------------------------------------------------------------*
 *            valves
 * ------------------------------------------------------------------*/

void PORT_Valve(struct PlantState *ps, t_valve valve)
{
  // watchdog reset
  BASIC_WDT_RESET;

  switch(valve)
  {
    case OPEN_Reserve:
      P_VALVE.OUTSET = O_RES;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_RES; 
      ps->port_state->valve_state |= V_RES;
      break;

    case CLOSE_Reserve:
      P_VALVE.OUTSET = C_RES;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_RES;
      ps->port_state->valve_state &= ~V_RES;      
      break;

    case OPEN_MudPump:
      P_VALVE.OUTSET = O_MUD;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_MUD;
      ps->port_state->valve_state |= V_MUD;     
      break;

    case CLOSE_MudPump:
      P_VALVE.OUTSET = C_MUD;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_MUD;
      ps->port_state->valve_state &= ~V_MUD;
      break;

    case OPEN_Air:
      P_VALVE.OUTSET = O_AIR;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_AIR;
      ps->port_state->valve_state |= V_AIR;   
      break;

    case CLOSE_Air:
      P_VALVE.OUTSET = C_AIR;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_AIR;
      ps->port_state->valve_state &= ~V_AIR;
      break;

    case OPEN_ClearWater:
      P_VALVE.OUTSET = O_CLRW;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_CLRW;
      ps->port_state->valve_state |= V_CLW;   
      break;

    case CLOSE_ClearWater:
      P_VALVE.OUTSET = C_CLRW;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_CLRW;
      ps->port_state->valve_state &= ~V_CLW;
      break;

    case CLOSE_IPAir:
      P_VALVE.OUTSET = C_AIR | C_RES;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_AIR | C_RES;
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            valve open all
 * ------------------------------------------------------------------*/

void PORT_Valve_OpenAll(struct PlantState *ps)
{
  // watchdog reset
  BASIC_WDT_RESET;

  // valve open
  P_VALVE.OUTSET = O_RES;
  TCC0_wait_ms(500);
  P_VALVE.OUTSET = O_MUD;
  TCC0_wait_ms(500);
  P_VALVE.OUTSET = O_AIR;
  TCC0_wait_ms(500);
  P_VALVE.OUTSET = O_CLRW;

  // spring or usual valves
  if(SPRING_VALVE_ON){ TCC0_wait_sec(2); TCC0_wait_ms(850); }
  else{ TCC0_wait_sec(1); TCC0_wait_ms(500); }

  // stop opening
  P_VALVE.OUTCLR= O_RES;
  TCC0_wait_ms(500);
  P_VALVE.OUTCLR = O_MUD;
  TCC0_wait_ms(500);
  P_VALVE.OUTCLR = O_AIR;
  TCC0_wait_ms(500);
  P_VALVE.OUTCLR = O_CLRW;

  // set state all open
  ps->port_state->valve_state = 0x0F;
}


/* ------------------------------------------------------------------*
 *            valve close all
 * ------------------------------------------------------------------*/

void PORT_Valve_CloseAll(struct PlantState *ps)
{
  // watchdog reset
  BASIC_WDT_RESET;
  
  P_VALVE.OUTSET = C_RES;
  TCC0_wait_ms(500);
  P_VALVE.OUTSET = C_MUD;
  TCC0_wait_ms(500);
  P_VALVE.OUTSET = C_AIR;
  TCC0_wait_ms(500);
  P_VALVE.OUTSET = C_CLRW;

  // spring valves: [2 - 4]
  if(SPRING_VALVE_ON){ TCC0_wait_sec(2); TCC0_wait_ms(800); }
  else{ TCC0_wait_sec(2); }

  P_VALVE.OUTCLR = C_RES;
  TCC0_wait_ms(500);
  P_VALVE.OUTCLR = C_MUD;
  TCC0_wait_ms(500);
  P_VALVE.OUTCLR = C_AIR;
  TCC0_wait_ms(500);
  P_VALVE.OUTCLR = C_CLRW;

  // set state
  ps->port_state->valve_state = 0x00;
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
      if(MEM_EEPROM_ReadVar(ALARM_sensor))
      {
        Modem_Alert(ps, "Error: floating switch");
        Error_On(ps);
      }
      ps->input_handler->float_sw_alarm = 1;
    }
    else if(!IN_FLOAT_S3 && ps->input_handler->float_sw_alarm)
    {
      if(MEM_EEPROM_ReadVar(ALARM_sensor))
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

  // print register names
  if(ps->frame_counter->frame % 2)
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
      if(P_RELAIS.OUT & (1<<relais)){ LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 18 + 18 * relais, "1 "); }
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
