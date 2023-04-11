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
  // times
  struct Tms page_time = { .min = 5, .sec = 0 };
  struct Tms auto_save_page_time = { .min = 5, .sec = 0 };
  struct Tms phosphor_time = { .min = 0, .sec = 5 };
  struct Thms ip_thms = { .hou = 0, .min = 0, .sec = 6 };
  struct Tms air_tms = { .min = 0, .sec = 0 };

  // datatypes and states
  struct LcdBacklight lcd_backlight = { .state = _off, .count = 0 };
  struct FrameCounter frame_counter = { .usv = 0, .lcd_reset = 0 };
  struct PageState page_state = { .page = DataPage, .page_time = &page_time };
  struct PageState auto_save_page_state = { .page = NoPage, .page_time = &auto_save_page_time };
  struct ErrorState error_state = { .page = ErrorTreat, .err_code = 0x00, .err_reset_flag = 0x00 };
  struct MPXState mpx_state = { .mpx_count = 0, .mpx_values = { 0x00 }, .error_counter = 0, .level_cal = 0 };
  struct PhosphorState phosphor_state = { .ph_count = 0, .ph_tms = &phosphor_time, .ph_state = ph_off };
  struct InflowPumpState inflow_pump_state = { .ip_count = 0, .ip_thms = &ip_thms, .ip_state = ip_off };
  struct AirCircState air_circ_state = { .old_sec = 0, .air_tms = &air_tms };

  // plant state
  struct PlantState ps = { .init = 0, .page_state = &page_state, .auto_save_page_state = &auto_save_page_state, .lcd_backlight = &lcd_backlight, .frame_counter = &frame_counter, .error_state = &error_state, .mpx_state = &mpx_state, .phosphor_state = &phosphor_state, .inflow_pump_state = &inflow_pump_state, .air_circ_state = &air_circ_state};


  // init
  Basic_Init();
  LCD_Backlight(_on, ps.lcd_backlight);

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
    Modem_Check(ps.page_state->page, &modem);

    //*** debug port and lcd page
    if(DEBUG)
    {
      if (DEB_PORT) PORT_Debug();
      LCD_WriteAnyValue(f_6x8_p, 2, 0, 70, ps.page_state->page);
    }

    // GreatLinker
    switch(ps.page_state->page)
    {
      case AutoPage: LCD_AutoPage(&ps); break;
      case ManualPage: LCD_ManualPage(&ps); break;
      case SetupPage: LCD_SetupPage(&ps); break;
      case DataPage: LCD_DataPage(&ps); break;

      // pin-pages
      case PinManual: LCD_PinPage_Main(&ps); break;
      case PinSetup: LCD_PinPage_Main(&ps); break;

      // auto pages
      case AutoZone:
      case AutoSetDown:
      case AutoPumpOff:
      case AutoMud:
      case AutoCircOn:
      case AutoCircOff:
      case AutoAirOn:
      case AutoAirOff:
        LCD_AutoPage(&ps);
        PORT_RunTime(&input_handler, &ps);
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
        LCD_ManualPage(&ps);
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
        LCD_SetupPage(&ps);
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
        LCD_DataPage(&ps);
        break;

      default: ps.page_state->page = AutoPage; break;
    }
  }
}
