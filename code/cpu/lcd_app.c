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
 *  main automatic page
 * ------------------------------------------------------------------*/

void LCD_AutoPage(struct PlantState *ps)
{
  // static t_page save_page = AUTO_START_PAGE;
  // static int min = 5;
  // static int sec = 0;
  // static int lcd_reset = 0;
  // static unsigned char initVar = 0;
  
  //int *p_min = &ps->page_state->page_time->min;
  //int *p_sec = &ps->page_state->page_time->sec;

  // switch(ps->page_state->page)
  // {
  //   case AutoPage:
  //     LCD_Sym_Auto_Main();
  //     ps->page_state = ps->prev_page_state;

  //     // init
  //     if(!ps->init || (!ps->page_state->page_time->min && !(MEM_EEPROM_ReadVar(SENSOR_inTank))))
  //     {
  //       ps->init = 1;
  //       LCD_AirState(ps, _init);
  //       LCD_Auto_InflowPump(ps, _init);
  //       LCD_Auto_Phosphor(0, _init);
  //       OUT_Init_Valves();
  //     }
  //     else
  //     {
  //       // SaveTime
  //       //int sMin = *p_min;
  //       //int sSec = *p_sec;
  //       //*p_min = sMin;
  //       //*p_sec = sSec;

  //       // rewrite countdown
  //       if(ps->page_state->page != AutoAir && ps->page_state->page != AutoAirOff && ps->page_state->page != AutoCirc && ps->page_state->page != AutoCircOff){ LCD_Sym_Auto_CountDown(*p_min, *p_sec); }
  //     }
  //     LCD_Auto_SetState(ps);
  //     break;

  //   case AutoZone: LCD_AutoPage_Zone(ps); break;
  //   case AutoSetDown: LCD_AutoPage_SetDown(ps); break;
  //   case AutoPumpOff: LCD_AutoPage_PumpOff(ps); break;
  //   case AutoMud: LCD_AutoPage_Mud(ps); break;

  //   case AutoCirc:
  //   case AutoCircOff:
  //     LCD_AutoPage_Circ(ps);
  //     if(ps->page_state->page == AutoCirc && save_page == AutoCircOff) save_page = ps->page_state->page;
  //     else if(ps->page_state->page == AutoCircOff && save_page == AutoCirc) save_page = ps->page_state->page;
  //     break;

  //   case AutoAir:
  //   case AutoAirOff:
  //     LCD_AutoPage_Air(ps);
  //     if(ps->page_state->page == AutoAir && save_page == AutoAirOff) save_page = ps->page_state->page;
  //     else if(ps->page_state->page == AutoAirOff && save_page == AutoAir) save_page = ps->page_state->page;
  //     break;

  //   default: break;
  // }

  // specials and symbols
  switch(ps->page_state->page)
  {
    // entry page
    case AutoPage:

      // set auto page
      LCD_Auto_SetState(ps);

      // get previous page or start page
      if(ps->prev_page_state->page != NoPage){ ps->page_state = ps->prev_page_state; }
      else{ ps->page_state->page = AUTO_START_PAGE; }
      ps->prev_page_state = ps->page_state;

      // init procedure
      if(!ps->init || (!ps->page_state->page_time->min && !(MEM_EEPROM_ReadVar(SENSOR_inTank))))
      {
        ps->init = 1;
        LCD_AirState(ps, _init); LCD_Auto_InflowPump(ps, _init); LCD_Auto_Phosphor(0, _init);
        OUT_Init_Valves();
      }

    // air states
    case AutoAir: case AutoAirOff: case AutoCirc: case AutoCircOff: LCD_AirState(ps, _exe); break;

    // other
    default: LCD_Sym_Auto_PageTime(ps, ps->page_state->page_time); break;
  }

  // save page
  t_page save_page = ps->page_state->page;

  // error detection
  Error_Detection(ps);

  // countdown end handling
  if((Eval_CountDown(ps->page_state->page_time)) && (ps->page_state->page != ErrorTreat)){ LCD_Auto_CountDownEndAction(ps); }

  // touch linker
  Touch_AutoLinker(ps);

  // sonic
  Sonic_ReadTank(ps, _exe);

  // change page through countdown, touch, or sonic
  if(save_page != ps->page_state->page){ LCD_Auto_SetState(ps); }

  // execute in all auto pages
  LCD_Backlight(_exe, ps->lcd_backlight);
  LCD_Auto_InflowPump(ps, _exe);
  LCD_Auto_Phosphor(ps, _exe);
  MPX_ReadAverage(Auto, _exe);

  // lcd_reset 30s default: 32000
  if(ps->frame_counter->lcd_reset == 10){ LCD_Init(); TCC0_wait_ms(2); }
  else if(ps->frame_counter->lcd_reset == 10000){ LCD_Sym_MarkTextButton(Auto); }
  else if(ps->frame_counter->lcd_reset == 20000){ LCD_Sym_Logo(); }
  else if(ps->frame_counter->lcd_reset > 30000){ LCD_Auto_Symbols(ps); ps->frame_counter->lcd_reset = 0; }

  // update frame counter
  ps->frame_counter->lcd_reset++;
}


/* ------------------------------------------------------------------*
 *            Auto Set Pages
 * ------------------------------------------------------------------*/

void LCD_Auto_SetState(struct PlantState *ps)
{
  // set symbol
  LCD_Auto_Symbols(ps);

  int *p_min = &ps->page_state->page_time->min;
  int *p_sec = &ps->page_state->page_time->sec;

  // set page specific
  switch(ps->page_state->page)
  {
    // init auto page
    //case AutoPage: OUT_Init_Valves(); break;

    // other auto pages
    case AutoZone: *p_sec = 0; *p_min = 2; OUT_Set_Air(); break;
    case AutoSetDown: *p_sec = 0; *p_min = MEM_EEPROM_ReadVar(TIME_setDown); OUT_SetDown(); break;
    case AutoPumpOff: *p_sec = 0; *p_min = MEM_EEPROM_ReadVar(ON_pumpOff); OUT_Set_PumpOff(); break;

    case AutoMud:
      *p_min = MEM_EEPROM_ReadVar(ON_MIN_mud);
      *p_sec = MEM_EEPROM_ReadVar(ON_SEC_mud);

      // do not open Mud
      if((*p_min == 0) && (*p_sec == 0)){ *p_sec = 1; break; }
      OUT_Set_Mud();
      break;

    case AutoCirc: case AutoCircOff: case AutoAir: case AutoAirOff:
      // time
      *p_sec = 0;
      if(ps->page_state->page == AutoCirc || ps->page_state->page == AutoCircOff){ *p_min = (( MEM_EEPROM_ReadVar(TIME_H_circ) << 8) | MEM_EEPROM_ReadVar(TIME_L_circ)); }
      else{ *p_min = (( MEM_EEPROM_ReadVar(TIME_H_air) << 8) | MEM_EEPROM_ReadVar(TIME_L_air)); }

      LCD_AirState(ps, _set);
      LCD_Auto_InflowPump(ps, _set);
      break;

    default: ps->prev_page_state = ps->page_state; break;
  }
}


/* ------------------------------------------------------------------*
 *            Auto Set Symbols
 * ------------------------------------------------------------------*/

void LCD_Auto_Symbols(struct PlantState *ps)
{
  // write text
  LCD_Sym_Auto_Text(ps);

  // clear actual symbol space
  LCD_ClrSpace(5, 0, 2, 35);

  // page dependend symbols
  switch(ps->page_state->page)
  {
    case AutoPage: LCD_Sym_Auto_Main(); break;
    case AutoZone: LCD_Sym_Auto_Zone(ps->page_state->page_time); break;
    case AutoSetDown: LCD_Sym_Auto_SetDown(ps->page_state->page_time); break;
    case AutoPumpOff: LCD_Sym_Auto_PumpOff(ps->page_state->page_time); break;
    case AutoMud: LCD_Sym_Auto_Mud(ps->page_state->page_time); break;
    case AutoAir: case AutoAirOff: case AutoCirc: case AutoCircOff: LCD_AirState(ps, _write); break;
    default: break;
  }
  //AddOns
  LCD_Auto_Phosphor(ps, _sym);
  LCD_Auto_InflowPump(ps, _sym);
}


/* ------------------------------------------------------------------*
 *            Auto End State
 * ------------------------------------------------------------------*/

void LCD_Auto_CountDownEndAction(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    case AutoZone: MPX_ReadTank(ps, _exe); OUT_Clr_Air(); break;
    case AutoSetDown: ps->page_state->page = AutoPumpOff; break;
    case AutoPumpOff: OUT_Clr_PumpOff(); ps->page_state->page = AutoMud; break;
    
    case AutoMud: 
      OUT_Clr_Mud();

      // oxygen entry, todo: check this
      Eval_Oxygen(_entry, 0);
      MEM_EEPROM_WriteAutoEntry(0, 0, Write_Entry);
      Eval_Oxygen(_clear, 0);

      // calibration for pressure sensing
      if(MEM_EEPROM_ReadVar(CAL_Redo_on) && !(MEM_EEPROM_ReadVar(SONIC_on))) ps->page_state->page = AutoZone;
      else ps->page_state->page = AutoCirc;
      break;

    case AutoCirc: 
    case AutoCircOff:
      LCD_Auto_InflowPump(ps, _reset);
      if(ps->page_state->page == AutoCirc) OUT_Clr_Air();
      ps->page_state->page = AutoAir;
      break;

    case AutoAir:
    case AutoAirOff:
      LCD_Auto_InflowPump(ps, _reset);
      if(ps->page_state->page == AutoAir) OUT_Clr_Air();
      ps->page_state->page = AutoSetDown;
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Auto ZoneCalc
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_Zone(struct PlantState *ps)
// {
//   t_page page = AutoZone;
//   LCD_Sym_WriteAutoVar_Comp(ps->page_state->page_time);
//   Eval_Oxygen(_count, *p_min);
//   page = Error_Detection(page, *p_min, *p_sec, ps);
//   if((Eval_CountDown(ps->page_state->page_time)) && (page != ErrorTreat))
//   {
//     MPX_ReadTank(ps, _exe);
//     OUT_Clr_Air();
//   }
// }


/* ------------------------------------------------------------------*
 *            Auto Set-Down
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_SetDown(struct PlantState *ps)
// {
//   LCD_Sym_WriteAutoVar(ps);
//   page = Error_Detection(page, *p_min, *p_sec, ps);
//   if((Eval_CountDown(ps->page_state->page_time)) && (page != ErrorTreat))
//   {
//     ps->page_state->page = AutoPumpOff;
//   }
// }


/* ------------------------------------------------------------------*
 *            Auto Pump-Off
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_PumpOff(struct PlantState *ps)
// {
//   LCD_Sym_WriteAutoVar_Comp(ps->page_state->page_time);
//   page = Error_Detection(page, *p_min, *p_sec, ps);
//   if((Eval_CountDown(ps->page_state->page_time)) && (page != ErrorTreat))
//   {
//     OUT_Clr_PumpOff();
//     ps->page_state->page = AutoMud;
//   }
// }


/* ------------------------------------------------------------------*
 *            Auto Mud
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_Mud(struct PlantState *ps)
// {
//   LCD_Sym_WriteAutoVar_Comp(ps->page_state->page_time);
//   Error_Detection(ps);
//   if((Eval_CountDown(ps->page_state->page_time)) && (ps->page_state->page != ErrorTreat))
//   {
//     // close mud valve
//     OUT_Clr_Mud();

//     // oxygen entry
//     Eval_Oxygen(_entry, 0);
//     MEM_EEPROM_WriteAutoEntry(0, 0, Write_Entry);
//     Eval_Oxygen(_clear, 0);

//     // calibration for pressure sensing
//     if(MEM_EEPROM_ReadVar(CAL_Redo_on) && !(MEM_EEPROM_ReadVar(SONIC_on))) return AutoZone;
//     else return AutoCirc;
//   }
//   return AutoMud;
// }


/* ------------------------------------------------------------------*
 *            Auto Circulate
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_Circ(struct PlantState *ps)
// {
//   int min = *p_min;
//   int sec = *p_sec;

//   // save for error treat
//   t_page save_page = page;                        
//   Error_Detection(ps);
//   page = LCD_AirState(page, sec, _exe);

//   // change page condition
//   if(Eval_CountDown(ps->page_state->page_time) && (page != ErrorTreat) && (!MEM_EEPROM_ReadVar(SENSOR_inTank) || LCD_Sym_NoUS(page, _check)))
//   {
//     LCD_Auto_InflowPump(ps, _reset);
//     if(page == AutoCirc) OUT_Clr_Air();
//     return AutoAir;
//   }
//   if(page == ErrorTreat) page = save_page;
// }


/* ------------------------------------------------------------------*
 *            Auto Air
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_Air(struct PlantState *ps)
// {
//   int min = *p_min;
//   int sec = *p_sec;

//   Error_Detection(ps);
//   page = LCD_AirState(page, sec, _exe);

//   if((Eval_CountDown(ps->page_state->page_time)) && (ps->page_state->page != ErrorTreat))
//   {
//     LCD_Auto_InflowPump(ps, _reset);
//     if(page == AutoAir) OUT_Clr_Air();
//     return AutoSetDown;
//   }
// }


/* ------------------------------------------------------------------*
 *            Auto Air
 * ------------------------------------------------------------------*/

void LCD_AirState(struct PlantState *ps, t_FuncCmd cmd)
{
  static int  cOld = 0;
  //static int  tms->min = 0;
  //static int  airSec = 0;
  static struct Tms tms = { .min = 0, .sec = 0 };

  // shortcuts
  t_page p = ps->page_state->page;
  int sec = ps->page_state->page_time->sec;

  t_page cPage = AutoAir;

  // init
  if(cmd == _init)
  {
    tms.min = LCD_AutoRead_StartTime(ps);
    tms.sec = 0;
  }

  // set
  else if(cmd == _set)
  {
    if(p == AutoAir || p == AutoCirc){ OUT_Set_Air(); }
  }

  // reset
  else if(cmd == _reset)
  {
    if(p == AutoAir || p == AutoCirc){ OUT_Clr_Air(); }
  }

  // symbols
  else if(cmd == _write)
  {
    LCD_Sym_AutoAir(p);
    LCD_Sym_Auto_CountDown(&tms);
  }

  // exe
  else if(cmd == _exe)
  {
    // change to off
    if(!tms.min && !tms.sec)
    {
      switch(p)
      {
        // auto air on
        case AutoAir:
        case AutoCirc:
          cPage = p;
          MPX_ReadTank(ps, _exe);
          MPX_ReadTank(ps, _write);
          OUT_Clr_Air();
          if(cPage != p){ LCD_Auto_InflowPump(ps, _reset); }

          // set ps
          if(p == AutoAir){ ps->page_state->page = AutoAirOff; }
          else if(p == AutoCirc){ ps->page_state->page = AutoCircOff; }
          LCD_Sym_AutoAir(p);
          LCD_Auto_InflowPump(ps, _set);
          tms.min = LCD_AutoRead_StartTime(ps);
          tms.sec = 0;
          break;

        // auto air off
        case AutoAirOff:
        case AutoCircOff:
          LCD_Auto_InflowPump(ps, _reset);
          OUT_Set_Air();

          // set pages
          if(p == AutoAirOff){ ps->page_state->page = AutoAir; }
          else if(p == AutoCircOff){ ps->page_state->page = AutoCirc; }
          LCD_Sym_AutoAir(p);
          tms.min = LCD_AutoRead_StartTime(ps);
          tms.sec = 0;
          break;

        // manual air on
        case ManualCirc:
          if(p == ManualCirc){ ps->page_state->page = ManualCircOff; }
          MPX_ReadTank(ps, _exe);
          MPX_ReadTank(ps, _write);
          OUT_Clr_Air();
          tms.min = LCD_AutoRead_StartTime(ps);
          tms.sec = 0;
          break;

        // manual air off
        case ManualCircOff:
          if(p == ManualCircOff) ps->page_state->page = ManualCirc;
          OUT_Set_Air();
          tms.min = LCD_AutoRead_StartTime(ps);
          tms.sec = 0;
          break;

        default: break;
      }
    }

    // countdown
    if(sec != cOld)
    {
      cOld = sec;
      if(!tms.sec && (p != ErrorTreat))
      {
        tms.sec = 60;
        if(tms.min) tms.min--;
      }
      if(tms.sec) tms.sec--;
    }

    // manual return page
    if(p == ManualCirc || p == ManualCircOff) return;

    // auto variables
    if((p == AutoCirc) || (p == AutoAir) || (p == ErrorTreat))
    {
      LCD_Sym_Auto_PageTime(ps, &tms);
      Eval_Oxygen(_count, tms.min);
    }
    else LCD_Sym_Auto_PageTime(ps, &tms);
  }
}


/* ------------------------------------------------------------------*
 *            Auto Inflow Pump
 * ------------------------------------------------------------------*/

t_FuncCmd LCD_Auto_InflowPump(struct PlantState *ps, t_FuncCmd cmd)
{
  static int ip_count = 0;

  // time[ h = 2 | min = 1 | sec = 0]
  static unsigned char t_ip[3] = {0, 0, 0};
  static t_FuncCmd ip_state = _off;

  t_page p = ps->page_state->page;

  //--------------------------------------------------init
  if(cmd == _init)
  {
    // on time for inflow pump
    t_ip[1] = MEM_EEPROM_ReadVar(ON_inflowPump);
    if(!t_ip[1])
    {
      t_ip[0] = 0;
      ip_state = _disabled;
      return ip_state;
    }
    t_ip[2] = 0;
    t_ip[1] = 1;
    t_ip[0] = 2;
    ip_state = _off;
  }

  //--------------------------------------------------State
  else if(cmd == _state)
  {
    return ip_state;
  }

  //--------------------------------------------------OutSet
  else if(cmd == _sym)
  {
    LCD_Sym_Auto_IP(ps, ip_state);
    LCD_Sym_Auto_IP_Time(0x07, t_ip);
  }

  else if(ip_state == _disabled) return ip_state;

  //--------------------------------------------------Set
  else if(cmd == _set)
  {
    if(ip_state == _on && (p == AutoAirOff || p == AutoCircOff))
    {
      LCD_Sym_Auto_IP(ps, ip_state);
      OUT_Set_InflowPump();
    }
  }

  //--------------------------------------------------Reset
  else if(cmd == _reset)
  {
    if(ip_state == _on)
    {
      LCD_Sym_Auto_IP(ps, ip_state);
      OUT_Clr_InflowPump();
    }
  }

  //--------------------------------------------------Execution
  else if(cmd == _exe)
  {
    // AutoChange2Off
    if(ip_state == _on && p != ErrorTreat && !t_ip[1] && !t_ip[0])
    {
      ip_state = _off;
      t_ip[2] = MEM_EEPROM_ReadVar(T_IP_off_h);
      t_ip[1] = MEM_EEPROM_ReadVar(OFF_inflowPump);
      t_ip[0] = 2;
      LCD_Sym_Auto_IP(ps, _off);
      LCD_Sym_Auto_IP_Time(0x07, t_ip);
      OUT_Clr_InflowPump();
    }

    // auto change to on state
    else if( (ip_state == _off) && (p != ErrorTreat) && !t_ip[2] && !t_ip[1] && !t_ip[0] && (p == AutoAirOff || p == AutoCircOff))
    {
      ip_state = _on;
      t_ip[2] = 0;
      t_ip[1] = MEM_EEPROM_ReadVar(ON_inflowPump);
      t_ip[0] = 2;
      LCD_Sym_Auto_IP(ps, ip_state);
      LCD_Sym_Auto_IP_Time(0x07, t_ip);
      OUT_Set_InflowPump();
    }

    // countdown
    if(ip_state == _off || ((p == AutoAirOff || p == AutoCircOff) && ip_state == _on))
    {
      if(ip_count != ps->page_state->page_time->sec)
      {
        ip_count = ps->page_state->page_time->sec;
        if(!t_ip[0])
        {
          if(t_ip[1] || t_ip[2])
          {
            t_ip[0] = 60;
          }
          if((!t_ip[1]) && t_ip[2])
          {
            t_ip[1] = 60;
            // decrease h
            t_ip[2]--;
            LCD_Sym_Auto_IP_Time(0x06, t_ip);
          }
          // decrease min
          if(t_ip[1]) t_ip[1]--;
          LCD_Sym_Auto_IP_Time(0x02, t_ip);
        }
        // decrease sec
        if(t_ip[0]) t_ip[0]--;
        LCD_Sym_Auto_IP_Time(0x01, t_ip);
      }
    }
  }

  return ip_state;
}


/* ------------------------------------------------------------------*
 *            Auto Phosphor
 * ------------------------------------------------------------------*/

void LCD_Auto_Phosphor(struct PlantState *ps, t_FuncCmd cmd)
{
  static int count = 0;
  // static int min = 0;
  // static int sec = 5;
  static struct Tms ph_tms = { .min = 0, .sec = 5};
  static t_FuncCmd   state = _off;

  //--------------------------------------------------Initialization
  if(cmd == _init)
  {
    ph_tms.min = MEM_EEPROM_ReadVar(ON_phosphor);
    if(!ph_tms.min){ ph_tms.sec = 0; state = _disabled; return; }
    ph_tms.min = 1;
    ph_tms.sec = 5;
    state = _off;
  }

  //--------------------------------------------------Symbols
  else if(cmd == _sym)
  {
    LCD_Sym_Auto_Ph(state);
    LCD_Sym_Auto_Ph_Time(&ph_tms);
  }

  //--------------------------------------------------Disabled
  else if(state == _disabled)   return;

  //--------------------------------------------------Execution
  else if(cmd == _exe)
  {
    //Counter
    if(count != ps->page_state->page_time->sec)
    {
      count = ps->page_state->page_time->sec;
      if(!ph_tms.sec)
      {
        ph_tms.sec = 60;
        if(ph_tms.min) ph_tms.min--;
        LCD_WriteAnyValue(f_4x6_p, 2, 13, 135, ph_tms.min);
      }
      if(ph_tms.sec) ph_tms.sec--;
      LCD_WriteAnyValue(f_4x6_p, 2, 13, 147, ph_tms.sec);
    }

    // change to off state
    if(state == _on && !ph_tms.min && !ph_tms.sec)
    {
      state =_off;
      ph_tms.min = MEM_EEPROM_ReadVar(OFF_phosphor);
      ph_tms.sec = 0;
      LCD_WriteAnySymbol(s_19x19, 6, 134, p_phosphor);
      OUT_Clr_Phosphor();
    }

    // change to on state
    else if(state == _off && !ph_tms.min && !ph_tms.sec)
    {
      state = _on;
      ph_tms.min = MEM_EEPROM_ReadVar(ON_phosphor);
      ph_tms.sec = 0;
      LCD_WriteAnySymbol(s_19x19, 6, 134, n_phosphor);
      OUT_Set_Phosphor();
    }
  }
}


/* ------------------------------------------------------------------*
 *            Manual Pages
 * ------------------------------------------------------------------*/

void LCD_ManualPage(struct PlantState *ps)
{
  //static int mMin = 5;
  //static int mSec = 0;
  //static int lcd_reset = 0;
  // int *p_min;
  // int *p_sec;

  //p_min = &mMin;
  //p_sec = &mSec;

  // switch(page)
  // {
  //   case ManualPage: page = ManualMain; mMin = 5; mSec = 0; LCD_Manual_Symbols(ps); break;
  //   case ManualMain: page = LCD_ManualPage_Main(ps); break;
  //   case ManualCirc:
  //   case ManualCircOff: page = LCD_ManualPage_Circ(page, p_min, p_sec, ps); break;
  //   case ManualAir: page = LCD_ManualPage_Air(ps); break;
  //   case ManualSetDown: page = LCD_ManualPage_SetDown(ps); break;
  //   case ManualPumpOff:
  //   case ManualPumpOff_On: page = LCD_ManualPage_PumpOff(page, p_min, p_sec, ps); break;
  //   case ManualMud: page = LCD_ManualPage_Mud(ps); break;
  //   case ManualCompressor: page = LCD_ManualPage_Compressor(ps); break;
  //   case ManualPhosphor: page = LCD_ManualPage_Phosphor(ps); break;
  //   case ManualInflowPump: page = LCD_ManualPage_InflowPump(ps); break;
  //   default: break;
  // }

  // save page to detect changes by touch
  t_page save_page = ps->page_state->page;

  // touch
  Touch_SelectLinker(ps);

  // specials
  switch(ps->page_state->page)
  {
    case ManualPage: ps->page_state->page = ManualMain; break;
    default: break;
  }

  // count down
  if(Eval_CountDown(ps->page_state->page_time)){ MEM_EEPROM_WriteManualEntry(0,0, _write); ps->page_state->page = AutoPage; }

  // check if page changed
  if(save_page != ps->page_state->page) 
  {
    // clear old state
    LCD_Manual_ResetState(save_page);

    // set new state
    LCD_Manual_SetState(ps);

    // special ones in change
  }

  // special cases
  //if(ps->page_state->page == SetupCalPressure)



  // backlight
  LCD_Backlight(_exe, ps->lcd_backlight);
  if(ps->page_state->page != ManualPumpOff)
  {
    LCD_Sym_Manual_PageTime(ps->page_state->page_time);
    MPX_ReadAverage(Manual, _exe);
    Sonic_ReadTank(ps, _exe);
  }

  //--------------------------------------------------lcd_reset 30s
  if(ps->frame_counter->lcd_reset > 32000)
  {
    ps->frame_counter->lcd_reset = 0;
    LCD_Init();
    TCC0_wait_ms(2);
    LCD_Sym_MarkTextButton(Manual);
  }
  ps->frame_counter->lcd_reset++;
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
    case ManualMain: *p_min = 5; *p_sec = 0; LCD_Manual_Symbols(ps); break;
    case ManualCirc: LCD_AirState(ps, _init); OUT_Set_Air(); *p_min = 60; *p_sec = 0; break;
    case ManualAir: LCD_AirState(ps, _init); OUT_Set_Air(); *p_min = 60; *p_sec = 0; break;
    case ManualSetDown: *p_min = 60; *p_sec = 0; break;

    case ManualPumpOff:
      LCD_ClrSpace(15, 2, 5, 120);
      LCD_WriteAnyStringFont(f_6x8_p, 17, 15,"PRESS OK!:");
      LCD_WriteAnySymbol(s_19x19, 15, 85, p_ok);
      *p_min = 30;
      *p_sec = 0;
      LCD_WriteAnyValue(f_6x8_p, 2, 17, 124, *p_min);
      LCD_WriteAnyValue(f_6x8_p, 2, 17, 142, *p_sec); break;

    case ManualPumpOff_On:
      OUT_Set_PumpOff();
      LCD_ClrSpace(15, 2, 5, 120);
      LCD_WriteAnyStringFont(f_6x8_p, 17, 136,":");
      *p_min = 29;
      *p_sec = 59;
      LCD_Sym_Manual_Text(ps);
      break;

    case ManualMud: OUT_Set_Mud(); *p_min = 5; *p_sec = 0; break;
    case ManualCompressor: OUT_Set_Compressor(); *p_min = 0; *p_sec = 10; break;
    case ManualPhosphor: OUT_Set_Phosphor(); *p_min = 30; *p_sec = 0; break;
    case ManualInflowPump: OUT_Set_InflowPump(); *p_min = 5; *p_sec = 0; break;
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
    case ManualCirc: OUT_Clr_Air(); break;
    case ManualAir: OUT_Clr_Air(); break;
    case ManualPumpOff_On: OUT_Clr_PumpOff(); break;
    case ManualMud: OUT_Clr_Mud(); break;
    case ManualCompressor: OUT_Clr_Compressor(); break;
    case ManualPhosphor: OUT_Clr_Phosphor(); break;
    case ManualInflowPump: OUT_Clr_InflowPump(); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            manual symbols
 * ------------------------------------------------------------------*/

void LCD_Manual_Symbols(struct PlantState *ps)
{
  unsigned char i = 0;

  LCD_Sym_MarkTextButton(Manual);
  LCD_Clean();

  // positive setup symbols
  for(i = 12; i < 20; i++){ LCD_Sym_SetupSymbols(i); }
  LCD_Sym_MarkTextButton(Manual);
  LCD_Sym_Manual_Text(ps);

  // init valves if it is the first page
  OUT_Init_Valves();

  // save manual entry
  MEM_EEPROM_WriteManualEntry(MCP7941_ReadByte(TIC_HOUR), MCP7941_ReadByte(TIC_MIN), _saveValue);
}


/* ------------------------------------------------------------------*
 *            Manual CountDown
 * ------------------------------------------------------------------*/

// void LCD_ManualCD(struct PlantState *ps)
// {
//   if(Eval_CountDown(ps->page_state->page_time)){ MEM_EEPROM_WriteManualEntry(0,0, _write); ps->page_state->page = AutoPage; }
// }




/* ------------------------------------------------------------------*
 *            Manual Main
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_Main(struct PlantState *ps)
// {
//   Touch_ManualLinker(ps);
//   LCD_ManualCD(ps);
//   if(page != ManualMain) LCD_Manual_SetState(page, p_min, p_sec, ps);
// }



/* ------------------------------------------------------------------*
 *            Manual Circulate
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_Circ(struct PlantState *ps)
// {
//   t_page save_page = page;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualCirc && page != ManualCircOff)
//   {
//     // close if open
//     if(save_page == ManualCirc) OUT_Clr_Air();
//     LCD_Manual_SetState(page, p_min, p_sec, ps);
 
//   }
//   page = LCD_AirState(page, *p_sec, _exe);
// }


/* ------------------------------------------------------------------*
 *            Manual Air
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_Air(struct PlantState *ps)
// {
//   t_page page = ManualAir;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualAir)
//   {
//     OUT_Clr_Air();
//     LCD_Manual_SetState(page, p_min, p_sec, ps);
 
//   }
// }


/* ------------------------------------------------------------------*
 *            Manual Set Down
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_SetDown(struct PlantState *ps)
// {
//   t_page page = ManualSetDown;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualSetDown)
//     LCD_Manual_SetState(page, p_min, p_sec, ps);
// }


/* ------------------------------------------------------------------*
 *            Manual Pump Off
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_PumpOff(struct PlantState *ps)
// {
//   t_page save_page = page;
//   static unsigned char count = 0;

//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != save_page)
//   {
//     if(page != ManualPumpOff_On)
//     {
//       LCD_ClrSpace(15, 2, 5, 120);
//       LCD_WriteAnyStringFont(f_6x8_p, 17, 61, "mbar");
//     }
//     if(save_page == ManualPumpOff_On)
//     {
//       OUT_Clr_PumpOff();
//     }
//     LCD_Manual_SetState(page, p_min, p_sec, ps);
//   }
//   // blink
//   else if(page == ManualPumpOff)
//   {
//     TCC0_DisplayManual_Wait();
//     count++;
//     if(count == 125) LCD_WriteAnyStringFont(f_6x8_n, 17, 15,"PRESS OK!:");
//     if(count == 250) LCD_WriteAnyStringFont(f_6x8_p, 17, 15,"PRESS OK!:");
//   }
// }


/* ------------------------------------------------------------------*
 *            Manual Mud
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_Mud(struct PlantState *ps)
// {
//   t_page page = ManualMud;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualMud){
//     OUT_Clr_Mud();
//     LCD_Manual_SetState(page, p_min, p_sec, ps); }
// }


/* ------------------------------------------------------------------*
 *            Manual Compressor
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_Compressor(struct PlantState *ps)
// {
//   t_page page = ManualCompressor;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualCompressor){
//     OUT_Clr_Compressor();
//     LCD_Manual_SetState(page, p_min, p_sec, ps); }
// }


/* ------------------------------------------------------------------*
 *            Manual Phosphor
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_Phosphor(struct PlantState *ps)
// {
//   t_page page = ManualPhosphor;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualPhosphor){
//     OUT_Clr_Phosphor();
//     LCD_Manual_SetState(page, p_min, p_sec, ps); }
// }


/* ------------------------------------------------------------------*
 *            Manual Inflow Pump
 * ------------------------------------------------------------------*/

// void LCD_ManualPage_InflowPump(struct PlantState *ps)
// {
//   t_page page = ManualInflowPump;
//   page = Touch_ManualLinker(Touch_Matrix(), page, ps);
//   page = LCD_ManualCD(page, p_min, p_sec, ps);
//   if(page != ManualInflowPump){
//     OUT_Clr_InflowPump();
//     LCD_Manual_SetState(page, p_min, p_sec, ps); }
// }


/* ------------------------------------------------------------------*
 *            Setup Pages
 * ------------------------------------------------------------------*/

void LCD_SetupPage(struct PlantState *ps)
{
  //static int sMin = 5;
  //static int sSec = 0;

  int *p_min = &ps->page_state->page_time->min;
  int *p_sec = &ps->page_state->page_time->sec;

  //p_min = &sMin;
  //p_sec = &sSec;

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

  // switch(page)
  // {
  //   // main page
  //   case SetupPage: page = SetupMain; sMin = 5; sSec = 0; LCD_Sym_Setup_Page(); break;
  //   case SetupMain: page = LCD_SetupPage_Main(ps); break;
  //   case SetupCirculate: page = LCD_SetupPage_Circulate(ps); break;
  //   case SetupAir: page = LCD_SetupPage_Air(ps); break;
  //   case SetupSetDown: page = LCD_SetupPage_SetDown(ps); break;
  //   case SetupPumpOff: page = LCD_SetupPage_PumpOff(ps); break;
  //   case SetupMud: page = LCD_SetupPage_Mud(ps); break;
  //   case SetupCompressor: page = LCD_SetupPage_Compressor(ps); break;
  //   case SetupPhosphor: page = LCD_SetupPage_Phosphor(ps); break;
  //   case SetupInflowPump: page = LCD_SetupPage_InflowPump(ps); break;
  //   case SetupCal: page = LCD_SetupPage_Cal(ps); break;
  //   case SetupCalPressure: page = LCD_SetupPage_Cal(ps); break;
  //   case SetupAlarm: page = LCD_SetupPage_Alarm(ps); break;
  //   case SetupWatch: page = LCD_SetupPage_Watch(ps); break;
  //   case SetupZone: page = LCD_SetupPage_Zone(ps); break;
  //   default: break;
  // }

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
        Sonic_LevelCal(_new);
        Sonic_LevelCal(_write);
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
          MPX_LevelCal(_new);
          MPX_LevelCal(_write);
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

  // wait frame
  TCC0_DisplaySetup_Wait();

  // countdown 
  if(Eval_CountDown(ps->page_state->page_time))
  { 
    if(ps->page_state->page == SetupCalPressure){ OUT_CloseAllValves(); }
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
    case SetupCal: LCD_Sym_Setup_Cal(); break;
    case SetupAlarm: LCD_Sym_Setup_Alarm(); break;
    case SetupWatch: LCD_Sym_Setup_Watch(); break;
    case SetupZone: LCD_Sym_Setup_Zone(); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Setup Circulate
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Main(struct PlantState *ps)
// {
//   t_page page = SetupMain;
//   page = Touch_SetupLinker(Touch_Matrix(), page);
//   if(page != SetupMain) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Circulate
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Circulate(struct PlantState *ps)
// {
//   t_page page = SetupCirculate;
//   page = Touch_SetupCirculateLinker(Touch_Matrix(), page);
//   if(page != SetupCirculate) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Air
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Air(struct PlantState *ps)
// {
//   t_page page = SetupAir;
//   page = Touch_SetupAirLinker(Touch_Matrix(), page);
//   if(page != SetupAir) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup SetDown
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_SetDown(struct PlantState *ps)
// {
//   t_page page = SetupSetDown;
//   page = Touch_SetupSetDownLinker(Touch_Matrix(), page);
//   if(page != SetupSetDown) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup PumpOff
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_PumpOff(struct PlantState *ps)
// {
//   t_page page = SetupPumpOff;
//   page = Touch_SetupPumpOffLinker(Touch_Matrix(), page);
//   if(page != SetupPumpOff) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Mud
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Mud(struct PlantState *ps)
// {
//   t_page page = SetupMud;
//   page = Touch_SetupMudLinker(Touch_Matrix(), page);
//   if(page != SetupMud) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Compressor
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Compressor(struct PlantState *ps)
// {
//   t_page page = SetupCompressor;
//   page = Touch_SetupCompressorLinker(Touch_Matrix(), page);
//   if(page != SetupCompressor) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Phosphor
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Phosphor(struct PlantState *ps)
// {
//   t_page page = SetupPhosphor;
//   page = Touch_SetupPhosphorLinker(Touch_Matrix(), page);
//   if(page != SetupPhosphor) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup InflowPump
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_InflowPump(struct PlantState *ps)
// {
//   t_page page = SetupInflowPump;
//   page = Touch_SetupInflowPumpLinker(Touch_Matrix(), page);
//   if(page != SetupInflowPump) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Cal
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Cal(struct PlantState *ps)
// {
//   t_page page = SetupCal;
//   static unsigned char iniTime = 0;

//   if(iniTime) page = SetupCalPressure;
//   page = Touch_SetupCalLinker(Touch_Matrix(), page);

//   if(page == SetupCalPressure)
//   {
//     //------------------------------------------------Sonic
//     if(MEM_EEPROM_ReadVar(SONIC_on))
//     {
//         Sonic_LevelCal(_new);
//         Sonic_LevelCal(_write);
//         LCD_WriteAnySymbol(s_29x17, 15, 1, p_level);
//         page = SetupCal;
//     }
//     //------------------------------------------------PressureSensor
//     else
//     {
//       // 2 min calibration
//       if(!iniTime){ iniTime = 1; *p_min = 4; *p_sec = 120; OUT_Set_Air(); }
//       LCD_WriteAnyValue(f_6x8_p, 3, 17, 100, *p_sec);
//       if(!*p_sec){
//         iniTime = 0;                        //--ReadPressure
//         MPX_LevelCal(_new);                 //Read WaterLevel
//         MPX_LevelCal(_write);               //Write Niveau Pressure
//         OUT_Clr_Air();                      //Close Air
//         LCD_ClrSpace(17, 100, 2, 20);       //ClearCountdown
//         LCD_WriteAnySymbol(s_29x17, 15, 1, p_level);  //Symbol
//         page = SetupCal; }                   //Back to Normal
//     }
//   }
//   else if(page != SetupCal){ iniTime = 0; LCD_Setup_Symbols(page, p_min, p_sec, ps); } 

//
// }


/* ------------------------------------------------------------------*
 *            Setup Alarm
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Alarm(struct PlantState *ps)
// {
//   t_page page = SetupAlarm;
//   page = Touch_SetupAlarmLinker(Touch_Matrix(), page);
//   MCP9800_WriteTemp();
//   if(page != SetupAlarm) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Watch
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Watch(struct PlantState *ps)
// {
//   t_page page = SetupWatch;
//   page = Touch_SetupWatchLinker(Touch_Matrix(), page);
//   if(page != SetupWatch) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Setup Zone
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Zone(struct PlantState *ps)
// {
//   t_page page = SetupZone;
//   page = Touch_SetupZoneLinker(Touch_Matrix(), page);
//   if(page != SetupZone) LCD_Setup_Symbols(page, p_min, p_sec, ps);
//
// }


/* ------------------------------------------------------------------*
 *            Data Page
 * ------------------------------------------------------------------*/

void LCD_DataPage(struct PlantState *ps)
{
  // static int dMin = 5;
  // static int dSec = 0;

  // int *p_min;
  // int *p_sec;

  // p_min = &dMin;
  // p_sec = &dSec;

  // switch(ps->page_state->page)
  // {
  //   case DataPage: page = DataMain; ps->page_state->page_time = { .min = 5, .sec = 0}; LCD_Sym_Data_Page(); break;
  //   case DataMain: page = LCD_DataPage_Main(ps); break;
  //   case DataAuto: page = LCD_DataPage_Auto(ps); break;
  //   case DataManual: page = LCD_DataPage_Manual(ps); break;
  //   case DataSetup: page = LCD_DataPage_Setup(ps); break;

  //   //------------------------------------------------UltraSonic
  //   case DataSonic:
  //   case DataSonicAuto:
  //   case DataSonicBoot:
  //   case DataSonicBootR:
  //   case DataSonicBootW: page = LCD_DataPage_Sonic(page, p_min, p_sec, ps); break;

  //   default: break;
  // }

  // save page
  t_page save_page = ps->page_state->page;

  // touch
  Touch_SelectLinker(ps);

  // specials
  switch(ps->page_state->page)
  {
    case DataPage: ps->page_state->page = DataMain; break;
    case DataSonicAuto: Sonic_Data_Auto(); break;
    case DataSonicBootR: Sonic_Data_BootRead(); ps->page_state->page = DataSonic; break;
    case DataSonicBootW: Sonic_Data_BootWrite(); ps->page_state->page = DataSonic; break;
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
  TCC0_DisplayData_Wait();

  // timeout -> auto page
  if(Eval_CountDown(ps->page_state->page_time)){ ps->page_state->page = AutoPage; }
}


/* ------------------------------------------------------------------*
 *            Data Main
 * ------------------------------------------------------------------*/

// void LCD_DataPage_Main(struct PlantState *ps)
// {
//   t_page page = DataMain;
//   page = Touch_DataLinker(Touch_Matrix(), page);
//   if(page != DataMain) LCD_Data_Symbols(page, p_min, p_sec, ps);
// }


/* ------------------------------------------------------------------*
 *            Data Auto
 * ------------------------------------------------------------------*/

// void LCD_DataPage_Auto(struct PlantState *ps)
// {
//   t_page page = DataAuto;
//   page = Touch_DataAutoLinker(Touch_Matrix(), page);
//   if(page != DataAuto) LCD_Data_Symbols(page, p_min, p_sec, ps); 
// }


/* ------------------------------------------------------------------*
 *            Data Manual
 * ------------------------------------------------------------------*/

// void LCD_DataPage_Manual(struct PlantState *ps)
// {
//   t_page page = DataManual;
//   page = Touch_DataManualLinker(Touch_Matrix(), page);
//   if(page != DataManual) LCD_Data_Symbols(page, p_min, p_sec, ps);
// }


/* ------------------------------------------------------------------*
 *            Data Setup
 * ------------------------------------------------------------------*/

// void LCD_DataPage_Setup(struct PlantState *ps)
// {
//   t_page page = DataSetup;
//   page = Touch_DataSetupLinker(Touch_Matrix(), page);
//   if(page != DataSetup) LCD_Data_Symbols(page, p_min, p_sec, ps);
// }


/* ------------------------------------------------------------------*
 *            Data UltraSonic
 * ------------------------------------------------------------------*/

// void LCD_DataPage_Sonic(struct PlantState *ps)
// {
//   //*p_min = 5;
//   ps->page_state->page_time->min = 5;
//   page = Touch_DataSonicLinker(Touch_Matrix(), page);

//   switch(ps->page_state->page)
//   {
//     case DataSonicAuto: Sonic_Data_Auto(); break;
//     case DataSonicBootR: Sonic_Data_BootRead(); page = DataSonic; break;
//     case DataSonicBootW: Sonic_Data_BootWrite(); page = DataSonic; break;
//     default: break;
//   }

//   if(page != DataSonic && page != DataSonicAuto && page != DataSonicBootR && page != DataSonicBootW) LCD_Data_Symbols(page, p_min, p_sec, ps);
// }



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
    case DataSonic: LCD_Sym_Data_Sonic(); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Pin Page
 * ------------------------------------------------------------------*/

void LCD_PinPage(struct PlantState *ps)
{
  static unsigned char init = 0;

  // save page
  t_page save_page = ps->page_state->page;

  // init pin page
  if(!init){ init = 1; LCD_PinSet_Page(); ps->page_state->page_time->min = 5; ps->page_state->page_time->sec = 0; }

  BASIC_WDT_RESET;
  TCC0_DisplayManual_Wait();

  // touch
  Touch_PinLinker(ps);

  // clean display
  if(save_page != ps->page_state->page){ init = 0; LCD_Clean(); }

  // end condition
  if(Eval_CountDown(ps->page_state->page_time)){ init = 0; LCD_Clean(); ps->page_state->page = AutoPage; }
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
    if (eep < startPa) eep = endPa;
  }

  return eep;
}


/* ------------------------------------------------------------------*
 *            StartTime
 * ------------------------------------------------------------------*/

int LCD_AutoRead_StartTime(struct PlantState *ps)
{
  int sMin = 5;

  switch(ps->page_state->page)
  {
    case AutoCirc:
    case ManualCirc: sMin = MEM_EEPROM_ReadVar(ON_circ); break;

    case AutoCircOff:
    case ManualCircOff: sMin = MEM_EEPROM_ReadVar(OFF_circ); break;

    case AutoAir:
    case ManualAir: sMin = MEM_EEPROM_ReadVar(ON_air); break;

    case AutoAirOff: sMin = MEM_EEPROM_ReadVar(OFF_air); break;

    default: break;
  }
  return sMin;
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
  Touch_Cal();
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

    xRead = (Touch_X_ReadData() >> 4);
    yRead = (Touch_Y_ReadData() >> 4);
    y = Touch_Y_Cal(yRead);
    x = Touch_X_Cal(xRead);

    LCD_WriteAnyValue(f_6x8_p, 3, 10, 100, xRead);
    LCD_WriteAnyValue(f_6x8_p, 3, 13, 100, yRead);
    LCD_WriteAnyValue(f_6x8_p, 3, 18, 100, x);
    LCD_WriteAnyValue(f_6x8_p, 3, 21, 100, y);
  }
}