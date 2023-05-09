// --
// basic functions, such as init, watchdog, clock

#include <avr/io.h>
#include <avr/interrupt.h>

#include "basic_func.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "ad8555_driver.h"
#include "at24c_driver.h"
#include "mcp7941_driver.h"
#include "modem_driver.h"
#include "adc_func.h"
#include "usart_func.h"
#include "can_app.h"
#include "tc_func.h"
#include "port_func.h"
#include "twi_func.h"
#include "sonic_app.h"
#include "can_app.h"
#include "memory_app.h"
#include "mpx_driver.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Basic_Init(struct PlantState *ps)
{
  // ports init
  Basic_Clock_Init();
  PORT_Init();
  ADC_Init();

  // communication init
  USART_Init();
  TWI_C_Master_Init();
  TWI_D_Master_Init();

  // display init
  LCD_HardwareRst();
  TCC0_wait_ms(200);
  LCD_Init();
  TCC0_wait_ms(200);
  LCD_Clean();

  // Memory init
  if(MEM_INIT){ Basic_Init_Mem(); }

  // watchdog
  Basic_Watchdog_Init();

  // devices init
  MCP7941_Init();
  AD8555_Init();
  CAN_Init();
  TCC0_wait_ms(200);

  // sonic jump to app version
  TCE1_WaitMilliSec_Init(25);
  CAN_TxCmd(ps->can_state, CAN_CMD_sonic_app);
  while(CAN_RxB0_Ack(ps->can_state) != CAN_CMD_sonic_ack)
  {
    CAN_RxB0_Read(ps->can_state);
    if(TCE1_Wait_Query()){ break; }
  }
  CAN_RxB0_Clear(ps->can_state);
  TCE1_Stop();

  // mpx init
  MPX_Init(ps);

  // sonic init
  Sonic_Init(ps);

  // timer ic variables init
  Basic_TimeState_Init(ps);

  // modem init
  Modem_Init(ps);

  // backlight on
  PORT_Backlight_On(ps->backlight);
}


/* ------------------------------------------------------------------*
 *            memory init
 * ------------------------------------------------------------------*/

void Basic_Init_Mem(void)
{
  // tel. nr. in EEPROM
  AT24C_MemoryReset();

  // set data pages zero
  MEM_EEPROM_SetZero();

  //***VarDefaultShort
  if(DEBUG){ MEM_EEPROM_WriteVarDefault_Short(); }
  else{ MEM_EEPROM_WriteVarDefault(); }

  // timer ic init
  MCP7941_InitDefault();

  // lcd calibration
  LCD_Calibration();
}


/* ------------------------------------------------------------------*
 *            Clock
 * ------------------------------------------------------------------*/

void Basic_Clock_Init(void)
{
  // init oscillator
  OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
  OSC.CTRL = OSC_XOSCEN_bm;

  // wait until clock is ready
  while(!(OSC.STATUS & OSC_XOSCRDY_bm));

  // protection
  CCP = 0xD8;

  // selection
  CLK.CTRL = CLK_SCLKSEL_XOSC_gc;
}


/*-------------------------------------------------------------------*
 *            watchdog init
 * ------------------------------------------------------------------*/

void Basic_Watchdog_Init(void)
{
  // io protection
  CCP = CCP_IOREG_gc;

  // watchdog init
  WDT.CTRL =  WDT_CEN_bm | WDT_PER_8KCLK_gc | WDT_ENABLE_bm;
}


/* ------------------------------------------------------------------*
 *            timer update
 * ------------------------------------------------------------------*/

void Basic_TimeState_Init(struct PlantState *ps)
{
  // read timer ic vars
  ps->time_state->tic_sec = MCP7941_ReadTime(ps->twi_state, TIC_SEC);
  ps->time_state->tic_min = MCP7941_ReadTime(ps->twi_state, TIC_MIN);
  ps->time_state->tic_hou = MCP7941_ReadTime(ps->twi_state, TIC_HOUR);
  ps->time_state->tic_dat = MCP7941_ReadTime(ps->twi_state, TIC_DATE);
  ps->time_state->tic_mon = MCP7941_ReadTime(ps->twi_state, TIC_MONTH);
  ps->time_state->tic_yea = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
}


/* ------------------------------------------------------------------*
 *            timer update
 * ------------------------------------------------------------------*/

void Basic_TimeState_Update(struct PlantState *ps)
{
  // read seconds from timer ic
  unsigned char act_sec = MCP7941_ReadTime(ps->twi_state, TIC_SEC);

  // second change
  if(act_sec != ps->time_state->tic_sec)
  {
    // second update
    ps->time_state->tic_sec = act_sec;
    ps->time_state->tic_sec_update_flag = true;

    // minute change
    if(!act_sec)
    {
      // minute update
      ps->time_state->tic_min = MCP7941_ReadTime(ps->twi_state, TIC_MIN);

      // hour change
      if(!ps->time_state->tic_min)
      {
        // hour update
        ps->time_state->tic_hou = MCP7941_ReadTime(ps->twi_state, TIC_HOUR);

        // day change
        if(!ps->time_state->tic_hou)
        {
          // other updates
          ps->time_state->tic_dat = MCP7941_ReadTime(ps->twi_state, TIC_DATE);
          ps->time_state->tic_mon = MCP7941_ReadTime(ps->twi_state, TIC_MONTH);
          ps->time_state->tic_yea = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
        }
      }
    }
  }

  // no second change
  else
  {
    ps->time_state->tic_sec_update_flag = false;
  }
}


/* ------------------------------------------------------------------*
 *            countdown of page time
 * ------------------------------------------------------------------*/

unsigned char Basic_CountDown(struct PlantState *ps)
{
  // countdown update
  if(ps->time_state->tic_sec_update_flag)
  {
    // safety for seconds
    if(ps->page_state->page_time->sec < 0 || ps->page_state->page_time->sec > 61){ ps->page_state->page_time->sec = 0; }

    // minute update
    if(!ps->page_state->page_time->sec && ps->page_state->page_time->min)
    {
      ps->page_state->page_time->sec = 60;
      ps->page_state->page_time->min--;

      //*** entry debug every minute
      // if(DEB_ENTRY)
      // {
      //   MEM_EEPROM_WriteAutoEntry(ps);
      //   MEM_EEPROM_WriteManualEntry(ps, 0, 0, _write);
      //   MEM_EEPROM_WriteSetupEntry(ps);
      // }
    }

    // second update
    if(ps->page_state->page_time->sec){ ps->page_state->page_time->sec--; }

    // end of page time
    if(!ps->page_state->page_time->sec && !ps->page_state->page_time->min)
    {
      ps->page_state->page_time->min = 0;
      ps->page_state->page_time->sec = 5;
      return 1;
    }
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *  limit functions
 * ------------------------------------------------------------------*/

int Basic_LimitAdd(int value, int max)
{
  if(value < max){ value++; }
  else{ value = value; }
  return value;
}

int Basic_LimitDec(int value, int min)
{
  if(value > min){ value--; }
  else{ value = value; }
  return value;
}