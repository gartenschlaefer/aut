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
    LCD_Sym_Auto_Main(ps); 
    LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);

    // get previous state and time
    if(ps->auto_save_page_state->page != NoPage)
    { 
      // get back state and time
      ps->page_state->page = ps->auto_save_page_state->page; 
      ps->page_state->page_time->min = ps->auto_save_page_state->page_time->min; 
      ps->page_state->page_time->sec = ps->auto_save_page_state->page_time->sec;
    }
    // start page and time
    else
    { 
      ps->page_state->page = AUTO_START_PAGE;
      LCD_Auto_SetStateTime(ps);
    }

    // new state symbols
    LCD_Sym_Auto_SetManager(ps);

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
  ps->auto_save_page_state->page = ps->page_state->page;
  ps->auto_save_page_state->page_time->min = ps->page_state->page_time->min;
  ps->auto_save_page_state->page_time->sec = ps->page_state->page_time->sec;

  // error detection
  Error_Detection(ps);

  // countdown end handling
  if((Basic_CountDown(ps)) && (ps->page_state->page != ErrorTreat)){ LCD_Auto_CountDownEndAction(ps); }

  // touch linker
  Touch_Auto_Linker(ps);

  // sonic
  Sonic_ReadTank(ps);

  // change page through countdown, touch, or sonic
  if(ps->auto_save_page_state->page != ps->page_state->page)
  { 
    // close old page
    LCD_Auto_ResetAutoSavePageState(ps);

    // set new page
    LCD_Sym_Auto_SetManager(ps);
    LCD_Auto_SetStateTime(ps);
    LCD_Auto_SetStateOutput(ps);
  }

  // updates
  PORT_Backlight_Update(ps->backlight);
  LCD_AirState_Update(ps);
  LCD_Auto_InflowPump_Update(ps);
  LCD_Auto_Phosphor_Update(ps);
  MPX_ReadAverage_Update(ps);

  // display update
  LCD_DisplayRefresh(ps);
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
      *p_min = MEM_EEPROM_ReadVar(TIME_setDown);
      *p_sec = 0;
       break;

    case AutoPumpOff:
      *p_min = MEM_EEPROM_ReadVar(ON_pumpOff);
      *p_sec = 0;
      break;

    case AutoMud:
      *p_min = MEM_EEPROM_ReadVar(ON_MIN_mud);
      *p_sec = MEM_EEPROM_ReadVar(ON_SEC_mud);
      break;

    case AutoCircOn: 
    case AutoCircOff:
      *p_min = (( MEM_EEPROM_ReadVar(TIME_H_circ) << 8) | MEM_EEPROM_ReadVar(TIME_L_circ));
      *p_sec = 0;
      // circulate start time
      ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_circ);
      ps->air_circ_state->air_tms->sec = 0;
      break;

    case AutoAirOn: 
    case AutoAirOff:
      *p_min = (( MEM_EEPROM_ReadVar(TIME_H_air) << 8) | MEM_EEPROM_ReadVar(TIME_L_air));
      *p_sec = 0;
      // air start time
      ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_air);
      ps->air_circ_state->air_tms->sec = 0;
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
    // init auto page
    case AutoPage: break;

    // other auto pages
    case AutoZone: OUT_Set_Air(ps); break;
    case AutoSetDown: OUT_SetDown(); break;
    case AutoPumpOff: OUT_Set_PumpOff(ps); break;
    case AutoMud: if((ps->page_state->page_time->min == 0) && (ps->page_state->page_time->sec == 0)){ ps->page_state->page_time->sec = 1; break; }
      OUT_Set_Mud(ps);
      break;

    case AutoCircOn: 
    case AutoAirOn: 
      OUT_Set_Air(ps);
      break;

    case AutoCircOff:
    case AutoAirOff:
      // inflow pump
      if(ps->inflow_pump_state->ip_state == _ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Set_InflowPump(ps); }
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
    case AutoSetDown: break;
    case AutoPumpOff: OUT_Clr_PumpOff(ps); break;
    case AutoMud: OUT_Clr_Mud(ps); break;
    case AutoCircOn: OUT_Clr_Air(ps); break;
    case AutoAirOn: OUT_Clr_Air(ps); break;

    case AutoCircOff: 
    case AutoAirOff:
      if(ps->inflow_pump_state->ip_state == _ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Clr_InflowPump(ps); } 
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
    case AutoZone: 
      MPX_ReadTank(ps); 
      ps->page_state->page = AutoCircOn; 
      break;

    case AutoSetDown: ps->page_state->page = AutoPumpOff; break;
    case AutoPumpOff: ps->page_state->page = AutoMud; break;
    
    case AutoMud:

      // oxygen entry, todo: check this
      MEM_EEPROM_WriteAutoEntry(ps);
      ps->compressor_state->cycle_o2_min = 0;

      // calibration for pressure sensing
      if(MEM_EEPROM_ReadVar(CAL_Redo_on) && !(MEM_EEPROM_ReadVar(SONIC_on))){ ps->page_state->page = AutoZone; }
      else{ ps->page_state->page = AutoCircOn; }
      break;

    case AutoCircOn:
    case AutoCircOff: ps->page_state->page = AutoAirOn; break;
    case AutoAirOn:
    case AutoAirOff: ps->page_state->page = AutoSetDown; break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            auto air manager
 * ------------------------------------------------------------------*/

void LCD_AirState_Update(struct PlantState *ps)
{
  // only on update
  if(ps->time_state->tic_sec_update_flag)
  {
    // handlers
    struct Tms *air_tms = ps->air_circ_state->air_tms;
    t_page p = ps->page_state->page;

    // no update
    if((p != AutoAirOn) && (p != AutoAirOff) && (p != AutoCircOn) && (p != AutoCircOff)){ return; }

    // count down
    if(!air_tms->sec && (p != ErrorTreat))
    {
      air_tms->sec = 60;
      if(air_tms->min){ air_tms->min--; }
    }
    if(air_tms->sec){ air_tms->sec--; }

    // change air state
    if(!air_tms->min && !air_tms->sec)
    {
      switch(p)
      {
        // auto air on
        case AutoAirOn:
        case AutoCircOn:
          t_page cPage = p;
          MPX_ReadTank(ps);
          LCD_Sym_MPX_LevelPerc(ps);
          OUT_Clr_Air(ps);

          // change page
          if(cPage != p)
          { 
            // inflow pump reset
            if(ps->inflow_pump_state->ip_state == _ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Clr_InflowPump(ps); }
          }

          // set ps
          if(p == AutoAirOn){ ps->page_state->page = AutoAirOff; }
          else if(p == AutoCircOn){ ps->page_state->page = AutoCircOff; }
          LCD_Sym_Auto_AirPageSelect(p);

          // inflow pump
          if(ps->inflow_pump_state->ip_state == _ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Set_InflowPump(ps); }

          LCD_AirState_SetAutoStartTime(ps);
          break;

        // auto air off
        case AutoAirOff:
        case AutoCircOff:

          // inflow pump reset
          if(ps->inflow_pump_state->ip_state == _ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Clr_InflowPump(ps); }

          // set air on
          OUT_Set_Air(ps);

          // set pages
          if(p == AutoAirOff){ ps->page_state->page = AutoAirOn; }
          else if(p == AutoCircOff){ ps->page_state->page = AutoCircOn; }
          LCD_Sym_Auto_AirPageSelect(p);
          LCD_AirState_SetAutoStartTime(ps);
          break;

        // manual air on
        case ManualCircOn:
          ps->page_state->page = ManualCircOff;
          MPX_ReadTank(ps);
          LCD_Sym_MPX_LevelPerc(ps);
          OUT_Clr_Air(ps);
          air_tms->min = MEM_EEPROM_ReadVar(ON_circ);
          air_tms->sec = 0;
          break;

        // manual air off
        case ManualCircOff:
          if(p == ManualCircOff) ps->page_state->page = ManualCircOn;
          OUT_Set_Air(ps);
          air_tms->min = MEM_EEPROM_ReadVar(OFF_circ);
          air_tms->sec = 0;
          break;

        default: break;
      }
    }

    // manual return page
    if(p == ManualCircOn || p == ManualCircOff){ return; }

    // auto variables
    LCD_Sym_Auto_AirTime_Print(air_tms);

    // o2 counting
    if((p == AutoCircOn) || (p == AutoAirOn) || (p == ErrorTreat))
    {    
      // o2 min counting
      if(ps->compressor_state->old_min != air_tms->min)
      {
        ps->compressor_state->old_min = air_tms->min;
        ps->compressor_state->cycle_o2_min++;
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            StartTime
 * ------------------------------------------------------------------*/

void LCD_AirState_SetAutoStartTime(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    case AutoCircOn: ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_circ); break;
    case AutoCircOff: ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(OFF_circ); break;
    case AutoAirOn: ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_air); break;
    case AutoAirOff: ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(OFF_air); break;
    default: break;
  }
  ps->air_circ_state->air_tms->sec = 0;
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
    ps->inflow_pump_state->ip_thms->min = MEM_EEPROM_ReadVar(ON_inflowPump);

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

  // AutoChange2Off
  if(ps->inflow_pump_state->ip_state == _ip_on && ps->page_state->page != ErrorTreat && !ps->inflow_pump_state->ip_thms->min && !ps->inflow_pump_state->ip_thms->sec)
  {
    ps->inflow_pump_state->ip_state = _ip_off;
    ps->inflow_pump_state->ip_thms->hou = MEM_EEPROM_ReadVar(T_IP_off_h);
    ps->inflow_pump_state->ip_thms->min = MEM_EEPROM_ReadVar(OFF_inflowPump); 
    ps->inflow_pump_state->ip_thms->sec = 2;
    LCD_Sym_Auto_Ip_Base(ps);
    OUT_Clr_InflowPump(ps);
  }

  // auto change to on state
  else if((ps->inflow_pump_state->ip_state == _ip_off) && (ps->page_state->page != ErrorTreat) && !ps->inflow_pump_state->ip_thms->hou && !ps->inflow_pump_state->ip_thms->min && !ps->inflow_pump_state->ip_thms->sec && (ps->page_state->page == AutoAirOff || ps->page_state->page == AutoCircOff))
  {
    ps->inflow_pump_state->ip_state = _ip_on;
    ps->inflow_pump_state->ip_thms->hou = 0;
    ps->inflow_pump_state->ip_thms->min = MEM_EEPROM_ReadVar(ON_inflowPump);
    ps->inflow_pump_state->ip_thms->sec = 2;
    LCD_Sym_Auto_Ip_Base(ps);
    OUT_Set_InflowPump(ps);
  }

  // countdown each second
  if(ps->time_state->tic_sec_update_flag)
  {
    // only in off states
    if(ps->inflow_pump_state->ip_state == _ip_off || ((ps->page_state->page == AutoAirOff || ps->page_state->page == AutoCircOff) && ps->inflow_pump_state->ip_state == _ip_on))
    {
      // minute update
      if(!ps->inflow_pump_state->ip_thms->sec)
      {
        if(ps->inflow_pump_state->ip_thms->min || ps->inflow_pump_state->ip_thms->hou){ ps->inflow_pump_state->ip_thms->sec = 60; }
        if((!ps->inflow_pump_state->ip_thms->min) && ps->inflow_pump_state->ip_thms->hou)
        {
          ps->inflow_pump_state->ip_thms->min = 60;
          // decrease h
          ps->inflow_pump_state->ip_thms->hou--;
          LCD_Sym_Auto_Ip_Time(0x06, ps->inflow_pump_state->ip_thms);
        }
        // decrease min
        if(ps->inflow_pump_state->ip_thms->min){ ps->inflow_pump_state->ip_thms->min--; }
        LCD_Sym_Auto_Ip_Time(0x02, ps->inflow_pump_state->ip_thms);
      }
      // decrease sec
      if(ps->inflow_pump_state->ip_thms->sec){ ps->inflow_pump_state->ip_thms->sec--; }
      LCD_Sym_Auto_Ip_Time(0x01, ps->inflow_pump_state->ip_thms);
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
    ps->phosphor_state->ph_tms->min = MEM_EEPROM_ReadVar(ON_phosphor);
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
        ps->phosphor_state->ph_tms->min = MEM_EEPROM_ReadVar(OFF_phosphor);
        ps->phosphor_state->ph_tms->sec = 0;
        LCD_Sym_Auto_Ph(ps);
        OUT_Clr_Phosphor();
      }

      // change to on state
      else if(ps->phosphor_state->ph_state == _ph_off)
      {
        ps->phosphor_state->ph_state = _ph_on;
        ps->phosphor_state->ph_tms->min = MEM_EEPROM_ReadVar(ON_phosphor);
        ps->phosphor_state->ph_tms->sec = 0;
        LCD_Sym_Auto_Ph(ps);
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
    case ManualPage: ps->page_state->page = ManualMain; break;
    case ManualCircOn: case ManualCircOff: LCD_AirState_Update(ps); break;

    // blinking pump off text
    case ManualPumpOff:
      if(ps->frame_counter->frame == 29){ LCD_Sym_Manual_PumpOff_OkButton(true); }
      if(ps->frame_counter->frame == 59){ LCD_Sym_Manual_PumpOff_OkButton(false); }
      break;

    default: break;
  }

  // count down
  if(Basic_CountDown(ps)){ MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = AutoPage; }

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
  PORT_Backlight_Update(ps->backlight);
  if(ps->page_state->page != ManualPumpOff)
  {
    LCD_Sym_Manual_PageTime_Update(ps);
    MPX_ReadAverage_Update(ps);
    Sonic_ReadTank(ps);
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
      LCD_Sym_Manual_Main(ps);

      // save manual entry time
      ps->eeprom_state->time_manual_entry.hou = ps->time_state->tic_hou;
      ps->eeprom_state->time_manual_entry.min = ps->time_state->tic_min;
      OUT_Valve_Init(ps);
      break;

    case ManualCircOn:
      ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_circ);
      ps->air_circ_state->air_tms->sec = 0;
      OUT_Set_Air(ps); *p_min = 60; *p_sec = 0;
      break;

    case ManualAir:
      ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_air);
      ps->air_circ_state->air_tms->sec = 0;
     OUT_Set_Air(ps); *p_min = 60; *p_sec = 0;
     break;

    case ManualSetDown: *p_min = 60; *p_sec = 0; break;

    case ManualPumpOff:
      LCD_Sym_Manual_PumpOff_OkButton(true);
      *p_min = 30;
      *p_sec = 0;
      LCD_Sym_Manual_PageTime_Print(ps);
      break;

    case ManualPumpOff_On:
      OUT_Set_PumpOff(ps);
      LCD_Sym_Manual_PumpOff_OkButton_Clr();
      *p_min = 29;
      *p_sec = 59;
      LCD_Sym_Manual_Text(ps);
      break;

    case ManualMud: OUT_Set_Mud(ps); *p_min = 5; *p_sec = 0; break;
    case ManualCompressor: OUT_Set_Compressor(); *p_min = 0; *p_sec = 10; break;
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
    case ManualSetDown: break;
    case ManualCircOn: OUT_Clr_Air(ps); break;
    case ManualAir: OUT_Clr_Air(ps); break;
    case ManualPumpOff: break;
    case ManualPumpOff_On: OUT_Clr_PumpOff(ps); break;
    case ManualMud: OUT_Clr_Mud(ps); break;
    case ManualCompressor: OUT_Clr_Compressor(); break;
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
  if(ps->page_state->page == SetupPage){ ps->page_state->page = SetupMain; }

  // check if page changed
  if(save_page != ps->page_state->page) 
  {
    // reset time
    ps->page_state->page_time->min = 5;
    ps->page_state->page_time->sec = 0;

    // set new symbols
    LCD_Setup_Symbols(ps);

    // special ones in change
    if(ps->page_state->page == SetupCalPressure){ ps->page_state->page_time->min = 4; ps->page_state->page_time->sec = 120; OUT_Set_Air(ps); }
  }

  // special cases
  switch(ps->page_state->page)
  {
    case SetupCalPressure:

      // sonic
      if(MEM_EEPROM_ReadVar(SONIC_on))
      {
        Sonic_LevelCal(ps);
        LCD_Sym_Setup_Cal_Level_Sonic(ps->sonic_state->level_cal);
        LCD_Sym_Setup_Cal_Level_Sym(false);
        ps->page_state->page = SetupCal;
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
            LCD_Sym_Setup_Cal_Level_MPX(ps->sonic_state->level_cal);
            OUT_Clr_Air(ps);

            // clear countdown
            LCD_Sym_Setup_Cal_Clr_MPXCountDown();
            LCD_Sym_Setup_Cal_Level_Sym(false);
            ps->page_state->page = SetupCal; 
          }
        }
      }
      break;

    case SetupAlarm: MCP9800_WriteTemp(ps->twi_state); break;
    default: break;
  }

  // countdown
  if(Basic_CountDown(ps))
  { 
    if(ps->page_state->page == SetupCalPressure){ OUT_Valve_Action(ps, CLOSE_All); }
    ps->page_state->page = AutoPage;
  }
}


/* ------------------------------------------------------------------*
 *            setup symbols
 * ------------------------------------------------------------------*/

void LCD_Setup_Symbols(struct PlantState *ps)
{
  // set symbols
  switch(ps->page_state->page)
  {
    case SetupMain: LCD_Sym_Setup_Page(); break;
    case SetupCirculate: LCD_Sym_Setup_Circulate(); break;
    case SetupAir: LCD_Sym_Setup_Air(); break;
    case SetupSetDown: LCD_Sym_Setup_SetDown(); break;
    case SetupPumpOff: LCD_Sym_Setup_PumpOff(); break;
    case SetupMud: LCD_Sym_Setup_Mud(); break;
    case SetupCompressor: LCD_Sym_Setup_Compressor(); break;
    case SetupPhosphor: LCD_Sym_Setup_Phosphor(); break;
    case SetupInflowPump: LCD_Sym_Setup_InflowPump(); break;
    case SetupCal: LCD_Sym_Setup_Cal(ps); break;
    case SetupAlarm: LCD_Sym_Setup_Alarm(ps); break;
    case SetupWatch: LCD_Sym_Setup_Watch(); break;
    case SetupZone: LCD_Sym_Setup_Zone(); break;
    default: break;
  }
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
    case DataPage: ps->page_state->page = DataMain; break;
    case DataSonicAuto: Sonic_Data_Auto(ps); break;
    case DataSonicBootR: Sonic_Data_BootRead(ps); ps->page_state->page = DataSonic; break;
    case DataSonicBootW: Sonic_Data_BootWrite(ps); ps->page_state->page = DataSonic; break;
    default: break;
  }

  // page change
  if(save_page != ps->page_state->page)
  { 
    // reset time
    ps->page_state->page_time->min = 5;
    ps->page_state->page_time->sec = 0;
    //if(ps->page_state->page != DataSonic && ps->page_state->page != DataSonicAuto && ps->page_state->page != DataSonicBootR && ps->page_state->page != DataSonicBootW){ LCD_Data_Symbols(ps); }
    if(ps->page_state->page != DataSonicAuto && ps->page_state->page != DataSonicBootR && ps->page_state->page != DataSonicBootW){ LCD_Data_Symbols(ps); }
  }

  // watchdog and wait
  BASIC_WDT_RESET;

  // timeout -> auto page
  if(Basic_CountDown(ps)){ ps->page_state->page = AutoPage; }
}


/* ------------------------------------------------------------------*
 *            data set pages
 * ------------------------------------------------------------------*/

void LCD_Data_Symbols(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    case DataMain: LCD_Sym_Data_Page(); break;
    case DataAuto: LCD_Sym_Data_Auto(); break;
    case DataManual: LCD_Sym_Data_Manual(); break;
    case DataSetup: LCD_Sym_Data_Setup(); break;
    case DataSonic: LCD_Sym_Data_Sonic(ps); break;
    default: break;
  }
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
  if (page >= DATA_PAGE_NUM_AUTO)
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
  if (page >= DATA_PAGE_NUM_MANUAL)
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
  if (page >= DATA_PAGE_NUM_MANUAL)
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
  if (DEBUG){ LCD_WriteAnyValue(f_6x8_p, 2, 0, 0, eep); }

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
    if (i >= 4 && half){ return; }
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
  if(Basic_CountDown(ps)){ LCD_Clean(); ps->page_state->page = AutoPage; ps->touch_state->init = false; }
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