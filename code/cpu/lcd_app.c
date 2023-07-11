// --
// EADOGXL160-7 Display applications

#include <avr/io.h>

#include "lcd_app.h"

#include "lcd_driver.h"
#include "config.h"
#include "lcd_sym.h"
#include "touch_app.h"
#include "touch_driver.h"
#include "memory_app.h"
#include "mpx_driver.h"
#include "mcp9800_driver.h"
#include "mcp7941_driver.h"
#include "error_func.h"
#include "tc_func.h"
#include "sonic_app.h"
#include "basic_func.h"
#include "output_app.h"
#include "port_func.h"
#include "compressor_info.h"
#include "page_state.h"


/*-------------------------------------------------------------------*
 *            display refresh
 * ------------------------------------------------------------------*/

void LCD_DisplayRefresh(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    case AutoPage:

      // lcd_reset
      if(ps->frame_counter->lcd_reset == 600){ LCD_Init(); }
      else if(ps->frame_counter->lcd_reset == 1200){ LCD_Sym_MarkTextButton(TEXT_BUTTON_auto); }
      else if(ps->frame_counter->lcd_reset == 1800){ LCD_Sym_Logo(); }
      else if(ps->frame_counter->lcd_reset > 2400){ LCD_Sym_Auto_SetManager(ps); ps->frame_counter->lcd_reset = 0; }
      break;

    default: break;
  }

  // update frame counter
  ps->frame_counter->lcd_reset++;
}


/*-------------------------------------------------------------------*
 *            main auto page
 * ------------------------------------------------------------------*/

void LCD_AutoPage(struct PlantState *ps)
{
  // init page
  if(ps->page_state->page == AutoPage)
  {
    // set auto page
    LCD_Sym_Auto_Main();

    // get previous state and time
    if(ps->auto_save_page_state->page != NonePage)
    { 
      // get back state and time
      page_state_copy(ps->page_state, ps->auto_save_page_state);
    }
    // start page and time
    else
    { 
      page_state_change_page(ps->page_state, AUTO_START_PAGE);
      LCD_Auto_SetStateTime(ps);
      LCD_Auto_SetStateOutput(ps);
    }

    // init
    LCD_Auto_InflowPump_Init(ps);
    LCD_Auto_Phosphor_Init(ps);
    OUT_Valve_Init(ps);
    ps->sonic_state->read_tank_state = SONIC_TANK_timer_init;

    // set state output
    LCD_Auto_SetStateOutput(ps);
  }

  // page time
  LCD_Sym_Auto_PageTime_Update(ps);

  // update save page
  page_state_copy(ps->auto_save_page_state, ps->page_state);

  // countdown end handling
  if((Basic_CountDown(ps)) && !ps->error_state->error_code){ LCD_Auto_CountDownEndAction(ps); }

  // touch linker
  Touch_Auto_Linker(ps);

  // updates
  Error_Detection_Update(ps);
  MPX_Update(ps);
  Sonic_Update(ps);
  LCD_AirState_Update(ps);
  LCD_Auto_InflowPump_Update(ps);
  LCD_Auto_Phosphor_Update(ps);

  // change page through countdown, touch, or sonic
  if(ps->auto_save_page_state->page != ps->page_state->page)
  {
    // close old page
    LCD_Auto_ResetAutoSavePageState(ps);

    // set new page
    LCD_Auto_SetStateTime(ps);
    LCD_Auto_SetStateOutput(ps);
  }

  // display update
  //LCD_DisplayRefresh(ps);
}


/* ------------------------------------------------------------------*
 *            set state time
 * ------------------------------------------------------------------*/

void LCD_Auto_SetStateTime(struct PlantState *ps)
{
  // handlers
  int *p_min = &ps->page_state->page_time->min;
  int *p_sec = &ps->page_state->page_time->sec;

  // set page specific
  switch(ps->page_state->page)
  {
    // auto pages
    case AutoZone:
      *p_min = 2;
      *p_sec = 0; 
      break;

    case AutoSetDown:
      *p_min = ps->settings->settings_set_down->time_min;
      *p_sec = 0;
       break;

    case AutoPumpOff:
      *p_min = ps->settings->settings_pump_off->on_min;
      *p_sec = 0;
      break;

    case AutoMud:
      *p_min = ps->settings->settings_mud->on_min;
      *p_sec = ps->settings->settings_mud->on_sec;
      break;

    case AutoCirc: 
      *p_min = ps->settings->settings_circulate->time_min;
      *p_sec = 0;
      // circulate start time
      ps->air_circ_state->air_tms->min = ps->settings->settings_circulate->on_min;
      ps->air_circ_state->air_tms->sec = 0;
      ps->air_circ_state->ac_state = _ac_on;
      break;

    case AutoAir: 
      *p_min = ps->settings->settings_air->time_min;
      *p_sec = 0;
      // air start time
      ps->air_circ_state->air_tms->min = ps->settings->settings_air->on_min;
      ps->air_circ_state->air_tms->sec = 0;
      ps->air_circ_state->ac_state = _ac_on;
      break;

    default: break;
  }
}



/* ------------------------------------------------------------------*
 *            set state output
 * ------------------------------------------------------------------*/

void LCD_Auto_SetStateOutput(struct PlantState *ps)
{
  // set page specific
  switch(ps->page_state->page)
  {
    // other auto pages
    case AutoZone: OUT_Set_Air(ps); break;
    case AutoSetDown: OUT_SetDown(ps); break;
    case AutoPumpOff: OUT_Set_PumpOff(ps); break;

    case AutoMud: 
      if(ps->settings->settings_mud->disabled){ ps->page_state->page_time->sec = 1; }
      else{ OUT_Set_Mud(ps); }
      break;

    case AutoCirc: case AutoAir: 
      if(ps->air_circ_state->ac_state == _ac_on){ OUT_Set_Air(ps); }
      else{ if(ps->inflow_pump_state->ip_state == _ip_on){ OUT_Set_InflowPump(ps); } }
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            reset state output
 * ------------------------------------------------------------------*/

void LCD_Auto_ResetAutoSavePageState(struct PlantState *ps)
{
  switch(ps->auto_save_page_state->page)
  {
    case AutoZone: OUT_Clr_Air(ps); break;
    case AutoPumpOff: OUT_Clr_PumpOff(ps); break;
    case AutoMud: if(!ps->settings->settings_mud->disabled){ OUT_Clr_Mud(ps); } break;

    case AutoCirc: case AutoAir: 
      if(ps->air_circ_state->ac_state == _ac_on){ OUT_Clr_Air(ps); }
      else{ if(ps->inflow_pump_state->ip_state == _ip_on){ OUT_Set_InflowPump(ps); } }
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            end state action
 * ------------------------------------------------------------------*/

void LCD_Auto_CountDownEndAction(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    case AutoZone: page_state_change_page(ps->page_state, AutoCirc); break;
    case AutoSetDown: page_state_change_page(ps->page_state, AutoPumpOff); break;
    case AutoPumpOff: page_state_change_page(ps->page_state, AutoMud); break;
    
    case AutoMud:

      // oxygen entry, todo: check this
      MEM_EEPROM_WriteAutoEntry(ps);
      Compressor_Info_Reset_CycleO2(ps);

      // calibration for pressure sensing
      page_state_change_page(ps->page_state, (ps->settings->settings_calibration->redo_on && !ps->settings->settings_zone->sonic_on ? AutoZone : AutoCirc));
      break;

    case AutoCirc: page_state_change_page(ps->page_state, AutoAir); break;
    case AutoAir: page_state_change_page(ps->page_state, AutoSetDown); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            air state init
 * ------------------------------------------------------------------*/

void LCD_AirState_Init(struct PlantState *ps)
{
  ;
}


/* ------------------------------------------------------------------*
 *            auto state update, call in auto and manual
 * ------------------------------------------------------------------*/

void LCD_AirState_Update(struct PlantState *ps)
{
  // only on update
  if(ps->time_state->tic_sec_update_flag)
  {
    // handlers and no update rule
    t_page p = ps->page_state->page;
    if((p != AutoAir) && (p != AutoCirc) && (p != ManualCirc)){ return; }
    struct Tms *air_tms = ps->air_circ_state->air_tms;

    // count down
    if(!air_tms->sec && !ps->error_state->error_code)
    {
      air_tms->sec = 60;
      // min update
      if(air_tms->min){ air_tms->min--; }
    }

    // second update
    if(air_tms->sec){ air_tms->sec--; }

    // change air state
    if(!air_tms->min && !air_tms->sec)
    {
      if(ps->air_circ_state->ac_state == _ac_on)
      {
        OUT_Clr_Air(ps);
        if(ps->inflow_pump_state->ip_state == _ip_on && !(p == ManualCirc)){ LCD_Sym_Auto_InflowPump_Symbols(ps); OUT_Set_InflowPump(ps); }
        ps->air_circ_state->ac_state = _ac_off;
        (p == AutoAir ? LCD_Sym_Auto_AirOff() : LCD_Sym_Auto_CircOff());
        ps->air_circ_state->air_tms->min = (p == AutoAir ? ps->settings->settings_air->off_min : ps->settings->settings_circulate->off_min);
      }
      else
      {
        if(ps->inflow_pump_state->ip_state == _ip_on && !(p == ManualCirc)){ LCD_Sym_Auto_InflowPump_Symbols(ps); OUT_Clr_InflowPump(ps); }
        OUT_Set_Air(ps);
        ps->air_circ_state->ac_state = _ac_on;
        (p == AutoAir ? LCD_Sym_Auto_AirOn() : LCD_Sym_Auto_CircOn());
        ps->air_circ_state->air_tms->min = (p == AutoAir ? ps->settings->settings_air->on_min : ps->settings->settings_circulate->on_min);
      }
    }

    // auto variables
    if(p != ManualCirc){ LCD_Sym_Auto_AirTime_Print(air_tms); }
  }
}


/* ------------------------------------------------------------------*
 *            auto inflow pump init
 * ------------------------------------------------------------------*/

void LCD_Auto_InflowPump_Init(struct PlantState *ps)
{
  if(!ps->inflow_pump_state->init_flag)
  {
    // on time for inflow pump
    ps->inflow_pump_state->ip_thms->hou = 0;
    ps->inflow_pump_state->ip_thms->min = ps->settings->settings_inflow_pump->on_min;

    // disabled mode
    if(!ps->inflow_pump_state->ip_thms->min)
    {
      ps->inflow_pump_state->ip_thms->sec = 0;
      ps->inflow_pump_state->ip_state = _ip_disabled;
    }
    else
    {
      ps->inflow_pump_state->ip_thms->min = 1;
      ps->inflow_pump_state->ip_thms->sec = 2;
      ps->inflow_pump_state->ip_state = _ip_off;
    }
    ps->inflow_pump_state->init_flag = true;
  }
}


/* ------------------------------------------------------------------*
 *            auto inflow pump main
 * ------------------------------------------------------------------*/

void LCD_Auto_InflowPump_Update(struct PlantState *ps)
{
  // disabled
  if(ps->inflow_pump_state->ip_state == _ip_disabled){ return; }

  // countdown each second
  if(ps->time_state->tic_sec_update_flag)
  {
    // only in air or circ times and if not air in those
    if(ps->page_state->page != AutoAir && ps->page_state->page != AutoCirc){ return; }
    else if((ps->air_circ_state->ac_state == _ac_on) && ps->inflow_pump_state->ip_state == _ip_on){ return; }

    // minute update
    unsigned char ip_time_cho = 0;
    if(!ps->inflow_pump_state->ip_thms->sec)
    {
      if(ps->inflow_pump_state->ip_thms->min || ps->inflow_pump_state->ip_thms->hou){ ps->inflow_pump_state->ip_thms->sec = 60; }
      if((!ps->inflow_pump_state->ip_thms->min) && ps->inflow_pump_state->ip_thms->hou)
      {
        // decrease h
        ps->inflow_pump_state->ip_thms->min = 60;
        ps->inflow_pump_state->ip_thms->hou--;
        ip_time_cho |= 0x04;
      }
      // decrease min
      if(ps->inflow_pump_state->ip_thms->min){ ps->inflow_pump_state->ip_thms->min--; ip_time_cho |= 0x02; }
    }
    // decrease sec
    if(ps->inflow_pump_state->ip_thms->sec){ ps->inflow_pump_state->ip_thms->sec--; ip_time_cho |= 0x01; }
    LCD_Sym_Auto_Ip_Time(ip_time_cho, ps->inflow_pump_state->ip_thms);

    // change state
    if(!ps->inflow_pump_state->ip_thms->hou && !ps->inflow_pump_state->ip_thms->min && !ps->inflow_pump_state->ip_thms->sec && !ps->error_state->error_code)
    {
      // change to off
      if(ps->inflow_pump_state->ip_state == _ip_on)
      {
        ps->inflow_pump_state->ip_state = _ip_off;
        ps->inflow_pump_state->ip_thms->hou = ps->settings->settings_inflow_pump->off_hou;
        ps->inflow_pump_state->ip_thms->min = ps->settings->settings_inflow_pump->off_min;
        ps->inflow_pump_state->ip_thms->sec = 2;
        LCD_Sym_Auto_InflowPump_Symbols(ps);
        OUT_Clr_InflowPump(ps);
      }
      // auto change to on state
      else
      {
        ps->inflow_pump_state->ip_state = _ip_on;
        ps->inflow_pump_state->ip_thms->hou = 0;
        ps->inflow_pump_state->ip_thms->min = ps->settings->settings_inflow_pump->on_min;
        ps->inflow_pump_state->ip_thms->sec = 2;
        LCD_Sym_Auto_InflowPump_Symbols(ps);
        OUT_Set_InflowPump(ps);
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            auto phosphor init
 * ------------------------------------------------------------------*/

void LCD_Auto_Phosphor_Init(struct PlantState *ps)
{
  if(!ps->phosphor_state->init_flag)
  {
    ps->phosphor_state->ph_tms->min = ps->settings->settings_phosphor->on_min;
    if(!ps->phosphor_state->ph_tms->min){ ps->phosphor_state->ph_tms->sec = 0; ps->phosphor_state->ph_state = _ph_disabled; return; }
    ps->phosphor_state->ph_tms->min = 1;
    ps->phosphor_state->ph_tms->sec = 5;
    ps->phosphor_state->ph_state = _ph_off;
    ps->phosphor_state->init_flag = true;
  }
}


/* ------------------------------------------------------------------*
 *            auto phosphor
 * ------------------------------------------------------------------*/

void LCD_Auto_Phosphor_Update(struct PlantState *ps)
{
  // disabled
  if(ps->phosphor_state->ph_state == _ph_disabled){ return; }

  // phosphor time update
  if(ps->time_state->tic_sec_update_flag)
  {
    // time update
    if(!ps->phosphor_state->ph_tms->sec)
    {
      ps->phosphor_state->ph_tms->sec = 60;
      if(ps->phosphor_state->ph_tms->min){ ps->phosphor_state->ph_tms->min--; }
      LCD_Sym_Auto_Ph_Time_Min(ps->phosphor_state->ph_tms->min);
    }
    if(ps->phosphor_state->ph_tms->sec){ ps->phosphor_state->ph_tms->sec--; }
    LCD_Sym_Auto_Ph_Time_Sec(ps->phosphor_state->ph_tms->sec);

    // change states
    if(!ps->phosphor_state->ph_tms->min && !ps->phosphor_state->ph_tms->sec)
    {
      // change to off state
      if(ps->phosphor_state->ph_state == _ph_on)
      {
        ps->phosphor_state->ph_state = _ph_off;
        ps->phosphor_state->ph_tms->min = ps->settings->settings_phosphor->off_min;
        ps->phosphor_state->ph_tms->sec = 0;
        LCD_Sym_Auto_Phosphor_Symbols(ps);
        OUT_Clr_Phosphor();
      }

      // change to on state
      else if(ps->phosphor_state->ph_state == _ph_off)
      {
        ps->phosphor_state->ph_state = _ph_on;
        ps->phosphor_state->ph_tms->min = ps->settings->settings_phosphor->on_min;
        ps->phosphor_state->ph_tms->sec = 0;
        LCD_Sym_Auto_Phosphor_Symbols(ps);
        OUT_Set_Phosphor();
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            manual page
 * ------------------------------------------------------------------*/

void LCD_ManualPage(struct PlantState *ps)
{
  // save page to detect changes by touch
  t_page save_page = ps->page_state->page;

  // touch
  Touch_Manual_Linker(ps);

  // specials
  switch(ps->page_state->page)
  {
    case ManualPage: page_state_change_page(ps->page_state, ManualMain); break;
    case ManualCirc: LCD_AirState_Update(ps); break;

    // blinking pump off text
    case ManualPumpOff:
      if(ps->frame_counter->frame == 29){ LCD_Sym_Manual_PumpOff_OkButton(true); }
      else if(ps->frame_counter->frame == 59){ LCD_Sym_Manual_PumpOff_OkButton(false); }
      break;

    default: break;
  }

  // updates
  MPX_Update(ps);
  Sonic_Update(ps);

  // count down
  if(Basic_CountDown(ps)){ MEM_EEPROM_WriteManualEntry(ps); page_state_change_page(ps->page_state, AutoPage); }

  // check if page changed
  if(save_page != ps->page_state->page) 
  {
    // clear old state
    LCD_Manual_ResetState(ps, save_page);

    // set new state
    LCD_Manual_SetState(ps);

    // new page time
    LCD_Sym_Manual_PageTime_Print(ps);
  }

  // backlight
  if(ps->page_state->page != ManualPumpOff)
  {
    LCD_Sym_Manual_PageTime_Update(ps);
  }

  // lcd refresh
  //if(ps->frame_counter->sixty_sec_counter == 30){ LCD_Init(); }
  //if(ps->frame_counter->lcd_reset > 120){ LCD_Sym_MarkTextButton(TEXT_BUTTON_manual); ps->frame_counter->lcd_reset = 0; }
  //ps->frame_counter->lcd_reset++;
}


/* ------------------------------------------------------------------*
 *            manual set page
 * ------------------------------------------------------------------*/

void LCD_Manual_SetState(struct PlantState *ps)
{
  // shortcut variables
  int *p_min = &ps->page_state->page_time->min;
  int *p_sec = &ps->page_state->page_time->sec;

  switch(ps->page_state->page)
  {
    case ManualMain: 
      *p_min = 5; *p_sec = 0; 
      // save manual entry time
      ps->eeprom_state->time_manual_entry.hou = ps->time_state->tic_hou;
      ps->eeprom_state->time_manual_entry.min = ps->time_state->tic_min;
      OUT_Valve_Init(ps);
      break;

    case ManualCirc:
      ps->air_circ_state->air_tms->min = ps->settings->settings_circulate->on_min;
      ps->air_circ_state->air_tms->sec = 0;
      ps->air_circ_state->ac_state = _ac_on;
      OUT_Set_Air(ps); *p_min = 60; *p_sec = 0;
      break;

    case ManualAir:
      ps->air_circ_state->air_tms->min = ps->settings->settings_air->on_min;
      ps->air_circ_state->air_tms->sec = 0;
     OUT_Set_Air(ps); *p_min = 60; *p_sec = 0;
     break;

    case ManualSetDown: *p_min = 60; *p_sec = 0; break;

    case ManualPumpOff:
      *p_min = 30;
      *p_sec = 0;
      break;

    case ManualPumpOff_On:
      OUT_Set_PumpOff(ps);
      *p_min = 29;
      *p_sec = 59;
      break;

    case ManualMud: OUT_Set_Mud(ps); *p_min = 5; *p_sec = 0; break;
    case ManualCompressor: OUT_Set_Compressor(ps); *p_min = 0; *p_sec = 10; break;
    case ManualPhosphor: OUT_Set_Phosphor(); *p_min = 30; *p_sec = 0; break;
    case ManualInflowPump: OUT_Set_InflowPump(ps); *p_min = 5; *p_sec = 0; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            manual reset state
 * ------------------------------------------------------------------*/

void LCD_Manual_ResetState(struct PlantState *ps, t_page save_page)
{
  switch(save_page)
  {
    case ManualCirc: if(ps->air_circ_state->ac_state == _ac_on){ OUT_Clr_Air(ps); } break;
    case ManualAir: OUT_Clr_Air(ps); break;
    case ManualPumpOff_On: OUT_Clr_PumpOff(ps); break;
    case ManualMud: OUT_Clr_Mud(ps); break;
    case ManualCompressor: OUT_Clr_Compressor(ps); break;
    case ManualPhosphor: OUT_Clr_Phosphor(); break;
    case ManualInflowPump: OUT_Clr_InflowPump(ps); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            setup pages
 * ------------------------------------------------------------------*/

void LCD_SetupPage(struct PlantState *ps)
{
  // save page to detect changes by touch
  t_page save_page = ps->page_state->page;

  // touch
  Touch_SelectLinker(ps);

  // special case
  if(ps->page_state->page == SetupPage){ page_state_change_page(ps->page_state, SetupMain); }

  // check if page changed
  if(save_page != ps->page_state->page) 
  {
    // reset time
    ps->page_state->page_time->min = 4;
    ps->page_state->page_time->sec = 60;

    // special ones in change
    if(ps->page_state->page == SetupCalPressure && !ps->settings->settings_zone->sonic_on){ OUT_Set_Air(ps); }
  }

  // page dependent updates
  if((ps->page_state->page == SetupCal) | (ps->page_state->page == SetupCalPressure) )
  {
    // mpx update
    MPX_Update(ps);
    
    if(ps->page_state->page == SetupCalPressure)
    {
      // sonic
      if(ps->settings->settings_zone->sonic_on)
      {
        Sonic_LevelCal(ps);
        LCD_Sym_Setup_Cal_Level_Sonic(ps->settings->settings_calibration->tank_level_min_sonic);
        page_state_change_page(ps->page_state, SetupCal);
      }

      // pressure sensor
      else
      {
        if(ps->time_state->tic_sec_update_flag)
        {
          // write countdown
          LCD_Sym_Setup_Cal_MPXCountDown(ps->page_state->page_time->sec);

          // stop air and read the preassure sensor
          if(!ps->page_state->page_time->sec)
          {
            MPX_LevelCal_New(ps);
            LCD_Sym_Setup_Cal_Level_MPX(ps->settings->settings_calibration->tank_level_min_pressure);
            OUT_Clr_Air(ps);

            // clear countdown
            LCD_Sym_Setup_Cal_Clr_MPXCountDown();
            LCD_Sym_Setup_Cal_Level_Sym(false);
            page_state_change_page(ps->page_state, SetupCal); 
          }
        }
      }
    }
  }
  else if((ps->page_state->page == SetupAlarm)){ MCP9800_WriteTemp(ps->twi_state); }

  // countdown
  if(Basic_CountDown(ps)){ page_state_change_page(ps->page_state, AutoPage); }
}


/* ------------------------------------------------------------------*
 *            data page
 * ------------------------------------------------------------------*/

void LCD_DataPage(struct PlantState *ps)
{
  // save page
  t_page save_page = ps->page_state->page;

  // touch
  Touch_SelectLinker(ps);

  // specials
  switch(ps->page_state->page)
  {
    case DataPage: page_state_change_page(ps->page_state, DataMain); break;
    case DataMain: if(ps->time_state->tic_sec_update_flag){ LCD_Sym_Auto_WorldTime_Print(ps); } break;
    case DataSonicAuto: Sonic_Data_Auto(ps); break;
    case DataSonicBootR: Sonic_Data_BootRead(ps); page_state_change_page(ps->page_state, DataSonic); break;
    case DataSonicBootW: Sonic_Data_BootWrite(ps); page_state_change_page(ps->page_state, DataSonic); break;
    default: break;
  }

  // page change
  if(save_page != ps->page_state->page)
  { 
    // reset time
    ps->page_state->page_time->min = 5;
    ps->page_state->page_time->sec = 0;
  }

  // watchdog and wait
  BASIC_WDT_RESET;

  // timeout -> auto page
  if(Basic_CountDown(ps)){ page_state_change_page(ps->page_state, AutoPage); }
}


/* ------------------------------------------------------------------*
 *            auto entry
 * ------------------------------------------------------------------*/

void LCD_Data_WriteAutoEntryPage(unsigned char page)
{
  struct MemoryEntryPos latest = MEM_FindLatestEntry(TEXT_BUTTON_auto);

  // clear display section
  LCD_Sym_Clr_DataEntrySpace();

  // page num
  LCD_Sym_Data_ActualPageNum((int)(page + 1));

  // get right eep
  unsigned char wep = LCD_Data_EEP_Minus(TEXT_BUTTON_auto, latest.page, (2 * page));

  // write corresponding page
  if(page >= DATA_PAGE_NUM_AUTO)
  {
    // half page
    LCD_Data_wPage(TEXT_BUTTON_auto, wep, latest.entry, true);
    LCD_Sym_Data_EndText();
  }
  else
  {
    // full page
    LCD_Data_wPage(TEXT_BUTTON_auto, wep, latest.entry, false);
  }
}



/* ------------------------------------------------------------------*
 *            data manual entry page
 * ------------------------------------------------------------------*/

void LCD_Data_WriteManualEntryPage(unsigned char page)
{
  struct MemoryEntryPos latest = MEM_FindLatestEntry(TEXT_BUTTON_manual);

  // clear display section
  LCD_Sym_Clr_DataEntrySpace();

  // page number
  LCD_Sym_Data_ActualPageNum((int)(page + 1));

  // get right latest page
  unsigned char wep = LCD_Data_EEP_Minus(TEXT_BUTTON_manual, latest.page, (2 * page));

  // write corresponding page
  if(page >= DATA_PAGE_NUM_MANUAL)
  {
    // half page
    LCD_Data_wPage(TEXT_BUTTON_manual, wep, latest.entry, true);
    LCD_Sym_Data_EndText();
  }
  else
  {
    // full page
    LCD_Data_wPage(TEXT_BUTTON_manual, wep, latest.entry, false);
  }
}



/* ------------------------------------------------------------------*
 *            data setup entry page
 * ------------------------------------------------------------------*/

void LCD_Data_WriteSetupEntryPage(unsigned char page)
{
  struct MemoryEntryPos latest = MEM_FindLatestEntry(TEXT_BUTTON_setup);

  // clear display section
  LCD_Sym_Clr_DataEntrySpace();

  // page number
  LCD_Sym_Data_ActualPageNum((int)(page + 1));

  // get right eep
  unsigned char wep = LCD_Data_EEP_Minus(TEXT_BUTTON_setup, latest.page, (2 * page));

  // write corresponding page
  if(page >= DATA_PAGE_NUM_MANUAL)
  {
    // half page
    LCD_Data_wPage(TEXT_BUTTON_setup, wep, latest.entry, true);
    LCD_Sym_Data_EndText();
  }
  else
  {
    // full page
    LCD_Data_wPage(TEXT_BUTTON_setup, wep, latest.entry, false);
  }
}


/*-------------------------------------------------------------------*
 *            write data entry page
 * ------------------------------------------------------------------*/

void LCD_Data_wPage(t_text_buttons data, unsigned char eep, unsigned char entry, bool half)
{
  //*** debug eep page
  if(DEBUG){ LCD_WriteAnyValue(f_6x8_p, 2, 0, 0, eep); }

  // get start end page
  struct MemoryStartEndPage msep = MEM_GetStartEndPage(data);

  // write the data page to display
  for(unsigned char i = 0; i < 8; i++)
  {
    // write entry
    switch(data)
    {
      case TEXT_BUTTON_auto: LCD_Sym_Data_WriteAutoEntry(5 + (2 * i), eep, entry); break;
      case TEXT_BUTTON_manual: LCD_Sym_Data_WriteManualEntry(5 + (2 * i), eep, entry); break;
      case TEXT_BUTTON_setup: LCD_Sym_Data_WriteSetupEntry(5 + (2 * i), eep, entry); break;
      default: break;
    }

    // update
    if(entry < 1)
    {
      entry = 4;
      eep--;
      if(eep < msep.start_page){ eep = msep.end_page; }
    }
    entry--;

    // return if only half page needed
    if(i >= 4 && half){ return; }
  }
}


/* ------------------------------------------------------------------*
 *            mem pages eep minus
 * ------------------------------------------------------------------*/

unsigned char LCD_Data_EEP_Minus(t_text_buttons data, unsigned char eep, unsigned char cnt)
{
  // get start end page
  struct MemoryStartEndPage msep = MEM_GetStartEndPage(data);

  // get right EEPROM page
  for(unsigned char i = 0; i < cnt; i++)
  {
    eep--;
    if(eep < msep.start_page){ eep = msep.end_page; }
  }

  return eep;
}


/* ------------------------------------------------------------------*
 *            pin page init
 * ------------------------------------------------------------------*/

void LCD_PinPage_Init(struct PlantState *ps)
{
  LCD_Sym_PinPage(); 
  ps->page_state->page_time->min = 5; 
  ps->page_state->page_time->sec = 0;
}


/* ------------------------------------------------------------------*
 *            pin page main
 * ------------------------------------------------------------------*/

void LCD_PinPage_Main(struct PlantState *ps)
{
  // save page
  t_page save_page = ps->page_state->page;

  // wdt and break
  BASIC_WDT_RESET;

  // touch
  Touch_Pin_Linker(ps);

  // touch change -> clean display
  if(save_page != ps->page_state->page){ LCD_Clean(); }

  // end condition on countdown
  if(Basic_CountDown(ps)){ LCD_Clean(); page_state_change_page(ps->page_state, AutoPage); ps->touch_state->init = false; }
}


/* ------------------------------------------------------------------*
 *            lcd calibration
 * ------------------------------------------------------------------*/

void LCD_Calibration(void)
{
  int xRead = 0;
  int yRead = 0;
  int x = 0;
  int y = 0;

  struct Backlight backlight = { .state = _bl_off, .count = 0 };

  PORT_Backlight_On(&backlight);
  Touch_Cal_Main();
  LCD_Clean();

  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "X-Cal:");
  LCD_WriteAnyStringFont(f_6x8_p, 5, 0, "Y-Cal:");
  LCD_WriteAnyStringFont(f_6x8_p, 10, 0, "X-Value:");
  LCD_WriteAnyStringFont(f_6x8_p, 13, 0, "Y-Value:");
  LCD_WriteAnyStringFont(f_6x8_p, 18, 0, "X-Value-Cal:");
  LCD_WriteAnyStringFont(f_6x8_p, 21, 0, "Y-Value-Cal:");

  LCD_WriteAnyValue(f_6x8_p, 3, 2, 60, MEM_EEPROM_ReadVar(TOUCH_X_min));
  LCD_WriteAnyValue(f_6x8_p, 3, 5, 60, MEM_EEPROM_ReadVar(TOUCH_Y_min));
  LCD_WriteAnyValue(f_6x8_p, 3, 2, 100, MEM_EEPROM_ReadVar(TOUCH_X_max));
  LCD_WriteAnyValue(f_6x8_p, 3, 5, 100, MEM_EEPROM_ReadVar(TOUCH_Y_max));

  while(1)
  {
    BASIC_WDT_RESET;

    xRead = (Touch_Cal_X_ReadData() >> 4);
    yRead = (Touch_Cal_Y_ReadData() >> 4);
    y = Touch_Cal_Y_Value(yRead);
    x = Touch_Cal_X_Value(xRead);

    LCD_WriteAnyValue(f_6x8_p, 3, 10, 100, xRead);
    LCD_WriteAnyValue(f_6x8_p, 3, 13, 100, yRead);
    LCD_WriteAnyValue(f_6x8_p, 3, 18, 100, x);
    LCD_WriteAnyValue(f_6x8_p, 3, 21, 100, y);
  }
}