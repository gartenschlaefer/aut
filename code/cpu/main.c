// --
// main
// author: Christian Walter
// uC: ATxmega128A1

#include <stdlib.h>
#include "config.h"

#include "lcd_driver.h"
#include "lcd_app.h"

#include "modem_driver.h"
#include "port_func.h"
#include "output_app.h"
#include "basic_func.h"
#include "tc_func.h"
#include "can_app.h"
#include "queue.h"
#include "settings.h"
#include "mpx_driver.h"
#include "compressor_info.h"
#include "time_state.h"
#include "mcp9800_driver.h"
#include "page_state.h"
#include "tank.h"

// main pattern
#include "view.h"
#include "controller.h"


/* ------------------------------------------------------------------*
 *            global variables
 * ------------------------------------------------------------------*/

struct USARTState global_usart_state = { .rx_buffer = {0}, .pos = 0 };


/* ------------------------------------------------------------------*
 *            main
 * ------------------------------------------------------------------*/

int main(void)
{
  // times
  struct Tms page_time = { .min = 5, .sec = 0 };
  struct Tms auto_save_page_time = { .min = 5, .sec = 0 };
  struct Tms ph_tms = { .min = 0, .sec = 5 };
  struct Thms ip_thms = { .hou = 0, .min = 0, .sec = 6 };
  struct Thms comp_thms = { .hou = 0, .min = 0, .sec = 0 };
  struct Tms air_tms = { .min = 0, .sec = 0 };
  struct TeleNr tele_nr1 = { .id = 1, .nr = { 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 } };
  struct TeleNr tele_nr2 = { .id = 2, .nr = { 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 } };
  struct TeleNr tele_nr_temp = { .id = 0, .nr = { 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 } };

  // datatypes and states
  struct Backlight backlight = { .state = _bl_on, .count = 0 };
  struct FrameCounter frame_counter = { .usv = 0, .lcd_reset = 0, .frame = 0, .sixty_sec_counter = 0, .fps = 0.0, .delta_t = 0 };
  struct PageState page_state = { .page = CONTROL_START_PAGE, .page_time = &page_time, .change_page_flag = false };
  struct PageState auto_save_page_state = { .page = NonePage, .page_time = &auto_save_page_time, .change_page_flag = false };
  struct PortState port_state = { .buzzer_on = false, .ventilator_on_flag = false, .valve_state = 0, .valve_action = VALVE_Idle, .valve_action_flag = false, .valve_handling = _valveHandling_idle, .queue_valve_action = queue_new(), .valve_init = false, .f_backlight_update = &PORT_Nope };
  struct CompressorState compressor_state = { .operation_time = &comp_thms, .cycle_o2_min = 0 };
  struct ErrorState error_state = { .error_code = 0, .pending_err_code = 0, .cycle_error_code_record = 0, .error_counter = { 0 }, .error_on_counter = { 0 }, .reset_error_indicator = 0 };
  struct MPXState mpx_state = { .mpx_idx = 0, .mpx_values = { 0x00 }, .actual_mpx_av = 0, .actual_level_perc = 0, .new_mpx_av_flag = false };
  struct TankState tank_state = { .level_mm = 0, .level_abs_zero_mm = 0, .level_perc = 0, .new_level_flag = false, .change_page_flag = false, .f_tank_level_measure = &Tank_Level_Measure_MPX };
  struct PhosphorState phosphor_state = { .ph_tms = &ph_tms, .ph_state = _ph_off, .init_flag = false };
  struct InflowPumpState inflow_pump_state = { .ip_thms = &ip_thms, .ip_state = _ip_off, .ip_active_pump_id = 0, .init_flag = false };
  struct AirCircState air_circ_state = { .air_tms = &air_tms };
  struct CANState can_state = { .mcp2525_ok_flag = true, .rxb0_buffer = { 0x00 }, .rxb0_data_av = 0 };
  struct TWIState twi_state = { .twid_rx_buffer = { 0x00, 0xFF } };
  struct TempSensor temp_sensor = { .actual_temp = 0, .new_temp_flag = false };
  struct SonicState sonic_state = { .app_type = SONIC_APP_none, .software_version = 0, .no_us_error_counter = 0, .no_us_flag = false, .new_distance_flag = false, .d_mm = 0, .d_mm_prev = 0, .d_mm_max = 0, .d_mm_min = 10000, .temp = 0, .temp_max = 0, .temp_min = 10000, .d_error = 0, .read_tank_state = SONIC_TANK_timer_init, .query_state = _usWait, .query_error_count = 0, .record_position = 5, .record_error_update = 0 };
  struct InputHandler input_handler = { .float_sw_alarm = 0 };
  struct Modem modem = { .turned_on = 0, .turn_on_state = 0, .turn_on_error = 0, .startup_delay = 0, .tele_nr1 = &tele_nr1, .tele_nr2 = &tele_nr2, .tele_nr_temp = &tele_nr_temp, .temp_digit_pos = 0 };
  struct TimeState time_state = { .tic_sec_update_flag = false };
  struct EEPROMState eeprom_state = { .time_manual_entry = { .hou = 0, .min = 0} };
  struct TouchState touch_state = { .state = _touch_clean, .x = 0, .y = 0, .chunk = 0, .x_data = { 0, 0 }, .y_data = { 0, 0 }, .init = false, .touched = 0, .select = 0, .var = { 0 }, .pin_touch = 0, .p_value_setting = NULL, .p_value_limit = NULL };
  struct Settings *settings = Settings_New();

  // main states
  struct PlantState plant_state = { .page_state = &page_state, .auto_save_page_state = &auto_save_page_state, .port_state = &port_state, .compressor_state = &compressor_state, .backlight = &backlight, .frame_counter = &frame_counter, .error_state = &error_state, .mpx_state = &mpx_state, .tank_state = &tank_state, .phosphor_state = &phosphor_state, .inflow_pump_state = &inflow_pump_state, .air_circ_state = &air_circ_state, .can_state = &can_state, .twi_state = &twi_state, .usart_state = &global_usart_state, .temp_sensor = &temp_sensor, .sonic_state = &sonic_state, .input_handler = &input_handler, .modem = &modem, .time_state = &time_state, .eeprom_state = &eeprom_state, .touch_state = &touch_state, .settings = settings};
  struct View *view = View_New();
  struct Controller *controller = Controller_New();

  // pointer to states
  struct PlantState *ps = &plant_state;

  // view
  View_Init(view);

  // init
  Basic_Init(ps);

  //*-* modem test loop
  //Modem_Test(ps);

  // frame timer (for fixed fps time)
  TCF1_FrameTimer_Init();

  while(1)
  {
    // watchdog
    BASIC_WDT_RESET;

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

    // compressor info
    Compressor_Info_Update(ps);

    // temp update
    MCP9800_Temp_Update(ps);

    //*** debug port and lcd page
    if(DEBUG)
    {
      if(DEB_PORT){ PORT_Debug(ps); }
      LCD_WriteAnyValue(f_6x8_p, 2, 0, 70, ps->page_state->page);

      // fps
      LCD_WriteAnyValue(f_4x6_p, 3, 18, 0, (int)ps->frame_counter->fps);
    }

    // handles page changes
    page_state_update(ps, view, controller);

    // controller update
    Controller_Update(controller, ps);

    // view update
    View_Update(view, ps);

    // frame timer wait
    TCF1_FrameTimer_WaitUntilFrameEnded(ps->frame_counter);
  }

  // free stuff
  queue_destroy(port_state.queue_valve_action);
  Settings_Destroy(settings);
  View_Destroy(view);
  Controller_Destroy(controller);
}
