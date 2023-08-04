// --
// controller

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "controller.h"

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
#include "settings.h"
#include "compressor_info.h"
#include "time_state.h"
#include "mcp9800_driver.h"
#include "tank.h"
#include "output_app.h"


/* ------------------------------------------------------------------*
 *            new
 * ------------------------------------------------------------------*/

struct Controller *Controller_New(void)
{
  // allocate memory
  struct Controller *controller = malloc(sizeof(struct Controller));
  if(!controller){ return NULL; }
  return controller;
}


/* ------------------------------------------------------------------*
 *            destroy controller
 * ------------------------------------------------------------------*/

void Controller_Destroy(struct Controller *controller)
{
  if(!controller){ return; }
  free(controller);
}


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Controller_Init(struct Controller *controller, struct PlantState *ps)
{
  // uc inits
  Controller_uC_Clock_Init();
  PORT_Init(ps);
  ADC_Init();

  // communication init
  USART_Init();
  TWI_C_Master_Init();
  TWI_D_Master_Init();
  CAN_Init();

  // display init
  LCD_Reset_Hardware();
  TCC0_wait_ms(200);
  LCD_Init();
  TCC0_wait_ms(200);
  LCD_Clean();

  // Memory init
  if(MEM_INIT){ Controller_Init_Mem(); }

  // init settings
  Settings_Init(ps->settings);

  // watchdog
  Controller_uC_Watchdog_Init();

  // devices init
  MCP7941_Init();
  AD8555_Init();
  MCP9800_Init(ps);
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

  // compressor info init
  Compressor_Info_Init(ps);

  // mpx init
  MPX_Init(ps);

  // sonic init
  Sonic_Init(ps);

  // tank init
  Tank_Init(ps);

  // timer ic variables init
  TimeState_Init(ps);

  // modem init
  Modem_Init(ps);

  // start page settings
  Controller_ChangePage(controller, CONTROL_START_PAGE);
}


/* ------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void Controller_Update(struct Controller *controller, struct PlantState *ps)
{
  // time update
  TimeState_Update(ps);

  // bootloader
  PORT_Update(ps);

  // modem
  Modem_Update(ps);

  // CAN update
  CAN_Update(ps->can_state);

  // valve update
  OUT_Valve_Update(ps);

  // temp update
  MCP9800_Temp_Update(ps);

  // update actual page
  controller->f_controller_update(ps);
}


/* ------------------------------------------------------------------*
 *            change page
 * ------------------------------------------------------------------*/

void Controller_ChangePage(struct Controller *controller, t_page new_page)
{
  // GreatLinker
  switch(new_page)
  {
    // auto pages
    case AutoPage: case AutoZone: case AutoSetDown: case AutoPumpOff: case AutoMud: case AutoCirc: case AutoAir:
      controller->f_controller_update = &LCD_AutoPage; 
      break;

    // manual pages
    case ManualPage: case ManualMain: case ManualCirc: case ManualAir: case ManualSetDown: case ManualPumpOff: case ManualPumpOff_On: case ManualMud:
    case ManualCompressor: case ManualPhosphor: case ManualInflowPump: case ManualValveTest:
      controller->f_controller_update = &LCD_ManualPage;
      break;

    // setup pages
    case SetupPage: case SetupMain: case SetupCirculate: case SetupAir: case SetupSetDown: case SetupPumpOff: case SetupMud: case SetupCompressor:
    case SetupPhosphor: case SetupInflowPump: case SetupCal: case SetupCalPressure: case SetupAlarm: case SetupWatch: case SetupZone:
      controller->f_controller_update = &LCD_SetupPage;
      break;

    // data pages
    case DataPage: case DataMain: case DataAuto: case DataManual: case DataSetup: case DataSonic: case DataSonicAuto: case DataSonicBoot: case DataSonicBootR: case DataSonicBootW:
      controller->f_controller_update = &LCD_DataPage;
      break;

    // pin-pages
    case PinManual: case PinSetup: 
      controller->f_controller_update = &LCD_PinPage_Main; 
      break;

    default: controller->f_controller_update = &LCD_AutoPage; break;
  }
}


/* ------------------------------------------------------------------*
 *            memory init
 * ------------------------------------------------------------------*/

void Controller_Init_Mem(void)
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

void Controller_uC_Clock_Init(void)
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

void Controller_uC_Watchdog_Init(void)
{
  // io protection
  CCP = CCP_IOREG_gc;

  // watchdog init
  WDT.CTRL =  WDT_CEN_bm | WDT_PER_8KCLK_gc | WDT_ENABLE_bm;
}
