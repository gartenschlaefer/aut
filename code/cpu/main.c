// --
// main

#include "config.h"

#include "lcd_driver.h"
#include "lcd_app.h"

#include "modem_driver.h"
#include "port_func.h"
#include "basic_func.h"


/* ------------------------------------------------------------------*
 *            main
 * ------------------------------------------------------------------*/

int main(void)
{
  // start page
  t_page page = DataPage;

  // datatypes
  struct LcdBacklight lcd_backlight_default = { .state = _off, .count = 0 };
  struct PlantState plant_state = { .page = DataPage, .lcd_backlight = lcd_backlight_default };


  // init
  Basic_Init();
  LCD_Backlight(_on, &plant_state.lcd_backlight);

  //*-* modem test loop
  //Modem_Test();

  // Input
  struct InputHandler input_handler;
  struct Modem modem;
  InputHandler_init(&input_handler);
  Modem_init(&modem);

  while(1)
  {
    BASIC_WDT_RESET;
    PORT_Bootloader();
    Modem_Check(page, &modem);

    //*** debug port and lcd page
    if(DEBUG)
    {
      if (DEB_PORT) PORT_Debug();
      LCD_WriteAnyValue(f_6x8_p, 2, 0, 70, page);
    }

    // GreatLinker
    switch(page)
    {
      case AutoPage:    page = LCD_AutoPage(page, &plant_state);    break;
      case ManualPage:  page = LCD_ManualPage(page, &plant_state);  break;
      case SetupPage:   page = LCD_SetupPage(page, &plant_state);   break;
      case DataPage:    page = LCD_DataPage(page, &plant_state);    break;

      // pin-pages
      case PinManual:   page = LCD_PinPage(page, &plant_state);   break;
      case PinSetup:    page = LCD_PinPage(page, &plant_state);   break;

      // auto pages
      case AutoZone:
      case AutoSetDown:
      case AutoPumpOff:
      case AutoMud:
      case AutoCirc:
      case AutoCircOff:
      case AutoAir:
      case AutoAirOff:
        page = LCD_AutoPage(page, &plant_state);
        PORT_RunTime(&input_handler, &plant_state);
        break;

      // manual pages
      case ManualMain:
      case ManualCirc:
      case ManualCircOff:
      case ManualAir:
      case ManualSetDown:
      case ManualPumpOff:
      case ManualPumpOff_On:
      case ManualMud:
      case ManualCompressor:
      case ManualPhosphor:
      case ManualInflowPump:
        page = LCD_ManualPage(page, &plant_state);
        break;

      // setup pages
      case SetupMain:
      case SetupCirculate:
      case SetupAir:
      case SetupSetDown:
      case SetupPumpOff:
      case SetupMud:
      case SetupCompressor:
      case SetupPhosphor:
      case SetupInflowPump:
      case SetupCal:
      case SetupCalPressure:
      case SetupAlarm:
      case SetupWatch:
      case SetupZone:
        page = LCD_SetupPage(page, &plant_state);
        break;

      // data pages
      case DataMain:
      case DataAuto:
      case DataManual:
      case DataSetup:
      case DataSonic:
      case DataSonicAuto:
      case DataSonicBoot:
      case DataSonicBootR:
      case DataSonicBootW:
        page = LCD_DataPage(page, &plant_state);
        break;

      default: page = AutoPage; break;
    }
  }
}
