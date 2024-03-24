// --
// lcd symbolds

#include <avr/io.h>

#include "lcd_sym.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_app.h"
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
  if(!COMPANY){ LCD_WriteAnySymbol(0, 0, _s_logo_purator, false); }
  else{ LCD_WriteAnySymbol(0, 0, _s_logo_hecs, false); }
}


/* ------------------------------------------------------------------*
 *            clear info space
 * ------------------------------------------------------------------*/

void LCD_Sym_Clr_InfoSpace(void){ LCD_ClrSpace(0, 0, LCD_SPEC_MAX_PAG - 4, LCD_SPEC_MAX_COL); }
void LCD_Sym_Clr_DataEntrySpace(void){ LCD_ClrSpace(4, 1, LCD_SPEC_MAX_PAG - 8, 135); }


/* ------------------------------------------------------------------*
 *            auto symbol set manager
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetManager(struct PlantState *ps, t_page new_page)
{
  // page dependend symbols
  switch(new_page)
  {
    case AutoPage: LCD_Sym_Auto_Main(); break;
    case AutoZone: LCD_Sym_Auto_Zone(); break;
    case AutoSetDown: LCD_Sym_Auto_SetDown(); break;
    case AutoPumpOff: LCD_Sym_Auto_PumpOff(); break;
    case AutoMud: LCD_Sym_Auto_Mud(); break;
    case AutoAir: (ps->air_circ_state->ac_state == _ac_on ? LCD_Sym_Auto_AirOn() : LCD_Sym_Auto_AirOff()); LCD_Sym_Auto_AirTime_Print(ps->air_circ_state->air_tms); break;
    case AutoCirc: (ps->air_circ_state->ac_state == _ac_on ? LCD_Sym_Auto_CircOn() : LCD_Sym_Auto_CircOff()); LCD_Sym_Auto_AirTime_Print(ps->air_circ_state->air_tms); break;
    default: return;
  }

  // time and date
  LCD_Sym_Auto_WorldTime_Print(ps);
  LCD_Sym_Auto_Date(ps);

  // page time
  LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);
  
  // pressure
  LCD_Sym_Auto_MPX_AverageValue(ps->mpx_state->actual_mpx_av);
  LCD_Sym_Auto_MPX_AverageValue_Mbar();

  // compressor hours
  LCD_Sym_Auto_Compressor_OpHours(ps->compressor_state->operation_time->hou);

  // read water level
  LCD_Sym_Auto_Tank_LevelPerc(false, 0);
  LCD_Sym_Auto_SonicVal(ps);

  // ip sensor
  if(ps->settings->settings_inflow_pump->sensor_out_tank){ LCD_WriteAnySymbol(16, 90, _s_sensor, true); }

  // phosphor symbols
  LCD_Sym_Auto_Phosphor_Symbols(ps);

  // inflow pump symbols
  LCD_Sym_Auto_InflowPump_Symbols(ps);
}


/* ------------------------------------------------------------------*
 *            auto page time
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PageTime_Print(struct Tms *tms)
{
  unsigned char col = 0;
  unsigned char row = 13;
  int h = tms->min / 60;
  int min = tms->min % 60;
  LCD_WriteAnyValue(_f_4x6, 2, row, col, h, false);
  col += 8;
  LCD_WriteAnyFont(_f_4x6, row, col, 10, false);
  col += 4;
  LCD_WriteAnyValue(_f_4x6, 2, row, col, min, false);
  col += 8;
  LCD_WriteAnyFont(_f_4x6, row, col, 10, false);
  col += 4;
  LCD_WriteAnyValue(_f_4x6, 2, row, col, tms->sec, false);
}


/* ------------------------------------------------------------------*
 *            auto air time
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_AirTime_Print(struct Tms *tms)
{
  unsigned char col = 12;
  unsigned char row = 11;
  LCD_WriteAnyValue(_f_4x6, 2, row, col, tms->min, false);
  col += 8;
  LCD_WriteAnyFont(_f_4x6, row, col, 10, false);
  col += 4;
  LCD_WriteAnyValue(_f_4x6, 2, row, col, tms->sec, false);
}


/* ------------------------------------------------------------------*
 *            tank
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Tank_LevelPerc(bool enabled, int level_perc)
{
  unsigned char row = 15;
  unsigned char col = 0;
  if(enabled){ LCD_WriteAnyValue(_f_4x6, 3, row, col, level_perc, false); }
  else{ for(unsigned char i = col; i < (col + 12); i += 4){ LCD_WriteAnyFont(_f_4x6, row, i, 11, false); } }
  col += 12;
  LCD_WriteAnyFont(_f_4x6, row, col, 19, false);
}

void LCD_Sym_Auto_Tank_SonicDmm(int value)
{
  unsigned char row = 17;
  unsigned char col = 0;
  LCD_WriteAnyValue(_f_4x6, 4, row, col, value, false);
  col += 16;
  LCD_WriteAnyFont(_f_4x6, row, col, 13, false);
  col += 4;
  LCD_WriteAnyFont(_f_4x6, row, col, 13, false);
}


/* ------------------------------------------------------------------*
 *            mpx
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_MPX_AverageValue(int value)
{ 
  unsigned char row = 13;
  unsigned char col = 43;
  // negative / positive pressure
  if(value < 0){ value = -value; LCD_WriteAnyFont(_f_4x6, row, col - 4, 11, false); }
  else{ LCD_ClrSpace(row, col - 4, 2, 4); }
  LCD_WriteAnyValue(_f_4x6, 3, 13, 43, value, false);
}

void LCD_Sym_Auto_MPX_AverageValue_Mbar(void){ for(unsigned char i = 0; i < 4; i++){ LCD_WriteAnyFont(_f_4x6, 13, 57 + i * 4, 13 + i, false); } }


/* ------------------------------------------------------------------*
 *            pagetime update
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PageTime_Update(struct PlantState *ps)
{
  // sec change
  if(ps->time_state->tic_sec_update_flag)
  {
    // time
    LCD_Sym_Auto_WorldTime_Print(ps);
    LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);
  }
}


/* ------------------------------------------------------------------*
 *            set auto page
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Main(void)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_auto);
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Logo();
}


/* ------------------------------------------------------------------*
 *            auto compressor symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Compressor(bool negative){ LCD_WriteAnySymbol(6, 45, _s_compressor, negative); }
void LCD_Sym_Auto_Compressor_OpHours(int value){ LCD_WriteAnyValue(_f_4x6, 5, 15, 43, value, false); LCD_WriteAnyFont(_f_4x6, 15, 69, 17, false); }


/* ------------------------------------------------------------------*
 *            auto clear spaces
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_ClrActualCycleSpace(void){ LCD_ClrSpace(5, 0, 1, 35); }


/* ------------------------------------------------------------------*
 *            set auto zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Zone(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _s_circulate, true);
  LCD_Sym_Auto_Compressor(true);
  LCD_WriteAnyStringFont(_f_6x8, 8, 1, "z", true);
}


/* ------------------------------------------------------------------*
 *            set auto set down
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetDown(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _s_set_down, true);
  LCD_Sym_Auto_Compressor(false);
}


/* ------------------------------------------------------------------*
 *            set auto PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PumpOff(void)
{
  LCD_WriteAnySymbol(5, 0, _s_pump_off, true);
  LCD_Sym_Auto_Compressor((bool)!MEM_EEPROM_ReadVar(PUMP_pumpOff));
}


/* ------------------------------------------------------------------*
 *            set auto Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Mud(void)
{
  LCD_WriteAnySymbol(5, 0, _s_mud, true);
  LCD_Sym_Auto_Compressor(true);
}


/* ------------------------------------------------------------------*
 *            circulate and air symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CircOn(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _s_circulate, true);
  LCD_Sym_Auto_Compressor(true);
}

void LCD_Sym_Auto_CircOff(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _s_circulate, false);
  LCD_Sym_Auto_Compressor(false);
}

void LCD_Sym_Auto_AirOn(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _s_air, true);
  LCD_Sym_Auto_Compressor(true);
}

void LCD_Sym_Auto_AirOff(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _s_air, false);
  LCD_Sym_Auto_Compressor(false);
}


/* ------------------------------------------------------------------*
 *            Inflow Pump Symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_InflowPump_Symbols(struct PlantState *ps)
{
  // handlers
  t_page p = ps->page_state->page;
  t_inflow_pump_states ip_state = ps->inflow_pump_state->ip_state;

  switch(ps->settings->settings_inflow_pump->pump)
  {
    case 0: LCD_Sym_Auto_InflowPump_Comp(ip_state == _ip_on); LCD_Sym_Auto_Compressor((ip_state == _ip_on && (p == AutoAir || p == AutoCirc))); break;
    case 1: LCD_Sym_Auto_InflowPump_Pump1(ip_state == _ip_on); break;
    case 2: LCD_Sym_Auto_InflowPump_Pump2(ip_state == _ip_on); break;
    default: break;
  }

  // time
  LCD_Sym_Auto_Ip_Time(0x07, ps->inflow_pump_state->ip_thms);
}

void LCD_Sym_Auto_InflowPump_Comp(bool negative){ LCD_WriteAnySymbol(5, 89, _s_inflow_pump, negative); }
void LCD_Sym_Auto_InflowPump_Pump1(bool negative){ LCD_WriteAnySymbol(5, 90, _s_pump, negative); }
void LCD_Sym_Auto_InflowPump_Pump2(bool negative){ LCD_WriteAnySymbol(5, 90, _s_pump2, negative); }


/* ------------------------------------------------------------------*
 *            Inflow Pump Var
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ip_Time(unsigned char cho, struct Thms *t_hms)
{
  // sec, min, h
  if(cho & 0x01){ LCD_WriteAnyValue(_f_4x6, 2, 13, 109, t_hms->sec, false); }
  LCD_WriteAnyFont(_f_4x6, 13, 105, 10, false);
  if(cho & 0x02){ LCD_WriteAnyValue(_f_4x6, 2, 13, 97, t_hms->min, false); }
  LCD_WriteAnyFont(_f_4x6, 13, 93, 10, false);
  if(cho & 0x04){ LCD_WriteAnyValue(_f_4x6, 2, 13, 85, t_hms->hou, false); }
}


/* ------------------------------------------------------------------*
 *            Phosphor Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Phosphor_Symbols(struct PlantState *ps)
{
  LCD_Sym_Auto_Phosphor_Symbol(ps->phosphor_state->ph_state == _ph_on);
  LCD_Sym_Auto_Ph_Time(ps->phosphor_state->ph_tms);
}

void LCD_Sym_Auto_Phosphor_Symbol(bool negative){ LCD_WriteAnySymbol(6, 134, _s_phosphor, negative);  }


/* ------------------------------------------------------------------*
 *            Phosphor time
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ph_Time(struct Tms *tms)
{
  LCD_Sym_Auto_Ph_Time_Min(tms->min);
  LCD_WriteAnyFont(_f_4x6, 13, 143, 10, false);
  LCD_Sym_Auto_Ph_Time_Sec(tms->sec);
}

void LCD_Sym_Auto_Ph_Time_Min(int min){ LCD_WriteAnyValue(_f_4x6, 2, 13, 135, min, false); }
void LCD_Sym_Auto_Ph_Time_Sec(int sec){ LCD_WriteAnyValue(_f_4x6, 2, 13, 147, sec, false); }


/* ------------------------------------------------------------------*
 *            manual symbol data
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Manual_GetSymbolPosition(t_any_symbol sym)
{
  struct RowColPos position = { .row = 0, .col = 0 };

  switch(sym)
  {
    case _s_circulate: position.row = 3; position.col = 0; break;
    case _s_air: position.row = 3; position.col = 40; break;
    case _s_set_down: position.row = 3; position.col = 80; break;
    case _s_pump_off: position.row = 2; position.col = 120; break;
    case _s_mud: position.row = 8; position.col = 0; break;
    case _s_compressor: position.row = 9; position.col = 40; break;
    case _s_phosphor: position.row = 9; position.col = 85; break;
    case _s_inflow_pump: position.row = 8; position.col = 120; break;
    case _s_valve: position.row = 15; position.col = 0; break;
    default: break;
  }
  return position;
}


/* ------------------------------------------------------------------*
 *            Mark manual Select
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Draw(t_any_symbol sym, bool negative)
{
  struct RowColPos position = LCD_Sym_Manual_GetSymbolPosition(sym);
  LCD_WriteAnySymbol(position.row, position.col, sym, negative);
}


/* ------------------------------------------------------------------*
 *            Mark manual Select
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageDraw(t_page page, bool negative)
{
  t_any_symbol sym = _s_none_symbol;
  switch(page)
  {
    case ManualMain:  break;
    case ManualCirc: sym = _s_circulate; break;
    case ManualAir: sym = _s_air; break;
    case ManualSetDown: sym = _s_set_down; break;
    case ManualPumpOff: sym = _s_pump_off; break;
    case ManualPumpOff_On: break;
    case ManualMud: sym = _s_mud; break;
    case ManualCompressor: sym = _s_compressor; break;
    case ManualPhosphor: sym = _s_phosphor; break;
    case ManualInflowPump: sym = _s_inflow_pump; break;
    case ManualValveTest: sym = _s_valve; break;
    default: break;
  }
  LCD_Sym_Manual_Draw(sym, negative);
}


/* ------------------------------------------------------------------*
 *            all manual symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_AllSymbols(void)
{
  t_any_symbol symbols[9] = { _s_circulate, _s_air, _s_set_down, _s_pump_off, _s_mud, _s_compressor, _s_phosphor, _s_inflow_pump, _s_valve };
  for(unsigned char i = 0; i < 9; i++){ LCD_Sym_Manual_Draw(symbols[i], false); }
}


/* ------------------------------------------------------------------*
 *            manual symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Main(struct PlantState *ps)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_manual);

  // positive setup symbols
  LCD_Sym_Manual_AllSymbols();
  LCD_Sym_Manual_Text(ps);
}


/* ------------------------------------------------------------------*
 *            manual text
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Text(struct PlantState *ps)
{
  LCD_Sym_Manual_PageTime_Print(ps);
  LCD_WriteAnyStringFont(_f_6x8, 17, 136, ":", false);
  LCD_WriteAnyStringFont(_f_6x8, 17, 61, "mbar", false);
}


/* ------------------------------------------------------------------*
 *            page time
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageTime_Min(int min){ LCD_WriteAnyValue(_f_6x8, (min >= 100 ? 3 : 2), 17, (min >= 100 ? 118 : 124), min, false); }
void LCD_Sym_Manual_PageTime_Sec(int sec){ LCD_WriteAnyValue(_f_6x8, 2, 17, 142, sec, false); }


/* ------------------------------------------------------------------*
 *            page time print
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageTime_Print(struct PlantState *ps)
{
  LCD_Sym_Manual_PageTime_Min(ps->page_state->page_time->min);
  LCD_Sym_Manual_PageTime_Sec(ps->page_state->page_time->sec);
}


/* ------------------------------------------------------------------*
 *            manual page time
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageTime_Update(struct PlantState *ps)
{
  // sec change
  if(ps->time_state->tic_sec_update_flag)
  {
    // second
    LCD_Sym_Manual_PageTime_Sec(ps->page_state->page_time->sec);

    // minute
    if(ps->page_state->page_time->sec == 59)
    {
      LCD_Sym_Manual_PageTime_Min(ps->page_state->page_time->min);
    }
  }
}


/* ------------------------------------------------------------------*
 *            manual okay button
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PumpOff_OkButton(bool negative){ LCD_WriteAnySymbol(15, 85, _s_ok, negative); }
void LCD_Sym_Manual_PumpOff_OkButton_Clr(void){ LCD_ClrSpace(15, 85, 5, 104); }


/* ------------------------------------------------------------------*
 *            mpx and sonic
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_MPX_AverageValue(int value)
{ 
  unsigned char row = 17;
  unsigned char col = 42;
  // negative / positive pressure
  if(value < 0){ value = -value; LCD_WriteAnyStringFont(_f_6x8, row, col - 6, "-", false); }
  else{ LCD_ClrSpace(row, col - 6, 2, 6); }
  LCD_WriteAnyValue(_f_6x8, 3, row, col, value, false);
}

void LCD_Sym_Manual_Tank_LevelPerc(int value){ LCD_WriteAnyValue(_f_6x8, 3, 17, 2, value, false); LCD_WriteAnyStringFont(_f_6x8, 17, 22, "%", false); }


/* ------------------------------------------------------------------*
 *            manual symbol data
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Setup_GetSymbolPosition(t_any_symbol sym)
{
  struct RowColPos position = { .row = 0, .col = 0 };

  switch(sym)
  {
    case _s_circulate: position.row = 3; position.col = 0; break;
    case _s_air: position.row = 3; position.col = 40; break;
    case _s_set_down: position.row = 3; position.col = 80; break;
    case _s_pump_off: position.row = 2; position.col = 120; break;
    case _s_mud: position.row = 8; position.col = 0; break;
    case _s_compressor: position.row = 9; position.col = 40; break;
    case _s_phosphor: position.row = 9; position.col = 85; break;
    case _s_inflow_pump: position.row = 8; position.col = 120; break;
    case _s_cal: position.row = 15; position.col = 0; break;
    case _s_alarm: position.row = 15; position.col = 40; break;
    case _s_watch: position.row = 15; position.col = 80; break;
    case _s_zone: position.row = 15; position.col = 120; break;
    default: break;
  }
  return position;
}


/* ------------------------------------------------------------------*
 *            manual symbol data under sub
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Setup_GetSymbolPosition_Sub(t_any_symbol sym)
{
  struct RowColPos position = { .row = 3, .col = 0 };

  switch(sym)
  {
    case _s_cal: case _s_zone: position.row = 2; break;
    case _s_mud: case _s_pump_off: case _s_inflow_pump: position.row = 1; break;
    default: break;
  }
  return position;
}


/* ------------------------------------------------------------------*
 *            draw setup symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Draw(t_any_symbol sym, bool negative)
{
  struct RowColPos position = LCD_Sym_Setup_GetSymbolPosition(sym);
  LCD_WriteAnySymbol(position.row, position.col, sym, negative);
}

/* ------------------------------------------------------------------*
 *            draw setup symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Draw_Sub(t_any_symbol sym, bool negative)
{
  struct RowColPos position = LCD_Sym_Setup_GetSymbolPosition_Sub(sym);
  LCD_WriteAnySymbol(position.row, position.col, sym, negative);
}

/* ------------------------------------------------------------------*
 *            all setup symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_AllSymbols(void)
{
  t_any_symbol symbols[12] = { _s_circulate, _s_air, _s_set_down, _s_pump_off, _s_mud, _s_compressor, _s_phosphor, _s_inflow_pump, _s_cal, _s_alarm, _s_watch, _s_zone };
  for(unsigned char i = 0; i < 12; i++){ LCD_Sym_Setup_Draw(symbols[i], false); }
}


/* ------------------------------------------------------------------*
 *            set setup pages
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Page(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
  LCD_Sym_Setup_AllSymbols();
}


/* ------------------------------------------------------------------*
 *            set setup circulate
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_circulate, false);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            circulate values and texts
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_General_TimeText(bool negative){ LCD_FillOrClrSpace(negative, 15, 39, 4, 31); LCD_WriteAnyStringFont(_f_6x8, 16, 40, "Time:", negative); }
void LCD_Sym_Setup_General_TimeValue(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 3, 16, 72, value, negative); }

void LCD_Sym_Setup_General_OnText(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 1, "On:  '", negative); }
void LCD_Sym_Setup_General_OnValue(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 10, 19, value, negative); }
void LCD_Sym_Setup_General_OnTextValue(bool negative, int value){ LCD_Sym_Setup_General_OnText(negative); LCD_Sym_Setup_General_OnValue(negative, value); }

void LCD_Sym_Setup_General_OffText(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 40, "Off:  '", negative); }
void LCD_Sym_Setup_General_OffValue(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 10, 64, value, negative); }
void LCD_Sym_Setup_General_OffTextValue(bool negative, int value){ LCD_Sym_Setup_General_OffText(negative); LCD_Sym_Setup_General_OffValue(negative, value); }


/* ------------------------------------------------------------------*
 *            circulate sensor sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate_Change_Sensor(unsigned char sensor)
{
  LCD_WriteAnySymbol(15, 0, _s_sensor, (bool)sensor);
  LCD_Sym_Setup_General_TimeText(!(bool)sensor);
}


/* ------------------------------------------------------------------*
 *            general use on off time
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_General_OnOffTime(unsigned char select, int on_min, int off_min, int time_min)
{
  LCD_Sym_Setup_General_OnTextValue((select == 0), on_min);
  LCD_Sym_Setup_General_OffTextValue((select == 1), off_min);
  // fill for values
  bool time_select = (select == 2);
  LCD_FillOrClrSpace(time_select, 15, 70, 4, 2);
  LCD_FillOrClrSpace(time_select, 15, 71, 1, 19);
  LCD_FillOrClrSpace(time_select, 18, 71, 1, 19);
  LCD_Sym_Setup_General_TimeValue(time_select, time_min);
}

/* ------------------------------------------------------------------*
 *            circulate change values
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate_Change_Values(unsigned char select, struct SettingsCirculate *settings_circulate)
{
  LCD_Sym_Setup_General_OnOffTime(select, settings_circulate->on_min, settings_circulate->off_min, settings_circulate->time_min);
}


/* ------------------------------------------------------------------*
 *            set setup Air
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Air(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_air, false);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            air text (same as circulate)
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Air_Change_Values(unsigned char select, struct SettingsAir *settings_air)
{ 
  LCD_Sym_Setup_General_OnOffTime(select, settings_air->on_min, settings_air->off_min, settings_air->time_min);
  LCD_Sym_Setup_General_TimeText(select == 2);
}


/* ------------------------------------------------------------------*
 *            set setup set down
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_SetDown(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_set_down, false);
  LCD_Sym_WriteCtrlButton();
  LCD_WriteAnyStringFont(_f_6x8, 11, 0, "Time:  '", true);
}

void LCD_Sym_Setup_SetDown_TimeValue(int value){ LCD_WriteAnyValue(_f_6x8, 2, 11, 30, value, true); }


/* ------------------------------------------------------------------*
 *            set setup PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_PumpOff(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_pump_off, false);
  LCD_Sym_WriteCtrlButton();
}

void LCD_Sym_Setup_PumpOff_Compressor(bool negative){ LCD_WriteAnySymbol(15, 0, _s_compressor, negative); }
void LCD_Sym_Setup_PumpOff_Pump(bool negative){ LCD_WriteAnySymbol(14, 50, _s_pump, negative); }


/* ------------------------------------------------------------------*
 *            set setup Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Mud(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_mud, false);
  LCD_Sym_WriteCtrlButton();
  LCD_WriteAnyStringFont(_f_6x8, 10, 1, "On:", true);
  LCD_Sym_Setup_Mud_MinText(true);
  LCD_Sym_Setup_Mud_SecText(false);
}

void LCD_Sym_Setup_Mud_Min(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 10, 19, value, negative); }
void LCD_Sym_Setup_Mud_Sec(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 10, 40, value, negative); }
void LCD_Sym_Setup_Mud_MinText(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 31, "'", negative); }
void LCD_Sym_Setup_Mud_SecText(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 52, "''", negative); }


/* ------------------------------------------------------------------*
 *            set setup Compressor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Compressor(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_WriteAnyStringFont(_f_6x8, 11, 28, "mbar min.", false);
  LCD_WriteAnyStringFont(_f_6x8, 16, 28, "mbar max.", false);
  LCD_Sym_Setup_Draw_Sub(_s_compressor, false);
  LCD_Sym_WriteCtrlButton();
}

void LCD_Sym_Setup_Compressor_MinP(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 3, 11, 7, value, negative); }
void LCD_Sym_Setup_Compressor_MaxP(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 3, 16, 7, value, negative); }


/* ------------------------------------------------------------------*
 *            set setup Phosphor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Phosphor(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_phosphor, false);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            set setup inflow pump
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_inflow_pump, false);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            sensor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Sensor(bool negative){ LCD_WriteAnySymbol(15, 5, _s_sensor, negative); }


/* ------------------------------------------------------------------*
 *            inflow pump - values
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Values(unsigned char select, struct SettingsInflowPump *settings_inflow_pump)
{
  LCD_Sym_Setup_InflowPump_ValuesOnMin((select == 0), settings_inflow_pump->on_min);
  LCD_Sym_Setup_InflowPump_ValuesOffMin((select == 1), settings_inflow_pump->off_min);
  LCD_Sym_Setup_InflowPump_ValuesOffHou((select == 2), settings_inflow_pump->off_hou);
}

void LCD_Sym_Setup_InflowPump_ValuesOffHou(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 5, 47, value, negative); }
void LCD_Sym_Setup_InflowPump_ValuesOffMin(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 10, 47, value, negative); }
void LCD_Sym_Setup_InflowPump_ValuesOnMin(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 2, 10, 19, value, negative); }


/* ------------------------------------------------------------------*
 *            inflow pump - text
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Text(unsigned char select)
{
  LCD_Sym_Setup_InflowPump_TextOn((select == 0));
  LCD_Sym_Setup_InflowPump_TextOnMin((select == 0));
  LCD_Sym_Setup_InflowPump_TextOffMin((select == 1));
  LCD_Sym_Setup_InflowPump_TextOffHou((select == 2));
  LCD_Sym_Setup_InflowPump_TextOff((select == 1) | (select == 2));
}

void LCD_Sym_Setup_InflowPump_TextOffHou(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 5, 59, "h", negative); }
void LCD_Sym_Setup_InflowPump_TextOnMin(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 31, "'", negative); }
void LCD_Sym_Setup_InflowPump_TextOffMin(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 59, "'", negative); }
void LCD_Sym_Setup_InflowPump_TextOn(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 10, 1, "On:", negative); }
void LCD_Sym_Setup_InflowPump_TextOff(bool negative){ LCD_WriteAnyStringFont(_f_6x8, 1, 47, "Off:", negative); }


/* ------------------------------------------------------------------*
 *            set setup calibration
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal(struct PlantState *ps)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_cal, false);
  LCD_Sym_Setup_Cal_Level_Sym(false);
  LCD_Sym_ControlButtons(_ctrl_esc, false);
  LCD_Sym_ControlButtons(_ctrl_ok, false);
  LCD_Sym_Setup_Cal_OpenValveButton(false);
  LCD_Sym_Setup_Cal_Button(false);
  LCD_WriteAnyStringFont(_f_6x8, 10, 1, "mbar:", false);

  bool sonic_flag = (bool)ps->settings->settings_zone->sonic_on;
  LCD_Sym_Setup_Cal_Sonic_Sym(sonic_flag);

  if(sonic_flag){ LCD_Sym_Setup_Cal_Level_Sonic(ps->settings->settings_calibration->tank_level_min_sonic); }
  else
  {
    LCD_Sym_Setup_Cal_Level_MPX(ps->settings->settings_calibration->tank_level_min_pressure);
    LCD_Sym_Setup_Cal_MPX_Redo((bool)ps->settings->settings_calibration->redo_on);
  }
}


/* ------------------------------------------------------------------*
 *            cal symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_Sonic_Sym(bool negative){ LCD_WriteAnySymbol(2, 40, _s_sonic, negative); }
void LCD_Sym_Setup_Cal_MPX_Redo(bool negative){ LCD_WriteAnySymbol(15, 130, _s_arrow_redo, negative); }
void LCD_Sym_Setup_Cal_OpenValveButton(bool negative){ LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_valve, negative); }
void LCD_Sym_Setup_Cal_Level_Sym(bool negative){ LCD_WriteAnySymbol(15, 1, _s_level, negative); }
void LCD_Sym_Setup_Cal_Button(bool negative){ LCD_WriteAnySymbol(9, 125, _s_cal, negative); }


/* ------------------------------------------------------------------*
 *            set setup calibration level measures
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_MPX_AverageValue(int value)
{
  // negative / positive pressure
  if(value < 0){ value = -value; LCD_WriteAnyStringFont(_f_6x8, 10, 34, "-", false); }
  else{ LCD_ClrSpace(10, 34, 2, 5); }
  LCD_WriteAnyValue(_f_6x8, 3, 10, 40, value, false);
}


/* ------------------------------------------------------------------*
 *            set setup calibration level measures and count down
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_Level_Sonic(int level_cal)
{ 
  LCD_WriteAnyValue(_f_6x8, 4, 17, 40, level_cal, false); 
  LCD_WriteAnyStringFont(_f_6x8, 17, 66, "mm", false);
}

void LCD_Sym_Setup_Cal_Level_MPX(int level_cal)
{ 
  LCD_WriteAnyValue(_f_6x8, 3, 17, 40, level_cal, false);
  LCD_WriteAnyStringFont(_f_6x8, 17, 60, "mbar", false);
}

void LCD_Sym_Setup_Cal_MPXCountDown(int sec){ LCD_WriteAnyValue(_f_6x8, 2, 17, 100, sec, false); }
void LCD_Sym_Setup_Cal_Clr_MPXCountDown(void){ LCD_ClrSpace(17, 100, 2, 12); }


/* ------------------------------------------------------------------*
 *            set setup alarm
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Alarm(struct PlantState *ps)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_alarm, false);
  LCD_WriteAnyStringFont(_f_6x8, 10, 3, "T:", true);

  // degree symbol
  LCD_WriteAnyFont(_f_6x8, 10, 32, 94, true);
  LCD_WriteAnyStringFont(_f_6x8, 10, 38, "C", true);

  // temp
  LCD_Sym_Setup_Alarm_MCP9800_WriteTemp(ps->temp_sensor->actual_temp);

  // degree symbol
  LCD_WriteAnyFont(_f_6x8, 17, 104, 94, false);
  LCD_WriteAnyStringFont(_f_6x8, 17, 110, "C", false);
  LCD_Sym_WriteCtrlButton();
}

void LCD_Sym_Setup_Alarm_TempValue(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 3, 10, 15, value, negative); }
void LCD_Sym_Setup_Alarm_Compressor(bool negative){ LCD_WriteAnySymbol(15, 40, _s_compressor, negative); }
void LCD_Sym_Setup_Alarm_Sensor(bool negative){ LCD_WriteAnySymbol(15, 0, _s_sensor, negative); }


/* ------------------------------------------------------------------*
 *            set setup watch
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_watch, false);
  LCD_Sym_WriteCtrlButton2();

  LCD_WriteAnyStringFont(_f_6x8, 11, 10, "hh", false);
  LCD_WriteAnyStringFont(_f_6x8, 11, 50, "mm", false);
  LCD_WriteAnyStringFont(_f_6x8, 17, 10, "dd", false);
  LCD_WriteAnyStringFont(_f_6x8, 17, 50, "mm", false);
  LCD_WriteAnyStringFont(_f_6x8, 17, 96, "yy", false);

  LCD_WriteAnyStringFont(_f_6x8, 9, 33, ":", false);
  LCD_WriteAnyStringFont(_f_6x8, 15, 33, "/", false);
  LCD_WriteAnyStringFont(_f_6x8, 15, 70, "/", false);
  LCD_WriteAnyStringFont(_f_6x8, 15, 84, "20", false);
}


/* ------------------------------------------------------------------*
 *            set setup zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Zone(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_s_zone, false);
  LCD_Sym_WriteCtrlButton();
  LCD_WriteAnyStringFont(_f_6x8, 11, 60, "cm", false);
  LCD_WriteAnyStringFont(_f_6x8, 16, 60, "cm", false);
}


/* ------------------------------------------------------------------*
 *            zone symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Zone_LevelToAir_Value(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 3, 11, 40, value, negative); }
void LCD_Sym_Setup_Zone_LevelToSetDown_Value(bool negative, int value){ LCD_WriteAnyValue(_f_6x8, 3, 16, 40, value, negative); }
void LCD_Sym_Setup_Zone_Sonic(bool negative){ LCD_WriteAnySymbol(3, 47, _s_sonic, negative); }
void LCD_Sym_Setup_Zone_Air(bool negative){ LCD_WriteAnySymbol(9, 0, _s_air, negative); }
void LCD_Sym_Setup_Zone_SetDown(bool negative){ LCD_WriteAnySymbol(14, 0, _s_set_down, negative); }


/* ------------------------------------------------------------------*
 *            setup pump sym  
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Pump(unsigned char mark)
{
  LCD_WriteAnySymbol(15, 45, _s_compressor, (mark == 0 ? true : false));
  LCD_WriteAnySymbol(15, 90, _s_pump, (mark == 1 ? true : false));
  LCD_WriteAnySymbol(15, 120, _s_pump2, (mark == 2 ? true : false));
}


/* ------------------------------------------------------------------*
 *            first clear, then mark
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch_Mark(t_DateTime time, unsigned char *p_dT)
{
  // time positive
  for(unsigned char i = _dt_hou; i <= _dt_year; i++){ LCD_Sym_Setup_Watch_DateTime(i, *p_dT++, (i == time)); }
}


/* ------------------------------------------------------------------*
 *            watch date and time
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch_DateTime(t_DateTime time, unsigned char value, bool negative)
{
  switch(time)
  {
    case _dt_hou: LCD_WriteAnyValue(_f_6x8, 2, 9, 10, value, negative); break;
    case _dt_min: LCD_WriteAnyValue(_f_6x8, 2, 9, 50, value, negative); break;
    case _dt_day: LCD_WriteAnyValue(_f_6x8, 2, 15, 10, value, negative); break;
    case _dt_month: LCD_WriteAnyValue(_f_6x8, 2, 15, 50, value, negative); break;
    case _dt_year: LCD_WriteAnyValue(_f_6x8, 2, 15, 96, value, negative); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            set data pages
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Page(struct PlantState *ps)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_data);
  LCD_Sym_Clr_InfoSpace();

  // Sx.x.x
  LCD_WriteAnyFont(_f_4x6, 1, 0, 21, false);
  LCD_WriteAnyFont(_f_4x6, 1, 8, SV1, false);
  LCD_WriteAnyFont(_f_4x6, 1, 16, SV2, false);
  LCD_WriteAnyFont(_f_4x6, 1, 24, SV3, false);
  LCD_WriteAnyFont(_f_4x6, 1, 4, 22, false);
  LCD_WriteAnyFont(_f_4x6, 1, 12, 22, false);
  LCD_WriteAnyFont(_f_4x6, 1, 20, 22, false);

  // time and date
  LCD_Sym_Auto_WorldTime_Print(ps);
  LCD_Sym_Auto_Date(ps);

  LCD_WriteAnyStringFont(_f_6x8, 6, 0, "choose data:", false);

  LCD_Write_TextButton(9, 0, TEXT_BUTTON_auto, false);
  LCD_Write_TextButton(9, 40, TEXT_BUTTON_manual, false);
  LCD_Write_TextButton(9, 80, TEXT_BUTTON_setup, false);
  LCD_Write_TextButton(9, 120, TEXT_BUTTON_sonic, false);
}


/* ------------------------------------------------------------------*
 *            actual page num
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_ActualPageNum(int page_num){ LCD_WriteAnyValue(_f_4x6, 2, 1, 140, page_num, false); }
void LCD_Sym_Data_TotalPageNum(int page_num){ LCD_WriteAnyFont(_f_4x6, 1, 148, 11, false); LCD_WriteAnyValue(_f_4x6, 2, 1, 152, page_num, false); }

/* ------------------------------------------------------------------*
 *            set data auto
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Auto(void)
{
  LCD_Sym_Clr_InfoSpace();

  // header
  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(_f_6x8, 1, 1, "Date", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 37, "|", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 45, "Time", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 73, "|", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 85, "O2", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 103, "|", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 113, "Err", true);

  LCD_Write_TextButton(22, 0, TEXT_BUTTON_auto, true);
  LCD_Sym_Data_Arrows();

  LCD_Sym_Data_ActualPageNum(1);
  LCD_Sym_Data_TotalPageNum(16);
  LCD_Data_WriteLogEntries(0, _eeprom_section_auto);
}


/* ------------------------------------------------------------------*
 *            set data manual
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Manual(void)
{
  LCD_Sym_Clr_InfoSpace();
  
  // header
  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(_f_6x8, 1, 1, "Date", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 37, "|", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 45, "Begin", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 77, "|", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 88, "End", true);

  LCD_Write_TextButton(22, 40, TEXT_BUTTON_manual, true);
  LCD_Sym_Data_Arrows();

  LCD_Sym_Data_ActualPageNum(1);
  LCD_Sym_Data_TotalPageNum(3);
  LCD_Data_WriteLogEntries(0, _eeprom_section_manual);
}


/* ------------------------------------------------------------------*
 *            set data setup
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Setup(void)
{
  LCD_Sym_Clr_InfoSpace();

  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(_f_6x8, 1, 1, "Date", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 37, "|", true);
  LCD_WriteAnyStringFont(_f_6x8, 1, 45, "Time", true);

  LCD_Write_TextButton(22, 80, TEXT_BUTTON_setup, true);
  LCD_Sym_Data_Arrows();

  LCD_Sym_Data_ActualPageNum(1);
  LCD_Sym_Data_TotalPageNum(3);
  LCD_Data_WriteLogEntries(0, _eeprom_section_setup);
}


/* ------------------------------------------------------------------*
 *            arrows
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Arrows(void)
{
  LCD_Sym_Data_Arrow_Up(false);
  LCD_FillSpace(8, 150, 3, 3);
  LCD_Sym_Data_Arrow_Down(false);
}

void LCD_Sym_Data_Arrow_Up(bool negative){ LCD_WriteAnySymbol(3, 140, _s_arrow_up, negative); }
void LCD_Sym_Data_Arrow_Down(bool negative){ LCD_WriteAnySymbol(14, 140, _s_arrow_down, negative); }


/* ------------------------------------------------------------------*
 *            set data sonic
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic(struct PlantState *ps)
{
  LCD_Sym_Clr_InfoSpace();
  Sonic_Data_Boot_Off(ps);
  LCD_WriteAnyStringFont(_f_6x8, 1, 91, "[ C]", false);

  // degree symbol
  LCD_WriteAnyFont(_f_6x8, 1, 97, 94, false);
  LCD_WriteAnyStringFont(_f_6x8, 1, 124, "[mm]", false);
  LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, false);
  LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, false);
  LCD_Write_TextButton(16, 0, TEXT_BUTTON_boot, false);

  // sonic symbol
  LCD_WriteAnySymbol(3, 50, _s_sonic, false);
}


/* ------------------------------------------------------------------*
 *            sonic - software version
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_ReadSversion(struct PlantState *ps)
{
  // get sonic version
  Sonic_ReadVersion(ps);

  // application version
  switch(ps->sonic_state->app_type)
  {
    case SONIC_APP_75kHz: LCD_WriteAnyStringFont(_f_6x8, 1, 2, "75kHz", false); break;
    case SONIC_APP_125kHz: LCD_WriteAnyStringFont(_f_6x8, 1, 2, "125kHz", false); break;
    case SONIC_APP_boot: LCD_WriteAnyStringFont(_f_6x8, 1, 2, "Boot  ", false); break;
    default: break;
  }

  // write software version Sx.x
  LCD_WriteAnyFont(_f_4x6, 1, 52, 21, false);
  LCD_WriteAnyFont(_f_4x6, 1, 57, ((ps->sonic_state->software_version & 0xF0) >> 4), false);
  LCD_WriteAnyFont(_f_4x6, 1, 61, 22, false);
  LCD_WriteAnyFont(_f_4x6, 1, 65, (ps->sonic_state->software_version & 0x0F), false);
}


/* ------------------------------------------------------------------*
 *            data sonic shot
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_ClearRecording(struct PlantState *ps)
{
  // init values
  ps->sonic_state->record_position = 5;
  ps->sonic_state->d_mm_max = 0;
  ps->sonic_state->d_mm_min = 10000;
  ps->sonic_state->temp_max = 0;
  ps->sonic_state->temp_min = 10000;

  // clear space
  LCD_ClrSpace(4, 91, 15, 69);

  // message line
  LCD_ClrSpace(17, 50, 4, 60);
}


/* ------------------------------------------------------------------*
 *            data sonic shot
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_SingleShot(struct PlantState *ps)
{
  LCD_WriteAnyValue(_f_4x6, 5, ps->sonic_state->record_position, 126, ps->sonic_state->d_mm, false);
  ps->sonic_state->record_position += 3;
}


/* ------------------------------------------------------------------*
 *            data sonic sequential shots
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_SequentialShots(struct PlantState *ps)
{
  LCD_WriteAnyValue(_f_4x6, 5, 5, 126, ps->sonic_state->d_mm, false);

  // could not meassure a distance
  if(ps->sonic_state->d_mm <= 0){ return; }

  // limits for updating min and max
  if((ps->sonic_state->d_mm > (ps->sonic_state->d_mm_max + D_LIM)) || (ps->sonic_state->d_mm < (ps->sonic_state->d_mm_min - D_LIM))){ ps->sonic_state->record_error_update++; }
  else{ ps->sonic_state->record_error_update = 0; }

  // tries to accept the new distance
  if(ps->sonic_state->record_error_update > 2){ ps->sonic_state->record_error_update = 0; }
  if(ps->sonic_state->record_error_update){ return; }

  // new max and min
  if(ps->sonic_state->d_mm > ps->sonic_state->d_mm_max)
  {
    ps->sonic_state->d_mm_max = ps->sonic_state->d_mm;
    LCD_WriteAnyValue(_f_4x6, 5, 11, 126, ps->sonic_state->d_mm_max, false);
  }
  if(ps->sonic_state->d_mm < ps->sonic_state->d_mm_min)
  {
    ps->sonic_state->d_mm_min = ps->sonic_state->d_mm;
    LCD_WriteAnyValue(_f_4x6, 5, 17, 126, ps->sonic_state->d_mm_min, false);
  }
}


/* ------------------------------------------------------------------*
 *            data sonic temp
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_SingleTemp(struct PlantState *ps)
{
  if(ps->sonic_state->record_position > 17)
  {
    ps->sonic_state->record_position = 5;
    LCD_ClrSpace(4, 95, 15, 60);
  }
  if(ps->sonic_state->temp & 0x8000)
  {
    ps->sonic_state->temp &= 0x7FFF;
    //-
    LCD_WriteAnyFont(_f_4x6, ps->sonic_state->record_position, 91, 11, false);
  }
  //.
  LCD_WriteAnyFont(_f_4x6, ps->sonic_state->record_position, 102, 22, false);
  LCD_WriteAnyValue(_f_4x6, 2, ps->sonic_state->record_position, 105, (ps->sonic_state->temp & 0x00FF), false);
  LCD_WriteAnyValue(_f_4x6, 2, ps->sonic_state->record_position, 95, ((ps->sonic_state->temp >> 8) & 0x00FF), false);
}


/* ------------------------------------------------------------------*
 *            data sonic sequential temperature
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_SequentialTemp(struct PlantState *ps)
{
  unsigned char minus_sign = 0;
  if(ps->sonic_state->temp & 0x8000)
  {
    minus_sign = 1;
    ps->sonic_state->temp &= 0x7FFF;

    //-
    LCD_WriteAnyFont(_f_4x6, 5, 91, 11, false);
  }
  LCD_WriteAnyValue(_f_4x6, 2, 5, 105, (ps->sonic_state->temp & 0x00FF), false);
  LCD_WriteAnyValue(_f_4x6, 2, 5, 95, ((ps->sonic_state->temp >> 8) & 0x00FF), false);
  if(ps->sonic_state->temp > ps->sonic_state->temp_max)
  {
    ps->sonic_state->temp_max = ps->sonic_state->temp;

    //-
    if(minus_sign){ LCD_WriteAnyFont(_f_4x6, 11, 91, 11, false); }
    LCD_WriteAnyValue(_f_4x6, 2, 11, 105, (ps->sonic_state->temp_max & 0x00FF), false);
    LCD_WriteAnyValue(_f_4x6, 2, 11, 95, ((ps->sonic_state->temp_max >> 8) & 0x00FF), false);
  }
  if(ps->sonic_state->temp < ps->sonic_state->temp_min)
  {
    ps->sonic_state->temp_min = ps->sonic_state->temp;

    //-
    if(minus_sign){ LCD_WriteAnyFont(_f_4x6, 17, 91, 11, false); }
    LCD_WriteAnyValue(_f_4x6, 2, 17, 105, (ps->sonic_state->temp_min & 0x00FF), false);
    LCD_WriteAnyValue(_f_4x6, 2, 17, 95, ((ps->sonic_state->temp_min >> 8) & 0x00FF), false);
  }
}


/* ------------------------------------------------------------------*
 *            data sonic boot text
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_AutoText(void)
{
  LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, true);

  // max
  LCD_WriteAnyFont(_f_4x6, 9, 113, 13, false);
  LCD_WriteAnyFont(_f_4x6, 9, 117, 15, false);
  LCD_WriteAnyFont(_f_4x6, 9, 121, 25, false);

  // min
  LCD_WriteAnyFont(_f_4x6, 15, 113, 13, false);
  LCD_WriteAnyFont(_f_4x6, 15, 117, 23, false);
  LCD_WriteAnyFont(_f_4x6, 15, 121, 24, false);

  // ...
  LCD_WriteAnyFont(_f_4x6, 17, 102, 22, false);
  LCD_WriteAnyFont(_f_4x6, 11, 102, 22, false);
  LCD_WriteAnyFont(_f_4x6, 5, 102, 22, false);
}


/* ------------------------------------------------------------------*
 *            data sonic boot text
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_NoUs(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 50, "NoUS", false); }
void LCD_Sym_Data_Sonic_BootOk(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 50, "OK...Cycle Power!", false); }
void LCD_Sym_Data_Sonic_BootNoData(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 50, "NoData", false); }
void LCD_Sym_Data_Sonic_BootError(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 50, "Error", false); }
void LCD_Sym_Data_Sonic_BootNone(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 50, "NoBoot", false); }
void LCD_Sym_Data_Sonic_BootSucess(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 50, "Success", false); }
void LCD_Sym_Data_Sonic_ReadProgramPage(int page){ LCD_WriteAnyValue(_f_4x6, 3, 17, 50, page, false); }


/*-------------------------------------------------------------------*
 *  reads variable from EEPROM, PageSize: 32Bytes
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_WriteAutoEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[8] = {0x00};
  unsigned char i = 0;
  int o2 = 0;

  for(i = 0; i < 8; i++){ varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i); }
  o2 = ((varEnt[5] << 8) | (varEnt[6]));

  // write no data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    // 11: -
    for(i = 1; i < 130; i = i + 4){ LCD_WriteAnyFont(_f_4x6, pa, i, 11, false); }
  }

  // write data
  else
  {
    // day - month
    LCD_WriteAnyValue(_f_4x6, 2, pa, 1, varEnt[0], false);
    LCD_WriteAnyFont(_f_4x6, pa, 9, 11, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 13, varEnt[1], false);

    // - year |
    LCD_WriteAnyFont(_f_4x6, pa, 21, 11, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 25, varEnt[2], false);
    LCD_WriteAnyFont(_f_4x6, pa, 38, 12, false);

    // hour : minutes
    LCD_WriteAnyValue(_f_4x6, 2, pa, 47, varEnt[3], false);
    LCD_WriteAnyFont(_f_4x6, pa, 55, 10, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 59, varEnt[4], false);

    // | 02 in min |
    LCD_WriteAnyFont(_f_4x6, pa, 74, 12, false);
    LCD_WriteAnyValue(_f_4x6, 3, pa, 85, o2, false);
    LCD_WriteAnyFont(_f_4x6, pa, 104, 12, false);

    // alarm code
    if(varEnt[7] & 0x01){ LCD_WriteAnyFont(_f_4x6, pa, 113, 1, false); }
    else{ LCD_WriteAnyFont(_f_4x6, pa, 113, 0, false); }

    if(varEnt[7] & 0x02){ LCD_WriteAnyFont(_f_4x6, pa, 117, 2, false); }
    else{ LCD_WriteAnyFont(_f_4x6, pa, 117, 0, false); }

    if(varEnt[7] & 0x04){ LCD_WriteAnyFont(_f_4x6, pa, 121, 3, false); }
    else{ LCD_WriteAnyFont(_f_4x6, pa, 121, 0, false); }

    if(varEnt[7] & 0x08){ LCD_WriteAnyFont(_f_4x6, pa, 125, 4, false); }
    else{ LCD_WriteAnyFont(_f_4x6, pa, 125, 0, false); }

    if(varEnt[7] & 0x10){ LCD_WriteAnyFont(_f_4x6, pa, 129, 5, false); }
    else{ LCD_WriteAnyFont(_f_4x6, pa, 129, 0, false); }
  }
}


/* ------------------------------------------------------------------*
 *            manual entry
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_WriteManualEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[7] = {0x00};
  unsigned char i = 0;

  for(i = 0; i < 7; i++){ varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i); }

  // no data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    for(i = 1; i < 108; i = i + 4){ LCD_WriteAnyFont(_f_4x6, pa, i, 11, false); }
  }

  // write data
  else
  {
    // day - month
    LCD_WriteAnyValue(_f_4x6, 2, pa, 1, varEnt[0], false);
    LCD_WriteAnyFont(_f_4x6, pa, 9, 11, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 13, varEnt[1], false);

    // - year |
    LCD_WriteAnyFont(_f_4x6, pa, 21, 11, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 25, varEnt[2], false);
    LCD_WriteAnyFont(_f_4x6, pa, 38, 12, false);

    // hour : minutes
    LCD_WriteAnyValue(_f_4x6, 2, pa, 47, varEnt[3], false);
    LCD_WriteAnyFont(_f_4x6, pa, 55, 10, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 59, varEnt[4], false);

    // |
    LCD_WriteAnyFont(_f_4x6, pa, 78, 12, false);

    // hour : minutes
    LCD_WriteAnyValue(_f_4x6, 2, pa, 88, varEnt[5], false);
    LCD_WriteAnyFont(_f_4x6, pa, 96, 10, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 100, varEnt[6], false);
  }
}


/* ------------------------------------------------------------------*
 *            setup entry
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_WriteSetupEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[7] = {0x00};
  unsigned char i = 0;

  for(i = 0; i < 5; i++){ varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i); }

  // no data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    //--------
    for(i = 1; i < 68; i = i + 4){ LCD_WriteAnyFont(_f_4x6, pa, i, 11, false); }
  }

  // write data
  else
  {
    // day - month
    LCD_WriteAnyValue(_f_4x6, 2, pa, 1, varEnt[0], false);
    LCD_WriteAnyFont(_f_4x6, pa, 9, 11, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 13, varEnt[1], false);

    // - year |
    LCD_WriteAnyFont(_f_4x6, pa, 21, 11, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 25, varEnt[2], false);
    LCD_WriteAnyFont(_f_4x6, pa, 38, 12, false);

    // hour : minutes
    LCD_WriteAnyValue(_f_4x6, 2, pa, 47, varEnt[3], false);
    LCD_WriteAnyFont(_f_4x6, pa, 55, 10, false);
    LCD_WriteAnyValue(_f_4x6, 2, pa, 59, varEnt[4], false);
  }
}


/* ------------------------------------------------------------------*
 *            positive pin buttons
 * ------------------------------------------------------------------*/

void LCD_PinButtons(bool negative, unsigned char pin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol =  _s_frame;
  unsigned char num = 0;

  switch(pin)
  {
    case 1: row = 2; col = 0; num = 0x01; break;
    case 4: row = 8; col = 0; num = 0x04; break;
    case 7: row = 14; col = 0; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = _s_escape; num = 0x20; break;

    case 2: row = 2; col = 40; num = 0x02; break;
    case 5: row = 8; col = 40; num = 0x05; break;
    case 8: row = 14; col = 40; num = 0x08; break;
    case 0: row = 20; col = 40; num = 0x00; break;

    case 3: row = 2; col = 80; num = 0x03; break;
    case 6: row = 8; col = 80; num = 0x06; break;
    case 9: row = 14; col = 80; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = _s_del; num = 0x20; break;
    default: break;
  }

  // write symbol
  LCD_WriteAnySymbol(row, col, any_symbol, negative);

  // write number if it is one
  if(num < 0x20){ LCD_WriteAnyFont(_f_8x16, row + 1, col + 13, num, negative); }
}


/* ------------------------------------------------------------------*
 *            pin page
 * ------------------------------------------------------------------*/

void LCD_Sym_PinPage(void)
{
  LCD_Clean();
  for(unsigned char i = 0; i < 12; i++){ LCD_PinButtons(false, i); }
  LCD_Sym_Pin_DelDigits();
}


/* ------------------------------------------------------------------*
 *            pin page messages and texts
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_RightMessage(void){ LCD_WriteAnyStringFont(_f_6x8, 6, 125, "right!", false); }

void LCD_Sym_Pin_WrongMessage(void)
{
  LCD_WriteAnyStringFont(_f_6x8, 6, 119, "wrong!", false);
  LCD_WriteAnyStringFont(_f_6x8, 9, 119, "try", false);
  LCD_WriteAnyStringFont(_f_6x8, 11, 119, "again", false);
}

void LCD_Sym_Pin_OpHoursMessage(void){ LCD_WriteAnyStringFont(_f_6x8, 6, 119, "OP", false); }


/* ------------------------------------------------------------------*
 *            pin page clear
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_ClearPinCode(void){ LCD_ClrSpace(6, 119, 25, 41); }


/* ------------------------------------------------------------------*
 *  delete written digits
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_DelDigits(void){ LCD_WriteAnyStringFont(_f_6x8, 3, 125, "xxxx", false); }


/* ------------------------------------------------------------------*
 *            pin page telephone number
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_PrintWholeTelNumber(struct TeleNr *tele_nr)
{
  // telx:
  LCD_WriteAnyStringFont(_f_6x8, 6, 119, "Tel", false);
  LCD_WriteAnyFont(_f_6x8, 6, 137, tele_nr->id + 14, false);
  LCD_WriteAnyStringFont(_f_6x8, 6, 143, ":", false);

  // +
  LCD_WriteAnyFont(_f_4x6, 9, 119, 20, false);

  // 43
  for(unsigned char i = 0; i < 2; i++){ LCD_WriteAnyFont(_f_4x6, 9, 123 + 4 * i, tele_nr->nr[i], false); }

  // 680
  for(unsigned char i = 2; i < 5; i++){ LCD_WriteAnyFont(_f_4x6, 9, 135 + 4 * (i - 2), tele_nr->nr[i], false); }

  // number
  for(unsigned char i = 5; i < 14; i++){ LCD_WriteAnyFont(_f_4x6, 11, 119 + 4 * (i - 5), tele_nr->nr[i], false); }
}


/* ------------------------------------------------------------------*
 *            pin page telephone digit
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_PrintOneTelNumberDigit(unsigned char digit, unsigned char pos)
{
  LCD_PinButtons(true, digit);
  if(pos < 2)
  {
    LCD_WriteAnyFont(_f_4x6, 9, 123 + 4 * pos, digit, false);
    pos++;
    if(pos < 2){ LCD_WriteAnyFont(_f_4x6, 9, 127, 14, true); }
    else{ LCD_WriteAnyFont(_f_4x6, 9, 135, 14, true); }
  }

  else if((pos > 1) && (pos < 5))
  {
    LCD_WriteAnyFont(_f_4x6, 9, 135 + 4 * (pos - 2), digit, false);
    pos++;
    if(pos < 5){ LCD_WriteAnyFont(_f_4x6, 9, 135 + 4 * (pos - 2), 14, true); }
    else{ LCD_WriteAnyFont(_f_4x6, 11, 119, 14, true); }
  }
  else
  {
    LCD_WriteAnyFont(_f_4x6, 11, 119 + 4 * (pos - 5), digit, false);
    pos++;
    if(pos < 14){ LCD_WriteAnyFont(_f_4x6, 11, 119 + 4 * (pos - 5), 14, true); }
  }
}


/* ------------------------------------------------------------------*
 *  writes digit on pin field, corresponding to pressed one
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_WriteDigit(unsigned char pin, unsigned char code_pos)
{
  LCD_PinButtons(true, pin);
  LCD_WriteAnyFont(_f_6x8, 3, 125 + 6 * code_pos, pin + 15, false);
}


/* ------------------------------------------------------------------*
 *            text buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_TextButton(t_text_buttons text, bool negative)
{
  unsigned char row = 22;
  unsigned char col = 0;
  switch(text)
  {
    case TEXT_BUTTON_auto: col = 0; break;
    case TEXT_BUTTON_manual: col = 40; break;
    case TEXT_BUTTON_setup: col = 80; break;
    case TEXT_BUTTON_data: col = 120; break;
    default: break;
  }
  LCD_Write_TextButton(row, col, text, negative);
}


/* ------------------------------------------------------------------*
 *            Control Buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_ControlButtons(t_CtrlButtons ctrl, bool negative)
{
  switch(ctrl)
  {
    case _ctrl_esc: LCD_WriteAnySymbol(3, 90, _s_esc, negative); break;
    case _ctrl_ok: LCD_WriteAnySymbol(3, 130, _s_ok, negative); break;
    case _ctrl_minus: LCD_WriteAnySymbol(9, 90, _s_minus, negative); break;
    case _ctrl_plus: LCD_WriteAnySymbol(9, 130, _s_plus, negative); break;
    default: break;
  }
}

void LCD_Sym_ControlButtons2(t_CtrlButtons ctrl, bool negative)
{
  switch(ctrl)
  {
    case _ctrl_esc: LCD_WriteAnySymbol(3, 90, _s_esc, negative); break;
    case _ctrl_ok: LCD_WriteAnySymbol(3, 130, _s_ok, negative); break;
    case _ctrl_minus: LCD_WriteAnySymbol(15, 130, _s_minus, negative); break;
    case _ctrl_plus: LCD_WriteAnySymbol(9, 130, _s_plus, negative); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Pin Okay
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_OkButton(unsigned char select){ LCD_WriteAnySymbol(20, 130, _s_ok, select); }


/* ------------------------------------------------------------------*
 *            write control buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_WriteCtrlButton(void){ for(unsigned char i = _ctrl_plus; i <= _ctrl_ok; i++){ LCD_Sym_ControlButtons(i, false); } }
void LCD_Sym_WriteCtrlButton2(void){ for(unsigned char i = _ctrl_plus; i <= _ctrl_ok; i++){ LCD_Sym_ControlButtons2(i, false); } }


/* ------------------------------------------------------------------*
 *            Mark Text Button
 * ------------------------------------------------------------------*/

void LCD_Sym_MarkTextButton(t_text_buttons text){ for(unsigned char i = 0; i < 4; i++){ LCD_Sym_TextButton(i, (i == text ? true : false)); } }


/* ------------------------------------------------------------------*
 *            can messages
 * ------------------------------------------------------------------*/

void LCD_Sym_CAN_MCPFail(void){ LCD_WriteAnyStringFont(_f_6x8, 17, 100, "MCP-fail", false); }


/* ------------------------------------------------------------------*
 *            sonic none position
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Sonic_NoUS_Pos(t_page page)
{
  struct RowColPos pos = {.row = 0, .col = 0};

  switch(page)
  {
    case AutoSetDown: case AutoMud: case AutoPumpOff:
    case AutoCirc: case AutoPage: case AutoAir:
      pos.row = 17; pos.col = 5;
      break;

    case ManualMain: case ManualCirc: case ManualAir: case ManualSetDown: case ManualPumpOff:
    case ManualPumpOff_On: case ManualMud: case ManualCompressor: case ManualPhosphor: case ManualInflowPump: 
      pos.row = 17; pos.col = 2;
      break;

    case SetupCal: 
      pos.row = 0; pos.col = 38;
      break;
      
    default: break;
  }
  return pos;
}


/* ------------------------------------------------------------------*
 *            sonic none clear
 * ------------------------------------------------------------------*/

void LCD_Sym_Sonic_NoUS_Clear(struct PlantState *ps)
{
  if(ps->sonic_state->no_us_flag)
  {
    struct RowColPos pos = LCD_Sym_Sonic_NoUS_Pos(ps->page_state->page);
    LCD_ClrSpace(pos.row, pos.col, 2, 24);
  }
}


/* ------------------------------------------------------------------*
 *            sonic none message
 * ------------------------------------------------------------------*/

void LCD_Sym_Sonic_NoUS_Message(struct PlantState *ps)
{
  // get postion regarding page
  struct RowColPos pos = LCD_Sym_Sonic_NoUS_Pos(ps->page_state->page);
  LCD_WriteAnyStringFont(_f_6x8, pos.row, pos.col, "noUS", false);
}


/* ------------------------------------------------------------------*
 *            auto sonic value
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SonicVal(struct PlantState *ps)
{
  // deactivated sonic
  if(!ps->settings->settings_zone->sonic_on || ps->sonic_state->no_us_flag){ return; }

  // percentage
  int zero = ps->settings->settings_calibration->tank_level_min_sonic;
  int lv_to_set_down = ps->settings->settings_zone->level_to_set_down * 10;
  
  //water-level-difference
  int cal = ps->sonic_state->d_mm - (zero - (lv_to_set_down));
  int level_perc = (ps->sonic_state->d_mm > zero ? 0 : 100 - ((cal * 10) / lv_to_set_down) * 10);
  if(!ps->sonic_state->d_mm){ level_perc = 0; }

  // page dependend action
  switch(ps->page_state->page)
  {
    case AutoZone:  case AutoSetDown: case AutoPumpOff:
    case AutoMud:   case AutoCirc: case AutoAir:  case AutoPage:
      LCD_Sym_Auto_Tank_SonicDmm(ps->sonic_state->d_mm);
      LCD_Sym_Auto_Tank_LevelPerc(true, level_perc);
      break;

    case ManualMain:  case ManualCirc: case ManualAir:   case ManualSetDown: case ManualPumpOff:
    case ManualPumpOff_On:  case ManualMud: case ManualCompressor: case ManualPhosphor:  case ManualInflowPump:
      LCD_Sym_Manual_Tank_LevelPerc(level_perc);
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Error Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Error(unsigned char error_id)
{
  switch(error_id)
  {
    case ERROR_ID_T: LCD_WriteAnySymbol(16, 134, _s_grad, true); break;
    case ERROR_ID_OP:
    case ERROR_ID_UP: LCD_ClrSpace(6, 44, 6, 35); LCD_WriteAnySymbol(6, 45, _s_alarm, true); break;
    case ERROR_ID_IT: LCD_WriteAnySymbol(17, 1, _s_alarm, true); LCD_Sym_TextButton(TEXT_BUTTON_auto, true); break;
    case ERROR_ID_OT: LCD_WriteAnySymbol(17, 90, _s_alarm, true); LCD_Sym_TextButton(TEXT_BUTTON_setup, false); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            auto time symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_WorldTime_Print(struct PlantState *ps)
{
  // : symbol
  LCD_WriteAnyFont(_f_4x6, 2, 128, 10, false);
  LCD_WriteAnyFont(_f_4x6, 2, 140, 10, false);

  // time
  LCD_WriteAnyValue(_f_4x6, 2, 2, 120, (int)ps->time_state->tic_hou, false);
  LCD_WriteAnyValue(_f_4x6, 2, 2, 132, (int)ps->time_state->tic_min, false);
  LCD_WriteAnyValue(_f_4x6, 2, 2, 144, (int)ps->time_state->tic_sec, false);
}


/* ------------------------------------------------------------------*
 *            auto date symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Date(struct PlantState *ps)
{
  // - symbol
  LCD_WriteAnyFont(_f_4x6, 0, 128, 11, false);
  LCD_WriteAnyFont(_f_4x6, 0, 140, 11, false);

  // year 20xx
  LCD_WriteAnyStringFont(_f_4x6, 0, 144, "20", false);

  // date
  LCD_WriteAnyValue(_f_4x6, 2, 0, 120, MCP7941_ReadTime(ps->twi_state, TIC_DATE), false);
  LCD_WriteAnyValue(_f_4x6, 2, 0, 132, MCP7941_ReadTime(ps->twi_state, TIC_MONTH), false);
  LCD_WriteAnyValue(_f_4x6, 2, 0, 152, MCP7941_ReadTime(ps->twi_state, TIC_YEAR), false);
}


/* ------------------------------------------------------------------*
 *            end text sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_EndText(void){ LCD_WriteAnyStringFont(_f_6x8, 15, 1, "End", false); }


/* ------------------------------------------------------------------*
 *            modem sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Modem_Data_SLED(bool show){ (show ? LCD_WriteAnyFont(_f_4x6, 0, 105, 26, false) : LCD_ClrSpace(0, 105, 2, 4)); }
void LCD_Sym_Modem_Pin_SLED(bool show){ (show ? LCD_WriteAnyFont(_f_4x6, 18, 137, 26, false) : LCD_ClrSpace(18, 137, 2, 4)); }


/* ------------------------------------------------------------------*
 *            temp
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Alarm_MCP9800_WriteTemp(unsigned char temp)
{
  // indicate minus
  if(temp & 0x80)
  {
    LCD_WriteAnyStringFont(_f_6x8, 17, 84, "-", false);
    temp = -temp;
  }

  // clear minus sign
  else{ LCD_ClrSpace(17, 84, 2, 6); }

  // write temperature
  LCD_WriteAnyValue(_f_6x8, 2, 17, 90, (temp & 0x7F), false);
}