// --
// EADOGXL160-7 Display applications

#include <avr/io.h>

#include "lcd_app.h"

#include "lcd_driver.h"
#include "config.h"
#include "lcd_sym.h"
#include "touch_app.h"
#include "touch_driver.h"
#include "eval_app.h"
#include "memory_app.h"
#include "mpx_driver.h"
#include "mcp9800_driver.h"
#include "mcp7941_driver.h"
#include "error_func.h"
#include "tc_func.h"
#include "sonic_app.h"
#include "basic_func.h"
#include "output_app.h"


/*-------------------------------------------------------------------*
 *  display refresh
 * ------------------------------------------------------------------*/

void LCD_DisplayRefresh(t_page main_page, struct PlantState *ps)
{
  switch(main_page)
  {
    case AutoPage:

      // lcd_reset
      if(ps->frame_counter->lcd_reset == 600){ LCD_Init(); }
      else if(ps->frame_counter->lcd_reset == 1200){ LCD_Sym_MarkTextButton(Auto); }
      else if(ps->frame_counter->lcd_reset == 1800){ LCD_Sym_Logo(); }
      else if(ps->frame_counter->lcd_reset > 2400){ LCD_Sym_Auto_SetManager(ps); ps->frame_counter->lcd_reset = 0; }
      break;

    default: break;
  }

  // update frame counter
  ps->frame_counter->lcd_reset++;
}


/*-------------------------------------------------------------------*
 *  main automatic page
 * ------------------------------------------------------------------*/

void LCD_AutoPage(struct PlantState *ps)
{
  // specials and symbols
  switch(ps->page_state->page)
  {
    // entry page
    case AutoPage:

      // set auto page
      LCD_Sym_Auto_Main(); 
      LCD_Sym_Auto_CountDown(ps->page_state->page_time);

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

      // init procedure
      if(!ps->init || (!ps->page_state->page_time->min && !(MEM_EEPROM_ReadVar(SENSOR_inTank))))
      {
        LCD_AirState_SetAutoStartTime(ps);
        LCD_Auto_InflowPump_Init(ps);
        LCD_Auto_Phosphor_Init(ps);
        OUT_Valve_Init();
        ps->init = 1;

        // read tank init timer
        ps->sonic_state->read_tank_state = SONIC_TANK_timer_init;
      }

      // set state output
      LCD_Auto_SetStateOutput(ps);
      break;

    // air states
    case AutoAirOn: case AutoAirOff: case AutoCircOn: case AutoCircOff: LCD_AirState_Manager(ps); break;

    // other
    default: LCD_Sym_Auto_PageTime(ps, ps->page_state->page_time); break;
  }

  // update save page
  ps->auto_save_page_state->page = ps->page_state->page; 
  ps->auto_save_page_state->page_time->min = ps->page_state->page_time->min; 
  ps->auto_save_page_state->page_time->sec = ps->page_state->page_time->sec; 

  // error detection
  Error_Detection(ps);

  // countdown end handling
  if((Eval_CountDown(ps->page_state->page_time)) && (ps->page_state->page != ErrorTreat)){ LCD_Auto_CountDownEndAction(ps); }

  // touch linker
  Touch_AutoLinker(ps);

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

  // execute in all auto pages
  LCD_Backlight(_exe, ps->lcd_backlight);
  LCD_Auto_InflowPump_Main(ps);
  LCD_Auto_Phosphor(ps);
  MPX_ReadAverage(ps, _exe);

  // display update
  LCD_DisplayRefresh(AutoPage, ps);
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
    case AutoZone: OUT_Set_Air(); break;
    case AutoSetDown: OUT_SetDown(); break;
    case AutoPumpOff: OUT_Set_PumpOff(); break;
    case AutoMud: if((ps->page_state->page_time->min == 0) && (ps->page_state->page_time->sec == 0)){ ps->page_state->page_time->sec = 1; break; }
      OUT_Set_Mud();
      break;

    case AutoCircOn: 
    case AutoAirOn: 
      OUT_Set_Air();
      break;

    case AutoCircOff:
    case AutoAirOff:
      // inflow pump
      if(ps->inflow_pump_state->ip_state == ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Set_InflowPump(ps); }
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
    case AutoZone: OUT_Clr_Air(); break;
    case AutoSetDown: break;
    case AutoPumpOff: OUT_Clr_PumpOff(); break;
    case AutoMud: OUT_Clr_Mud(); break;
    case AutoCircOn: OUT_Clr_Air(); break;
    case AutoAirOn: OUT_Clr_Air(); break;

    case AutoCircOff: 
    case AutoAirOff:
      if(ps->inflow_pump_state->ip_state == ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Clr_InflowPump(); } 
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
      MPX_ReadTank(ps, _exe); 
      ps->page_state->page = AutoCircOn; 
      break;

    case AutoSetDown: ps->page_state->page = AutoPumpOff; break;
    case AutoPumpOff: ps->page_state->page = AutoMud; break;
    
    case AutoMud:

      // oxygen entry, todo: check this
      Eval_Oxygen(_entry, 0);
      MEM_EEPROM_WriteAutoEntry(0, 0, Write_Entry);
      Eval_Oxygen(_clear, 0);

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
 *            Auto Air
 * ------------------------------------------------------------------*/

void LCD_AirState_Manager(struct PlantState *ps)
{
  // handlers
  struct Tms *air_tms = ps->air_circ_state->air_tms;
  t_page p = ps->page_state->page;

  // change air state
  if(!air_tms->min && !air_tms->sec)
  {
    switch(p)
    {
      // auto air on
      case AutoAirOn:
      case AutoCircOn:
        t_page cPage = p;
        MPX_ReadTank(ps, _exe);
        MPX_ReadTank(ps, _write);
        OUT_Clr_Air();

        // change page
        if(cPage != p)
        { 
          // inflow pump reset
          if(ps->inflow_pump_state->ip_state == ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Clr_InflowPump(); }
        }

        // set ps
        if(p == AutoAirOn){ ps->page_state->page = AutoAirOff; }
        else if(p == AutoCircOn){ ps->page_state->page = AutoCircOff; }
        LCD_Sym_AutoAirOn(p);

        // inflow pump
        if(ps->inflow_pump_state->ip_state == ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Set_InflowPump(ps); }

        LCD_AirState_SetAutoStartTime(ps);
        break;

      // auto air off
      case AutoAirOff:
      case AutoCircOff:

        // inflow pump reset
        if(ps->inflow_pump_state->ip_state == ip_on){ LCD_Sym_Auto_Ip_Base(ps); OUT_Clr_InflowPump(); }

        // set air on
        OUT_Set_Air();

        // set pages
        if(p == AutoAirOff){ ps->page_state->page = AutoAirOn; }
        else if(p == AutoCircOff){ ps->page_state->page = AutoCircOn; }
        LCD_Sym_AutoAirOn(p);
        LCD_AirState_SetAutoStartTime(ps);
        break;

      // manual air on
      case ManualCircOn:
        if(p == ManualCircOn){ ps->page_state->page = ManualCircOff; }
        MPX_ReadTank(ps, _exe);
        MPX_ReadTank(ps, _write);
        OUT_Clr_Air();
        air_tms->min = MEM_EEPROM_ReadVar(ON_circ);
        air_tms->sec = 0;
        break;

      // manual air off
      case ManualCircOff:
        if(p == ManualCircOff) ps->page_state->page = ManualCircOn;
        OUT_Set_Air();
        air_tms->min = MEM_EEPROM_ReadVar(OFF_circ);
        air_tms->sec = 0;
        break;

      default: break;
    }
  }

  // countdown
  if(ps->page_state->page_time->sec != ps->air_circ_state->old_sec)
  {
    ps->air_circ_state->old_sec = ps->page_state->page_time->sec;
    if(!air_tms->sec && (p != ErrorTreat))
    {
      air_tms->sec = 60;
      if(air_tms->min){ air_tms->min--; }
    }
    if(air_tms->sec){ air_tms->sec--; }
  }

  // manual return page
  if(p == ManualCircOn || p == ManualCircOff){ return; }

  // auto variables
  if((p == AutoCircOn) || (p == AutoAirOn) || (p == ErrorTreat))
  {
    LCD_Sym_Auto_PageTime(ps, air_tms);
    Eval_Oxygen(_count, air_tms->min);
  }
  else{ LCD_Sym_Auto_PageTime(ps, air_tms); }
}


/* ------------------------------------------------------------------*
 *            Auto Inflow Pump
 * ------------------------------------------------------------------*/

void LCD_Auto_InflowPump_Init(struct PlantState *ps)
{
  // handlers
  struct Thms *ip_thms = ps->inflow_pump_state->ip_thms;
  t_inflow_pump_states *ip_state = &ps->inflow_pump_state->ip_state;

  // on time for inflow pump
  ip_thms->hou = 0;
  ip_thms->min = MEM_EEPROM_ReadVar(ON_inflowPump);

  // disabled mode
  if(!ip_thms->min)
  {
    ip_thms->sec = 0;
    *ip_state = _disabled;
  }
  else
  {
    ip_thms->min = 1;
    ip_thms->sec = 2;
    *ip_state = _off;
  }
}

void LCD_Auto_InflowPump_Main(struct PlantState *ps)
{
  // disabled
  if(ps->inflow_pump_state->ip_state == ip_disabled){ return; }

  // handlers
  int *ip_count = &ps->inflow_pump_state->ip_count;
  struct Thms *ip_thms = ps->inflow_pump_state->ip_thms;
  t_inflow_pump_states *ip_state = &ps->inflow_pump_state->ip_state;
  t_page p = ps->page_state->page;

  // AutoChange2Off
  if(*ip_state == ip_on && p != ErrorTreat && !ip_thms->min && !ip_thms->sec)
  {
    *ip_state = _off;
    ip_thms->hou = MEM_EEPROM_ReadVar(T_IP_off_h); ip_thms->min = MEM_EEPROM_ReadVar(OFF_inflowPump); ip_thms->sec = 2;
    LCD_Sym_Auto_Ip_Base(ps);
    OUT_Clr_InflowPump();
  }

  // auto change to on state
  else if( (*ip_state == ip_off) && (p != ErrorTreat) && !ip_thms->hou && !ip_thms->min && !ip_thms->sec && (p == AutoAirOff || p == AutoCircOff))
  {
    *ip_state = _on;
    ip_thms->hou = 0; ip_thms->min = MEM_EEPROM_ReadVar(ON_inflowPump); ip_thms->sec = 2;
    LCD_Sym_Auto_Ip_Base(ps);
    OUT_Set_InflowPump(ps);
  }

  // countdown
  if(*ip_state == ip_off || ((p == AutoAirOff || p == AutoCircOff) && *ip_state == ip_on))
  {
    if(*ip_count != ps->page_state->page_time->sec)
    {
      *ip_count = ps->page_state->page_time->sec;
      if(!ip_thms->sec)
      {
        if(ip_thms->min || ip_thms->hou)
        {
          ip_thms->sec = 60;
        }
        if((!ip_thms->min) && ip_thms->hou)
        {
          ip_thms->min = 60;
          // decrease h
          ip_thms->hou--;
          LCD_Sym_Auto_Ip_Time(0x06, ip_thms);
        }
        // decrease min
        if(ip_thms->min) ip_thms->min--;
        LCD_Sym_Auto_Ip_Time(0x02, ip_thms);
      }
      // decrease sec
      if(ip_thms->sec) ip_thms->sec--;
      LCD_Sym_Auto_Ip_Time(0x01, ip_thms);
    }
  }
}


/* ------------------------------------------------------------------*
 *            Auto Phosphor Init
 * ------------------------------------------------------------------*/

void LCD_Auto_Phosphor_Init(struct PlantState *ps)
{
  ps->phosphor_state->ph_tms->min = MEM_EEPROM_ReadVar(ON_phosphor);
  if(!ps->phosphor_state->ph_tms->min){ ps->phosphor_state->ph_tms->sec = 0; ps->phosphor_state->ph_state = ph_disabled; return; }
  ps->phosphor_state->ph_tms->min = 1;
  ps->phosphor_state->ph_tms->sec = 5;
  ps->phosphor_state->ph_state = ph_off;
}


/* ------------------------------------------------------------------*
 *            Auto Phosphor
 * ------------------------------------------------------------------*/

void LCD_Auto_Phosphor(struct PlantState *ps)
{
  // disabled
  if(ps->phosphor_state->ph_state == ph_disabled){ return; }

  // phosphor count
  if(ps->phosphor_state->ph_count != ps->page_state->page_time->sec)
  {
    ps->phosphor_state->ph_count = ps->page_state->page_time->sec;
    if(!ps->phosphor_state->ph_tms->sec)
    {
      ps->phosphor_state->ph_tms->sec = 60;
      if(ps->phosphor_state->ph_tms->min){ ps->phosphor_state->ph_tms->min--; }
      LCD_WriteAnyValue(f_4x6_p, 2, 13, 135, ps->phosphor_state->ph_tms->min);
    }
    if(ps->phosphor_state->ph_tms->sec){ ps->phosphor_state->ph_tms->sec--; }
    LCD_WriteAnyValue(f_4x6_p, 2, 13, 147, ps->phosphor_state->ph_tms->sec);
  }

  // change to off state
  if(ps->phosphor_state->ph_state == ph_on && !ps->phosphor_state->ph_tms->min && !ps->phosphor_state->ph_tms->sec)
  {
    ps->phosphor_state->ph_state = ph_off;
    ps->phosphor_state->ph_tms->min = MEM_EEPROM_ReadVar(OFF_phosphor); ps->phosphor_state->ph_tms->sec = 0;
    LCD_WriteAnySymbol(s_19x19, 6, 134, p_phosphor);
    OUT_Clr_Phosphor();
  }

  // change to on state
  else if(ps->phosphor_state->ph_state == ph_off && !ps->phosphor_state->ph_tms->min && !ps->phosphor_state->ph_tms->sec)
  {
    ps->phosphor_state->ph_state = ph_on;
    ps->phosphor_state->ph_tms->min = MEM_EEPROM_ReadVar(ON_phosphor); ps->phosphor_state->ph_tms->sec = 0;
    LCD_WriteAnySymbol(s_19x19, 6, 134, n_phosphor);
    OUT_Set_Phosphor();
  }
}


/* ------------------------------------------------------------------*
 *            Manual Pages
 * ------------------------------------------------------------------*/

void LCD_ManualPage(struct PlantState *ps)
{
  // save page to detect changes by touch
  t_page save_page = ps->page_state->page;

  // touch
  Touch_SelectLinker(ps);

  // specials
  switch(ps->page_state->page)
  {
    case ManualPage: ps->page_state->page = ManualMain; break;
    case ManualCircOn: case ManualCircOff: LCD_AirState_Manager(ps); break;

    // blinking pump off text
    case ManualPumpOff:
      if(ps->frame_counter->frame == 29){ LCD_Sym_Manual_PumpOff_PressOk(f_6x8_n); }
      if(ps->frame_counter->frame == 59){ LCD_Sym_Manual_PumpOff_PressOk(f_6x8_p); }
      break;

    default: break;
  }

  // count down
  if(Eval_CountDown(ps->page_state->page_time)){ MEM_EEPROM_WriteManualEntry(0, 0, _write); ps->page_state->page = AutoPage; }

  // check if page changed
  if(save_page != ps->page_state->page) 
  {
    // clear old state
    LCD_Manual_ResetState(save_page);

    // set new state
    LCD_Manual_SetState(ps);
  }

  // backlight
  LCD_Backlight(_exe, ps->lcd_backlight);
  if(ps->page_state->page != ManualPumpOff)
  {
    LCD_Sym_Manual_PageTime(ps->page_state->page_time);
    MPX_ReadAverage(ps, _exe);
    Sonic_ReadTank(ps);
  }

  // lcd refresh
  if(ps->frame_counter->sixty_sec_counter == 30){ LCD_Init(); }
  if(ps->frame_counter->lcd_reset > 120){ LCD_Sym_MarkTextButton(Manual); ps->frame_counter->lcd_reset = 0; }
  //ps->frame_counter->lcd_reset++;
}


/* ------------------------------------------------------------------*
 *            Manual Set Page
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
      if(!ps->init){ OUT_Valve_Init(); }
      break;

    case ManualCircOn:
      ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_circ);
      ps->air_circ_state->air_tms->sec = 0;
      OUT_Set_Air(); *p_min = 60; *p_sec = 0;
      break;

    case ManualAir:
      ps->air_circ_state->air_tms->min = MEM_EEPROM_ReadVar(ON_air);
      ps->air_circ_state->air_tms->sec = 0;
     OUT_Set_Air(); *p_min = 60; *p_sec = 0;
     break;

    case ManualSetDown: *p_min = 60; *p_sec = 0; break;

    case ManualPumpOff:
      LCD_ClrSpace(15, 2, 5, 120);
      LCD_Sym_Manual_PumpOff_PressOk(f_6x8_p);
      LCD_WriteAnySymbol(s_19x19, 15, 85, p_ok);
      *p_min = 30;
      *p_sec = 0;
      LCD_Sym_Manual_CountDown(ps->page_state->page_time);
      break;

    case ManualPumpOff_On:
      OUT_Set_PumpOff();
      LCD_ClrSpace(15, 2, 5, 120);
      *p_min = 29;
      *p_sec = 59;
      LCD_Sym_Manual_Text(ps);
      break;

    case ManualMud: OUT_Set_Mud(); *p_min = 5; *p_sec = 0; break;
    case ManualCompressor: OUT_Set_Compressor(); *p_min = 0; *p_sec = 10; break;
    case ManualPhosphor: OUT_Set_Phosphor(); *p_min = 30; *p_sec = 0; break;
    case ManualInflowPump: OUT_Set_InflowPump(ps); *p_min = 5; *p_sec = 0; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            manual reset state
 * ------------------------------------------------------------------*/

void LCD_Manual_ResetState(t_page save_page)
{
  switch(save_page)
  {
    case ManualSetDown: break;
    case ManualCircOn: OUT_Clr_Air(); break;
    case ManualAir: OUT_Clr_Air(); break;
    case ManualPumpOff: break;
    case ManualPumpOff_On: OUT_Clr_PumpOff(); break;
    case ManualMud: OUT_Clr_Mud(); break;
    case ManualCompressor: OUT_Clr_Compressor(); break;
    case ManualPhosphor: OUT_Clr_Phosphor(); break;
    case ManualInflowPump: OUT_Clr_InflowPump(); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            setup pages
 * ------------------------------------------------------------------*/

void LCD_SetupPage(struct PlantState *ps)
{
  // handlers
  int *p_min = &ps->page_state->page_time->min;
  int *p_sec = &ps->page_state->page_time->sec;

  // save page to detect changes by touch
  t_page save_page = ps->page_state->page;

  // touch
  Touch_SelectLinker(ps);

  // specials
  switch(ps->page_state->page)
  {
    case SetupPage: ps->page_state->page = SetupMain; break;
    default: break;
  }

  // check if page changed
  if(save_page != ps->page_state->page) 
  {
    // set new symbols
    LCD_Setup_Symbols(ps);

    // special ones in change
    if(ps->page_state->page == SetupCalPressure){ *p_min = 4; *p_sec = 120; OUT_Set_Air(); }
  }

  // special cases
  switch(ps->page_state->page)
  {
    case SetupCalPressure:

      // sonic
      if(MEM_EEPROM_ReadVar(SONIC_on))
      {
        Sonic_LevelCal(ps);
        LCD_WriteAnyValue(f_6x8_p, 4, 17, 40, ps->sonic_state->level_cal);
        LCD_WriteAnySymbol(s_29x17, 15, 1, p_level);
        ps->page_state->page = SetupCal;
      }

      // pressure sensor
      else
      {
        // write countdown
        LCD_WriteAnyValue(f_6x8_p, 3, 17, 100, *p_sec);

        // stop air and read the preassure sensor
        if(!*p_sec)
        {
          MPX_LevelCal(ps, _new);
          MPX_LevelCal(ps, _write);
          OUT_Clr_Air();

          // clear countdown
          LCD_ClrSpace(17, 100, 2, 20);
          LCD_WriteAnySymbol(s_29x17, 15, 1, p_level);
          ps->page_state->page = SetupCal; 
        }
      }
      break;

    case SetupAlarm: MCP9800_WriteTemp(); break;
    default: break;
  }

  //else if(page != SetupCal){ LCD_Setup_Symbols(ps); } 

  // countdown 
  if(Eval_CountDown(ps->page_state->page_time))
  { 
    if(ps->page_state->page == SetupCalPressure){ OUT_Valve_CloseAll(); }
    ps->page_state->page = AutoPage;
  }
}


/* ------------------------------------------------------------------*
 *            setup symbols
 * ------------------------------------------------------------------*/

void LCD_Setup_Symbols(struct PlantState *ps)
{
  // reset time
  ps->page_state->page_time->min = 5;
  ps->page_state->page_time->sec = 0;

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
    case SetupAlarm: LCD_Sym_Setup_Alarm(); break;
    case SetupWatch: LCD_Sym_Setup_Watch(); break;
    case SetupZone: LCD_Sym_Setup_Zone(); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Data Page
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
    //if(ps->page_state->page != DataSonic && ps->page_state->page != DataSonicAuto && ps->page_state->page != DataSonicBootR && ps->page_state->page != DataSonicBootW){ LCD_Data_Symbols(ps); }
    if(ps->page_state->page != DataSonicAuto && ps->page_state->page != DataSonicBootR && ps->page_state->page != DataSonicBootW){ LCD_Data_Symbols(ps); }
  }

  // watchdog and wait
  BASIC_WDT_RESET;

  // timeout -> auto page
  if(Eval_CountDown(ps->page_state->page_time)){ ps->page_state->page = AutoPage; }
}


/* ------------------------------------------------------------------*
 *            Data Set Pages
 * ------------------------------------------------------------------*/

void LCD_Data_Symbols(struct PlantState *ps)
{
  ps->page_state->page_time->min = 5;
  ps->page_state->page_time->sec = 0;

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
 *            Pin Page
 * ------------------------------------------------------------------*/

void LCD_PinPage_Init(struct PlantState *ps)
{
  LCD_Sym_PinPage(); 
  ps->page_state->page_time->min = 5; 
  ps->page_state->page_time->sec = 0;
}

void LCD_PinPage_Main(struct PlantState *ps)
{
  // save page
  t_page save_page = ps->page_state->page;

  // wdt and break
  BASIC_WDT_RESET;

  // touch
  Touch_PinLinker(ps);

  // touch change -> clean display
  if(save_page != ps->page_state->page){ LCD_Clean(); }

  // end condition on countdown
  if(Eval_CountDown(ps->page_state->page_time)){ LCD_Clean(); ps->page_state->page = AutoPage; }
}


/* ------------------------------------------------------------------*
 *            Auto Entry Clear Pages
 * ------------------------------------------------------------------*/

void LCD_Entry_Clr(void)
{
  LCD_ClrSpace(4, 1, 24, 135);
}


/* ------------------------------------------------------------------*
 *            Auto Entry
 * ------------------------------------------------------------------*/

void LCD_WriteAutoEntryPage(unsigned char page)
{
  unsigned char entry = 0;
  unsigned char eep = 0;
  unsigned char *p_ct = Eval_Memory_LatestEntry(Auto);

  // get pointer stuff
  eep = *p_ct;
  p_ct++;
  entry = *p_ct;

  // clear display section
  LCD_Entry_Clr();

  // page num
  LCD_WriteAnyFont(f_4x6_p, 1, 140, (page + 1) / 10);
  LCD_WriteAnyFont(f_4x6_p, 1, 144, (page + 1) % 10);

  // get right eep
  unsigned char wep = LCD_eep_minus(Auto, eep, (2 * page));

  // write corresponding page
  if (page >= DATA_PAGE_NUM_AUTO)
  {
    // half page
    LCD_wPage(Auto, wep, entry, true);
    LCD_Sym_Data_EndText();
  }
  else
  {
    // full page
    LCD_wPage(Auto, wep, entry, false);
  }
}



/* ------------------------------------------------------------------*
 *            Data Manual Entry Page
 * ------------------------------------------------------------------*/

void LCD_WriteManualEntryPage(unsigned char page)
{
  unsigned char entry = 0;
  unsigned char eep = 0;
  unsigned char *p_ct = Eval_Memory_LatestEntry(Manual);

  // pointer stuff
  eep = *p_ct;
  p_ct++;
  entry = *p_ct;

  // clear display section
  LCD_Entry_Clr();

  // page number
  LCD_WriteAnyFont(f_4x6_p, 1, 144, page + 1);

  // get right eep
  unsigned char wep = LCD_eep_minus(Manual, eep, (2 * page));

  // write corresponding page
  if (page >= DATA_PAGE_NUM_MANUAL)
  {
    // half page
    LCD_wPage(Manual, wep, entry, true);
    LCD_Sym_Data_EndText();
  }
  else
  {
    // full page
    LCD_wPage(Manual, wep, entry, false);
  }
}



/* ------------------------------------------------------------------*
 *            Data Setup Entry Page
 * ------------------------------------------------------------------*/

void LCD_WriteSetupEntryPage(unsigned char page)
{
  unsigned char entry = 0;
  unsigned char eep = 0;
  unsigned char *p_ct = Eval_Memory_LatestEntry(Setup);

  // pointer stuff
  eep = *p_ct;
  p_ct++;
  entry = *p_ct;

  // clear display section
  LCD_Entry_Clr();

  // page number
  LCD_WriteAnyFont(f_4x6_p, 1, 144, page + 1);

  // get right eep
  unsigned char wep = LCD_eep_minus(Setup, eep, (2 * page));

  // write corresponding page
  if (page >= DATA_PAGE_NUM_MANUAL)
  {
    // half page
    LCD_wPage(Setup, wep, entry, true);
    LCD_Sym_Data_EndText();
  }
  else
  {
    // full page
    LCD_wPage(Setup, wep, entry, false);
  }
}


/*-------------------------------------------------------------------*
 *  LCD_wPage
 * --------------------------------------------------------------
 *  t_textSymbols data  - Display Page Selection
 *  unsigned char eep   - EEPROM-Page
 *  unsigned char entry - entry
 * --------------------------------------------------------------
 *  Writes One Entry line, call in entryPages
 * ------------------------------------------------------------------*/

void LCD_wPage(t_textButtons data, unsigned char eep, unsigned char entry, bool half)
{
  unsigned char i = 0;
  unsigned char startPa = 0;
  unsigned char endPa = 0;

  //*** debug eep page
  if (DEBUG) LCD_WriteAnyValue(f_6x8_p, 2, 0, 0, eep);

  // determine start and end page
  switch(data)
  {
    case Auto: startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case Manual: startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case Setup: startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
    default: break;
  }

  // Write the data page to display
  for(i = 0; i < 8; i++)
  {
    //-------------------------------------------Write-Entry-------
    switch(data)
    {
      case Auto: LCD_WriteAutoEntry(5+(2*i), eep, entry); break;
      case Manual: LCD_WriteManualEntry(5+(2*i), eep, entry); break;
      case Setup: LCD_WriteSetupEntry(5+(2*i), eep, entry); break;
      default: break;
    }
    //-------------------------------------------Update------------
    if(entry < 1)
    {
      entry = 4;
      eep--;
      if(eep < startPa) eep = endPa;
    }
    entry--;

    // return if only half page needed
    if (i >= 4 && half) return;
  }
}


/* ------------------------------------------------------------------*
 *            Minus
 * ------------------------------------------------------------------*/

unsigned char LCD_eep_minus(t_textButtons data, unsigned char eep, unsigned char cnt)
{
  unsigned char i = 0;
  unsigned char startPa = 0;
  unsigned char endPa = 0;

  // determine start and end page
  switch(data)
  {
    case Auto: startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case Manual: startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case Setup: startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
    default: break;
  }

  // get right EEPROM page
  for(i = 0; i < cnt; i++)
  {
    eep--;
    if (eep < startPa){ eep = endPa; }
  }

  return eep;
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
 *            lcd calibration
 * ------------------------------------------------------------------*/

void LCD_Calibration(void)
{
  int xRead = 0;
  int yRead = 0;
  int x = 0;
  int y = 0;

  struct LcdBacklight lcd_backlight = { .state = _off, .count = 0 };

  LCD_Backlight(_on, &lcd_backlight);
  Touch_Cal_Main();
  LCD_Clean();

  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "X-Cal:");
  LCD_WriteAnyStringFont(f_6x8_p, 5, 0, "Y-Cal:");
  LCD_WriteAnyStringFont(f_6x8_p, 10, 0, "X-Value:");
  LCD_WriteAnyStringFont(f_6x8_p, 13, 0, "Y-Value:");
  LCD_WriteAnyStringFont(f_6x8_p, 18, 0, "X-Value-Cal:");
  LCD_WriteAnyStringFont(f_6x8_p, 21, 0, "Y-Value-Cal:");

  LCD_WriteAnyValue(f_6x8_p, 3, 2, 60,  MEM_EEPROM_ReadVar(TOUCH_X_min));
  LCD_WriteAnyValue(f_6x8_p, 3, 5, 60,  MEM_EEPROM_ReadVar(TOUCH_Y_min));
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