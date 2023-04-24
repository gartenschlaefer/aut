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

void PORT_Buzzer(t_FuncCmd cmd)
{
  static int count = 0;
  static t_FuncCmd state = _off;

  switch(cmd)
  {
    case _error:
      //*** debug disable buzzer
      if(DEB_BUZ) return;
      state = _error;       
      break;

    case _off:
      state = _off;
      PORTD.DIRCLR =  PIN6_bm;
      PORTD.OUTCLR =  PIN6_bm;    
      break;

    case _exe:
      if(state == _error)
      {
        count++;
        if(count > 400)
        {
          PORTD.DIRCLR =  PIN6_bm;
          PORTD.OUTCLR =  PIN6_bm;
        }
        if(count > 2000)
        {
          count = 0;
          PORTD.DIRSET =  PIN6_bm;
          PORTD.OUTSET =  PIN6_bm;
        }
      }
      break;

    default: break;
  }

}


/* ------------------------------------------------------------------*
 *            ventilator
 * ------------------------------------------------------------------*/

void PORT_Ventilator(void)
{
  unsigned char temp = 0;
  unsigned char hystOn = 0;
  unsigned char hystOff = 0;

  // temperature
  temp = MCP9800_PlusTemp();

  // hysteresis
  hystOn =  (MEM_EEPROM_ReadVar(ALARM_temp) - 15);
  hystOff = (MEM_EEPROM_ReadVar(ALARM_temp) - 20);

  if(!(temp & 0x80))
  {
    if(temp > hystOn)   PORT_RelaisSet(R_VENTILATOR);
    if(temp < hystOff)  PORT_RelaisClr(R_VENTILATOR);
  }
  else PORT_RelaisClr(R_VENTILATOR);
}


/* ------------------------------------------------------------------*
 *            valves
 * ------------------------------------------------------------------*/

unsigned char PORT_Valve(t_valve valve, unsigned char new_state)
{
  static unsigned char state = 0;

  // watchdog reset
  BASIC_WDT_RESET;

  switch(valve)
  {
    case OPEN_Reserve:
      P_VALVE.OUTSET = O_RES;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_RES; 
      state |= V_RES;
      break;

    case CLOSE_Reserve:
      P_VALVE.OUTSET = C_RES;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_RES;
      state &= ~V_RES;      
      break;

    case OPEN_MudPump:
      P_VALVE.OUTSET = O_MUD;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_MUD;
      state |= V_MUD;     
      break;

    case CLOSE_MudPump:
      P_VALVE.OUTSET = C_MUD;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_MUD;
      state &= ~V_MUD;
      break;

    case OPEN_Air:
      P_VALVE.OUTSET = O_AIR;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_AIR;
      state |= V_AIR;   
      break;

    case CLOSE_Air:
      P_VALVE.OUTSET = C_AIR;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_AIR;
      state &= ~V_AIR;
      break;

    case OPEN_ClearWater:
      P_VALVE.OUTSET = O_CLRW;
      TCC0_wait_openValve();
      P_VALVE.OUTCLR = O_CLRW;
      state |= V_CLW;   
      break;

    case CLOSE_ClearWater:
      P_VALVE.OUTSET = C_CLRW;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_CLRW;
      state &= ~V_CLW;
      break;

    case CLOSE_IPAir:
      P_VALVE.OUTSET = C_AIR | C_RES;
      TCC0_wait_closeValve();
      P_VALVE.OUTCLR = C_AIR | C_RES;  
      break;

    // states
    case SET_STATE_CLOSE: state &= ~new_state; break;
    case SET_STATE_OPEN: state |= new_state; break;
    case SET_STATE_ALL_CLOSED: state = 0x00; break;
    case SET_STATE_ALL_OPEN: state = 0x0F; break;
    default: break;
  }
  return state;
}


/* ------------------------------------------------------------------*
 *            valve open all
 * ------------------------------------------------------------------*/

void PORT_Valve_OpenAll(void)
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

  // set state
  PORT_Valve(SET_STATE_ALL_OPEN, 0);
}


/* ------------------------------------------------------------------*
 *            valve close all
 * ------------------------------------------------------------------*/

void PORT_Valve_CloseAll(void)
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
  PORT_Valve(SET_STATE_ALL_CLOSED, 0);
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
    PORT_Ventilator();

    // Floating switch alarm
    if(IN_FLOAT_S3 && !ps->input_handler->float_sw_alarm)
    {
      if(MEM_EEPROM_ReadVar(ALARM_sensor))
      {
        Modem_Alert("Error: floating switch");
        Error_ON(ps->lcd_backlight);
      }
      ps->input_handler->float_sw_alarm = 1;
    }
    else if(!IN_FLOAT_S3 && ps->input_handler->float_sw_alarm)
    {
      if(MEM_EEPROM_ReadVar(ALARM_sensor))
      {
        Error_OFF(ps->lcd_backlight);
      }
      ps->input_handler->float_sw_alarm = 0;
    }

    //*** debug USVCheckVoltageSupply
    if(!DEBUG)
    {
      ADC_USV_Check(&ps->frame_counter->usv);
    }
  }
}


/* ------------------------------------------------------------------*
 *            debug
 * ------------------------------------------------------------------*/

void PORT_Debug(void)
{ 
  static unsigned char refresh = 0;
  static unsigned char blink = 1;

  unsigned char y_pos_r = 19;
  unsigned char y_pos_v = 21;

  refresh++;

  if(refresh == 150)
  {
    refresh = 0;

    // print register names
    if(blink)
    {
      blink = 0;
      LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 0, "R: I1 I2 Ph Ex Co Cl Al Ve");
      LCD_WriteAnyStringFont(f_6x8_n, y_pos_v, 0, "V: Ai Mu Cl Re");
    }
    
    // print register states
    else
    {
      blink = 1;

      // relays
      for(int relais = 0; relais < 8; relais++)
      {
        if(P_RELAIS.OUT & (1<<relais)) LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 18 + 18 * relais, "1 ");
        else LCD_WriteAnyStringFont(f_6x8_n, y_pos_r, 18 + 18 * relais, "0 ");
      }

      // valves
      for(int valve = 0; valve < 4; valve++)
      {
        if(PORT_Valve(READ_STATE, 0) & (1<<valve)) LCD_WriteAnyStringFont(f_6x8_n, y_pos_v, 18 + 18 * valve, "1 ");
        else LCD_WriteAnyStringFont(f_6x8_n, y_pos_v, 18 + 18 * valve, "0 ");
      }
    }
  }
}
