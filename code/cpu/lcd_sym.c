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
  if(!COMPANY){ LCD_WriteAnySymbol(s_logo_purator, 0, 0, _logo_purator); }
  else{ LCD_WriteAnySymbol(s_logo_hecs, 0, 0, _logo_hecs); }
}


/* ------------------------------------------------------------------*
 *            auto symbol set manager
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetManager(struct PlantState *ps)
{
  // clear actual symbol space
  LCD_ClrSpace(5, 0, 2, 35);

  // page dependend symbols
  switch(ps->page_state->page)
  {
    case AutoPage: LCD_Sym_Auto_Main(ps); LCD_Sym_Auto_CountDown(ps->page_state->page_time); break;
    case AutoZone: LCD_Sym_Auto_Zone(); LCD_Sym_Auto_CountDown(ps->page_state->page_time); break;
    case AutoSetDown: LCD_Sym_Auto_SetDown(); LCD_Sym_Auto_CountDown(ps->page_state->page_time); break;
    case AutoPumpOff: LCD_Sym_Auto_PumpOff(); LCD_Sym_Auto_CountDown(ps->page_state->page_time); break;
    case AutoMud: LCD_Sym_Auto_Mud(); LCD_Sym_Auto_CountDown(ps->page_state->page_time); break;

    case AutoAirOn: LCD_Sym_Auto_AirOn(); LCD_Sym_Auto_CountDown(ps->air_circ_state->air_tms); break;
    case AutoAirOff: LCD_Sym_Auto_AirOff(); LCD_Sym_Auto_CountDown(ps->air_circ_state->air_tms); break;
    case AutoCircOn: LCD_Sym_Auto_CircOn(); LCD_Sym_Auto_CountDown(ps->air_circ_state->air_tms); break;
    case AutoCircOff:LCD_Sym_Auto_CircOff(); LCD_Sym_Auto_CountDown(ps->air_circ_state->air_tms); break;

    default: return;
  }

  // write text
  LCD_Sym_Auto_Text(ps);

  // phosphor symbols
  LCD_Sym_Auto_Ph(ps);

  // inflow pump symbols
  LCD_Sym_Auto_Ip_Base(ps);
}


/* ------------------------------------------------------------------*
 *            auto countdown
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CountDown(struct Tms *tms)
{
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 5, tms->min);
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 17, tms->sec);
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
  LCD_WriteAnyFont(f_4x6_p, 13, 13, 10);

  // ip
  LCD_WriteAnyFont(f_4x6_p, 13, 105, 10);
  LCD_WriteAnyFont(f_4x6_p, 13, 93, 10);

  // ph
  LCD_WriteAnyFont(f_4x6_p, 13, 143, 10);

  // time and date
  LCD_Sym_Auto_Time(ps);
  LCD_Sym_Auto_Date(ps);

  // read water level
  MPX_ReadTank(ps, _write);
  LCD_Sym_Auto_SonicVal(ps);

  // compressor hours
  LCD_WriteAnyValue(f_4x6_p, 5, 15,43, MCP7941_Read_Comp_OpHours(ps->twi_state));

  // ip sensor
  if(MEM_EEPROM_ReadVar(SENSOR_outTank)){ LCD_WriteAnySymbol(s_29x17, 16, 90, _p_sensor); }
}


/* ------------------------------------------------------------------*
 *            set auto page
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Main(struct PlantState *ps)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_auto);
  LCD_Clean();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_auto);

  LCD_Sym_Logo();
  LCD_Sym_Auto_Time(ps);
}

/* ------------------------------------------------------------------*
 *            auto compressor symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CompressorNeg(void){ LCD_WriteAnySymbol(s_29x17, 6, 45, _n_compressor); }
void LCD_Sym_Auto_CompressorPos(void){ LCD_WriteAnySymbol(s_29x17, 6, 45, _p_compressor); }


/* ------------------------------------------------------------------*
 *            set auto zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Zone(void)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, _n_circulate);
  LCD_Sym_Auto_CompressorNeg();
  LCD_WriteAnyStringFont(f_6x8_n, 8, 1, "z");
}


/* ------------------------------------------------------------------*
 *            set auto set down
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_SetDown(void)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, _n_setDown);
  LCD_Sym_Auto_CompressorPos();
}


/* ------------------------------------------------------------------*
 *            set auto PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PumpOff(void)
{
  LCD_WriteAnySymbol(s_35x23, 5, 0, _n_pumpOff);
  if(!MEM_EEPROM_ReadVar(PUMP_pumpOff)){ LCD_Sym_Auto_CompressorNeg(); }
  else{ LCD_Sym_Auto_CompressorPos(); }
}


/* ------------------------------------------------------------------*
 *            set auto Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Mud(void)
{
  LCD_WriteAnySymbol(s_35x23, 5, 0, _n_mud);
  LCD_Sym_Auto_CompressorNeg();
}


/* ------------------------------------------------------------------*
 *            circulate and air symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_CircOn(void)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, _n_circulate);
  LCD_Sym_Auto_CompressorNeg();
}

void LCD_Sym_Auto_CircOff(void)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, _p_circulate);
  LCD_Sym_Auto_CompressorPos();
}

void LCD_Sym_Auto_AirOn(void)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, _n_air);
  LCD_Sym_Auto_CompressorNeg();
}

void LCD_Sym_Auto_AirOff(void)
{
  LCD_WriteAnySymbol(s_29x17, 6, 0, _p_air);
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
        LCD_WriteAnySymbol(s_35x23, 5, 89, _p_inflowPump);
        if(p == AutoAirOff || p == AutoCircOff){ LCD_Sym_Auto_CompressorPos(); }
        break;

      case 1: LCD_WriteAnySymbol(s_19x19, 5, 90, _p_pump); break;
      case 2: LCD_WriteAnySymbol(s_35x23, 5, 90, _p_pump2); break;
      default: break;
    }
  }

  // ip on state
  else if(ip_state == _ip_on)
  {
    switch (pump)
    {
      case 0:
        LCD_WriteAnySymbol(s_35x23, 5, 89, _n_inflowPump);
        if(p == AutoAirOff || p == AutoCircOff){ LCD_Sym_Auto_CompressorNeg(); }
        break;

      case 1: LCD_WriteAnySymbol(s_19x19, 5, 90, _n_pump); break;
      case 2: LCD_WriteAnySymbol(s_35x23, 5, 90, _n_pump2); break;
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
  if(cho & 0x02){ LCD_WriteAnyValue(f_4x6_p, 2, 13, 97, t_hms->min); }
  if(cho & 0x04){ LCD_WriteAnyValue(f_4x6_p, 2, 13,85, t_hms->hou); }
}


/* ------------------------------------------------------------------*
 *            Phosphor Symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Ph(struct PlantState *ps)
{
  // ph symbol
  switch(ps->phosphor_state->ph_state)
  {
    case _ph_on: LCD_WriteAnySymbol(s_19x19, 6, 134, _n_phosphor); break;
    case _ph_disabled:
    case _ph_off: LCD_WriteAnySymbol(s_19x19, 6, 134, _p_phosphor); break;
    default: break;
  }

  // time
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 135, ps->phosphor_state->ph_tms->min);
  LCD_WriteAnyValue(f_4x6_p, 2, 13, 147, ps->phosphor_state->ph_tms->sec);
}


/* ------------------------------------------------------------------*
 *            AutoVar - Comp
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_PageTime(struct PlantState *ps, struct Tms *tms)
{
  // sec change
  if(ps->time_state->tic_sec_update_flag)
  {
    // time
    LCD_Sym_Auto_Time(ps);
    LCD_WriteAnyValue(f_4x6_p, 2, 13, 17, tms->sec);
    LCD_WriteAnyValue(f_4x6_p, 2, 13, 5, tms->min);

    // minute update
    if(tms->sec == 59)
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
 *            manual symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Main(struct PlantState *ps)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_manual);
  LCD_Clean();

  // positive setup symbols
  LCD_Sym_Manual_AllSymbols();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_manual);
  LCD_Sym_Manual_Text(ps);
}


/* ------------------------------------------------------------------*
 *            all manual symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_AllSymbols(void)
{
  LCD_WriteAnySymbol(s_29x17, 3, 0, _p_circulate);
  LCD_WriteAnySymbol(s_29x17, 3, 40, _p_air);
  LCD_WriteAnySymbol(s_29x17, 3, 80, _p_setDown);
  LCD_WriteAnySymbol(s_35x23, 2, 120, _p_pumpOff);
  LCD_WriteAnySymbol(s_35x23, 8, 0, _p_mud);
  LCD_WriteAnySymbol(s_29x17, 9, 40, _p_compressor);
  LCD_WriteAnySymbol(s_19x19, 9, 85, _p_phosphor);
  LCD_WriteAnySymbol(s_35x23, 8, 120, _p_inflowPump);
}


/* ------------------------------------------------------------------*
 *            Mark manual Select
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Select(t_any_symbol sym)
{
  // all positive symbols
  LCD_Sym_Manual_AllSymbols();

  switch(sym)
  {
    case _n_circulate: LCD_WriteAnySymbol(s_29x17, 3, 0, _n_circulate); break;
    case _n_air: LCD_WriteAnySymbol(s_29x17, 3, 40, _n_air); break;
    case _n_setDown: LCD_WriteAnySymbol(s_29x17, 3, 80, _n_setDown); break;
    case _n_pumpOff: LCD_WriteAnySymbol(s_35x23, 2, 120, _n_pumpOff); break;
    case _n_mud: LCD_WriteAnySymbol(s_35x23, 8, 0, _n_mud); break;
    case _n_compressor: LCD_WriteAnySymbol(s_29x17, 9, 40, _n_compressor); break;
    case _n_phosphor: LCD_WriteAnySymbol(s_19x19, 9, 85, _n_phosphor); break;
    case _n_inflowPump: LCD_WriteAnySymbol(s_35x23, 8, 120, _n_inflowPump); break;
    default: break;
  }

}

/* ------------------------------------------------------------------*
 *            manual text
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Text(struct PlantState *ps)
{
  LCD_Sym_Manual_CountDown(ps->page_state->page_time);
  LCD_WriteAnyStringFont(f_6x8_p, 17, 136, ":");
  LCD_WriteAnyStringFont(f_6x8_p, 17, 61, "mbar");
  MPX_ReadTank(ps, _write);
}


/* ------------------------------------------------------------------*
 *            countdown
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_CountDown(struct Tms *tms)
{
  LCD_WriteAnyValue(f_6x8_p, 2, 17, 124, tms->min);
  LCD_WriteAnyValue(f_6x8_p, 2, 17, 142, tms->sec);
}


/* ------------------------------------------------------------------*
 *            manual page time
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PageTime(struct PlantState *ps)
{
  // sec change
  if(ps->time_state->tic_sec_update_flag)
  {
    // second
    LCD_WriteAnyValue(f_6x8_p, 2, 17, 142, ps->page_state->page_time->sec);

    // minute
    if(ps->page_state->page_time->sec == 59)
    {
      if(ps->page_state->page_time->min >= 100){ LCD_WriteAnyValue(f_6x8_p, 3, 17, 118, ps->page_state->page_time->min); }
      else{ LCD_WriteAnyValue(f_6x8_p, 2, 17, 124, ps->page_state->page_time->min); }
    }
  }
}


/* ------------------------------------------------------------------*
 *            manual variables
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_PumpOff_PressOk(t_font_type font_type)
{
  LCD_WriteAnyStringFont(font_type, 17, 15, "PRESS OK!:");
}



/* ------------------------------------------------------------------*
 *            set setup pages
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Page(void)
{
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
  LCD_Clean();
  LCD_Sym_Setup_AllSymbols();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup circulate
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Circulate(void)
{
  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_circulate);
  LCD_Clean();

  LCD_WriteAnyStringFont(f_6x8_p, 16, 40, "Time:");
  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_circulate);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup Air
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Air(void)
{
  LCD_WriteAnySymbol(s_29x17, 3, 40, _n_air);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_air);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup set down
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_SetDown(void)
{
  LCD_WriteAnySymbol(s_29x17, 3, 80, _n_setDown);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_setDown);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 0, "Time:");
}


/* ------------------------------------------------------------------*
 *            set setup PumpOff
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_PumpOff(void)
{
  LCD_WriteAnySymbol(s_35x23, 2, 120, _n_pumpOff);
  LCD_Clean();

  LCD_WriteAnySymbol(s_35x23, 3, 0, _n_pumpOff);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup Mud
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Mud(void)
{
  LCD_WriteAnySymbol(s_35x23, 8, 0, _n_mud);
  LCD_Clean();

  LCD_WriteAnySymbol(s_35x23, 2, 0, _n_mud);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup Compressor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Compressor(void)
{
  LCD_WriteAnySymbol(s_29x17, 9, 40, _n_compressor);
  LCD_Clean();

  LCD_WriteAnyStringFont(f_6x8_p, 11,28, "mbar MIN.");
  LCD_WriteAnyStringFont(f_6x8_p, 16,28, "mbar MAX.");
  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_compressor);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup Phosphor
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Phosphor(void)
{
  LCD_WriteAnySymbol(s_19x19, 9, 85, _n_phosphor);
  LCD_Clean();

  LCD_WriteAnySymbol(s_19x19, 3, 0, _n_phosphor);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup inflow pump
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump(void)
{
  LCD_WriteAnySymbol(s_35x23, 8, 120, _n_inflowPump);
  LCD_Clean();

  LCD_WriteAnySymbol(s_35x23, 1, 0, _n_inflowPump);
  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}

/* ------------------------------------------------------------------*
 *            inflow pump - values
 * -------------------------------------------------------------------
 *    0000 0000 -> 0 on min h 0 Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Values(unsigned char select, unsigned char *val)
{
  // h
  if(select & (1 << 0)){ LCD_WriteAnyValue(f_6x8_n, 2, 5, 47, *val); }
  if(select & (1 << 4)){ LCD_WriteAnyValue(f_6x8_p, 2, 5, 47, *val); }
  val++;

  // min
  if(select & (1 << 1)){ LCD_WriteAnyValue(f_6x8_n, 2, 10, 47, *val); }
  if(select & (1 << 5)){ LCD_WriteAnyValue(f_6x8_p, 2, 10, 47, *val); }
  val++;

  // on
  if(select & (1 << 2)){ LCD_WriteAnyValue(f_6x8_n, 2, 10, 19, *val); }
  if(select & (1 << 6)){ LCD_WriteAnyValue(f_6x8_p, 2, 10, 19, *val); }
}


/* ------------------------------------------------------------------*
 *            inflow pump - values - text select
 * -------------------------------------------------------------------
 *    0000 0000 -> off on min h Noff Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_InflowPump_Text(unsigned char select)
{
  if(select & (1 << 0)) LCD_WriteAnyStringFont(f_6x8_n, 5, 59, "h");
  if(select & (1 << 4)) LCD_WriteAnyStringFont(f_6x8_p, 5, 59, "h");

  if(select & (1 << 1)) LCD_WriteAnyStringFont(f_6x8_n, 10, 59, "min");
  if(select & (1 << 5)) LCD_WriteAnyStringFont(f_6x8_p, 10, 59, "min");

  if(select & (1 << 2)) LCD_WriteAnyStringFont(f_6x8_n, 10, 1, "ON:");
  if(select & (1 << 6)) LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "ON:");

  if(select & (1 << 3)) LCD_WriteAnyStringFont(f_6x8_n, 1,47, "OFF");
  if(select & (1 << 7)) LCD_WriteAnyStringFont(f_6x8_p, 1,47, "OFF");
}


/* ------------------------------------------------------------------*
 *            set setup calibration
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Cal(struct PlantState *ps)
{
  LCD_WriteAnySymbol(s_29x17, 15, 0, _n_cal);
  LCD_Clean();
  LCD_WriteAnySymbol(s_29x17, 2, 0, _n_cal);
  LCD_WriteAnySymbol(s_29x17, 15, 1, _p_level);
  LCD_ControlButtons(_setup_pos_sym_esc);
  LCD_ControlButtons(_setup_pos_sym_ok);
  LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_ventil, 1);
  LCD_WriteAnySymbol(s_29x17, 9, 125, _p_cal);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "mbar:");

  // sonic
  unsigned char sonic = MEM_EEPROM_ReadVar(SONIC_on);
  if(sonic)
  { 
    LCD_WriteAnySymbol(s_19x19, 2, 40, _n_sonic);
    LCD_WriteAnyStringFont(f_6x8_p, 17,66, "mm");
    LCD_WriteAnyValue(f_6x8_p, 4, 17, 40, ps->sonic_state->level_cal);
  }
  else
  { 
    LCD_WriteAnySymbol(s_19x19, 2, 40, _p_sonic);
    LCD_WriteAnyStringFont(f_6x8_p, 17,60, "mbar");
    MPX_LevelCal(ps, _init);
  }

  // calibration redo with pressure -> auto zone page
  if(!MEM_EEPROM_ReadVar(SONIC_on))
  {
    unsigned char calRedo = MEM_EEPROM_ReadVar(CAL_Redo_on);
    if(calRedo){ LCD_WriteAnySymbol(s_19x19, 15, 130, _n_arrow_redo); }
    else{ LCD_WriteAnySymbol(s_19x19, 15, 130, _p_arrow_redo); }
  }


  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup alarm
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Alarm(struct PlantState *ps)
{
  LCD_WriteAnySymbol(s_29x17, 15, 40, _n_alarm);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_alarm);
  LCD_WriteAnyStringFont(f_6x8_n, 10,3, "T:");

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 10, 32, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 10, 38, "C");

  // temp
  MCP9800_WriteTemp(ps->twi_state);

  // degree symbol
  LCD_WriteAnyFont(f_6x8_p, 17, 104, 94);
  LCD_WriteAnyStringFont(f_6x8_p, 17, 110, "C");

  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            set setup watch
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch(void)
{
  LCD_WriteAnySymbol(s_29x17, 15, 80, _n_watch);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 3, 0, _n_watch);
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
  LCD_Sym_WriteCtrlButton2();

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
 *            set setup zone
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Zone(void)
{
  LCD_WriteAnySymbol(s_29x17, 15, 120, _n_zone);
  LCD_Clean();

  LCD_WriteAnySymbol(s_29x17, 2, 0, _n_zone);

  LCD_Sym_WriteCtrlButton();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
}


/* ------------------------------------------------------------------*
 *            all setup symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_AllSymbols(void)
{
  LCD_WriteAnySymbol(s_29x17, 3, 0, _p_circulate);
  LCD_WriteAnySymbol(s_29x17, 3, 40, _p_air);
  LCD_WriteAnySymbol(s_29x17, 3, 80, _p_setDown);
  LCD_WriteAnySymbol(s_35x23, 2, 120, _p_pumpOff);
  LCD_WriteAnySymbol(s_35x23, 8, 0, _p_mud);
  LCD_WriteAnySymbol(s_29x17, 9, 40, _p_compressor);
  LCD_WriteAnySymbol(s_19x19, 9, 85, _p_phosphor);
  LCD_WriteAnySymbol(s_35x23, 8, 120, _p_inflowPump);
  LCD_WriteAnySymbol(s_29x17, 15, 0, _p_cal);
  LCD_WriteAnySymbol(s_29x17, 15, 40, _p_alarm);
  LCD_WriteAnySymbol(s_29x17, 15, 80, _p_watch);
  LCD_WriteAnySymbol(s_29x17, 15, 120, _p_zone);
}


/* ------------------------------------------------------------------*
 *            circulate sensor sym
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_CircSensor(unsigned char sensor)
{
  if(sensor)
  {
    LCD_WriteAnySymbol(s_29x17, 15, 0, _n_sensor);
    LCD_ClrSpace(15, 39, 4, 31);
    LCD_WriteAnyStringFont(f_6x8_p, 16, 40, "Time:");
  }
  else
  {
    LCD_WriteAnySymbol(s_29x17, 15, 0, _p_sensor);
    LCD_FillSpace(15, 39, 4, 31);
    LCD_WriteAnyStringFont(f_6x8_n, 16, 40, "Time:"); 
  }
}


/* ------------------------------------------------------------------*
 *            circulate text
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_CircText(unsigned char on, unsigned char *p_var)
{
  unsigned char var[4] = {0};
  unsigned char i = 0;

  LCD_ClrSpace(15, 70, 4, 20);

  for(i = 0; i < 4; i++)
  {
    var[i] = *p_var;
    p_var++;
  }

  LCD_Sym_Setup_OnValue(var[0]);
  LCD_Sym_Setup_OffValue(var[1]);
  LCD_WriteAnyValue(f_6x8_p, 3, 16, 72, (int)((var[3] << 8) | var[2]));

  switch (on)
  {
    case 0: LCD_Sym_Setup_OnValueNeg(var[0]); break;
    case 1: LCD_Sym_Setup_OffValueNeg(var[1]); break;
    case 2: 
      LCD_FillSpace (15, 70, 4, 20);
      LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, (int)((var[3] << 8) | var[2])); 
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            air text
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_AirText(unsigned char on, unsigned char *p_var)
{
  unsigned char var[4] = {0};
  unsigned char i = 0;

  LCD_ClrSpace(15, 39, 4, 51);
  LCD_WriteAnyStringFont(f_6x8_p, 16, 40, "Time:");

  for(i = 0; i < 4; i++)
  {
    var[i] = *p_var;
    p_var++;
  }

  LCD_Sym_Setup_OnValue(var[0]);
  LCD_Sym_Setup_OffValue(var[1]);
  LCD_WriteAnyValue(f_6x8_p, 3, 16, 72, ((var[3] << 8) | var[2]));

  switch (on)
  {
    case 0: LCD_Sym_Setup_OnValueNeg(var[0]); break;
    case 1: LCD_Sym_Setup_OffValueNeg(var[1]); break;
    case 2:
      LCD_FillSpace (15, 70, 4, 20);
      LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, ((var[3] << 8) | var[2]));
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            setup pump sym  
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Pump(unsigned char mark)
{
  LCD_WriteAnySymbol(s_29x17, 15, 45, _p_compressor);
  LCD_WriteAnySymbol(s_19x19, 15, 90, _p_pump);
  LCD_WriteAnySymbol(s_35x23, 15, 120, _p_pump2);

  switch (mark)
  {
    case 0: LCD_WriteAnySymbol(s_29x17, 15, 45, _n_compressor); break;
    case 1: LCD_WriteAnySymbol(s_19x19, 15, 90, _n_pump); break;
    case 2: LCD_WriteAnySymbol(s_35x23, 15, 120, _n_pump2); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            first clear, then mark
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Watch_Mark(t_DateTime time, unsigned char *p_dT)
{
  // time positive
  for(unsigned char i = 5; i < 11; i++)
  {
    LCD_Sym_Setup_Watch_DateTime(i, *p_dT);
    p_dT++;
  }

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
  LCD_Clean();
  LCD_Sym_MarkTextButton(TEXT_BUTTON_data);

  // Sx.x.x
  LCD_WriteAnyFont(f_4x6_p, 1, 0, 21);
  LCD_WriteAnyFont(f_4x6_p, 1,8, SV1);
  LCD_WriteAnyFont(f_4x6_p, 1, 16, SV2);
  LCD_WriteAnyFont(f_4x6_p, 1,24, SV3);
  LCD_WriteAnyFont(f_4x6_p, 1,4, 22);
  LCD_WriteAnyFont(f_4x6_p, 1, 12, 22);
  LCD_WriteAnyFont(f_4x6_p, 1,20, 22);

  LCD_WriteAnyStringFont(f_6x8_p, 6, 0, "choose data:");

  LCD_Write_TextButton(9, 0, TEXT_BUTTON_auto, 1);
  LCD_Write_TextButton(9, 40, TEXT_BUTTON_manual, 1);
  LCD_Write_TextButton(9, 80, TEXT_BUTTON_setup, 1);
  LCD_Write_TextButton(9, 120, TEXT_BUTTON_sonic, 1);
}


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
  LCD_Sym_DataArrows();

  LCD_WriteAnyStringFont(f_4x6_p, 1, 140, "01;16");
  LCD_WriteAutoEntryPage(0);
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
  LCD_Sym_DataArrows();

  LCD_WriteAnyStringFont(f_4x6_p, 1, 144, "1;3");
  LCD_WriteManualEntryPage(0);
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
  LCD_Sym_DataArrows();

  LCD_WriteAnyStringFont(f_4x6_p, 1, 144, "1;3");
  LCD_WriteSetupEntryPage(0);
}


/* ------------------------------------------------------------------*
 *            arrows
 * ------------------------------------------------------------------*/

void LCD_Sym_DataArrows(void)
{
  LCD_WriteAnySymbol(s_19x19, 3, 140, _p_arrow_up);
  LCD_WriteAnySymbol(s_19x19, 14, 140, _p_arrow_down);
  LCD_WriteAnySymbol(s_19x19, 8, 140, _p_line);
}


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

  LCD_WriteAnySymbol(s_19x19, 3, 50, _p_sonic);
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
  LCD_WriteAnyFont(f_4x6_p, 1,61, 22);
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

  for(i = 0; i < 8; i++)
  {
    varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i);
  }
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

  for(i = 0; i < 7; i++)
  {
    varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i);
  }

  // no data
  if(!varEnt[0] && !varEnt[1] && !varEnt[2])
  {
    for(i = 1; i < 108; i = i + 4){ LCD_WriteAnyFont(f_4x6_p, pa, i, 11); }
  }

  // write data
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
 *            setup entry
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_WriteSetupEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
  unsigned char varEnt[7] = {0x00};
  unsigned char i = 0;

  for(i = 0; i < 5; i++)
  {
    varEnt[i] = MEM_EEPROM_ReadData(eePage, entry, i);
  }

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
 *            MPX
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_Auto_LevelPerc(int value)
{
  LCD_WriteAnyValue(f_4x6_p, 3, 15, 5, value);
  LCD_WriteAnyFont(f_4x6_p, 15, 18, 19);
}

/* ------------------------------------------------------------------*
 *            MPX
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_Manual_LevelPerc(int value)
{
  // value and %
  LCD_WriteAnyValue(f_4x6_p, 3, 17, 2, value);
  LCD_WriteAnyFont(f_4x6_p, 17, 17, 19);
}


/* ------------------------------------------------------------------*
 *            positive pin buttons
 * ------------------------------------------------------------------*/

void LCD_pPinButtons(unsigned char pPin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol = _frame;
  unsigned char num = 0;

  switch(pPin)
  {
    case 1: row = 2; col = 0; any_symbol = _frame; num = 0x01; break;
    case 4: row = 8; col = 0; any_symbol = _frame; num = 0x04; break;
    case 7: row = 14; col = 0; any_symbol = _frame; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = _p_escape; num = 0x20; break;

    case 2: row = 2; col = 40; any_symbol = _frame; num = 0x02; break;
    case 5: row = 8; col = 40; any_symbol = _frame; num = 0x05; break;
    case 8: row = 14; col = 40; any_symbol = _frame; num = 0x08; break;
    case 0: row = 20; col = 40; any_symbol = _frame; num = 0x00; break;

    case 3: row = 2; col = 80; any_symbol = _frame; num = 0x03; break;
    case 6: row = 8; col = 80; any_symbol = _frame; num = 0x06; break;
    case 9: row = 14; col = 80; any_symbol = _frame; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = _p_del; num = 0x20; break;
    default: break;
  }

  // write symbol
  LCD_WriteAnySymbol(s_34x21, row, col, any_symbol);

  // write number if it is one
  if(num < 0x20){ LCD_WriteAnyFont(f_8x16_p, row + 1, col + 13, num); }
}


void LCD_nPinButtons(unsigned char nPin)
{
  unsigned char row = 0;
  unsigned char col = 0;
  t_any_symbol any_symbol = _frame;
  unsigned char num = 0;

  switch(nPin)
  {
    case 1: row = 2; col = 0; any_symbol = _black; num = 0x01; break;
    case 4: row = 8; col = 0; any_symbol = _black; num = 0x04; break;
    case 7: row = 14; col = 0; any_symbol = _black; num = 0x07; break;
    case 11: row = 20; col = 0; any_symbol = _n_escape; num = 0x20; break;

    case 2: row = 2; col = 40; any_symbol = _black; num = 0x02; break;
    case 5: row = 8; col = 40; any_symbol = _black; num = 0x05; break;
    case 8: row = 14; col = 40; any_symbol = _black; num = 0x08; break;
    case 0: row = 20; col = 40; any_symbol = _black; num = 0x00; break;

    case 3: row = 2; col = 80; any_symbol = _black; num = 0x03; break;
    case 6: row = 8; col = 80; any_symbol = _black; num = 0x06; break;
    case 9: row = 14; col = 80; any_symbol = _black; num = 0x09; break;
    case 10: row = 20; col = 80; any_symbol = _n_del; num = 0x20; break;
    default: break;
  }

  LCD_WriteAnySymbol(s_34x21, row, col, any_symbol);

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

void LCD_Sym_Pin_Clear(void)
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

void LCD_Sym_Pin_WriteDigit(unsigned char pin, unsigned char codePos)
{
  LCD_nPinButtons(pin);
  LCD_WriteAnyFont(f_6x8_p, 3, 125 + 6 * codePos, pin + 15);
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

void LCD_ControlButtons(t_CtrlButtons ctrl)
{
  switch(ctrl)
  {
    case _setup_pos_sym_esc: LCD_WriteAnySymbol(s_19x19, 3, 90, _p_esc); break;
    case _setup_pos_sym_ok: LCD_WriteAnySymbol(s_19x19, 3, 130, _p_ok); break;
    case _setup_pos_sym_minus: LCD_WriteAnySymbol(s_19x19, 9, 90, _p_minus); break;
    case _setup_pos_sym_plus: LCD_WriteAnySymbol(s_19x19, 9, 130, _p_plus); break;

    case _setup_neg_sym_esc: LCD_WriteAnySymbol(s_19x19, 3, 90, _n_esc); break;
    case _setup_neg_sym_ok: LCD_WriteAnySymbol(s_19x19, 3, 130, _n_ok); break;
    case _setup_neg_sym_minus: LCD_WriteAnySymbol(s_19x19, 9, 90, _n_minus); break;
    case _setup_neg_sym_plus: LCD_WriteAnySymbol(s_19x19, 9, 130, _n_plus); break;

    default: break;
  }
}

void LCD_ControlButtons2(t_CtrlButtons ctrl)
{
  switch(ctrl)
  {
    case _setup_pos_sym_esc: LCD_WriteAnySymbol(s_19x19,  3,  90, _p_esc); break;
    case _setup_pos_sym_ok: LCD_WriteAnySymbol(s_19x19,  3,  130, _p_ok); break;
    case _setup_pos_sym_minus: LCD_WriteAnySymbol(s_19x19,  15, 130, _p_minus); break;
    case _setup_pos_sym_plus: LCD_WriteAnySymbol(s_19x19,  9,  130, _p_plus); break;

    case _setup_neg_sym_esc: LCD_WriteAnySymbol(s_19x19,  3,  90, _n_esc); break;
    case _setup_neg_sym_ok: LCD_WriteAnySymbol(s_19x19,  3,  130, _n_ok); break;
    case _setup_neg_sym_minus: LCD_WriteAnySymbol(s_19x19,  15, 130, _n_minus); break;
    case _setup_neg_sym_plus: LCD_WriteAnySymbol(s_19x19,  9,  130, _n_plus); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Pin Okay
 * ------------------------------------------------------------------*/

void LCD_Sym_Pin_OkButton(unsigned char on)
{
  switch(on)
  {
    case 0: LCD_WriteAnySymbol(s_19x19, 20, 130, _p_ok); break;
    case 1: LCD_WriteAnySymbol(s_19x19, 20, 130, _n_ok); break;
    default: LCD_WriteAnySymbol(s_19x19, 20, 130, _n_ok); break;
  }
}


/* ------------------------------------------------------------------*
 *            write control buttons
 * ------------------------------------------------------------------*/

void LCD_Sym_WriteCtrlButton(void)
{
  for(unsigned char i = 4; i < 9; i++){ LCD_ControlButtons(i); }
}

void LCD_Sym_WriteCtrlButton2(void)
{
  for(unsigned char i = 4; i < 9; i++){ LCD_ControlButtons2(i); }
}


/* ------------------------------------------------------------------*
 *            on/off values
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_OnValue(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_p, 10, 1, "ON:");
  LCD_WriteAnyValue(f_6x8_p, 2, 10, 19, value);
}

void LCD_Sym_Setup_OffValue(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_p, 10, 40, "OFF:");
  LCD_WriteAnyValue(f_6x8_p, 2, 10, 64, value);
}

void LCD_Sym_Setup_OnValueNeg(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_n, 10, 1, "ON:");
  LCD_WriteAnyValue(f_6x8_n, 2, 10, 19, value);
}

void LCD_Sym_Setup_OffValueNeg(unsigned char value)
{
  LCD_WriteAnyStringFont(f_6x8_n, 10, 40, "OFF:");
  LCD_WriteAnyValue(f_6x8_n, 2, 10, 64, value);
}


/* ------------------------------------------------------------------*
 *            Mark Text Button
 * ------------------------------------------------------------------*/

void LCD_Sym_MarkTextButton(t_text_buttons text)
{
  for(unsigned char i = 0; i < 4; i++)
  {
    if(i != text){ LCD_Sym_TextButton(i, 1); }
  }
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
  int zero = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) | (MEM_EEPROM_ReadVar(SONIC_L_LV)));
  int lvO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | (MEM_EEPROM_ReadVar(TANK_L_O2)));
  
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
    LCD_WriteAnySymbol(s_19x19, 16, 134, _n_grad);
  }

  // over-pressure or under-pressure
  if((err & E_OP) || (err & E_UP))
  {
    LCD_ClrSpace(6, 44, 6, 35);
    LCD_WriteAnySymbol(s_29x17, 6, 45, _n_alarm);
  }

  // max in tank
  if(err & E_IT)
  {
    LCD_WriteAnySymbol(s_29x17, 17, 1, _n_alarm);
    LCD_Sym_TextButton(TEXT_BUTTON_auto, 1);
  }

  // max out tank
  if(err & E_OT)
  {
    LCD_WriteAnySymbol(s_29x17, 17, 90, _n_alarm);
    LCD_Sym_TextButton(TEXT_BUTTON_setup, 0);
  }
}


/* ------------------------------------------------------------------*
 *            auto time symbols
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Time(struct PlantState *ps)
{
  // : symbol
  LCD_WriteAnyFont(f_4x6_p, 2, 128, 10);
  LCD_WriteAnyFont(f_4x6_p, 2, 140, 10);

  // time
  LCD_WriteAnyValue(f_4x6_p, 2, 2, 120, (int)ps->time_state->tic_hou);
  LCD_WriteAnyValue(f_4x6_p, 2, 2, 132, (int)ps->time_state->tic_min);
  LCD_WriteAnyValue(f_4x6_p, 2, 2, 144, (int)ps->time_state->tic_sec);
}

// void LCD_Sym_Auto_Time_Change(struct PlantState *ps)
// {
//   // sec
//   unsigned char time = MCP7941_ReadTime(ps->twi_state, TIC_SEC);
//   LCD_WriteAnyValue(f_4x6_p, 2, 2, 144, MCP7941_ReadTime(ps->twi_state, TIC_SEC));

//   // min
//   if(!time)
//   {
//     time = MCP7941_ReadTime(ps->twi_state, TIC_MIN);
//     LCD_WriteAnyValue(f_4x6_p, 2, 2, 132, time);
//   }
//   else return;

//   // hour
//   if(!time)
//   {
//     time = MCP7941_ReadTime(ps->twi_state, TIC_HOUR);
//     LCD_WriteAnyValue(f_4x6_p, 2, 2, 120, time);
//   }
// }


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

void LCD_Sym_Data_EndText(void)
{
  LCD_WriteAnyStringFont(f_6x8_p, 15, 1, "End");
}