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
  if(!COMPANY){ LCD_WriteAnySymbol(0, 0, _logo_purator); }
  else{ LCD_WriteAnySymbol(0, 0, _logo_hecs); }
}


/* ------------------------------------------------------------------*
 *            clear info space
 * ------------------------------------------------------------------*/

void LCD_Sym_Clr_InfoSpace(void){ LCD_ClrSpace(0, 0, LCD_SPEC_MAX_PAG - 4, LCD_SPEC_MAX_COL); }
void LCD_Sym_Clr_DataEntrySpace(void){ LCD_ClrSpace(4, 1, LCD_SPEC_MAX_PAG - 8, 135); }


/* ------------------------------------------------------------------*
 *            auto symbol set manager
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetManager(struct PlantState *ps)
{
  // page dependend symbols
  switch(ps->page_state->page)
  {
    case AutoPage: LCD_Sym_Auto_Main(ps); break;
    case AutoZone: LCD_Sym_Auto_Zone(); break;
    case AutoSetDown: LCD_Sym_Auto_SetDown(); break;
    case AutoPumpOff: LCD_Sym_Auto_PumpOff(); break;
    case AutoMud: LCD_Sym_Auto_Mud(); break;

    case AutoAirOn: LCD_Sym_Auto_AirOn(); LCD_Sym_Auto_AirTime_Print(ps->air_circ_state->air_tms); break;
    case AutoAirOff: LCD_Sym_Auto_AirOff(); LCD_Sym_Auto_AirTime_Print(ps->air_circ_state->air_tms); break;
    case AutoCircOn: LCD_Sym_Auto_CircOn(); LCD_Sym_Auto_AirTime_Print(ps->air_circ_state->air_tms); break;
    case AutoCircOff:LCD_Sym_Auto_CircOff(); LCD_Sym_Auto_AirTime_Print(ps->air_circ_state->air_tms); break;

    default: return;
  }

  // page time
  LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);

  // write text
  LCD_Sym_Auto_Text(ps);

  // phosphor symbols
  LCD_Sym_Auto_Ph(ps);

  // inflow pump symbols
  LCD_Sym_Auto_Ip_Base(ps);
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
  LCD_WriteAnyValue(f_4x6_p, 2, row, col, h);
  col += 8;
  LCD_WriteAnyFont(f_4x6_p, row, col, 10);
  col += 4;
  LCD_WriteAnyValue(f_4x6_p, 2, row, col, min);
  col += 8;
  LCD_WriteAnyFont(f_4x6_p, row, col, 10);
  col += 4;
  LCD_WriteAnyValue(f_4x6_p, 2, row, col, tms->sec);
}


/* ------------------------------------------------------------------*
 *            auto air time
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_AirTime_Print(struct Tms *tms)
{
  unsigned char col = 12;
  unsigned char row = 15;
  LCD_WriteAnyValue(f_4x6_p, 2, row, col, tms->min);
  col += 8;
  LCD_WriteAnyFont(f_4x6_p, row, col, 10);
  col += 4;
  LCD_WriteAnyValue(f_4x6_p, 2, row, col, tms->sec);
}



/* ------------------------------------------------------------------*
 *            pagetime update
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PageTime_Update(struct PlantState *ps)
{
  // sec change
  if(ps->time_state->tic_sec_update_flag)
  {
    // time
    LCD_Sym_Auto_WorldTime(ps);
    LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);

    // minute update
    if(ps->page_state->page_time->sec == 59)
    { 
      // update compressor hours
      switch(ps->page_state->page)
      {
        // compressor on cases
        case AutoPumpOff:
        case AutoMud:
        case AutoCircOn:
        case AutoAirOn:
        case AutoZone:
          
          // add counter
          ps->compressor_state->operation_sixty_min_count++;
          if(ps->compressor_state->operation_sixty_min_count >= 60)
          {
            // update compressor hours
            int comp_op_h = MCP7941_Read_Comp_OpHours(ps->twi_state);
            comp_op_h++;
            MCP7941_Write_Comp_OpHours(comp_op_h);
            comp_op_h = MCP7941_Read_Comp_OpHours(ps->twi_state);
            ps->compressor_state->operation_hours = comp_op_h;
            LCD_WriteAnyValue(f_4x6_p, 5, 15, 43, comp_op_h);
            ps->compressor_state->operation_sixty_min_count = 0;
          }
          break;
          
        default: break;
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            auto text
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

  // time and date
  LCD_Sym_Auto_WorldTime(ps);
  LCD_Sym_Auto_Date(ps);

  // read water level
  LCD_Sym_MPX_LevelPerc(ps);
  LCD_Sym_Auto_SonicVal(ps);

  // compressor hours
  LCD_WriteAnyValue(f_4x6_p, 5, 15, 43, MCP7941_Read_Comp_OpHours(ps->twi_state));

  // ip sensor
  if(MEM_EEPROM_ReadVar(SENSOR_outTank)){ LCD_WriteAnySymbol(16, 90, _p_sensor); }
}


/* ------------------------------------------------------------------*
 *            set auto page
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Main(struct PlantState *ps)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_auto);
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Logo();
  LCD_Sym_Auto_WorldTime(ps);
}


/* ------------------------------------------------------------------*
 *            auto compressor symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CompressorNeg(void){ LCD_WriteAnySymbol(6, 45, _n_compressor); }
void LCD_Sym_Auto_CompressorPos(void){ LCD_WriteAnySymbol(6, 45, _p_compressor); }


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
  LCD_WriteAnySymbol(6, 0, _n_circulate);
  LCD_Sym_Auto_CompressorNeg();
  LCD_WriteAnyStringFont(f_6x8_n, 8, 1, "z");
}


/* ------------------------------------------------------------------*
 *            set auto set down
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetDown(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _n_setDown);
  LCD_Sym_Auto_CompressorPos();
}


/* ------------------------------------------------------------------*
 *            set auto PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PumpOff(void)
{
  LCD_WriteAnySymbol(5, 0, _n_pumpOff);
  if(!MEM_EEPROM_ReadVar(PUMP_pumpOff)){ LCD_Sym_Auto_CompressorNeg(); }
  else{ LCD_Sym_Auto_CompressorPos(); }
}


/* ------------------------------------------------------------------*
 *            set auto Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Mud(void)
{
  LCD_WriteAnySymbol(5, 0, _n_mud);
  LCD_Sym_Auto_CompressorNeg();
}


/* ------------------------------------------------------------------*
 *            circulate and air symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CircOn(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _n_circulate);
  LCD_Sym_Auto_CompressorNeg();
}

void LCD_Sym_Auto_CircOff(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _p_circulate);
  LCD_Sym_Auto_CompressorPos();
}

void LCD_Sym_Auto_AirOn(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _n_air);
  LCD_Sym_Auto_CompressorNeg();
}

void LCD_Sym_Auto_AirOff(void)
{
  LCD_Sym_Auto_ClrActualCycleSpace();
  LCD_WriteAnySymbol(6, 0, _p_air);
  LCD_Sym_Auto_CompressorPos();
}


/* ------------------------------------------------------------------*
 *            auto air symbols select
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_AirPageSelect(t_page page)
{
  switch(page)
  {
    case AutoCircOn: LCD_Sym_Auto_CircOn(); break;
    case AutoCircOff: LCD_Sym_Auto_CircOff(); break;
    case AutoAirOn: LCD_Sym_Auto_AirOn(); break;
    case AutoAirOff: LCD_Sym_Auto_AirOff(); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Inflow Pump Symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ip_Base(struct PlantState *ps)
{
  // handlers
  unsigned char pump =  MEM_EEPROM_ReadVar(PUMP_inflowPump);
  t_page p = ps->page_state->page;
  t_inflow_pump_states ip_state = ps->inflow_pump_state->ip_state;

  // off or disabled ip
  if(ip_state == _ip_off || ip_state == _ip_disabled)
  {
    switch (pump)
    {
      case 0:
        LCD_WriteAnySymbol(5, 89, _p_inflowPump);
        if(p == AutoAirOff || p == AutoCircOff){ LCD_Sym_Auto_CompressorPos(); }
        break;

      case 1: LCD_WriteAnySymbol(5, 90, _p_pump); break;
      case 2: LCD_WriteAnySymbol(5, 90, _p_pump2); break;
      default: break;
    }
  }

  // ip on state
  else if(ip_state == _ip_on)
  {
    switch (pump)
    {
      case 0:
        LCD_WriteAnySymbol(5, 89, _n_inflowPump);
        if(p == AutoAirOff || p == AutoCircOff){ LCD_Sym_Auto_CompressorNeg(); }
        break;

      case 1: LCD_WriteAnySymbol(5, 90, _n_pump); break;
      case 2: LCD_WriteAnySymbol(5, 90, _n_pump2); break;
      default: break;
    }
  }

  // time
  LCD_Sym_Auto_Ip_Time(0x07, ps->inflow_pump_state->ip_thms);
}


/* ------------------------------------------------------------------*
 *            Inflow Pump Var
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ip_Time(unsigned char cho, struct Thms *t_hms)
{
  // sec, min, h
  if(cho & 0x01){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 109, t_hms->sec); }
  LCD_WriteAnyFont(f_4x6_p, 13, 105, 10);
  if(cho & 0x02){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 97, t_hms->min); }
  LCD_WriteAnyFont(f_4x6_p, 13, 93, 10);
  if(cho & 0x04){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 85, t_hms->hou); }
}


/* ------------------------------------------------------------------*
 *            Phosphor Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ph(struct PlantState *ps)
{
  // ph symbol
  switch(ps->phosphor_state->ph_state)
  {
    case _ph_on: LCD_WriteAnySymbol(6, 134, _n_phosphor); break;
    case _ph_disabled:
    case _ph_off: LCD_WriteAnySymbol(6, 134, _p_phosphor); break;
    default: break;
  }

  // time
  LCD_Sym_Auto_Ph_Time(ps->phosphor_state->ph_tms);
}


/* ------------------------------------------------------------------*
 *            Phosphor time
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ph_Time(struct Tms *tms)
{
  LCD_Sym_Auto_Ph_Time_Min(tms->min);
  LCD_WriteAnyFont(f_4x6_p, 13, 143, 10);
  LCD_Sym_Auto_Ph_Time_Sec(tms->sec);
}

void LCD_Sym_Auto_Ph_Time_Min(int min){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 135, min); }
void LCD_Sym_Auto_Ph_Time_Sec(int sec){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 147, sec); }

/* ------------------------------------------------------------------*
 *            manual symbol data
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Manual_GetSymbolPosition(t_any_symbol sym)
{
  struct RowColPos position = { .row = 0, .col = 0 };

  switch(sym)
  {
    case _p_circulate:
    case _n_circulate: position.row = 3; position.col = 0; break;
    case _p_air:
    case _n_air: position.row = 3; position.col = 40; break;
    case _p_setDown:
    case _n_setDown: position.row = 3; position.col = 80; break;
    case _p_pumpOff:
    case _n_pumpOff: position.row = 2; position.col = 120; break;
    case _p_mud:
    case _n_mud: position.row = 8; position.col = 0; break;
    case _p_compressor:
    case _n_compressor: position.row = 9; position.col = 40; break;
    case _p_phosphor:
    case _n_phosphor: position.row = 9; position.col = 85; break;
    case _p_inflowPump:
    case _n_inflowPump: position.row = 8; position.col = 120; break;
    case _p_valve: 
    case _n_valve: position.row = 15; position.col = 0; break;
    default: break;
  }
  return position;
}


/* ------------------------------------------------------------------*
 *            Mark manual Select
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Draw(t_any_symbol sym)
{
  struct RowColPos position = LCD_Sym_Manual_GetSymbolPosition(sym);
  LCD_WriteAnySymbol(position.row, position.col, sym);
}


/* ------------------------------------------------------------------*
 *            all manual symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_AllSymbols(void)
{
  t_any_symbol symbols[9] = { _p_circulate, _p_air, _p_setDown, _p_pumpOff, _p_mud, _p_compressor, _p_phosphor, _p_inflowPump, _p_valve };
  for(unsigned char i = 0; i < 9; i++){ LCD_Sym_Manual_Draw(symbols[i]); }
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
  LCD_WriteAnyStringFont(f_6x8_p, 17, 136, ":");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 61, "mbar");
  LCD_Sym_MPX_LevelPerc(ps);
}


/* ------------------------------------------------------------------*
 *            page time
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageTime_Min(int min){ LCD_WriteAnyValue(f_6x8_p, 2, 17, 124, min); }
void LCD_Sym_Manual_PageTime_Sec(int sec){ LCD_WriteAnyValue(f_6x8_p, 2, 17, 142, sec); }


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
      if(ps->page_state->page_time->min >= 100){ LCD_WriteAnyValue(f_6x8_p, 3, 17, 118, ps->page_state->page_time->min); }
      else{ LCD_Sym_Manual_PageTime_Min(ps->page_state->page_time->min); }
    }
  }
}


/* ------------------------------------------------------------------*
 *            manual okay button
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PumpOff_OkButton(bool p_sym){ LCD_WriteAnySymbol(15, 85, (p_sym ? _p_ok : _n_ok)); }
void LCD_Sym_Manual_PumpOff_OkButton_Clr(void){ LCD_ClrSpace(15, 85, 5, 104); }


/* ------------------------------------------------------------------*
 *            manual symbol data
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Setup_GetSymbolPosition(t_any_symbol sym)
{
  struct RowColPos position = { .row = 0, .col = 0 };

  switch(sym)
  {
    case _p_circulate:
    case _n_circulate: position.row = 3; position.col = 0; break;
    case _p_air:
    case _n_air: position.row = 3; position.col = 40; break;
    case _p_setDown:
    case _n_setDown: position.row = 3; position.col = 80; break;
    case _p_pumpOff:
    case _n_pumpOff: position.row = 2; position.col = 120; break;
    case _p_mud:
    case _n_mud: position.row = 8; position.col = 0; break;
    case _p_compressor:
    case _n_compressor: position.row = 9; position.col = 40; break;
    case _p_phosphor:
    case _n_phosphor: position.row = 9; position.col = 85; break;
    case _p_inflowPump:
    case _n_inflowPump: position.row = 8; position.col = 120; break;
    case _p_cal:
    case _n_cal: position.row = 15; position.col = 0; break;
    case _p_alarm:
    case _n_alarm: position.row = 15; position.col = 40; break;
    case _p_watch:
    case _n_watch: position.row = 15; position.col = 80; break;
    case _p_zone:
    case _n_zone: position.row = 15; position.col = 120; break;
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
    case _p_cal: case _n_cal:
    case _p_mud: case _n_mud:
    case _p_zone: case _n_zone:
      position.row = 2; position.col = 0; 
      break;

    case _p_inflowPump: case _n_inflowPump: 
      position.row = 1; position.col = 0; 
      break;

    default: break;
  }
  return position;
}


/* ------------------------------------------------------------------*
 *            draw setup symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Draw(t_any_symbol sym)
{
  struct RowColPos position = LCD_Sym_Setup_GetSymbolPosition(sym);
  LCD_WriteAnySymbol(position.row, position.col, sym);
}

/* ------------------------------------------------------------------*
 *            draw setup symbol
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Draw_Sub(t_any_symbol sym)
{
  struct RowColPos position = LCD_Sym_Setup_GetSymbolPosition_Sub(sym);
  LCD_WriteAnySymbol(position.row, position.col, sym);
}

/* ------------------------------------------------------------------*
 *            all setup symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_AllSymbols(void)
{
  t_any_symbol symbols[12] = { _p_circulate, _p_air, _p_setDown, _p_pumpOff, _p_mud, _p_compressor, _p_phosphor, _p_inflowPump, _p_cal, _p_alarm, _p_watch, _p_zone };
  for(unsigned char i = 0; i < 12; i++){ LCD_Sym_Setup_Draw(symbols[i]); }
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
  LCD_Sym_Setup_Draw_Sub(_n_circulate);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            circulate values and texts
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate_TimeText(bool negative){ LCD_FillOrClrSpace(negative, 15, 39, 4, 31); LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 16, 40, "Time:"); }
void LCD_Sym_Setup_Circulate_TimeValue(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 3, 16, 72, value); }

void LCD_Sym_Setup_Circulate_OnText(bool negative){ LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 10, 1, "On:  '"); }
void LCD_Sym_Setup_Circulate_OnValue(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 10, 19, value); }
void LCD_Sym_Setup_Circulate_OnTextValue(bool negative, int value){ LCD_Sym_Setup_Circulate_OnText(negative); LCD_Sym_Setup_Circulate_OnValue(negative, value); }

void LCD_Sym_Setup_Circulate_OffText(bool negative){ LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 10, 40, "Off:  '"); }
void LCD_Sym_Setup_Circulate_OffValue(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 10, 64, value); }
void LCD_Sym_Setup_Circulate_OffTextValue(bool negative, int value){ LCD_Sym_Setup_Circulate_OffText(negative); LCD_Sym_Setup_Circulate_OffValue(negative, value); }


/* ------------------------------------------------------------------*
 *            circulate sensor sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate_Change_Sensor(unsigned char sensor)
{
  LCD_WriteAnySymbol(15, 0, ((bool)sensor ? _n_sensor : _p_sensor));
  LCD_Sym_Setup_Circulate_TimeText(!(bool)sensor);
}


/* ------------------------------------------------------------------*
 *            general use on off time
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_General_OnOffTime(unsigned char select, int on_min, int off_min, int time_min)
{
  LCD_Sym_Setup_Circulate_OnTextValue((select == 0), on_min);
  LCD_Sym_Setup_Circulate_OffTextValue((select == 1), off_min);
  // fill for values
  bool time_select = (select == 2);
  LCD_FillOrClrSpace(!(time_select), 15, 70, 4, 2);
  LCD_FillOrClrSpace(!(time_select), 15, 71, 1, 19);
  LCD_FillOrClrSpace(!(time_select), 18, 71, 1, 19);
  LCD_Sym_Setup_Circulate_TimeValue(time_select, time_min);
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
  LCD_Sym_Setup_Draw_Sub(_n_air);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            air text (same as circulate)
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Air_Change_Values(unsigned char select, struct SettingsAir *settings_air)
{ 
  LCD_Sym_Setup_General_OnOffTime(select, settings_air->on_min, settings_air->off_min, settings_air->time_min);
  LCD_Sym_Setup_Circulate_TimeText(select == 2);
}


/* ------------------------------------------------------------------*
 *            set setup set down
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_SetDown(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_setDown);
  LCD_Sym_WriteCtrlButton();
  LCD_WriteAnyStringFont(f_6x8_p, 10, 0, "Time:");
}

void LCD_Sym_Setup_SetDown_TimeValue(int value){ LCD_WriteAnyValue(f_6x8_n, 3, 10, 30, value); }


/* ------------------------------------------------------------------*
 *            set setup PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_PumpOff(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_pumpOff);
  LCD_Sym_WriteCtrlButton();
}

void LCD_Sym_Setup_PumpOff_Compressor(bool negative){ LCD_WriteAnySymbol(15, 0, (negative ? _n_compressor : _p_compressor)); }
void LCD_Sym_Setup_PumpOff_Pump(bool negative){ LCD_WriteAnySymbol(14, 50, (negative ? _n_pump : _p_pump)); }


/* ------------------------------------------------------------------*
 *            set setup Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Mud(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_mud);
  LCD_Sym_WriteCtrlButton();
  LCD_WriteAnyStringFont(f_6x8_p, 11, 7, "On:");
  LCD_WriteAnyStringFont(f_6x8_p, 11, 55, "'");
  LCD_WriteAnyStringFont(f_6x8_p, 16, 55, "''");
}

void LCD_Sym_Setup_Mud_Min(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 11, 40, value); }
void LCD_Sym_Setup_Mud_Sec(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 16, 40, value); }


/* ------------------------------------------------------------------*
 *            set setup Compressor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Compressor(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_WriteAnyStringFont(f_6x8_p, 11, 28, "mbar MIN.");
  LCD_WriteAnyStringFont(f_6x8_p, 16, 28, "mbar MAX.");
  LCD_Sym_Setup_Draw_Sub(_n_compressor);
  LCD_Sym_WriteCtrlButton();
}

void LCD_Sym_Setup_Compressor_MinP(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 3, 11, 7, value); }
void LCD_Sym_Setup_Compressor_MaxP(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 3, 16, 7, value); }


/* ------------------------------------------------------------------*
 *            set setup Phosphor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Phosphor(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_phosphor);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            set setup inflow pump
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_inflowPump);
  LCD_Sym_WriteCtrlButton();
}


/* ------------------------------------------------------------------*
 *            sensor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Sensor(bool negative){ LCD_WriteAnySymbol(15, 5, (negative ? _n_sensor : _p_sensor)); }


/* ------------------------------------------------------------------*
 *            inflow pump - values
 * -------------------------------------------------------------------
 *    0000 0000 -> 0 on min h 0 Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Values(unsigned char select, struct SettingsInflowPump *settings_inflow_pump)
{
  LCD_Sym_Setup_InflowPump_ValuesOnMin((select == 0), settings_inflow_pump->on_min);
  LCD_Sym_Setup_InflowPump_ValuesOffMin((select == 1), settings_inflow_pump->off_min);
  LCD_Sym_Setup_InflowPump_ValuesOffHou((select == 2), settings_inflow_pump->off_hou);
  // // h
  // if(select & (1 << 0)){ LCD_WriteAnyValue(f_6x8_n, 2, 5, 47, val[0]); }
  // if(select & (1 << 4)){ LCD_WriteAnyValue(f_6x8_p, 2, 5, 47, val[0]); }

  // // min
  // if(select & (1 << 1)){ LCD_WriteAnyValue(f_6x8_n, 2, 10, 47, val[1]); }
  // if(select & (1 << 5)){ LCD_WriteAnyValue(f_6x8_p, 2, 10, 47, val[1]); }

  // // on
  // if(select & (1 << 2)){ LCD_WriteAnyValue(f_6x8_n, 2, 10, 19, val[2]); }
  // if(select & (1 << 6)){ LCD_WriteAnyValue(f_6x8_p, 2, 10, 19, val[2]); }
}

void LCD_Sym_Setup_InflowPump_ValuesOffHou(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 5, 47, value); }
void LCD_Sym_Setup_InflowPump_ValuesOffMin(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 10, 47, value); }
void LCD_Sym_Setup_InflowPump_ValuesOnMin(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 2, 10, 19, value); }


/* ------------------------------------------------------------------*
 *            inflow pump - values - text select
 * -------------------------------------------------------------------
 *    0000 0000 -> off on min h Noff Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Text(unsigned char select)
{
  LCD_Sym_Setup_InflowPump_TextOn((select == 0));
  LCD_Sym_Setup_InflowPump_TextOffMin((select == 1));
  LCD_Sym_Setup_InflowPump_TextOffHou((select == 2));
  LCD_Sym_Setup_InflowPump_TextOff((select == 1) | (select == 2));
  // if(select & (1 << 0)) LCD_WriteAnyStringFont(f_6x8_n, 5, 59, "h");
  // if(select & (1 << 4)) LCD_WriteAnyStringFont(f_6x8_p, 5, 59, "h");

  // if(select & (1 << 1)) LCD_WriteAnyStringFont(f_6x8_n, 10, 59, "min");
  // if(select & (1 << 5)) LCD_WriteAnyStringFont(f_6x8_p, 10, 59, "min");

  // if(select & (1 << 2)) LCD_WriteAnyStringFont(f_6x8_n, 10, 1, "On:");
  // if(select & (1 << 6)) LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "On:");

  // if(select & (1 << 3)) LCD_WriteAnyStringFont(f_6x8_n, 1, 47, "Off");
  // if(select & (1 << 7)) LCD_WriteAnyStringFont(f_6x8_p, 1, 47, "Off");
}

void LCD_Sym_Setup_InflowPump_TextOffHou(bool negative){ LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 5, 59, "h"); }
void LCD_Sym_Setup_InflowPump_TextOffMin(bool negative){ LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 10, 59, "'"); }
void LCD_Sym_Setup_InflowPump_TextOn(bool negative){ LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 10, 1, "On:"); }
void LCD_Sym_Setup_InflowPump_TextOff(bool negative){ LCD_WriteAnyStringFont((negative ? f_6x8_n : f_6x8_p), 1, 47, "Off:"); }


/* ------------------------------------------------------------------*
 *            set setup calibration
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal(struct PlantState *ps)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_cal);
  LCD_Sym_Setup_Cal_Level_Sym(false);
  LCD_Sym_ControlButtons(_ctrl_pos_esc);
  LCD_Sym_ControlButtons(_ctrl_pos_ok);
  LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_ventil, 1);
  LCD_WriteAnySymbol(9, 125, _p_cal);
  LCD_Sym_Setup_Cal_Button(false);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "mbar:");

  // sonic
  unsigned char sonic = MEM_EEPROM_ReadVar(SONIC_on);
  if(sonic)
  { 
    LCD_WriteAnySymbol(2, 40, _n_sonic);
    LCD_Sym_Setup_Cal_Level_Sonic(ps->sonic_state->level_cal);
  }
  else
  { 
    LCD_WriteAnySymbol(2, 40, _p_sonic);
    LCD_Sym_Setup_Cal_Level_MPX(ps->mpx_state->level_cal);
  }

  // calibration redo with pressure -> auto zone page
  if(!MEM_EEPROM_ReadVar(SONIC_on)){ LCD_Sym_Setup_Cal_MPX_Redo((bool)MEM_EEPROM_ReadVar(CAL_Redo_on)); }
}


/* ------------------------------------------------------------------*
 *            cal redo for pressure sensor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_MPX_Redo(bool negative){ LCD_WriteAnySymbol(15, 130, (negative ? _n_arrow_redo : _p_arrow_redo)); }


/* ------------------------------------------------------------------*
 *            set setup calibration level measures and cal sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_Level_Sym(bool negative){ LCD_WriteAnySymbol(15, 1, (negative ? _n_level : _p_level)); }
void LCD_Sym_Setup_Cal_Button(bool negative){ LCD_WriteAnySymbol(9, 125, (negative ? _n_cal : _p_cal)); }


/* ------------------------------------------------------------------*
 *            set setup calibration level measures
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_MPX_Value(int value)
{
  // negative / positive pressure
  if(value < 0){ value = -value; LCD_WriteAnyStringFont(f_6x8_p, 10, 34, "-"); }
  else{ LCD_ClrSpace(10, 34, 2, 5); }
  LCD_WriteAnyValue(f_6x8_p, 3, 10, 40, value);
}


/* ------------------------------------------------------------------*
 *            set setup calibration level measures and count down
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal_Level_Sonic(int level_cal)
{ 
  LCD_WriteAnyValue(f_6x8_p, 4, 17, 40, level_cal); 
  LCD_WriteAnyStringFont(f_6x8_p, 17, 66, "mm");
}

void LCD_Sym_Setup_Cal_Level_MPX(int level_cal)
{ 
  LCD_WriteAnyValue(f_6x8_p, 3, 17, 40, level_cal);
  LCD_WriteAnyStringFont(f_6x8_p, 17, 60, "mbar");
}

void LCD_Sym_Setup_Cal_MPXCountDown(int sec){ LCD_WriteAnyValue(f_6x8_p, 3, 17, 100, sec); }
void LCD_Sym_Setup_Cal_Clr_MPXCountDown(void){ LCD_ClrSpace(17, 100, 2, 18); }


/* ------------------------------------------------------------------*
 *            set setup alarm
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Alarm(struct PlantState *ps)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_alarm);
  LCD_WriteAnyStringFont(f_6x8_n, 10, 3, "T:");

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 10, 32, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 38, "C");

  // temp
  MCP9800_WriteTemp(ps->twi_state);

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 17, 104, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 17, 110, "C");
  LCD_Sym_WriteCtrlButton();
}

void LCD_Sym_Setup_Alarm_TempValue(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 3, 10, 15, value); }
void LCD_Sym_Setup_Alarm_Compressor(bool negative){ LCD_WriteAnySymbol(15, 40, (negative ? _n_compressor : _p_compressor)); }
void LCD_Sym_Setup_Alarm_Sensor(bool negative){ LCD_WriteAnySymbol(15, 0, (negative ? _n_sensor : _p_sensor)); }


/* ------------------------------------------------------------------*
 *            set setup watch
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_watch);
  LCD_Sym_WriteCtrlButton2();

  LCD_WriteAnyStringFont(f_6x8_p, 11, 10, "hh");
  LCD_WriteAnyStringFont(f_6x8_p, 11, 50, "mm");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 10, "dd");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "mm");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 96, "yy");

  LCD_WriteAnyStringFont(f_6x8_p, 9, 33, ":");
  LCD_WriteAnyStringFont(f_6x8_p, 15, 33, "/");
  LCD_WriteAnyStringFont(f_6x8_p, 15, 70, "/");
  LCD_WriteAnyStringFont(f_6x8_p, 15, 84, "20");
}


/* ------------------------------------------------------------------*
 *            set setup zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Zone(void)
{
  LCD_Sym_Clr_InfoSpace();
  LCD_Sym_Setup_Draw_Sub(_n_zone);
  LCD_Sym_WriteCtrlButton();
  LCD_WriteAnyStringFont(f_6x8_p, 11, 60, "cm");
  LCD_WriteAnyStringFont(f_6x8_p, 16, 60, "cm");
}


/* ------------------------------------------------------------------*
 *            zone symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Zone_Level_Air(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 3, 11, 40, value); }
void LCD_Sym_Setup_Zone_Level_SetDown(bool negative, int value){ LCD_WriteAnyValue((negative ? f_6x8_n : f_6x8_p), 3, 16, 40, value); }
void LCD_Sym_Setup_Zone_Sonic(bool negative){ LCD_WriteAnySymbol(3, 47, (negative ? _n_sonic : _p_sonic)); }
void LCD_Sym_Setup_Zone_Air(bool negative){ LCD_WriteAnySymbol(9, 0, (negative ? _n_air : _p_air)); }
void LCD_Sym_Setup_Zone_SetDown(bool negative){ LCD_WriteAnySymbol(14, 0, (negative ? _n_setDown : _p_setDown)); }


/* ------------------------------------------------------------------*
 *            setup pump sym  
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Pump(unsigned char mark)
{
  LCD_WriteAnySymbol(15, 45, _p_compressor);
  LCD_WriteAnySymbol(15, 90, _p_pump);
  LCD_WriteAnySymbol(15, 120, _p_pump2);

  switch (mark)
  {
    case 0: LCD_WriteAnySymbol(15, 45, _n_compressor); break;
    case 1: LCD_WriteAnySymbol(15, 90, _n_pump); break;
    case 2: LCD_WriteAnySymbol(15, 120, _n_pump2); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            first clear, then mark
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch_Mark(t_DateTime time, unsigned char *p_dT)
{
  // time positive
  for(unsigned char i = 5; i < 11; i++){ LCD_Sym_Setup_Watch_DateTime(i, *p_dT); p_dT++; }

  // get correct value
  p_dT = p_dT - 6 + time;

  // negative selection
  LCD_Sym_Setup_Watch_DateTime(time, *p_dT);
}


/* ------------------------------------------------------------------*
 *            watch date and time
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch_DateTime(t_DateTime time, unsigned char value)
{
  switch(time)
  {
    case _p_h: LCD_WriteAnyValue(f_6x8_p, 2, 9, 10, value); break;
    case _p_min: LCD_WriteAnyValue(f_6x8_p, 2, 9, 50, value); break;
    case _p_day: LCD_WriteAnyValue(f_6x8_p, 2, 15, 10, value); break;
    case _p_month: LCD_WriteAnyValue(f_6x8_p, 2, 15, 50, value); break;
    case _p_year: LCD_WriteAnyValue(f_6x8_p, 2, 15, 96, value); break;

    case _n_h: LCD_WriteAnyValue(f_6x8_n, 2, 9, 10, value); break;
    case _n_min: LCD_WriteAnyValue(f_6x8_n, 2, 9, 50, value); break;
    case _n_day: LCD_WriteAnyValue(f_6x8_n, 2, 15, 10, value); break;
    case _n_month: LCD_WriteAnyValue(f_6x8_n, 2, 15, 50, value); break;
    case _n_year: LCD_WriteAnyValue(f_6x8_n, 2, 15, 96, value); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            set data pages
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Page(void)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_data);
  LCD_Sym_Clr_InfoSpace();

  // Sx.x.x
  LCD_WriteAnyFont(f_4x6_p, 1, 0, 21);
  LCD_WriteAnyFont(f_4x6_p, 1, 8, SV1);
  LCD_WriteAnyFont(f_4x6_p, 1, 16, SV2);
  LCD_WriteAnyFont(f_4x6_p, 1, 24, SV3);
  LCD_WriteAnyFont(f_4x6_p, 1, 4, 22);
  LCD_WriteAnyFont(f_4x6_p, 1, 12, 22);
  LCD_WriteAnyFont(f_4x6_p, 1, 20, 22);

  LCD_WriteAnyStringFont(f_6x8_p, 6, 0, "choose data:");

  LCD_Write_TextButton(9, 0, TEXT_BUTTON_auto, 1);
  LCD_Write_TextButton(9, 40, TEXT_BUTTON_manual, 1);
  LCD_Write_TextButton(9, 80, TEXT_BUTTON_setup, 1);
  LCD_Write_TextButton(9, 120, TEXT_BUTTON_sonic, 1);
}


/* ------------------------------------------------------------------*
 *            actual page num
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_ActualPageNum(int page_num){ LCD_WriteAnyValue(f_4x6_p, 2, 1, 140, page_num); }
void LCD_Sym_Data_TotalPageNum(int page_num){ LCD_WriteAnyFont(f_4x6_p, 1, 148, 11); LCD_WriteAnyValue(f_4x6_p, 2, 1, 152, page_num); }

/* ------------------------------------------------------------------*
 *            set data auto
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

  LCD_Write_TextButton(22, 0, TEXT_BUTTON_auto, 0);
  LCD_Sym_Data_Arrows();

  LCD_Sym_Data_ActualPageNum(1);
  LCD_Sym_Data_TotalPageNum(16);
  LCD_Data_WriteAutoEntryPage(0);
}


/* ------------------------------------------------------------------*
 *            set data manual
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

  LCD_Write_TextButton(22, 40, TEXT_BUTTON_manual, 0);
  LCD_Sym_Data_Arrows();

  LCD_Sym_Data_ActualPageNum(1);
  LCD_Sym_Data_TotalPageNum(3);
  LCD_Data_WriteManualEntryPage(0);
}


/* ------------------------------------------------------------------*
 *            set data setup
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Setup(void)
{
  LCD_ClrSpace(4, 0, 9, 160);

  LCD_FillSpace(0, 0, 4, 137);
  LCD_WriteAnyStringFont(f_6x8_n, 1, 1, "Date");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 37, "|");
  LCD_WriteAnyStringFont(f_6x8_n, 1, 45, "Time");

  LCD_Write_TextButton(22, 80, TEXT_BUTTON_setup, 0);
  LCD_Sym_Data_Arrows();

  LCD_Sym_Data_ActualPageNum(1);
  LCD_Sym_Data_TotalPageNum(3);
  LCD_Data_WriteSetupEntryPage(0);
}


/* ------------------------------------------------------------------*
 *            arrows
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Arrows(void)
{
  LCD_Sym_Data_Arrow_Up(false);
  LCD_WriteAnySymbol(8, 140, _line);
  LCD_Sym_Data_Arrow_Down(false);
}

void LCD_Sym_Data_Arrow_Up(bool negative){ LCD_WriteAnySymbol(3, 140, (negative ? _n_arrow_up : _p_arrow_up)); }
void LCD_Sym_Data_Arrow_Down(bool negative){ LCD_WriteAnySymbol(14, 140, (negative ? _n_arrow_down : _p_arrow_down)); }


/* ------------------------------------------------------------------*
 *            set data sonic
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic(struct PlantState *ps)
{
  LCD_ClrSpace(1, 0, 12, 160);
  Sonic_Data_Boot_Off(ps);
  LCD_WriteAnyStringFont(f_6x8_p, 1, 91, "[ C]");

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 1, 97, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 1, 124, "[mm]");
  LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, 1);
  LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, 1);
  LCD_Write_TextButton(16, 0, TEXT_BUTTON_boot, 1);

  LCD_WriteAnySymbol(3, 50, _p_sonic);
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
    case SONIC_APP_75kHz: LCD_WriteAnyStringFont(f_6x8_p, 1, 2, "75kHz"); break;
    case SONIC_APP_125kHz: LCD_WriteAnyStringFont(f_6x8_p, 1, 2, "125kHz"); break;
    case SONIC_APP_boot: LCD_WriteAnyStringFont(f_6x8_p, 1, 2, "Boot  "); break;
    default: break;
  }

  // write software version Sx.x
  LCD_WriteAnyFont(f_4x6_p, 1, 52, 21);
  LCD_WriteAnyFont(f_4x6_p, 1, 57, ((ps->sonic_state->software_version & 0xF0) >> 4));
  LCD_WriteAnyFont(f_4x6_p, 1, 61, 22);
  LCD_WriteAnyFont(f_4x6_p, 1, 65, (ps->sonic_state->software_version & 0x0F));
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
  LCD_WriteAnyValue(f_4x6_p, 5, ps->sonic_state->record_position, 126, ps->sonic_state->d_mm);
  ps->sonic_state->record_position += 3;
}


/* ------------------------------------------------------------------*
 *            data sonic sequential shots
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_SequentialShots(struct PlantState *ps)
{
  LCD_WriteAnyValue(f_4x6_p, 5, 5, 126, ps->sonic_state->d_mm);

  // could not meassure a distance
  if (ps->sonic_state->d_mm <= 0){ return; }

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
    LCD_WriteAnyValue(f_4x6_p, 5, 11, 126, ps->sonic_state->d_mm_max);
  }
  if(ps->sonic_state->d_mm < ps->sonic_state->d_mm_min)
  {
    ps->sonic_state->d_mm_min = ps->sonic_state->d_mm;
    LCD_WriteAnyValue(f_4x6_p, 5, 17, 126, ps->sonic_state->d_mm_min);
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
    LCD_WriteAnyFont(f_4x6_p, ps->sonic_state->record_position, 91, 11);
  }
  //.
  LCD_WriteAnyFont(f_4x6_p, ps->sonic_state->record_position, 102, 22);
  LCD_WriteAnyValue(f_4x6_p, 2, ps->sonic_state->record_position, 105, (ps->sonic_state->temp & 0x00FF));
  LCD_WriteAnyValue(f_4x6_p, 2, ps->sonic_state->record_position, 95, ((ps->sonic_state->temp >> 8) & 0x00FF));
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
    LCD_WriteAnyFont(f_4x6_p, 5, 91, 11);
  }
  LCD_WriteAnyValue(f_4x6_p, 2, 5, 105, (ps->sonic_state->temp & 0x00FF));
  LCD_WriteAnyValue(f_4x6_p, 2, 5, 95, ((ps->sonic_state->temp >> 8) & 0x00FF));
  if(ps->sonic_state->temp > ps->sonic_state->temp_max)
  {
    ps->sonic_state->temp_max = ps->sonic_state->temp;

    //-
    if(minus_sign){ LCD_WriteAnyFont(f_4x6_p, 11, 91, 11); }
    LCD_WriteAnyValue(f_4x6_p, 2, 11, 105, (ps->sonic_state->temp_max & 0x00FF));
    LCD_WriteAnyValue(f_4x6_p, 2, 11, 95, ((ps->sonic_state->temp_max >> 8) & 0x00FF));
  }
  if(ps->sonic_state->temp < ps->sonic_state->temp_min)
  {
    ps->sonic_state->temp_min = ps->sonic_state->temp;

    //-
    if(minus_sign){ LCD_WriteAnyFont(f_4x6_p, 17, 91, 11); }
    LCD_WriteAnyValue(f_4x6_p, 2, 17, 105, (ps->sonic_state->temp_min & 0x00FF));
    LCD_WriteAnyValue(f_4x6_p, 2, 17, 95, ((ps->sonic_state->temp_min >> 8) & 0x00FF));
  }
}


/* ------------------------------------------------------------------*
 *            data sonic boot text
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_AutoText(void)
{
  LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, 0);

  // max
  LCD_WriteAnyFont(f_4x6_p, 9, 113, 13); //m
  LCD_WriteAnyFont(f_4x6_p, 9, 117, 15); //a
  LCD_WriteAnyFont(f_4x6_p, 9, 121, 25); //x

  // min
  LCD_WriteAnyFont(f_4x6_p, 15, 113, 13); //m
  LCD_WriteAnyFont(f_4x6_p, 15, 117, 23); //i
  LCD_WriteAnyFont(f_4x6_p, 15, 121, 24); //n

  // ...
  LCD_WriteAnyFont(f_4x6_p, 17, 102, 22); //.
  LCD_WriteAnyFont(f_4x6_p, 11, 102, 22); //.
  LCD_WriteAnyFont(f_4x6_p, 5, 102, 22);  //.
}


/* ------------------------------------------------------------------*
 *            data sonic boot text
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Sonic_NoUs(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "NoUS"); }
void LCD_Sym_Data_Sonic_BootOk(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "OK...Cycle Power!"); }
void LCD_Sym_Data_Sonic_BootNoData(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "NoData"); }
void LCD_Sym_Data_Sonic_BootError(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "Error"); }
void LCD_Sym_Data_Sonic_BootNone(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "NoBoot"); }
void LCD_Sym_Data_Sonic_BootSucess(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 50, "Success"); }


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
    for(i = 1; i < 130; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11); }
  }

  // write data
  else
  {
    // day - month
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 1, varEnt[0]);
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

    // alarm code
    if(varEnt[7] & 0x01){ LCD_WriteAnyFont(f_4x6_p, pa, 113, 1); }
    else{ LCD_WriteAnyFont(f_4x6_p, pa, 113, 0); }

    if(varEnt[7] & 0x02){ LCD_WriteAnyFont(f_4x6_p, pa, 117, 2); }
    else{ LCD_WriteAnyFont(f_4x6_p, pa, 117, 0); }

    if(varEnt[7] & 0x04){ LCD_WriteAnyFont(f_4x6_p, pa, 121, 3); }
    else{ LCD_WriteAnyFont(f_4x6_p, pa, 121, 0); }

    if(varEnt[7] & 0x08){ LCD_WriteAnyFont(f_4x6_p, pa, 125, 4); }
    else{ LCD_WriteAnyFont(f_4x6_p, pa, 125, 0); }

    if(varEnt[7] & 0x10){ LCD_WriteAnyFont(f_4x6_p, pa, 129, 5); }
    else{ LCD_WriteAnyFont(f_4x6_p, pa, 129, 0); }
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
    for(i = 1; i < 108; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11); }
  }

  // write data
  else
  {
    // day - month
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 1, varEnt[0]);
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
    for(i = 1; i < 68; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11); }
  }

  // write data
  else
  {
    // day - month
    LCD_WriteAnyValue(f_4x6_p, 2, pa, 1, varEnt[0]);
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


/* ------------------------------------------------------------------*
 *            MPX average value
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_AverageValue(t_page page, int av_value)
{
  // page action
  switch(page)
  {
    case AutoZone: case AutoSetDown: case AutoMud: case AutoCircOn: case AutoCircOff: case AutoPumpOff: case AutoAirOn: case AutoAirOff:
      LCD_WriteAnyValue(f_4x6_p, 3, 13, 43, av_value);
      break;

    case ManualMain: case ManualPumpOff_On: case ManualCircOn: case ManualCircOff: case ManualAir: case ManualSetDown: case ManualPumpOff:
    case ManualMud: case ManualCompressor: case ManualPhosphor: case ManualInflowPump:
      LCD_WriteAnyValue(f_6x8_p, 3, 17, 42, av_value);
      break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            MPX no level measure
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_Auto_DisabledLevelMeasure(void)
{
  // ---%
  LCD_WriteAnyFont(f_4x6_p, 15, 5, 11);
  LCD_WriteAnyFont(f_4x6_p, 15, 9, 11);
  LCD_WriteAnyFont(f_4x6_p, 15, 13, 11);
  LCD_WriteAnyFont(f_4x6_p, 15, 18, 19);
}


/* ------------------------------------------------------------------*
 *            MPX mbar value in auto
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_Auto_MbarValue(int value)
{
  // value and mbar
  LCD_WriteAnyValue(f_4x6_p, 3, 17, 43, value);
  LCD_WriteAnyFont(f_4x6_p, 17, 57, 13);
  LCD_WriteAnyFont(f_4x6_p, 17, 61, 14);
  LCD_WriteAnyFont(f_4x6_p, 17, 65, 15);
  LCD_WriteAnyFont(f_4x6_p, 17, 69, 16);
}


/* ------------------------------------------------------------------*
 *            level perc
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_LevelPerc(struct PlantState *ps)
{
  // return if Ultrasonic
  if(MEM_EEPROM_ReadVar(SONIC_on)){ return; }

  // read variables
  int hO2 = ((MEM_EEPROM_ReadVar(TANK_LV_LevelToSetDown_H) << 8) | (MEM_EEPROM_ReadVar(TANK_LV_LevelToSetDown_L)));
  int minP = ((MEM_EEPROM_ReadVar(TANK_LV_MinPressure_H) << 8) | (MEM_EEPROM_ReadVar(TANK_LV_MinPressure_L)));

  // calculate percentage
  int perP = ps->mpx_state->level_cal - minP;
  if(perP <= 0){ perP = 0; }
  perP = ((perP * 100) / hO2);

  // write value
  if(ps->page_state->page == ManualCircOn || ps->page_state->page == ManualCircOff)
  { 
    LCD_WriteAnyValue(f_4x6_p, 3, 17, 2, perP);
    LCD_WriteAnyFont(f_4x6_p, 17, 17, 19);
  }
  else
  { 
    unsigned char row = 17;
    unsigned char col = 0;
    LCD_WriteAnyValue(f_4x6_p, 3, row, col, perP);
    col += 12;
    LCD_WriteAnyFont(f_4x6_p, row, col, 19);
  }
}


/* ------------------------------------------------------------------*
 *            positive pin buttons
 * ------------------------------------------------------------------*/

void LCD_pPinButtons(unsigned char pPin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol = _p_frame;
  unsigned char num = 0;

  switch(pPin)
  {
    case 1: row = 2; col = 0; any_symbol = _p_frame; num = 0x01; break;
    case 4: row = 8; col = 0; any_symbol = _p_frame; num = 0x04; break;
    case 7: row = 14; col = 0; any_symbol = _p_frame; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = _p_escape; num = 0x20; break;

    case 2: row = 2; col = 40; any_symbol = _p_frame; num = 0x02; break;
    case 5: row = 8; col = 40; any_symbol = _p_frame; num = 0x05; break;
    case 8: row = 14; col = 40; any_symbol = _p_frame; num = 0x08; break;
    case 0: row = 20; col = 40; any_symbol = _p_frame; num = 0x00; break;

    case 3: row = 2; col = 80; any_symbol = _p_frame; num = 0x03; break;
    case 6: row = 8; col = 80; any_symbol = _p_frame; num = 0x06; break;
    case 9: row = 14; col = 80; any_symbol = _p_frame; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = _p_del; num = 0x20; break;
    default: break;
  }

  // write symbol
  LCD_WriteAnySymbol(row, col, any_symbol);

  // write number if it is one
  if(num < 0x20){ LCD_WriteAnyFont(f_8x16_p, row + 1, col + 13, num); }
}


void LCD_nPinButtons(unsigned char nPin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol = _p_frame;
  unsigned char num = 0;

  switch(nPin)
  {
    case 1: row = 2; col = 0; any_symbol = _n_frame; num = 0x01; break;
    case 4: row = 8; col = 0; any_symbol = _n_frame; num = 0x04; break;
    case 7: row = 14; col = 0; any_symbol = _n_frame; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = _n_escape; num = 0x20; break;

    case 2: row = 2; col = 40; any_symbol = _n_frame; num = 0x02; break;
    case 5: row = 8; col = 40; any_symbol = _n_frame; num = 0x05; break;
    case 8: row = 14; col = 40; any_symbol = _n_frame; num = 0x08; break;
    case 0: row = 20; col = 40; any_symbol = _n_frame; num = 0x00; break;

    case 3: row = 2; col = 80; any_symbol = _n_frame; num = 0x03; break;
    case 6: row = 8; col = 80; any_symbol = _n_frame; num = 0x06; break;
    case 9: row = 14; col = 80; any_symbol = _n_frame; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = _n_del; num = 0x20; break;
    default: break;
  }

  LCD_WriteAnySymbol(row, col, any_symbol);

  // write number if it is one
  if(num < 0x20) LCD_WriteAnyFont(f_8x16_n, row + 1, col + 13, num);
}


/* ------------------------------------------------------------------*
 *            pin page
 * ------------------------------------------------------------------*/

void LCD_Sym_PinPage(void)
{
  LCD_Clean();
  for(unsigned char i = 0; i < 12; i++){ LCD_pPinButtons(i); }
  LCD_Sym_Pin_DelDigits();
}


/* ------------------------------------------------------------------*
 *            pin page messages and texts
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_RightMessage(void)
{
  LCD_WriteAnyStringFont(f_6x8_p, 6, 125, "right!");
}

void LCD_Sym_Pin_WrongMessage(void)
{
  LCD_WriteAnyStringFont(f_6x8_p, 6, 119, "wrong!");
  LCD_WriteAnyStringFont(f_6x8_p, 9, 119, "try");
  LCD_WriteAnyStringFont(f_6x8_p, 11, 119, "again");
}

void LCD_Sym_Pin_OpHoursMessage(void)
{
  LCD_WriteAnyStringFont(f_6x8_p, 6, 119, "OP");
}


/* ------------------------------------------------------------------*
 *            pin page clear
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_ClearPinCode(void)
{
  LCD_ClrSpace(6, 119, 25, 41);
}


/* ------------------------------------------------------------------*
 *            pin page telephone number
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_PrintWholeTelNumber(struct TeleNr *tele_nr)
{
  // telx:
  LCD_WriteAnyStringFont(f_6x8_p, 6, 119, "Tel");
  LCD_WriteAnyFont(f_6x8_p, 6, 137, tele_nr->id + 14);
  LCD_WriteAnyStringFont(f_6x8_p, 6, 143, ":");

  // +
  LCD_WriteAnyFont(f_4x6_p, 9, 119, 20);

  // 43
  for(unsigned char i = 0; i < 2; i++){ LCD_WriteAnyFont(f_4x6_p, 9, 123 + 4 * i, tele_nr->nr[i]); }

  // 680
  for(unsigned char i = 2; i < 5; i++){ LCD_WriteAnyFont(f_4x6_p, 9, 135 + 4 * (i - 2), tele_nr->nr[i]); }

  // number
  for(unsigned char i = 5; i < 14; i++){ LCD_WriteAnyFont(f_4x6_p, 11, 119 + 4 * (i - 5), tele_nr->nr[i]); }
}


/* ------------------------------------------------------------------*
 *            pin page telephone digit
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_PrintOneTelNumberDigit(unsigned char digit, unsigned char pos)
{
  LCD_nPinButtons(digit);
  if(pos < 2)
  {
    LCD_WriteAnyFont(f_4x6_p, 9, 123 + 4 * pos, digit);
    pos++;
    if(pos < 2){ LCD_WriteAnyFont(f_4x6_n, 9, 127, 14); }
    else{ LCD_WriteAnyFont(f_4x6_n, 9, 135, 14); }
  }

  else if((pos > 1) && (pos < 5))
  {
    LCD_WriteAnyFont(f_4x6_p, 9, 135 + 4 * (pos - 2), digit);
    pos++;
    if(pos < 5){ LCD_WriteAnyFont(f_4x6_n, 9, 135 + 4 * (pos - 2), 14); }
    else{ LCD_WriteAnyFont(f_4x6_n, 11, 119, 14); }
  }
  else
  {
    LCD_WriteAnyFont(f_4x6_p, 11, 119 + 4 * (pos - 5), digit);
    pos++;
    if(pos < 14){ LCD_WriteAnyFont(f_4x6_n, 11, 119 + 4 * (pos - 5), 14); }
  }
}


/* ------------------------------------------------------------------*
 *  delete written digits
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_DelDigits(void)
{
  LCD_WriteAnyStringFont(f_6x8_p, 3, 125, "xxxx");
}


/* ------------------------------------------------------------------*
 *  writes digit on pin field, corresponding to pressed one
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_WriteDigit(unsigned char pin, unsigned char code_pos)
{
  LCD_nPinButtons(pin);
  LCD_WriteAnyFont(f_6x8_p, 3, 125 + 6 * code_pos, pin + 15);
}


/* ------------------------------------------------------------------*
 *            text buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_TextButton(t_text_buttons text, unsigned char pos)
{
  if(pos)
  {
    switch(text)
    {
      case TEXT_BUTTON_auto: LCD_Write_TextButton(22, 0, TEXT_BUTTON_auto, 1); break;
      case TEXT_BUTTON_manual: LCD_Write_TextButton(22, 40, TEXT_BUTTON_manual, 1); break;
      case TEXT_BUTTON_setup: LCD_Write_TextButton(22, 80, TEXT_BUTTON_setup, 1); break;
      case TEXT_BUTTON_data: LCD_Write_TextButton(22, 120, TEXT_BUTTON_data, 1); break;
      default: break;
    }
  }
  else
  {
    switch(text)
    {
      case TEXT_BUTTON_auto: LCD_Write_TextButton(22, 0, TEXT_BUTTON_auto, 0); break;
      case TEXT_BUTTON_manual: LCD_Write_TextButton(22, 40, TEXT_BUTTON_manual, 0); break;
      case TEXT_BUTTON_setup: LCD_Write_TextButton(22, 80, TEXT_BUTTON_setup, 0); break;
      case TEXT_BUTTON_data: LCD_Write_TextButton(22, 120, TEXT_BUTTON_data, 0); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            Control Buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_ControlButtons(t_CtrlButtons ctrl)
{
  switch(ctrl)
  {
    case _ctrl_pos_esc: LCD_WriteAnySymbol(3, 90, _p_esc); break;
    case _ctrl_pos_ok: LCD_WriteAnySymbol(3, 130, _p_ok); break;
    case _ctrl_pos_minus: LCD_WriteAnySymbol(9, 90, _p_minus); break;
    case _ctrl_pos_plus: LCD_WriteAnySymbol(9, 130, _p_plus); break;

    case _ctrl_neg_esc: LCD_WriteAnySymbol(3, 90, _n_esc); break;
    case _ctrl_neg_ok: LCD_WriteAnySymbol(3, 130, _n_ok); break;
    case _ctrl_neg_minus: LCD_WriteAnySymbol(9, 90, _n_minus); break;
    case _ctrl_neg_plus: LCD_WriteAnySymbol(9, 130, _n_plus); break;

    default: break;
  }
}

void LCD_Sym_ControlButtons2(t_CtrlButtons ctrl)
{
  switch(ctrl)
  {
    case _ctrl_pos_esc: LCD_WriteAnySymbol(3, 90, _p_esc); break;
    case _ctrl_pos_ok: LCD_WriteAnySymbol(3, 130, _p_ok); break;
    case _ctrl_pos_minus: LCD_WriteAnySymbol(15, 130, _p_minus); break;
    case _ctrl_pos_plus: LCD_WriteAnySymbol(9, 130, _p_plus); break;

    case _ctrl_neg_esc: LCD_WriteAnySymbol(3, 90, _n_esc); break;
    case _ctrl_neg_ok: LCD_WriteAnySymbol(3, 130, _n_ok); break;
    case _ctrl_neg_minus: LCD_WriteAnySymbol(15, 130, _n_minus); break;
    case _ctrl_neg_plus: LCD_WriteAnySymbol(9, 130, _n_plus); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Pin Okay
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_OkButton(unsigned char select)
{
  switch(select)
  {
    case 0: LCD_WriteAnySymbol(20, 130, _p_ok); break;
    case 1: LCD_WriteAnySymbol(20, 130, _n_ok); break;
    default: LCD_WriteAnySymbol(20, 130, _n_ok); break;
  }
}


/* ------------------------------------------------------------------*
 *            write control buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_WriteCtrlButton(void){ for(unsigned char i = _ctrl_pos_plus; i <= _ctrl_pos_ok; i++){ LCD_Sym_ControlButtons(i); } }
void LCD_Sym_WriteCtrlButton2(void){ for(unsigned char i = _ctrl_pos_plus; i <= _ctrl_pos_ok; i++){ LCD_Sym_ControlButtons2(i); } }


/* ------------------------------------------------------------------*
 *            Mark Text Button
 * ------------------------------------------------------------------*/

void LCD_Sym_MarkTextButton(t_text_buttons text)
{
  for(unsigned char i = 0; i < 4; i++){ if(i != text){ LCD_Sym_TextButton(i, 1); } }
  LCD_Sym_TextButton(text, 0);
}


/* ------------------------------------------------------------------*
 *            can messages
 * ------------------------------------------------------------------*/

void LCD_Sym_CAN_MCPFail(void){ LCD_WriteAnyStringFont(f_6x8_p, 17, 100, "MCP-fail"); }


/* ------------------------------------------------------------------*
 *            sonic none position
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Sonic_NoUS_Pos(t_page page)
{
  struct RowColPos pos = {.row = 0, .col = 0};

  switch(page)
  {
    case AutoSetDown: case AutoMud: case AutoPumpOff:
    case AutoCircOn: case AutoCircOff: case AutoPage:
    case AutoAirOn: case AutoAirOff: 
      pos.row = 17; pos.col = 5;
      break;

    case ManualMain: case ManualCircOn: case ManualCircOff:
    case ManualAir: case ManualSetDown: case ManualPumpOff:
    case ManualPumpOff_On: case ManualMud: case ManualCompressor:
    case ManualPhosphor: case ManualInflowPump: 
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
  LCD_WriteAnyStringFont(f_6x8_p, pos.row, pos.col, "noUS");
}


/* ------------------------------------------------------------------*
 *            auto sonic value
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SonicVal(struct PlantState *ps)
{
  // deactivated sonic
  if(!MEM_EEPROM_ReadVar(SONIC_on) || ps->sonic_state->no_us_flag){ return; }

  // value
  switch(ps->page_state->page)
  {
    case AutoZone:  case AutoSetDown: case AutoPumpOff:
    case AutoMud:   case AutoCircOn:    case AutoCircOff:
    case AutoAirOn:   case AutoAirOff:  case AutoPage:
      // value and mm
      LCD_WriteAnyValue(f_4x6_p, 4, 17, 5, ps->sonic_state->d_mm);
      LCD_WriteAnyFont(f_4x6_p, 17, 22, 13);
      LCD_WriteAnyFont(f_4x6_p, 17, 26, 13);
      break;

    default: break;
  }

  // percentage
  int zero = ((MEM_EEPROM_ReadVar(TANK_LV_Sonic_H) << 8) | (MEM_EEPROM_ReadVar(TANK_LV_Sonic_L)));
  int lvO2 = ((MEM_EEPROM_ReadVar(TANK_LV_LevelToSetDown_H) << 8) | (MEM_EEPROM_ReadVar(TANK_LV_LevelToSetDown_L)));
  
  //water-level-difference
  int per = 0;
  int dif = lvO2 * 10;
  int cal = ps->sonic_state->d_mm - (zero - (lvO2 * 10));
  if(ps->sonic_state->d_mm > zero){ per = 0; }
  else{ per = 100 - ((cal * 10) / dif) * 10; }
  if(!ps->sonic_state->d_mm){ per = 0; }

  // page dependend action
  switch(ps->page_state->page)
  {
    case AutoZone:  case AutoSetDown: case AutoPumpOff:
    case AutoMud:   case AutoCircOn:    case AutoCircOff:
    case AutoAirOn:   case AutoAirOff:  case AutoPage:
      // xxx%
      LCD_WriteAnyValue(f_4x6_p, 3, 15, 5, per);
      LCD_WriteAnyFont(f_4x6_p, 15, 18, 19);
      break;

    case ManualMain:  case ManualCircOn:  case ManualCircOff:
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
    LCD_WriteAnySymbol(16, 134, _n_grad);
  }

  // over-pressure or under-pressure
  if((err & E_OP) || (err & E_UP))
  {
    LCD_ClrSpace(6, 44, 6, 35);
    LCD_WriteAnySymbol(6, 45, _n_alarm);
  }

  // max in tank
  if(err & E_IT)
  {
    LCD_WriteAnySymbol(17, 1, _n_alarm);
    LCD_Sym_TextButton(TEXT_BUTTON_auto, 1);
  }

  // max out tank
  if(err & E_OT)
  {
    LCD_WriteAnySymbol(17, 90, _n_alarm);
    LCD_Sym_TextButton(TEXT_BUTTON_setup, 0);
  }
}


/* ------------------------------------------------------------------*
 *            auto time symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_WorldTime(struct PlantState *ps)
{
  // : symbol
  LCD_WriteAnyFont(f_4x6_p, 2, 128, 10);
  LCD_WriteAnyFont(f_4x6_p, 2, 140, 10);

  // time
  LCD_WriteAnyValue(f_4x6_p, 2, 2, 120, (int)ps->time_state->tic_hou);
  LCD_WriteAnyValue(f_4x6_p, 2, 2, 132, (int)ps->time_state->tic_min);
  LCD_WriteAnyValue(f_4x6_p, 2, 2, 144, (int)ps->time_state->tic_sec);
}


/* ------------------------------------------------------------------*
 *            auto date symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Date(struct PlantState *ps)
{
  // - symbol
  LCD_WriteAnyFont(f_4x6_p, 0, 128, 11);
  LCD_WriteAnyFont(f_4x6_p, 0, 140, 11);

  // year 20xx
  LCD_WriteAnyStringFont(f_4x6_p, 0, 144, "20");

  // date
  LCD_WriteAnyValue(f_4x6_p, 2, 0, 120, MCP7941_ReadTime(ps->twi_state, TIC_DATE));
  LCD_WriteAnyValue(f_4x6_p, 2, 0, 132, MCP7941_ReadTime(ps->twi_state, TIC_MONTH));
  LCD_WriteAnyValue(f_4x6_p, 2, 0, 152, MCP7941_ReadTime(ps->twi_state, TIC_YEAR));
}


/* ------------------------------------------------------------------*
 *            end text sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_EndText(void){ LCD_WriteAnyStringFont(f_6x8_p, 15, 1, "End"); }


/* ------------------------------------------------------------------*
 *            anti sym
 * ------------------------------------------------------------------*/

t_any_symbol LCD_Sym_GetAntiSymbol(t_any_symbol sym)
{
  switch(sym)
  {
    case _n_pumpOff: return _p_pumpOff;
    case _n_mud: return _p_mud;
    case _n_inflowPump: return _p_inflowPump;
    case _n_pump2: return _p_pump2;
    case _n_setDown: return _p_setDown; 
    case _n_alarm: return _p_alarm; 
    case _n_air: return _p_air;
    case _n_sensor: return _p_sensor;
    case _n_watch: return _p_watch;
    case _n_compressor: return _p_compressor;
    case _n_circulate: return _p_circulate;
    case _n_cal: return _p_cal;
    case _n_zone: return _p_zone;
    case _n_level: return _p_level;
    case _n_phosphor: return _p_phosphor;
    case _n_pump: return _p_pump;
    case _n_esc: return _p_esc;
    case _n_plus: return _p_plus;
    case _n_minus: return _p_minus;
    case _n_arrow_up: return _p_arrow_up;
    case _n_arrow_down: return _p_arrow_down;
    case _n_ok: return _p_ok;
    case _n_grad: return _p_grad;
    case _n_sonic: return _p_sonic;
    case _n_arrow_redo: return _p_arrow_redo;
    case _n_frame: return _p_frame;
    case _n_escape: return _p_escape;
    case _n_del: return _p_del;
    case _n_text_frame: return _p_text_frame;
    case _n_valve: return _p_valve;

    case _p_pumpOff: return _n_pumpOff;
    case _p_mud: return _n_mud;
    case _p_inflowPump: return _n_inflowPump;
    case _p_pump2: return _n_pump2;
    case _p_setDown: return _n_setDown; 
    case _p_alarm: return _n_alarm; 
    case _p_air: return _n_air;
    case _p_sensor: return _n_sensor;
    case _p_watch: return _n_watch;
    case _p_compressor: return _n_compressor;
    case _p_circulate: return _n_circulate;
    case _p_cal: return _n_cal;
    case _p_zone: return _n_zone;
    case _p_level: return _n_level;
    case _p_phosphor: return _n_phosphor;
    case _p_pump: return _n_pump;
    case _p_esc: return _n_esc;
    case _p_plus: return _n_plus;
    case _p_minus: return _n_minus;
    case _p_arrow_up: return _n_arrow_up;
    case _p_arrow_down: return _n_arrow_down;
    case _p_ok: return _n_ok;
    case _p_grad: return _n_grad;
    case _p_sonic: return _n_sonic;
    case _p_arrow_redo: return _n_arrow_redo;
    case _p_frame: return _n_frame;
    case _p_escape: return _n_escape;
    case _p_del: return _n_del;
    case _p_text_frame: return _n_text_frame;
    case _p_valve: return _n_valve;

    default: break;
  }
  return _none_symbol;
}