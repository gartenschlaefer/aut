// --
// basic functions, such as init, watchdog, clock

#include <avr/io.h>
#include <avr/interrupt.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "basic_func.h"

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
#include "memory_app.h"


/* ==================================================================*
 *            Init
 * ==================================================================*/

void Basic_Init(void)
{
  // ports init
  Clock_Init();
  PORT_Init();
  ADC_Init();

  // communication init
  USART_Init();
  TWI_Master_Init();
  TWI2_Master_Init();

  // display init
  LCD_HardwareRst();
  TCC0_Main_Wait();
  LCD_Init();
  TCC0_Main_Wait();
  LCD_Clean();

  // Memory init
  if(MEM_INIT) Basic_Init_Mem();

  // Watchdog
  Watchdog_Init();

  // Devices Init
  MCP7941_Init();
  AD8555_Init();
  CAN_Init();
  TCC0_Main_Wait();

  //--------------------------------------------------JumptoApp
  TCE1_WaitMilliSec_Init(25);
  CAN_TxCmd(_app);
  while(CAN_RxACK() != _ack)
    if(TCE1_Wait_Query()) break;
  TCE0_Stop();

  // Safety Timer
  TCD1_MainAuto_SafetyTC(_init);
}


/*-------------------------------------------------------------------*
 *  Basic - Memory Init
 * ------------------------------------------------------------------*/

void Basic_Init_Mem(void)
{
  // tel. nr. in EEPROM
  AT24C_Init();

  // set data pages zero
  MEM_EEPROM_SetZero();

  if(DEBUG)
  {
    //***VarDefaultShort
    MEM_EEPROM_WriteVarDefault_Short();
  }
  else
  {
    MEM_EEPROM_WriteVarDefault();
  }

  // timer ic init
  MCP7941_InitDefault();

  // lcd calibration
  LCD_Calibration();
}



/* ==================================================================*
 *            Clock
 * ==================================================================*/

void Clock_Init(void)
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



/* ==================================================================*
 *            Watchdog
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  Watchdog_Init
 * ------------------------------------------------------------------*/

void Watchdog_Init(void)
{
  // io protection
  CCP = CCP_IOREG_gc;

  // watchdog init
  WDT.CTRL =  WDT_CEN_bm | WDT_PER_8KCLK_gc | WDT_ENABLE_bm;
}