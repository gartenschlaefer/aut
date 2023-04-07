// --
// lcd symbolds

#include <avr/io.h>

#include "lcd_sym.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "eval_app.h"
#include "error_func.h"
#include "memory_app.h"
#include "mpx_driver.h"
#include "mcp9800_driver.h"
#include "mcp7941_driver.h"
#include "modem_driver.h"
#include "sonic_app.h"
#include "output_app.h"


/* ------------------------------------------------------------------*
 *            logo symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Logo(void)
{
  if(!COMPANY) LCD_WriteAnySymbol(s_logo_purator, 0, 0, logo_purator);
  else LCD_WriteAnySymbol(s_logo_hecs, 0, 0, logo_hecs);
}


/* ------------------------------------------------------------------*
 *            Auto Countdown
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CountDown(struct Tms *tms)
{
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 5, tms->min);
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 17, tms->sec);
}


/* ------------------------------------------------------------------*
 *            Auto Text
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Text(struct PlantState *ps)
{
  // clear text space
  LCD_ClrSpace(13, 0, 6, 160);
  
  // mbar
  LCD_WriteAnyFont(f_4x6_p, 13, 57, 13);
  LCD_WriteAnyFont(f_4x6_p, 13, 61, 14);
  LCD_WriteAnyFont(f_4x6_p, 13, 65, 15);
  LCD_WriteAnyFont(f_4x6_p, 13, 69, 16);

  // h
  LCD_WriteAnyFont(f_4x6_p, 15, 69, 17);
  LCD_WriteAnyFont(f_4x6_p, 13, 13, 10);

  // ip
  LCD_WriteAnyFont(f_4x6_p, 13, 105, 10);
  LCD_WriteAnyFont(f_4x6_p, 13, 93, 10);

  // ph
  LCD_WriteAnyFont(f_4x6_p, 13, 143, 10);

  LCD_Sym_Auto_Time(_init);
  LCD_Sym_Auto_Date();

  // Read water level
  MPX_ReadTank(ps, _write);
  LCD_Sym_Auto_SonicVal(AutoPage, _write);

  LCD_WriteAnyValue(f_4x6_p, 5, 15,43, Eval_Comp_OpHours(_init));

  // ip sensor
  if(MEM_EEPROM_ReadVar(SENSOR_outTank)) LCD_WriteAnySymbol(s_29x17, 16, 90, p_sensor);
  Sonic_ReadTank(AutoPage, _write);
}


/* ------------------------------------------------------------------*
 *            set auto page
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Main(void)
{
  LCD_Sym_MarkTextButton(Auto);
  LCD_Clean();
  LCD_Sym_MarkTextButton(Auto);

  LCD_Sym_Logo();
  LCD_Sym_Auto_Time(_init);
}


/* ------------------------------------------------------------------*
 *            Set Auto Zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Zone(struct Tms *tms)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, n_circulate);
  LCD_WriteAnySymbol(s_29x17, 6, 45, n_compressor);
  LCD_WriteAnyStringFont(f_6x8_n, 8, 1, "z");
  LCD_Sym_Auto_CountDown(tms);
}


/* ------------------------------------------------------------------*
 *            Set Auto SetDown
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetDown(struct Tms *tms)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, n_setDown);
  LCD_WriteAnySymbol(s_29x17, 6, 45, p_compressor);
  LCD_Sym_Auto_CountDown(tms);
}


/* ------------------------------------------------------------------*
 *            Set Auto PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PumpOff(struct Tms *tms)
{
  LCD_WriteAnySymbol(s_35x23, 5, 0, n_pumpOff);
  if(!MEM_EEPROM_ReadVar(PUMP_pumpOff)) LCD_WriteAnySymbol(s_29x17, 6, 45, n_compressor);
  else LCD_WriteAnySymbol(s_29x17, 6, 45, p_compressor);
  LCD_Sym_Auto_CountDown(tms);
}


/* ------------------------------------------------------------------*
 *            Set Auto Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Mud(struct Tms *tms)
{
  LCD_WriteAnySymbol(s_35x23, 5, 0, n_mud);
  LCD_WriteAnySymbol(s_29x17, 6, 45, n_compressor);
  LCD_Sym_Auto_CountDown(tms);
}


/* ------------------------------------------------------------------*
 *            Inflow Pump Sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_IP(struct PlantState *ps)
{
  // handlers
  unsigned char pump =  MEM_EEPROM_ReadVar(PUMP_inflowPump);
  t_page p = ps->page_state->page;
  t_inflow_pump_states ip_state = ps->inflow_pump_state->ip_state;

  // off or disabled ip
  if(ip_state == ip_off || ip_state == ip_disabled)
  {
    switch (pump)
    {
      case 0:
        LCD_WriteAnySymbol(s_35x23, 5, 89, p_inflowPump);
        if(p == AutoAirOff || p == AutoCircOff){ LCD_WriteAnySymbol(s_29x17, 6, 45, p_compressor); }
        break;

      case 1: LCD_WriteAnySymbol(s_19x19, 5, 90, p_pump); break;
      case 2: LCD_WriteAnySymbol(s_35x23, 5, 90, p_pump2); break;
      default: break;
    }
  }

  // ip on state
  else if(ip_state == ip_on)
  {
    switch (pump)
    {
      case 0:
        LCD_WriteAnySymbol(s_35x23, 5, 89, n_inflowPump);
        if(p == AutoAirOff || p == AutoCircOff){ LCD_WriteAnySymbol(s_29x17, 6, 45, n_compressor); }
        break;

      case 1: LCD_WriteAnySymbol(s_19x19, 5, 90, n_pump); break;
      case 2: LCD_WriteAnySymbol(s_35x23, 5, 90, n_pump2); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            Inflow Pump Var
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_IP_Time(unsigned char cho, struct Thms *t_hms)
{
  // sec, min, h
  if(cho & 0x01) LCD_WriteAnyValue(f_4x6_p, 2, 13, 109, t_hms->sec);
  if(cho & 0x02) LCD_WriteAnyValue(f_4x6_p, 2, 13, 97, t_hms->min);
  if(cho & 0x04) LCD_WriteAnyValue(f_4x6_p, 2, 13,85, t_hms->hou);
}


/* ------------------------------------------------------------------*
 *            Phosphor Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ph(struct PlantState *ps)
{
  // ph symbol
  switch(ps->phosphor_state->ph_state)
  {
    case ph_on: LCD_WriteAnySymbol(s_19x19, 6, 134, n_phosphor); break;
    case ph_disabled:
    case ph_off: LCD_WriteAnySymbol(s_19x19, 6, 134, p_phosphor); break;
    default: break;
  }

  // time
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 135, ps->phosphor_state->ph_tms->min);
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 147, ps->phosphor_state->ph_tms->sec);
}


/* ------------------------------------------------------------------*
 *            AutoVar
 * ------------------------------------------------------------------*/

// void LCD_Sym_WriteAutoVar(struct PageState *ps)
// {
//   static int oldSec = 0;

//   int sec = ps->page_state->sec;
//   int min = ps->page_state->min;

//   if(oldSec != sec)
//   {
//     oldSec = sec;

//     // time
//     LCD_Sym_Auto_Time(_exe);
//     LCD_WriteAnyValue(f_4x6_p, 2, 13, 17, sec);
//   }
//   if(sec == 59) LCD_WriteAnyValue(f_4x6_p, 2, 13, 5, min);
// }


/* ------------------------------------------------------------------*
 *            AutoVar - Comp
 * ------------------------------------------------------------------*/

// void LCD_Sym_WriteAutoVar_Comp(struct Tms *tms)
// {
//   static int oldSecc = 0;
//   static int opMin = 0;
//   static unsigned char opCounter = 0;

//   int opHour = 0;

//   int min = tms->min;
//   int sec = tms->sec;

//   if(oldSecc != sec)
//   {
//     oldSecc = sec;

//     // time
//     LCD_Sym_Auto_Time(_exe);
//     LCD_WriteAnyValue(f_4x6_p, 2, 13, 17, sec);
//   }
//   if(sec == 59) LCD_WriteAnyValue(f_4x6_p, 2, 13, 5, min);

//   if(opMin != min)
//   {
//     opMin = min;
//     opCounter++;
//   }
//   if(opCounter >= 60)
//   {
//     opCounter = 0;
//     opHour = Eval_Comp_OpHours(_add);
//     LCD_WriteAnyValue(f_4x6_p, 5, 15, 43, opHour);
//   }
// }


/* ------------------------------------------------------------------*
 *            AutoVar - Comp
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PageTime(struct PlantState *ps, struct Tms *tms)
{
  static int oldSecc = 0;
  static int opMin = 0;
  static unsigned char opCounter = 0;

  // sec change
  if(oldSecc != tms->sec)
  {
    oldSecc = tms->sec;

    // time
    LCD_Sym_Auto_Time(_exe);
    LCD_WriteAnyValue(f_4x6_p, 2, 13, 17, tms->sec);
  }

  // minute update
  if(tms->sec == 59){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 5, tms->min); }

  // update compressor hours
  switch(ps->page_state->page)
  {
    case AutoPumpOff: case AutoMud: case AutoCirc: case AutoAir: case AutoZone:
      if(opMin != tms->min)
      {
        opMin = tms->min;
        opCounter++;
      }
      if(opCounter >= 60)
      {
        opCounter = 0;
        int opHour = Eval_Comp_OpHours(_add);
        LCD_WriteAnyValue(f_4x6_p, 5, 15, 43, opHour);
      }
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            LCD_AutoSym - Auto Air Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_AutoAir(t_page page)
{
  switch(page)
  {
    case AutoCirc:
      LCD_WriteAnySymbol(s_29x17, 6, 0, n_circulate);
      LCD_WriteAnySymbol(s_29x17, 6, 45, n_compressor);
      break;

    case AutoAir:
      LCD_WriteAnySymbol(s_29x17, 6, 0, n_air);
      LCD_WriteAnySymbol(s_29x17, 6, 45, n_compressor);
      break;

    case AutoCircOff:
      LCD_WriteAnySymbol(s_29x17, 6, 0, p_circulate);
      LCD_WriteAnySymbol(s_29x17, 6, 45, p_compressor);
      break;

    case AutoAirOff:
      LCD_WriteAnySymbol(s_29x17, 6, 0, p_air);
      LCD_WriteAnySymbol(s_29x17, 6, 45, p_compressor);
      break;

    default: break;
  }
}



/* ------------------------------------------------------------------*
 *            manual text
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Text(struct PlantState *ps)
{
  LCD_WriteAnyValue(f_6x8_p, 2, 17, 124, ps->page_state->page_time->min);
  LCD_WriteAnyValue(f_6x8_p, 2, 17, 142, ps->page_state->page_time->sec);
  LCD_WriteAnyStringFont(f_6x8_p, 17, 136, ":");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 61, "mbar");
  MPX_ReadTank(ps, _write);
}


/* ------------------------------------------------------------------*
 *            manual variables
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageTime(struct Tms *tms)
{
  static unsigned char oldSec = 0;

  // change?
  if(oldSec != tms->sec)
  {
    oldSec = tms->sec;
    LCD_WriteAnyValue(f_6x8_p, 2, 17, 142, tms->sec);
  }
  // minute write
  if(tms->sec == 59)
  {
    if(tms->min >= 100) LCD_WriteAnyValue(f_6x8_p, 3, 17, 118, tms->min);
    else LCD_WriteAnyValue(f_6x8_p, 2, 17, 124, tms->min);
  }
}


/* ------------------------------------------------------------------*
 *            Set Setup Pages
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Page(void)
{
  LCD_Sym_MarkTextButton(Setup);
  LCD_Clean();
  for(unsigned char i = 12; i < 24; i++){ LCD_Sym_SetupSymbols(i); }
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Circulate
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate(void)
{
  LCD_Sym_SetupSymbols(sn_circulate);
  LCD_Clean();

  LCD_WriteAnyStringFont(f_6x8_p, 16,40,"Time:");
  LCD_WriteAnySymbol(s_29x17, 3, 0, n_circulate);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Air
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Air(void)
{
  LCD_Sym_SetupSymbols(sn_air);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, n_air);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup SetDown
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_SetDown(void)
{
  LCD_Sym_SetupSymbols(sn_setDown);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, n_setDown);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 0,"Time:");
}


/* ------------------------------------------------------------------*
 *            Set Setup PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_PumpOff(void)
{
  LCD_Sym_SetupSymbols(sn_pumpOff);
  LCD_Clean();

  LCD_WriteAnySymbol(s_35x23, 3, 0, n_pumpOff);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Mud(void)
{
  LCD_Sym_SetupSymbols(sn_mud);
  LCD_Clean();

  LCD_WriteAnySymbol(s_35x23, 2, 0, n_mud);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Compressor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Compressor(void)
{
  LCD_Sym_SetupSymbols(sn_compressor);
  LCD_Clean();

  LCD_WriteAnyStringFont(f_6x8_p, 11,28, "mbar MIN.");
  LCD_WriteAnyStringFont(f_6x8_p, 16,28, "mbar MAX.");
  LCD_WriteAnySymbol(s_29x17, 3, 0, n_compressor);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Phosphor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Phosphor(void)
{
  LCD_Sym_SetupSymbols(sn_phosphor);
  LCD_Clean();

  LCD_WriteAnySymbol(s_19x19, 3, 0, n_phosphor);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup InflowPump
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump(void)
{
  LCD_Sym_SetupSymbols(sn_inflowPump);
  LCD_Clean();

  LCD_WriteAnySymbol(s_35x23, 1, 0, n_inflowPump);
  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}

/* ------------------------------------------------------------------*
 *            InflowPump - Values
 * -------------------------------------------------------------------
 *    0000 0000 -> 0 on min h 0 Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Setup_IPValues(unsigned char select, unsigned char *val)
{
  // h
  if(select & (1 << 0)) LCD_WriteAnyValue(f_6x8_n, 2, 5, 47, *val);
  if(select & (1 << 4)) LCD_WriteAnyValue(f_6x8_p, 2, 5, 47, *val);
  val++;

  // min
  if(select & (1 << 1)) LCD_WriteAnyValue(f_6x8_n, 2, 10, 47, *val);
  if(select & (1 << 5)) LCD_WriteAnyValue(f_6x8_p, 2, 10, 47, *val);
  val++;

  // on
  if(select & (1 << 2)) LCD_WriteAnyValue(f_6x8_n, 2, 10, 19, *val);
  if(select & (1 << 6)) LCD_WriteAnyValue(f_6x8_p, 2, 10, 19, *val);
}


/* ------------------------------------------------------------------*
 *            InflowPump - Values - TextSelect
 * -------------------------------------------------------------------
 *    0000 0000 -> off on min h Noff Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Setup_IPText(unsigned char select)
{
  if(select & (1 << 0)) LCD_WriteAnyStringFont(f_6x8_n, 5, 59, "h");
  if(select & (1 << 4)) LCD_WriteAnyStringFont(f_6x8_p, 5, 59, "h");

  if(select & (1 << 1)) LCD_WriteAnyStringFont(f_6x8_n, 10, 59, "min");
  if(select & (1 << 5)) LCD_WriteAnyStringFont(f_6x8_p, 10, 59, "min");

  if(select & (1 << 2)) LCD_WriteAnyStringFont(f_6x8_n, 10, 1,"ON:");
  if(select & (1 << 6)) LCD_WriteAnyStringFont(f_6x8_p, 10, 1,"ON:");

  if(select & (1 << 3)) LCD_WriteAnyStringFont(f_6x8_n, 1,47,"OFF");
  if(select & (1 << 7)) LCD_WriteAnyStringFont(f_6x8_p, 1,47,"OFF");
}



/* ------------------------------------------------------------------*
 *            Set Setup Cal
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal(struct PlantState *ps)
{
  unsigned char sonic = 0;
  unsigned char calRedo = 0;
  LCD_Sym_SetupSymbols(sn_cal);
  LCD_Clean();
  LCD_WriteAnySymbol(s_29x17, 2, 0, n_cal);
  LCD_WriteAnySymbol(s_29x17, 15, 1, p_level);
  LCD_ControlButtons(sp_esc);
  LCD_ControlButtons(sp_ok);
  LCD_Write_TextButton(9, 80, OpenV, 1);
  LCD_WriteAnySymbol(s_29x17, 9, 125, p_cal);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "mbar:");

  // sonic
  sonic = MEM_EEPROM_ReadVar(SONIC_on);
  if(sonic)
  { 
    LCD_WriteAnySymbol(s_19x19, 2, 40, n_sonic);
    LCD_WriteAnyStringFont(f_6x8_p, 17,66, "mm");
    Sonic_LevelCal(_init);
  }
  else
  { 
    LCD_WriteAnySymbol(s_19x19, 2, 40, p_sonic);
    LCD_WriteAnyStringFont(f_6x8_p, 17,60, "mbar");
    MPX_LevelCal(ps, _init);
  }

  // Cal redo with pressure -> Auto Zone page
  if(!MEM_EEPROM_ReadVar(SONIC_on))
  {
    calRedo = MEM_EEPROM_ReadVar(CAL_Redo_on);
    if(calRedo) LCD_WriteAnySymbol(s_19x19, 15, 130, n_arrowRedo);
    else LCD_WriteAnySymbol(s_19x19, 15, 130, p_arrowRedo);
  }


  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Alarm
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Alarm(void)
{
  LCD_Sym_SetupSymbols(sn_alarm);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, n_alarm);
  LCD_WriteAnyStringFont(f_6x8_n, 10,3, "T:");

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 10, 32, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 38, "C");

  MCP9800_WriteTemp();

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 17, 104, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 17, 110, "C");

  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 *            Set Setup Watch
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch(void)
{
  LCD_Sym_SetupSymbols(sn_watch);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, n_watch);
  LCD_Sym_MarkTextButton(Setup);
  LCD_WriteCtrlButton2();

  LCD_WriteAnyStringFont(f_6x8_p, 11, 10, "hh");
  LCD_WriteAnyStringFont(f_6x8_p, 11, 50, "mm");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 10, "dd");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "mm");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 96, "yy");

  LCD_WriteAnyStringFont(f_6x8_p, 9,33, ":");
  LCD_WriteAnyStringFont(f_6x8_p, 15,33, "/");
  LCD_WriteAnyStringFont(f_6x8_p, 15,70, "/");
  LCD_WriteAnyStringFont(f_6x8_p, 15,84, "20");
}


/* ------------------------------------------------------------------*
 *            Set Setup Zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Zone(void)
{
  LCD_Sym_SetupSymbols(sn_zone);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 2, 0, n_zone);

  LCD_WriteCtrlButton();
  LCD_Sym_MarkTextButton(Setup);
}



/* ==================================================================*
 *            Set Setup Symbols
 * ==================================================================*/

void LCD_Sym_SetupSymbols(t_SetupSym sym)
{
  switch(sym)
  {
    case sp_circulate: LCD_WriteAnySymbol(s_29x17, 3, 0, p_circulate); break;
    case sp_air: LCD_WriteAnySymbol(s_29x17, 3, 40, p_air); break;
    case sp_setDown: LCD_WriteAnySymbol(s_29x17, 3, 80, p_setDown); break;
    case sp_pumpOff: LCD_WriteAnySymbol(s_35x23, 2, 120, p_pumpOff); break;
    case sp_mud: LCD_WriteAnySymbol(s_35x23, 8, 0, p_mud); break;
    case sp_compressor: LCD_WriteAnySymbol(s_29x17, 9, 40, p_compressor); break;
    case sp_phosphor: LCD_WriteAnySymbol(s_19x19, 9, 85, p_phosphor); break;
    case sp_inflowPump: LCD_WriteAnySymbol(s_35x23, 8, 120, p_inflowPump); break;
    case sp_cal: LCD_WriteAnySymbol(s_29x17, 15, 0, p_cal); break;
    case sp_alarm: LCD_WriteAnySymbol(s_29x17, 15, 40, p_alarm); break;
    case sp_watch: LCD_WriteAnySymbol(s_29x17, 15, 80, p_watch); break;
    case sp_zone: LCD_WriteAnySymbol(s_29x17, 15, 120, p_zone); break;

    case sn_circulate: LCD_WriteAnySymbol(s_29x17, 3, 0, n_circulate); break;
    case sn_air: LCD_WriteAnySymbol(s_29x17, 3, 40, n_air); break;
    case sn_setDown: LCD_WriteAnySymbol(s_29x17, 3, 80, n_setDown); break;
    case sn_pumpOff: LCD_WriteAnySymbol(s_35x23, 2, 120, n_pumpOff); break;
    case sn_mud: LCD_WriteAnySymbol(s_35x23, 8, 0, n_mud); break;
    case sn_compressor: LCD_WriteAnySymbol(s_29x17, 9, 40, n_compressor); break;
    case sn_phosphor: LCD_WriteAnySymbol(s_19x19, 9, 85, n_phosphor); break;
    case sn_inflowPump: LCD_WriteAnySymbol(s_35x23, 8, 120, n_inflowPump); break;
    case sn_cal: LCD_WriteAnySymbol(s_29x17, 15, 0, n_cal); break;
    case sn_alarm: LCD_WriteAnySymbol(s_29x17, 15, 40, n_alarm); break;
    case sn_watch: LCD_WriteAnySymbol(s_29x17, 15, 80, n_watch); break;
    case sn_zone: LCD_WriteAnySymbol(s_29x17, 15, 120, n_zone); break;
    default: break;
  }
}



/* ------------------------------------------------------------------*
 *            Date Time Page
 * ------------------------------------------------------------------*/

void LCD_DateTime(t_DateTime time, unsigned char value)
{
  switch(time)
  {
    case p_h: LCD_WriteAnyValue(f_6x8_p, 2, 9, 10, value); break;
    case p_min: LCD_WriteAnyValue(f_6x8_p, 2, 9, 50, value); break;
    case p_day: LCD_WriteAnyValue(f_6x8_p, 2, 15, 10, value); break;
    case p_month: LCD_WriteAnyValue(f_6x8_p, 2, 15, 50, value); break;
    case p_year: LCD_WriteAnyValue(f_6x8_p, 2, 15, 96, value); break;

    case n_h: LCD_WriteAnyValue(f_6x8_n, 2, 9, 10, value); break;
    case n_min: LCD_WriteAnyValue(f_6x8_n, 2, 9, 50, value); break;
    case n_day: LCD_WriteAnyValue(f_6x8_n, 2, 15, 10, value); break;
    case n_month: LCD_WriteAnyValue(f_6x8_n, 2, 15, 50, value); break;
    case n_year: LCD_WriteAnyValue(f_6x8_n, 2, 15, 96, value); break;
    default: break;
  }
}



/* ==================================================================*
 *            Data Page
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Set Data Pages
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Page(void)
{
  LCD_Sym_MarkTextButton(Data);
  LCD_Clean();
  LCD_Sym_MarkTextButton(Data);

  // Sx.x.x
  LCD_WriteAnyFont(f_4x6_p, 1, 0, 21);
  LCD_WriteAnyFont(f_4x6_p, 1,8, SV1);
  LCD_WriteAnyFont(f_4x6_p, 1, 16, SV2);
  LCD_WriteAnyFont(f_4x6_p, 1,24, SV3);
  LCD_WriteAnyFont(f_4x6_p, 1,4, 22);
  LCD_WriteAnyFont(f_4x6_p, 1, 12, 22);
  LCD_WriteAnyFont(f_4x6_p, 1,20, 22);

  LCD_WriteAnyStringFont(f_6x8_p, 6, 0, "Choose Data:");

  LCD_Write_TextButton(9, 0, Auto, 1);
  LCD_Write_TextButton(9, 40, Manual, 1);
  LCD_Write_TextButton(9, 80, Setup, 1);
  LCD_Write_TextButton(9, 120, Sonic, 1);
}


/* ------------------------------------------------------------------*
 *            Set Data Auto
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Auto(void)
{
  LCD_ClrSpace(4, 0, 9, 160);

  // header
  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(f_6x8_n, 1, 1, "Date");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 37, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 45, "Time");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 73, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 85, "O2");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 103, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 113, "Err");

  LCD_Write_TextButton(22, 0, Auto, 0);
  LCD_Sym_DataArrows();

  LCD_WriteAnyStringFont(f_4x6_p, 1, 140, "01;16");
  LCD_WriteAutoEntryPage(0);
}


/* ------------------------------------------------------------------*
 *            Set Data Manual
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Manual(void)
{
  LCD_ClrSpace(4, 0, 9, 160);
  
  // header
  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(f_6x8_n, 1, 1, "Date");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 37, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 45, "Begin");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 77, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 88, "End");

  LCD_Write_TextButton(22, 40, Manual, 0);
  LCD_Sym_DataArrows();

  LCD_WriteAnyStringFont(f_4x6_p, 1, 144, "1;3");
  LCD_WriteManualEntryPage(0);
}


/* ------------------------------------------------------------------*
 *            Set Data Setup
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Setup(void)
{
  LCD_ClrSpace(4, 0, 9, 160);

  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(f_6x8_n, 1, 1, "Date");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 37, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 45, "Time");

  LCD_Write_TextButton(22, 80, Setup, 0);
  LCD_Sym_DataArrows();

  LCD_WriteAnyStringFont(f_4x6_p, 1, 144, "1;3");
  LCD_WriteSetupEntryPage(0);
}



/* ------------------------------------------------------------------*
 *            Set Data Sonic
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic(void)
{
  LCD_ClrSpace(1, 0, 12, 160);
  Sonic_Data_Boot(_off);
  LCD_WriteAnyStringFont(f_6x8_p, 1, 91, "[ C]");

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 1, 97, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 1, 124, "[mm]");
  LCD_Write_TextButton(4, 0, Shot, 1);
  LCD_Write_TextButton(10, 0, Auto, 1);
  LCD_Write_TextButton(16, 0, Boot, 1);

  LCD_WriteAnySymbol(s_19x19, 3, 50, p_sonic);
}




/* ------------------------------------------------------------------*
 *            arrows
 * ------------------------------------------------------------------*/

void LCD_Sym_DataArrows(void)
{
  LCD_WriteAnySymbol(s_19x19, 3, 140, p_arrowUp);
  LCD_WriteAnySymbol(s_19x19, 14, 140, p_arrowDown);
  LCD_WriteAnySymbol(s_19x19, 8, 140, p_line);
}


/* ------------------------------------------------------------------*
 *            Data SonicShot
 * ------------------------------------------------------------------*/

void LCD_Data_SonicWrite(t_FuncCmd cmd, int shot)
{
  static unsigned char i = 5;
  static int max = 0;
  static int min = 10000;
  static int max_temp = 0;
  static int min_temp = 10000;
  static unsigned char error = 0;

  //--------------------------------------------------ClearDisplay
  if(cmd == _clear)
  {
    i = 5;
    max = 0;
    min = 10000;
    max_temp = 0;
    min_temp = 10000;
    LCD_ClrSpace(4, 91, 15, 69);

    // message line
    LCD_ClrSpace(17, 50, 4, 60);
  }
  //--------------------------------------------------OneShot
  else if(cmd == _shot)
  {
    LCD_WriteAnyValue(f_4x6_p, 5, i, 126, shot); //WriteOneShot
    i = i + 3;
  }
  //--------------------------------------------------AutoShot
  else if(cmd == _shot1)
  {
    LCD_WriteAnyValue(f_4x6_p, 5, 5, 126, shot);

    // could not meassure a distance
    if (shot <= 0) return;

    // limits for updating min and max
    if((shot > (max + D_LIM)) || (shot < (min - D_LIM))) error++;
    else error = 0;

    // tries to accept the new distance
    if(error > 2) error = 0;
    if(error) return;

    // new max and min
    if(shot > max)
    {
      max = shot;
      LCD_WriteAnyValue(f_4x6_p, 5, 11, 126, max);
    }
    if(shot < min)
    {
      min = shot;
      LCD_WriteAnyValue(f_4x6_p, 5, 17, 126, min);
    }
  }
  //--------------------------------------------------Temperature
  else if(cmd == _temp)
  {
    if(i > 17)
    {
      i = 5;
      LCD_ClrSpace(4, 95, 15, 60);
    }
    if(shot & 0x8000)
    {
      shot &= 0x7FFF;

      //-
      LCD_WriteAnyFont(f_4x6_p, i, 91, 11);
    }
    //.
    LCD_WriteAnyFont(f_4x6_p, i, 102, 22);
    LCD_WriteAnyValue(f_4x6_p, 2, i, 105, (shot & 0x00FF));
    LCD_WriteAnyValue(f_4x6_p, 2, i, 95, ((shot >> 8) & 0x00FF));
  }
  //--------------------------------------------------AutoTemp
  else if(cmd == _temp1)
  {
    unsigned char minus_sign = 0;
    if(shot & 0x8000)
    {
      minus_sign = 1;
      shot &= 0x7FFF;

      //-
      LCD_WriteAnyFont(f_4x6_p, 5, 91, 11);
    }
    LCD_WriteAnyValue(f_4x6_p, 2, 5, 105, (shot & 0x00FF));
    LCD_WriteAnyValue(f_4x6_p, 2, 5, 95, ((shot >> 8) & 0x00FF));
    if(shot > max_temp)
    {
      max_temp = shot;

      //-
      if(minus_sign) LCD_WriteAnyFont(f_4x6_p, 11, 91, 11);
      LCD_WriteAnyValue(f_4x6_p, 2, 11, 105, (max_temp & 0x00FF));
      LCD_WriteAnyValue(f_4x6_p, 2, 11, 95, ((max_temp >> 8) & 0x00FF));
    }
    if(shot < min_temp)
    {
      min_temp = shot;

      //-
      if(minus_sign) LCD_WriteAnyFont(f_4x6_p, 17, 91, 11);
      LCD_WriteAnyValue(f_4x6_p, 2, 17, 105, (min_temp & 0x00FF));
      LCD_WriteAnyValue(f_4x6_p, 2, 17, 95, ((min_temp >> 8) & 0x00FF));
    }
  }
  //--------------------------------------------------AutoTemp
  else if(cmd == _autotext)
  {
    LCD_Write_TextButton(10, 0, Auto, 0);
    LCD_WriteAnyFont(f_4x6_p, 9, 113, 13); //m
    LCD_WriteAnyFont(f_4x6_p, 9, 117, 15); //a
    LCD_WriteAnyFont(f_4x6_p, 9, 121, 25); //x
    LCD_WriteAnyFont(f_4x6_p, 15, 113, 13); //m
    LCD_WriteAnyFont(f_4x6_p, 15, 117, 23); //i
    LCD_WriteAnyFont(f_4x6_p, 15, 121, 24); //n
    LCD_WriteAnyFont(f_4x6_p, 17, 102, 22); //.
    LCD_WriteAnyFont(f_4x6_p, 11, 102, 22); //.
    LCD_WriteAnyFont(f_4x6_p, 5, 102, 22);  //.
  }
  //--------------------------------------------------Messages
  else if(cmd == _noUS) LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "NoUS");
  else if(cmd == _noBoot) LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "NoBoot");
  else if(cmd == _error) LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "Error");
  else if(cmd == _ok) LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "OK...Cycle Power!");
  else if(cmd == _noData) LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "NoData");
  else if(cmd == _success) LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "Success");
  else if(cmd == _mcp_fail) LCD_WriteAnyStringFont(f_6x8_p, 17, 100, "MCP-fail");
}



/* ==================================================================*
 *            Data Page - Entries
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  reads variable from EEPROM, PageSize: 32Bytes
 * ------------------------------------------------------------------*/

void LCD_WriteAutoEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[8] = {0x00};
  unsigned char i = 0;
  int o2 = 0;

  for(i = 0; i < 8; i++)
  {
    varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i);
  }
  o2 = ((varEnt[5] << 8) | (varEnt[6]));

  //--------------------------------------------------Write-No-Data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    // 11: -
    for(i = 1; i < 130; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11);}
  }

  //--------------------------------------------------Write-Data-
  else
  {
    // day - month
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 1,  varEnt[0]);
    LCD_WriteAnyFont(f_4x6_p, pa, 9, 11);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 13, varEnt[1]);

    // - year |
    LCD_WriteAnyFont(f_4x6_p, pa, 21, 11);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 25, varEnt[2]);
    LCD_WriteAnyFont(f_4x6_p, pa, 38, 12);

    // hour : minutes
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 47, varEnt[3]);
    LCD_WriteAnyFont(f_4x6_p, pa, 55, 10);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 59, varEnt[4]);

    // | 02 in min |
    LCD_WriteAnyFont(f_4x6_p, pa, 74, 12);
    LCD_WriteAnyValue(f_4x6_p, 3, pa, 85, o2);
    LCD_WriteAnyFont(f_4x6_p, pa, 104, 12);

    //------------------------------------------------Write-AlarmCode
    if(varEnt[7] & 0x01) LCD_WriteAnyFont(f_4x6_p, pa, 113, 1);
    else LCD_WriteAnyFont(f_4x6_p, pa, 113, 0);

    if(varEnt[7] & 0x02) LCD_WriteAnyFont(f_4x6_p, pa, 117, 2);
    else LCD_WriteAnyFont(f_4x6_p, pa, 117, 0);

    if(varEnt[7] & 0x04) LCD_WriteAnyFont(f_4x6_p, pa, 121, 3);
    else LCD_WriteAnyFont(f_4x6_p, pa, 121, 0);

    if(varEnt[7] & 0x08) LCD_WriteAnyFont(f_4x6_p, pa, 125, 4);
    else LCD_WriteAnyFont(f_4x6_p, pa, 125, 0);

    if(varEnt[7] & 0x10) LCD_WriteAnyFont(f_4x6_p, pa, 129, 5);
    else LCD_WriteAnyFont(f_4x6_p, pa, 129, 0);
  }
}


/* ------------------------------------------------------------------*
 *            Manual Entry
 * ------------------------------------------------------------------*/

void LCD_WriteManualEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[7] = {0x00};
  unsigned char i = 0;

  for(i = 0; i < 7; i++)
  {
    varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i);
  }

  //---------------------------------------------------------------Write-No-Data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    for(i = 1; i < 108; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11); }
  }

  //-----------------------------------------------------------------Write-Data-
  else
  {
    // day - month
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 1,  varEnt[0]);
    LCD_WriteAnyFont(f_4x6_p, pa, 9, 11);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 13, varEnt[1]);

    // - year |
    LCD_WriteAnyFont(f_4x6_p, pa, 21, 11);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 25, varEnt[2]);
    LCD_WriteAnyFont(f_4x6_p, pa, 38, 12);

    // hour : minutes
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 47, varEnt[3]);
    LCD_WriteAnyFont(f_4x6_p, pa, 55, 10);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 59, varEnt[4]);

    // |
    LCD_WriteAnyFont(f_4x6_p, pa, 78, 12);

    // hour : minutes
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 88, varEnt[5]);
    LCD_WriteAnyFont(f_4x6_p, pa, 96, 10);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 100, varEnt[6]);
  }
}


/* ------------------------------------------------------------------*
 *            Setup Entry
 * ------------------------------------------------------------------*/

void LCD_WriteSetupEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[7] = {0x00};
  unsigned char i = 0;

  for(i = 0; i < 5; i++)
  {
    varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i);
  }

  //---------------------------------------------------------------Write-No-Data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    //--------
    for(i = 1; i < 68; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11); }
  }

  //-----------------------------------------------------------------Write-Data-
  else
  {
    // day - month
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 1,  varEnt[0]);
    LCD_WriteAnyFont(f_4x6_p, pa, 9, 11);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 13, varEnt[1]);

    // - year |
    LCD_WriteAnyFont(f_4x6_p, pa, 21, 11);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 25, varEnt[2]);
    LCD_WriteAnyFont(f_4x6_p, pa, 38, 12);

    // hour : minutes
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 47, varEnt[3]);
    LCD_WriteAnyFont(f_4x6_p, pa, 55, 10);
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 59, varEnt[4]);
  }
}



/* ==================================================================*
 *            MPX - Layout
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            MPX-Text
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX(t_FuncCmd cmd, int value)
{
  //--------------------------------------------------NoMbarInAuto
  if(cmd == _notav)
  {
    LCD_WriteAnyFont(f_4x6_p, 15, 5, 11);   //-
    LCD_WriteAnyFont(f_4x6_p, 15, 9, 11);   //-
    LCD_WriteAnyFont(f_4x6_p, 15, 13, 11);  //-
    LCD_WriteAnyFont(f_4x6_p, 15, 18, 19);  //%
  }

  //--------------------------------------------------mbarInAuto
  else if(cmd == _mbar)
  {
    LCD_WriteAnyValue(f_4x6_p, 3, 17, 43, value);    //Write Value
    LCD_WriteAnyFont(f_4x6_p, 17, 57, 13);    //m
    LCD_WriteAnyFont(f_4x6_p, 17, 61, 14);    //b
    LCD_WriteAnyFont(f_4x6_p, 17, 65, 15);    //a
    LCD_WriteAnyFont(f_4x6_p, 17, 69, 16);    //r
  }

  //--------------------------------------------------mbarInManual
  else if(cmd == _mmbar)
  {
    LCD_WriteAnyValue(f_6x8_p, 3, 17, 2, value);
    LCD_WriteAnyStringFont(f_6x8_p, 17,22,"%");
  }

  //--------------------------------------------------2ControlValue
  else if(cmd == _debug)
  {
    LCD_WriteAnyValue(f_4x6_p, 3, 15, 5, value);   //Write
    LCD_WriteAnyFont(f_4x6_p, 15, 18, 19);            //%
  }
}



/* ==================================================================*
 *            Pin Page
 * ==================================================================*/

void LCD_pPinButtons(unsigned char pPin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol = frame;
  unsigned char num = 0;

  switch(pPin)
  {
    case 1: row = 2; col = 0; any_symbol = frame; num = 0x01; break;
    case 4: row = 8; col = 0; any_symbol = frame; num = 0x04; break;
    case 7: row = 14; col = 0; any_symbol = frame; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = p_escape; num = 0x20; break;

    case 2: row = 2; col = 40; any_symbol = frame; num = 0x02; break;
    case 5: row = 8; col = 40; any_symbol = frame; num = 0x05; break;
    case 8: row = 14; col = 40; any_symbol = frame; num = 0x08; break;
    case 0: row = 20; col = 40; any_symbol = frame; num = 0x00; break;

    case 3: row = 2; col = 80; any_symbol = frame; num = 0x03; break;
    case 6: row = 8; col = 80; any_symbol = frame; num = 0x06; break;
    case 9: row = 14; col = 80; any_symbol = frame; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = p_del; num = 0x20; break;
    default: break;
  }

  // write symbol
  LCD_WriteAnySymbol(s_34x21, row, col, any_symbol);

  // write number if it is one
  if(num < 0x20) LCD_WriteAnyFont(f_8x16_p, row + 1, col + 13, num);
}


void LCD_nPinButtons(unsigned char nPin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol = frame;
  unsigned char num = 0;

  switch(nPin)
  {
    case 1: row = 2; col = 0; any_symbol = black; num = 0x01; break;
    case 4: row = 8; col = 0; any_symbol = black; num = 0x04; break;
    case 7: row = 14; col = 0; any_symbol = black; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = n_escape; num = 0x20; break;

    case 2: row = 2; col = 40; any_symbol = black; num = 0x02; break;
    case 5: row = 8; col = 40; any_symbol = black; num = 0x05; break;
    case 8: row = 14; col = 40; any_symbol = black; num = 0x08; break;
    case 0: row = 20; col = 40; any_symbol = black; num = 0x00; break;

    case 3: row = 2; col = 80; any_symbol = black; num = 0x03; break;
    case 6: row = 8; col = 80; any_symbol = black; num = 0x06; break;
    case 9: row = 14; col = 80; any_symbol = black; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = n_del; num = 0x20; break;
    default: break;
  }

  LCD_WriteAnySymbol(s_34x21, row, col, any_symbol);

  // write number if it is one
  if(num < 0x20) LCD_WriteAnyFont(f_8x16_n, row + 1, col + 13, num);
}



/* ==================================================================*
 *            Set Pin Page
 * ==================================================================*/

void LCD_PinSet_Page(void)
{
  unsigned char i = 0;

  LCD_Clean();
  for(i = 0; i < 12; i++) LCD_pPinButtons(i);
  Eval_PinDel();
}


/* ------------------------------------------------------------------*
 *            Pin-Write
 * ------------------------------------------------------------------*/

void LCD_Pin_Write(t_FuncCmd cmd, struct TelNr nr)
{
  unsigned char i = 0;

  switch(cmd)
  {
    case _right: LCD_WriteAnyStringFont(f_6x8_p, 6, 125,"right!"); break;

    case _op: LCD_WriteAnyStringFont(f_6x8_p, 6, 119,"OP"); break;

    //------------------------------------------------WriteWholeNumber
    case _write:
      LCD_WriteAnyStringFont(f_6x8_p, 6, 119, "Tel");
      LCD_WriteAnyFont(f_6x8_p, 6, 137, nr.id + 14);
      LCD_WriteAnyStringFont(f_6x8_p, 6, 143,":");

      // +
      LCD_WriteAnyFont(f_4x6_p, 9, 119, 20); 

      // 43
      for(i = 0; i < 2; i++)
      {
        nr.pos = i;
        LCD_WriteAnyFont(f_4x6_p, 9, 123 + 4 * i, Modem_TelNr(_read2, nr));
      }

      // 680
      for(i = 2; i < 5; i++)
      {               
        nr.pos = i;
        LCD_WriteAnyFont(f_4x6_p, 9, 135 + 4 * (i - 2), Modem_TelNr(_read2, nr));
      }

      // number
      for(i = 5; i < 14; i++)
      {                   
        nr.pos = i;
        LCD_WriteAnyFont(f_4x6_p, 11, 119 + 4 * (i - 5), Modem_TelNr(_read2, nr));
      }
      break;

    //------------------------------------------------PrintOneNumber
    case _telnr:
      LCD_nPinButtons(nr.tel);
      if(nr.pos < 2)
      {
        LCD_WriteAnyFont(f_4x6_p, 9, 123 + 4 * nr.pos, nr.tel);
        nr.pos++;
        if(nr.pos < 2) LCD_WriteAnyFont(f_4x6_n, 9, 127, 14);
        else LCD_WriteAnyFont(f_4x6_n, 9, 135, 14);
      }

      else if((nr.pos > 1) && (nr.pos < 5))
      {
        LCD_WriteAnyFont(f_4x6_p, 9, 135 + 4 * (nr.pos - 2), nr.tel);
        nr.pos++;
        if(nr.pos < 5) LCD_WriteAnyFont(f_4x6_n, 9, 135 + 4 * (nr.pos - 2), 14);
        else LCD_WriteAnyFont(f_4x6_n, 11, 119, 14);
      }
      else
      {
        LCD_WriteAnyFont(f_4x6_p, 11, 119 + 4 * (nr.pos - 5), nr.tel);
        nr.pos++;
        if(nr.pos < 14) LCD_WriteAnyFont(f_4x6_n, 11, 119 + 4 * (nr.pos - 5), 14);
      }
      break;

    case _wrong:
      LCD_WriteAnyStringFont(f_6x8_p, 6, 119,"wrong!");
      LCD_WriteAnyStringFont(f_6x8_p, 9, 119,"try");
      LCD_WriteAnyStringFont(f_6x8_p, 11, 119,"again"); break;

    case _clear: LCD_ClrSpace(6, 119,25,41); break;

    default: break;
  }
}



/* ==================================================================*
 *            Control Utilities
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            text buttons
 * ------------------------------------------------------------------*/

void LCD_TextButton(t_textButtons text, unsigned char pos)
{
  if(pos)
  {
    switch(text)
    {
      case Auto: LCD_Write_TextButton(22, 0, Auto, 1); break;
      case Manual: LCD_Write_TextButton(22, 40, Manual, 1); break;
      case Setup: LCD_Write_TextButton(22, 80, Setup, 1); break;
      case Data: LCD_Write_TextButton(22, 120, Data, 1); break;
      default: break;
    }
  }
  else
  {
    switch(text)
    {
      case Auto: LCD_Write_TextButton(22, 0, Auto, 0); break;
      case Manual: LCD_Write_TextButton(22, 40, Manual, 0); break;
      case Setup: LCD_Write_TextButton(22, 80, Setup, 0); break;
      case Data: LCD_Write_TextButton(22, 120, Data, 0); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            Control Buttons
 * ------------------------------------------------------------------*/

void LCD_ControlButtons(t_CtrlButtons ctrl)
{
  switch(ctrl)
  {
    case sp_esc: LCD_WriteAnySymbol(s_19x19, 3, 90, p_esc); break;
    case sp_ok: LCD_WriteAnySymbol(s_19x19, 3, 130, p_ok); break;
    case sp_minus: LCD_WriteAnySymbol(s_19x19, 9, 90, p_minus); break;
    case sp_plus: LCD_WriteAnySymbol(s_19x19, 9, 130, p_plus); break;

    case sn_esc: LCD_WriteAnySymbol(s_19x19, 3, 90, n_esc); break;
    case sn_ok: LCD_WriteAnySymbol(s_19x19, 3, 130, n_ok); break;
    case sn_minus: LCD_WriteAnySymbol(s_19x19, 9, 90, n_minus); break;
    case sn_plus: LCD_WriteAnySymbol(s_19x19, 9, 130, n_plus); break;

    default: break;
  }
}

void LCD_ControlButtons2(t_CtrlButtons ctrl)
{
  switch(ctrl)
  {
    case sp_esc: LCD_WriteAnySymbol(s_19x19,  3,  90, p_esc); break;
    case sp_ok: LCD_WriteAnySymbol(s_19x19,  3,  130, p_ok); break;
    case sp_minus: LCD_WriteAnySymbol(s_19x19,  15, 130, p_minus); break;
    case sp_plus: LCD_WriteAnySymbol(s_19x19,  9,  130, p_plus); break;

    case sn_esc: LCD_WriteAnySymbol(s_19x19,  3,  90, n_esc); break;
    case sn_ok: LCD_WriteAnySymbol(s_19x19,  3,  130, n_ok); break;
    case sn_minus: LCD_WriteAnySymbol(s_19x19,  15, 130, n_minus); break;
    case sn_plus: LCD_WriteAnySymbol(s_19x19,  9,  130, n_plus); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Pin Okay
 * ------------------------------------------------------------------*/

void LCD_Pin_WriteOK(unsigned char on)
{
  switch(on)
  {
    case 0: LCD_WriteAnySymbol(s_19x19, 20, 130, p_ok); break;
    case 1: LCD_WriteAnySymbol(s_19x19, 20, 130, n_ok); break;
    default: LCD_WriteAnySymbol(s_19x19, 20, 130, n_ok); break;
  }
}


/* ------------------------------------------------------------------*
 *            Write Control Buttons App
 * ------------------------------------------------------------------*/

void LCD_WriteCtrlButton(void)
{
  for(unsigned char i = 4; i < 9; i++){ LCD_ControlButtons(i); }
}

void LCD_WriteCtrlButton2(void)
{
  for(unsigned char i = 4; i < 9; i++){ LCD_ControlButtons2(i); }
}


/* ------------------------------------------------------------------*
 *            On/Off Values
 * ------------------------------------------------------------------*/

void LCD_OnValue(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "ON:");
  LCD_WriteAnyValue(f_6x8_p, 2, 10, 19, value);
}

void LCD_OffValue(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_p, 10, 40, "OFF:");
  LCD_WriteAnyValue(f_6x8_p, 2, 10, 64, value);
}

void LCD_OnValueNeg(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_n, 10, 1, "ON:");
  LCD_WriteAnyValue(f_6x8_n, 2, 10, 19, value);
}

void LCD_OffValueNeg(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_n, 10, 40, "OFF:");
  LCD_WriteAnyValue(f_6x8_n, 2, 10, 64, value);
}



/* ==================================================================*
 *            Mark
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Mark Text Button
 * ------------------------------------------------------------------*/

void LCD_Sym_MarkTextButton(t_textButtons text)
{
  for(unsigned char i = 0; i < 4; i++)
  {
    if(i != text) LCD_TextButton(i, 1);
  }
  LCD_TextButton(text, 0);
}


/* ------------------------------------------------------------------*
 *            Mark Manual Symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Mark_ManualSymbol(t_SetupSym sym)
{
  for(unsigned char i = 12; i < 20; i++)
  {
    // write negative / positive Manual Symbol
    if((12 + sym == i)) LCD_Sym_SetupSymbols(sym);
    else LCD_Sym_SetupSymbols(i);
  }
}



/* ==================================================================*
 *            Sonic - General
 * ==================================================================*/

unsigned char LCD_Sym_NoUS(t_page page, t_FuncCmd cmd)
{
  static unsigned char errC = 0;
  unsigned char row = 0;
  unsigned char col = 0;

  switch(page)
  {
    case AutoSetDown: case AutoMud: case AutoPumpOff:
    case AutoCirc: case AutoCircOff: case AutoPage:
    case AutoAir: case AutoAirOff: row = 17; col = 5; break;

    case ManualMain: case ManualCirc: case ManualCircOff:
    case ManualAir: case ManualSetDown: case ManualPumpOff:
    case ManualPumpOff_On: case ManualMud: case ManualCompressor:
    case ManualPhosphor: case ManualInflowPump: row = 17; col = 2; break;

    case SetupCal: row = 0; col = 38; break;
    default: break;
  }

  if(cmd == _clear)
  {
    if(errC){ errC = 0; LCD_ClrSpace(row, col, 2, 24); }
  }
  //--------------------------------------------------Write
  else if(cmd == _write)
  {
    errC++;
    if(errC == 10) LCD_WriteAnyStringFont(f_6x8_p, row, col, "noUS");
  }
  //--------------------------------------------------Check
  else if(cmd == _check)
  {
    if(errC > 20) return 1;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            Auto UltraSonic Value
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SonicVal(t_page page, int sonic)
{
  int zero = 0;
  int dif = 0;
  int per = 0;
  int cal = 0;
  int lvO2 = 0;

  // deactivated sonic
  if(!MEM_EEPROM_ReadVar(SONIC_on)) return;
  //--------------------------------------------------mm
  switch(page)
  {
    case AutoZone:  case AutoSetDown: case AutoPumpOff:
    case AutoMud:   case AutoCirc:    case AutoCircOff:
    case AutoAir:   case AutoAirOff:  case AutoPage:
      LCD_WriteAnyValue(f_4x6_p, 4, 17, 5, sonic);

      // mm
      LCD_WriteAnyFont(f_4x6_p, 17, 22, 13);
      LCD_WriteAnyFont(f_4x6_p, 17, 26, 13);
      break;

    default: break;
  }

  // percentage
  zero = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) | (MEM_EEPROM_ReadVar(SONIC_L_LV)));
  lvO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | (MEM_EEPROM_ReadVar(TANK_L_O2)));
  
  //water-level-difference
  dif = lvO2 * 10;
  cal = sonic - (zero - (lvO2 * 10));
  if(sonic > zero) per = 0;
  else per = 100 - ((cal * 10) / dif) * 10;
  if(!sonic) per = 0;
  switch(page)
  {
    case AutoZone:  case AutoSetDown: case AutoPumpOff:
    case AutoMud:   case AutoCirc:    case AutoCircOff:
    case AutoAir:   case AutoAirOff:  case AutoPage:
      LCD_WriteAnyValue(f_4x6_p, 3, 15, 5, per);

      // %
      LCD_WriteAnyFont(f_4x6_p, 15, 18, 19);
      break;

    case ManualMain:  case ManualCirc:  case ManualCircOff:
    case ManualAir:   case ManualSetDown: case ManualPumpOff:
    case ManualPumpOff_On:  case ManualMud: case ManualCompressor:
    case ManualPhosphor:  case ManualInflowPump:
      LCD_WriteAnyValue(f_6x8_p, 3, 17, 2, per);
      LCD_WriteAnyStringFont(f_6x8_p, 17, 22, "%");
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Error Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Error(unsigned char err)
{
  // temp
  if(err & E_T)
  {
    LCD_WriteAnySymbol(s_19x19, 16, 134, n_grad);
  }

  // over-pressure or under-pressure
  if((err & E_OP) || (err & E_UP))
  {
    LCD_ClrSpace(6, 44, 6, 35);
    LCD_WriteAnySymbol(s_29x17, 6, 45, n_alarm);
  }

  // max in tank
  if(err & E_IT)
  {
    LCD_WriteAnySymbol(s_29x17, 17, 1, n_alarm);
    LCD_TextButton(Auto, 0);
  }

  // max out tank
  if(err & E_OT)
  {
    LCD_WriteAnySymbol(s_29x17, 17, 90, n_alarm);
    LCD_TextButton(Setup, 0);
  }
}


/* ------------------------------------------------------------------*
 *            auto time symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Time(t_FuncCmd cmd)
{
  unsigned char time = 0;

  switch(cmd)
  {
    case _init:

      // : symbol
      LCD_WriteAnyFont(f_4x6_p, 2, 128, 10);
      LCD_WriteAnyFont(f_4x6_p, 2, 140, 10);

      // time
      LCD_WriteAnyValue(f_4x6_p, 2, 2,120, MCP7941_ReadTime(TIC_HOUR));
      LCD_WriteAnyValue(f_4x6_p, 2, 2,132, MCP7941_ReadTime(TIC_MIN));
      LCD_WriteAnyValue(f_4x6_p, 2, 2,144, MCP7941_ReadTime(TIC_SEC));
      break;

    case _exe:

      // sec
      time = MCP7941_ReadTime(TIC_SEC);
      LCD_WriteAnyValue(f_4x6_p, 2, 2,144, time);

      // min
      if(!time)
      {
        time= MCP7941_ReadTime(TIC_MIN);
        LCD_WriteAnyValue(f_4x6_p, 2, 2,132, time);
      }
      else break;

      // hour
      if(!time)
      {
        time= MCP7941_ReadTime(TIC_HOUR);
        LCD_WriteAnyValue(f_4x6_p, 2, 2,132, time);
      }
      else break;
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            auto date symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Date(void)
{
  // - symbol
  LCD_WriteAnyFont(f_4x6_p, 0, 128, 11);
  LCD_WriteAnyFont(f_4x6_p, 0, 140, 11);

  // year 20xx
  LCD_WriteAnyStringFont(f_4x6_p, 0, 144, "20");

  // date
  LCD_WriteAnyValue(f_4x6_p, 2, 0,120, MCP7941_ReadTime(TIC_DATE));
  LCD_WriteAnyValue(f_4x6_p, 2, 0,132, MCP7941_ReadTime(TIC_MONTH));
  LCD_WriteAnyValue(f_4x6_p, 2, 0,152, MCP7941_ReadTime(TIC_YEAR));
}


/* ------------------------------------------------------------------*
 *            end text sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_EndText(void)
{
  LCD_WriteAnyStringFont(f_6x8_p, 15, 1, "End");
}