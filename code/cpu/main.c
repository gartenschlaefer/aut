// --
// main

#include "config.h"

#include "lcd_driver.h"
#include "lcd_app.h"

#include "modem_driver.h"
#include "port_func.h"
#include "basic_func.h"
#include "tc_func.h"
#include "can_app.h"


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
  struct Backlight backlight = { .state = _off, .count = 0 };
  struct FrameCounter frame_counter = { .usv = 0, .lcd_reset = 0, .frame = 0, .sixty_sec_counter = 0, .fps = 0.0, .delta_t = 0 };
  struct PageState page_state = { .page = DataPage, .page_time = &page_time };
  struct PageState auto_save_page_state = { .page = NoPage, .page_time = &auto_save_page_time };
  struct PortState port_state = { .buzzer_on = false, .valve_state = 0x00 };
  struct CompressorState compressor_state = { .operation_hours = 0, .cycle_o2_min = 0, .old_min = 0 };
  struct ErrorState error_state = { .page = ErrorTreat, .err_code = 0x00, .err_reset_flag = 0x00, .op_state = 0, .error_counter = { 0x00 } };
  struct MPXState mpx_state = { .mpx_count = 0, .mpx_values = { 0x00 }, .error_counter = 0, .level_cal = 0 };
  struct PhosphorState phosphor_state = { .ph_count = 0, .ph_tms = &phosphor_time, .ph_state = _ph_off };
  struct InflowPumpState inflow_pump_state = { .ip_count = 0, .ip_thms = &ip_thms, .ip_state = _ip_off, .ip_active_pump_id = 0 };
  struct AirCircState air_circ_state = { .old_sec = 0, .air_tms = &air_tms };
  struct CANState can_state = { .rxb0_buffer = { 0x00 }, .rxb0_data_av = 0 };
  struct TWIState twi_state = { .twid_rx_buffer = { 0x00, 0xFF } };
  struct SonicState sonic_state = { .app_type = SONIC_APP_none, .software_version = 0, .no_us_flag = false, .level_cal = 0, .d_mm = 0, .d_mm_prev = 0, .temp = 0, .d_error = 0, .read_tank_state = SONIC_TANK_timer_init, .query_state = _usWait, .query_error_count = 0 };
  struct InputHandler input_handler = { .float_sw_alarm = 0 };
  struct Modem modem = { .turned_on = 0, .turn_on_state = 0, .turn_on_error = 0, .startup_delay = 0 };
  struct TimeState time_state = { .tic_sec_update_flag = false };

  // plant state
  struct PlantState ps = { .init = 0, .page_state = &page_state, .auto_save_page_state = &auto_save_page_state, .port_state = &port_state, .compressor_state = &compressor_state, .backlight = &backlight, .frame_counter = &frame_counter, .error_state = &error_state, .mpx_state = &mpx_state, .phosphor_state = &phosphor_state, .inflow_pump_state = &inflow_pump_state, .air_circ_state = &air_circ_state, .can_state = &can_state, .twi_state = &twi_state, .sonic_state = &sonic_state, .input_handler = &input_handler, .modem = &modem, .time_state = &time_state};

  // init
  Basic_Init(&ps);

  //*-* modem test loop
  //Modem_Test(&ps);

  // frame timer (for fixed fps time)
  TCF1_FrameTimer_Init();

  while(1)
  {
    // watchdog
    BASIC_WDT_RESET;

    // time update
    Basic_TimeState_Update(&ps);

    // bootloader
    PORT_Bootloader();

    // modem
    Modem_Check(&ps);

    // CAN update
    CAN_Update(&can_state);

    //*** debug port and lcd page
    if(DEBUG)
    {
      if(DEB_PORT){ PORT_Debug(&ps); }
      LCD_WriteAnyValue(f_6x8_p, 2, 0, 70, ps.page_state->page);

      // fps
      LCD_WriteAnyValue(f_4x6_p, 3, 18, 0, (int)frame_counter.fps);
    }

    // GreatLinker
    switch(ps.page_state->page)
    {
      // main pages
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
        PORT_Auto_RunTime(&ps);
        break;

      // manual pages
      case ManualMain:
      case ManualCircOn:
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

    // frame timer wait
    TCF1_FrameTimer_WaitUntilFrameEnded(&frame_counter);
  }
}
