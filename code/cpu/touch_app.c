// --
// touch applications

#include "touch_app.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_sym.h"
#include "lcd_app.h"
#include "memory_app.h"
#include "touch_driver.h"
#include "mcp7941_driver.h"
#include "mpx_driver.h"
#include "modem_driver.h"
#include "tc_func.h"
#include "port_func.h"
#include "output_app.h"
#include "error_func.h"
#include "sonic_app.h"
#include "can_app.h"
#include "basic_func.h"
#include "at24c_app.h"


/*-------------------------------------------------------------------*
 *  Touch_Matrix
 * --------------------------------------------------------------
 *  return 0xYX
 *  Touch Matrix:
 *    1.Row: 11, 12, 13, 14
 *    2.Row: 21, 22, 23, 24, usw.
 * ------------------------------------------------------------------*/

unsigned char Touch_Matrix(struct TouchState *touch_state)
{
  unsigned char lx = 6;
  unsigned char hy = 6;

  // read touchpanel
  Touch_Read(touch_state);

  // ready for new data
  if(touch_state->state == _touch_ready)
  {
    // collect data
    if(touch_state->chunk < 2)
    {
      touch_state->y_data[touch_state->chunk] = touch_state->y;
      touch_state->x_data[touch_state->chunk] = touch_state->x;
      touch_state->chunk++;
    }

    // interpret data
    if(touch_state->chunk >= 2)
    {
      // reset chunk pos
      touch_state->chunk = 0;

      // too much diffs, reject sample
      if((touch_state->x_data[0] < touch_state->x_data[1] - 10) || (touch_state->x_data[0] > touch_state->x_data[1] + 10)){ return 0x66; }
      if((touch_state->y_data[0] < touch_state->y_data[1] - 10) || (touch_state->y_data[0] > touch_state->y_data[1] + 10)){ return 0x66; }

      // average data
      float x_av = (touch_state->x_data[0] + touch_state->x_data[1]) / 2.0;
      float y_av = (touch_state->y_data[0] + touch_state->y_data[1]) / 2.0;

      // calibrate with average
      int x_av_cal = Touch_Cal_X_Value(x_av);
      int y_av_cal = Touch_Cal_Y_Value(y_av);

      // x-matrix
      if(x_av_cal < 16){ lx = 0; }
      else if(x_av_cal > 16 && x_av_cal < 35){ lx = 1; }
      else if(x_av_cal > 47 && x_av_cal < 70){ lx = 2; }
      else if(x_av_cal > 80 && x_av_cal < 105){ lx = 3; }
      else if(x_av_cal > 115 && x_av_cal < 145){ lx = 4; }
      else{ lx = 5; }

      // y-matrix
      if(y_av_cal < 16){ hy = 0; }
      else if(y_av_cal > 18 && y_av_cal < 37){ hy = 1; }
      else if(y_av_cal > 45 && y_av_cal < 58){ hy = 2; }
      else if(y_av_cal > 65 && y_av_cal < 77){ hy = 3; }
      else if(y_av_cal > 85 && y_av_cal < 105){ hy = 4; }
      else{ hy = 5; }
    }
  }

  //*** debug ps->touch_state->touched matrix
  // if(DEBUG)
  // {
  //   if(hy != 6){ LCD_WriteAnyValue(f_4x6_p, 2, 22, 152, hy); }
  //   if(lx != 6){ LCD_WriteAnyValue(f_4x6_p, 2, 24, 152, lx); }
  // }
  
  return ((hy << 4) | lx);
}


/* ------------------------------------------------------------------*
 *            touch linker selection
 * ------------------------------------------------------------------*/

void Touch_SelectLinker(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    // setup pages
    case SetupMain: Touch_Setup_Linker(ps); break;
    case SetupCirculate: Touch_Setup_CirculateLinker(ps); break;
    case SetupAir: Touch_Setup_AirLinker(ps); break;
    case SetupSetDown: Touch_Setup_SetDownLinker(ps); break;
    case SetupPumpOff: Touch_Setup_PumpOffLinker(ps); break;
    case SetupMud: Touch_Setup_MudLinker(ps); break;
    case SetupCompressor: Touch_Setup_CompressorLinker(ps); break;
    case SetupPhosphor: Touch_Setup_PhosphorLinker(ps); break;
    case SetupInflowPump: Touch_Setup_InflowPumpLinker(ps); break;
    case SetupCal:
    case SetupCalPressure: Touch_Setup_CalLinker(ps); break;
    case SetupAlarm: Touch_Setup_AlarmLinker(ps); break;
    case SetupWatch: Touch_Setup_WatchLinker(ps); break;
    case SetupZone: Touch_Setup_ZoneLinker(ps); break;

    // data pages
    case DataMain: Touch_Data_Linker(ps); break;
    case DataAuto: Touch_Data_AutoLinker(ps); break;
    case DataManual: Touch_Data_ManualLinker(ps); break;
    case DataSetup: Touch_Data_SetupLinker(ps); break;
    case DataSonic: case DataSonicAuto: case DataSonicBoot: case DataSonicBootR: case DataSonicBootW: Touch_Data_SonicLinker(ps); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *  auto linker
 * ------------------------------------------------------------------*/

void Touch_Auto_Linker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = 0;
  }

  // handlers
  unsigned char *bug = &ps->touch_state->var[0];

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // backlight
    case 0x11: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x12: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x13: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x14: PORT_Backlight_On(ps->backlight); *bug = 0; break;

    // secret code
    case 0x21:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        if(*bug == 0){ *bug = 1; }
        else if(*bug == 1){ *bug = 2; }
        else{ *bug = 0; }
        PORT_Backlight_On(ps->backlight);
      }
      break;

    case 0x22: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x23: PORT_Backlight_On(ps->backlight); *bug = 0; break;

    // secret code
    case 0x24:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        if(*bug == 2){ *bug = 3; }
        else if(*bug == 3){ *bug = 4; }
        else{ *bug = 0; }
        PORT_Backlight_On(ps->backlight);
      }
      break;

    case 0x31: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x32: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x33: PORT_Backlight_On(ps->backlight); *bug = 0; break;
    case 0x34: PORT_Backlight_On(ps->backlight); *bug = 0; break;

    // auto
    case 0x41: 
      if(!ps->port_state->valve_action_flag)
      {
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        if(*bug == 4){ *bug = 5; }
        else{ *bug = 0; }
      }
      break;

    // manual
    case 0x42: 
      if(!ps->port_state->valve_action_flag)
      {
        ps->touch_state->init = false;
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        *bug = 0;
        LCD_Sym_MarkTextButton(TEXT_BUTTON_manual);
        ps->page_state->page = PinManual; LCD_PinPage_Init(ps);
      }
      break;

    // setup
    case 0x43: 
      if(!ps->port_state->valve_action_flag)
      {
        ps->touch_state->init = false;
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        *bug = 0;
        LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
        ps->page_state->page = PinSetup; LCD_PinPage_Init(ps);
      }
      break;

    // data
    case 0x44: 
      if(!ps->port_state->valve_action_flag)
      {
        ps->touch_state->init = false;
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        *bug = 0;
        LCD_Sym_MarkTextButton(TEXT_BUTTON_data);
        ps->page_state->page = DataPage;
      }
      break;

    case 0x00: 
      if(ps->touch_state->touched){ ps->touch_state->touched = _ctrl_zero; }
      break;

    default: break;
  }

  // bug correct
  if(*bug == 5)
  {
    ps->page_state->page_time->min = 0;
    ps->page_state->page_time->sec = 5;
    LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);
    *bug = 0;
  }
}


/* ------------------------------------------------------------------*
 *            manual linker
 * ------------------------------------------------------------------*/

void Touch_Manual_Linker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;

    // selected
    ps->touch_state->var[0] = _none_symbol;
    ps->touch_state->var[1] = _none_symbol;
    ps->touch_state->var[2] = _none_symbol;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    case 0x11: if(ps->page_state->page != ManualCircOn){ Touch_Manual_Linker_Select(ps, _n_circulate); } break;
    case 0x12: if(ps->page_state->page != ManualAir){ Touch_Manual_Linker_Select(ps, _n_air); } break;
    case 0x13: if(ps->page_state->page != ManualSetDown){ Touch_Manual_Linker_Select(ps, _n_setDown); } break;
    case 0x14: if(ps->page_state->page != ManualPumpOff){ Touch_Manual_Linker_Select(ps, _n_pumpOff); } break;

    case 0x21: if(ps->page_state->page != ManualMud){ Touch_Manual_Linker_Select(ps, _n_mud); } break;
    case 0x22: if(ps->page_state->page != ManualCompressor){ Touch_Manual_Linker_Select(ps, _n_compressor); } break;
    case 0x23: if(ps->page_state->page != ManualPhosphor){ Touch_Manual_Linker_Select(ps, _n_phosphor); } break;
    case 0x24: if(ps->page_state->page != ManualInflowPump){ Touch_Manual_Linker_Select(ps, _n_inflowPump); } break;

    case 0x31: if(ps->page_state->page != ManualValveTest){ Touch_Manual_Linker_Select(ps, _n_valve); } break;
    case 0x33: if(ps->page_state->page == ManualPumpOff){ LCD_Sym_Manual_PumpOff_OkButton(false); ps->page_state->page = ManualPumpOff_On; } break;

    // no touch
    case 0x00: if(ps->touch_state->var[2] != _none_symbol){ LCD_Sym_Manual_Draw(ps->touch_state->var[2]); ps->touch_state->var[2] = _none_symbol; } break;

    // main linker
    case 0x41: if(!ps->port_state->valve_action_flag){ ps->touch_state->init = false; PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_auto); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = AutoPage; } break;
    case 0x42: if(!ps->port_state->valve_action_flag){ PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_manual); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = ManualPage; } break;
    case 0x43: if(!ps->port_state->valve_action_flag){ ps->touch_state->init = false; PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_setup); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = SetupPage; } break;
    case 0x44: if(!ps->port_state->valve_action_flag){ ps->touch_state->init = false; PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_data); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = DataPage; } break;

    default: break;
  }
}


/*-------------------------------------------------------------------*
 *            select stuff
 * ------------------------------------------------------------------*/

void Touch_Manual_Linker_Select(struct PlantState *ps, t_any_symbol sym)
{
  // new symbol select
  ps->touch_state->var[0] = sym;

  // backlight
  PORT_Backlight_On(ps->backlight);

  // select
  LCD_Sym_Manual_Draw(sym);

  // new page
  if(!ps->port_state->valve_action_flag)
  {
    // deselect old state
    LCD_Sym_Manual_Draw(LCD_Sym_GetAntiSymbol(ps->touch_state->var[1]));
    switch(sym)
    {
      case _n_circulate: ps->page_state->page = ManualCircOn; break;
      case _n_air: ps->page_state->page = ManualAir; break;
      case _n_setDown: ps->page_state->page = ManualSetDown; break;
      case _n_pumpOff: ps->page_state->page = ManualPumpOff; break;
      case _n_mud: ps->page_state->page = ManualMud; break;
      case _n_compressor: ps->page_state->page = ManualCompressor; break;
      case _n_phosphor: ps->page_state->page = ManualPhosphor; break;
      case _n_inflowPump: ps->page_state->page = ManualInflowPump; break;
      case _n_valve: ps->page_state->page = ManualValveTest; break;
      default: break;  
    }
    // keep old symbol
    ps->touch_state->var[1] = ps->touch_state->var[0];
  }
  else
  {
    // deselect later
    ps->touch_state->var[2] = LCD_Sym_GetAntiSymbol(ps->touch_state->var[0]);
  }
}


/*-------------------------------------------------------------------*
 *            setup linker
 * ------------------------------------------------------------------*/

void Touch_Setup_Linker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;

    // times
    ps->touch_state->var[0] = NoPage;
    ps->touch_state->var[1] = _none_symbol;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    case 0x11: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupCirculate; ps->touch_state->var[1] = _n_circulate; } break;
    case 0x12: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupAir; ps->touch_state->var[1] = _n_air; } break;
    case 0x13: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupSetDown; ps->touch_state->var[1] = _n_setDown; } break;
    case 0x14: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupPumpOff; ps->touch_state->var[1] = _n_pumpOff; } break;

    case 0x21: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupMud; ps->touch_state->var[1] = _n_mud; } break;
    case 0x22: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupCompressor; ps->touch_state->var[1] = _n_compressor; } break;
    case 0x23: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupPhosphor; ps->touch_state->var[1] = _n_phosphor; } break;
    case 0x24: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupInflowPump; ps->touch_state->var[1] = _n_inflowPump; } break;

    case 0x31: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupCal; ps->touch_state->var[1] = _n_cal; } break;
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupAlarm; ps->touch_state->var[1] = _n_alarm; } break;
    case 0x33: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupWatch; ps->touch_state->var[1] = _n_watch; } break;
    case 0x34: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupZone; ps->touch_state->var[1] = _n_zone; } break;

    case 0x00: 
      if(ps->touch_state->touched)
      { 
        if(ps->touch_state->var[0] != NoPage){ ps->page_state->page = ps->touch_state->var[0]; }
        ps->touch_state->var[0] = NoPage;
        ps->touch_state->touched = _ctrl_zero;
        ps->touch_state->init = false;
      } 
      break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // draw symbol
  if(ps->touch_state->touched){ if(ps->touch_state->var[1] != _none_symbol){ LCD_Sym_Setup_Draw(ps->touch_state->var[1]); ps->touch_state->var[1] = _none_symbol; } }
}


/* ------------------------------------------------------------------*
 *            setup circulate linker
 * ------------------------------------------------------------------*/

void Touch_Setup_CirculateLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;

    // times
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(ON_circ);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(OFF_circ);
    ps->touch_state->var[2] = MEM_EEPROM_ReadVar(TIME_L_circ);
    ps->touch_state->var[3] = MEM_EEPROM_ReadVar(TIME_H_circ);
    ps->touch_state->var[4] = MEM_EEPROM_ReadVar(SENSOR_inTank);

    // selection
    ps->touch_state->var[5] = 0;
    ps->touch_state->int_var[0] = ((ps->touch_state->var[3] << 8) | ps->touch_state->var[2]);
    LCD_Sym_Setup_Circulate_Change_Sensor(ps->touch_state->var[4]);
    LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->var[5], ps->touch_state->var);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_esc;
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        ps->touch_state->init = false;
        ps->page_state->page = SetupPage;
      }
      break;

    // okay
    case 0x14:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_ok; 
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        ps->touch_state->init = false;
        MEM_EEPROM_WriteVar(ON_circ, ps->touch_state->var[0]);
        MEM_EEPROM_WriteVar(OFF_circ, ps->touch_state->var[1]);
        MEM_EEPROM_WriteVar(TIME_L_circ, ps->touch_state->var[2]);
        MEM_EEPROM_WriteVar(TIME_H_circ, ps->touch_state->var[3]);
        MEM_EEPROM_WriteVar(SENSOR_inTank, ps->touch_state->var[4]);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->page_state->page = SetupPage;
      }
      break;

    // circ sensor
    case 0x31: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_sensor; ps->touch_state->var[4] = 1; LCD_Sym_Setup_Circulate_Change_Sensor(1); } break;

    // time
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[4] = 0; ps->touch_state->var[5] = 2; LCD_Sym_Setup_Circulate_Change_Sensor(0); LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->var[5], ps->touch_state->var); } break;

    // no touch
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // common matrix linkers
  Touch_Setup_Matrix_OnOff(ps, touch_matrix);
  Touch_Matrix_MainLinker(ps, touch_matrix);

  if(ps->touch_state->touched)
  {
    switch(ps->touch_state->var[5])
    {
      case 0: LCD_Sym_Setup_Circulate_OnValue(true, ps->touch_state->var[0]); break;
      case 1: LCD_Sym_Setup_Circulate_OffValue(true, ps->touch_state->var[1]); break;
      case 2: LCD_Sym_Setup_Circulate_TimeValue(true, ps->touch_state->int_var[0]); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            on off touch matrix
 * ------------------------------------------------------------------*/

void Touch_Setup_Matrix_OnOff(struct PlantState *ps, unsigned char touch_matrix)
{
  switch(touch_matrix)
  {
    // minus
    case 0x23:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      switch(ps->touch_state->var[5])
      {
        // circulate on min / max
        case 0: ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 2); break;
        case 1: ps->touch_state->var[1] = Basic_LimitDec(ps->touch_state->var[1], 5); break;
        case 2: 
          ps->touch_state->int_var[0] = Basic_LimitDec(ps->touch_state->int_var[0], 0);
          ps->touch_state->var[2] = (ps->touch_state->int_var[0] & 0x00FF);
          ps->touch_state->var[3] = ((ps->touch_state->int_var[0]>>8) & 0x00FF); 
          break;
        default: break;
      } 
      break;

    // plus
    case 0x24: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      switch(ps->touch_state->var[5])
      {
        // circulate on min / max
        case 0: ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 30); break;
        case 1: ps->touch_state->var[1] = Basic_LimitAdd(ps->touch_state->var[1], 60); break;
        case 2: 
          ps->touch_state->int_var[0] = Basic_LimitAdd(ps->touch_state->int_var[0], 999);
          ps->touch_state->var[2] = (ps->touch_state->int_var[0] & 0x00FF);
          ps->touch_state->var[3] = ((ps->touch_state->int_var[0]>>8) & 0x00FF);
          break;
        default: break;
      } 
      break;

    // on
    case 0x21: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_on; ps->touch_state->var[5] = 0; LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->var[5], ps->touch_state->var); } break;

    // off
    case 0x22: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_off; ps->touch_state->var[5] = 1; LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->var[5], ps->touch_state->var); } break;


    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            setup air linker
 * ------------------------------------------------------------------*/

void Touch_Setup_AirLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(ON_air);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(OFF_air);
    ps->touch_state->var[2] = MEM_EEPROM_ReadVar(TIME_L_air);
    ps->touch_state->var[3] = MEM_EEPROM_ReadVar(TIME_H_air);

    // on
    ps->touch_state->var[5] = 0;
    ps->touch_state->int_var[0] = ((ps->touch_state->var[3] << 8) | ps->touch_state->var[2]);
    LCD_Sym_Setup_Air_Change_Values(ps->touch_state->var[5], ps->touch_state->var);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  { 
    // esc
    case 0x13:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_esc; LCD_Sym_ControlButtons(_ctrl_neg_esc); }
      ps->touch_state->init = false; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_ok; LCD_Sym_ControlButtons(_ctrl_neg_ok); }
      ps->touch_state->init = false;
      MEM_EEPROM_WriteVar(ON_air, ps->touch_state->var[0]);
      MEM_EEPROM_WriteVar(OFF_air, ps->touch_state->var[1]);
      MEM_EEPROM_WriteVar(TIME_L_air, ps->touch_state->var[2]);
      MEM_EEPROM_WriteVar(TIME_H_air, ps->touch_state->var[3]);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // time
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_time; ps->touch_state->var[5] = 2; LCD_Sym_Setup_Air_Change_Values(ps->touch_state->var[5], ps->touch_state->var); } break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ ps->touch_state->var[2] = (ps->touch_state->int_var[0] & 0x00FF); ps->touch_state->var[3] = ((ps->touch_state->int_var[0] >> 8) & 0x00FF); LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // common touch matrices
  Touch_Setup_Matrix_OnOff(ps, touch_matrix);
  Touch_Matrix_MainLinker(ps, touch_matrix);

  if(ps->touch_state->touched)
  {
    switch(ps->touch_state->var[5])
    {
      case 0: LCD_Sym_Setup_Circulate_OnValue(true, ps->touch_state->var[0]); break;
      case 1: LCD_Sym_Setup_Circulate_OffValue(true, ps->touch_state->var[1]); break;
      case 2: LCD_Sym_Setup_Circulate_TimeValue(true, ps->touch_state->int_var[0]); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            setup set down linker
 * ------------------------------------------------------------------*/

void Touch_Setup_SetDownLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(TIME_setDown);
    LCD_Sym_Setup_SetDown_TimeValue(ps->touch_state->var[0]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_esc; LCD_Sym_ControlButtons(_ctrl_neg_esc); }
      ps->touch_state->init = false; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_ok; LCD_Sym_ControlButtons(_ctrl_neg_ok); }
      ps->touch_state->init = false;
      MEM_EEPROM_WriteVar(TIME_setDown, ps->touch_state->var[0]);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 50); break;

    // plus
    case 0x24: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 90); break;

    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // time config
  if(ps->touch_state->touched){ LCD_Sym_Setup_SetDown_TimeValue(ps->touch_state->var[0]); }
}


/* ------------------------------------------------------------------*
 *            setup Pump-Off linker
 * ------------------------------------------------------------------*/

void Touch_Setup_PumpOffLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(ON_pumpOff);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(PUMP_pumpOff);

    // on value
    LCD_Sym_Setup_Circulate_OnTextValue(true, ps->touch_state->var[0]);

    // symbols
    LCD_Sym_Setup_PumpOff_Compressor(!(bool)ps->touch_state->var[1]);
    LCD_Sym_Setup_PumpOff_Pump((bool)ps->touch_state->var[1]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_esc; LCD_Sym_ControlButtons(_ctrl_neg_esc); }
      ps->touch_state->init = false; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_ok; LCD_Sym_ControlButtons(_ctrl_neg_ok); }
      ps->touch_state->init = false;
      MEM_EEPROM_WriteVar(ON_pumpOff, ps->touch_state->var[0]);
      MEM_EEPROM_WriteVar(PUMP_pumpOff, ps->touch_state->var[1]);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->touched){ ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 5); } break;

    // plus
    case 0x24: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      if(ps->touch_state->touched){ ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 60); } break;

    // mammoth pump
    case 0x31: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[1] = 0; LCD_Sym_Setup_PumpOff_Compressor(true); LCD_Sym_Setup_PumpOff_Pump(false); } break;

    // electrical pump
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[1] = 1; LCD_Sym_Setup_PumpOff_Compressor(false); LCD_Sym_Setup_PumpOff_Pump(true); } break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // output
  if(ps->touch_state->touched){ LCD_Sym_Setup_Circulate_OnValue(true, ps->touch_state->var[0]); }
}


/* ------------------------------------------------------------------*
 *            setup mud linker
 * ------------------------------------------------------------------*/

void Touch_Setup_MudLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(ON_MIN_mud);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(ON_SEC_mud);
    ps->touch_state->var[5] = 0;
    LCD_Sym_Setup_Mud_Min(!(bool)ps->touch_state->var[5], ps->touch_state->var[0]);
    LCD_Sym_Setup_Mud_Sec((bool)ps->touch_state->var[5], ps->touch_state->var[1]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    case 0x13:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_esc; LCD_Sym_ControlButtons(_ctrl_neg_esc); }
      ps->touch_state->init = false;
      ps->page_state->page = SetupPage;
      break;

    case 0x14:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_ok; LCD_Sym_ControlButtons(_ctrl_neg_ok); }
      ps->touch_state->init = false;
      MEM_EEPROM_WriteVar(ON_MIN_mud, ps->touch_state->var[0]);
      MEM_EEPROM_WriteVar(ON_SEC_mud, ps->touch_state->var[1]);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->var[5]){ ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 0); }
      else{ ps->touch_state->var[1] = Basic_LimitDec(ps->touch_state->var[1], 0); }
      break;

    // plus
    case 0x24: if(!ps->touch_state->touched){ LCD_Sym_ControlButtons(_ctrl_neg_plus); ps->touch_state->touched = _ctrl_pos_plus; }
      if(ps->touch_state->var[5]){ ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 20); }
      else{ ps->touch_state->var[1] = Basic_LimitAdd(ps->touch_state->var[1], 59); }
      break;

    // min
    case 0x22: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[5] = 0; LCD_Sym_Setup_Mud_Sec(false, ps->touch_state->var[1]); } break;
    
    // sec
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[5] = 1; LCD_Sym_Setup_Mud_Min(false, ps->touch_state->var[0]); } break;
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // min max
  if(ps->touch_state->touched){ (ps->touch_state->var[5] ? LCD_Sym_Setup_Mud_Sec(true, ps->touch_state->var[1]) : LCD_Sym_Setup_Mud_Min(true, ps->touch_state->var[0])); }
}


/* ------------------------------------------------------------------*
 *            setup compressor linker
 * ------------------------------------------------------------------*/

void Touch_Setup_CompressorLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    ps->touch_state->int_var[0] = ((MEM_EEPROM_ReadVar(MIN_H_druck) << 8) | MEM_EEPROM_ReadVar(MIN_L_druck));
    ps->touch_state->int_var[1] = ((MEM_EEPROM_ReadVar(MAX_H_druck) << 8) | MEM_EEPROM_ReadVar(MAX_L_druck));
    ps->touch_state->var[5] = 0;

    // min max
    LCD_Sym_Setup_Compressor_MinP(!ps->touch_state->var[5], ps->touch_state->int_var[0]);
    LCD_Sym_Setup_Compressor_MaxP(ps->touch_state->var[5], ps->touch_state->int_var[1]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_esc; 
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        ps->touch_state->init = false; 
        ps->page_state->page = SetupPage;
      }
      break;

    // okay
    case 0x14: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_ok;
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        ps->touch_state->init = false;
        MEM_EEPROM_WriteVar(MAX_L_druck, (ps->touch_state->int_var[1] & 0x00FF));
        MEM_EEPROM_WriteVar(MAX_H_druck, ((ps->touch_state->int_var[1] >> 8) & 0x00FF));
        MEM_EEPROM_WriteVar(MIN_L_druck, (ps->touch_state->int_var[0] & 0x00FF));
        MEM_EEPROM_WriteVar(MIN_H_druck, ((ps->touch_state->int_var[0] >> 8) & 0x00FF));
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->page_state->page = SetupPage;
      }
      break;

    // minus
    case 0x23:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->var[5]){ ps->touch_state->int_var[0] = Basic_LimitDec(ps->touch_state->int_var[0], 0); }
      else{ ps->touch_state->int_var[1] = Basic_LimitDec(ps->touch_state->int_var[1], 0); }
      break;

    // plus
    case 0x24:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      if(ps->touch_state->var[5]){ ps->touch_state->int_var[0] = Basic_LimitAdd(ps->touch_state->int_var[0], 999); }
      else{ ps->touch_state->int_var[1] = Basic_LimitAdd(ps->touch_state->int_var[1], 999); }
      break;

    // min
    case 0x21: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[5] = 0; LCD_Sym_Setup_Compressor_MaxP(false, ps->touch_state->int_var[1]); } break;

    // max
    case 0x31: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[5] = 1; LCD_Sym_Setup_Compressor_MinP(false, ps->touch_state->int_var[0]); } break;

    // nothing
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // min max value update
  if(ps->touch_state->touched){ (ps->touch_state->var[5] ? LCD_Sym_Setup_Compressor_MaxP(true, ps->touch_state->int_var[1]) : LCD_Sym_Setup_Compressor_MinP(true, ps->touch_state->int_var[0])); }
}


/* ------------------------------------------------------------------*
 *            setup phosphor linker
 * ------------------------------------------------------------------*/

void Touch_Setup_PhosphorLinker(struct PlantState *ps)
{
  // ps->touch_state->init
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(ON_phosphor);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(OFF_phosphor);
    ps->touch_state->var[5] = 0;

    LCD_Sym_Setup_Circulate_OnTextValue(!(bool)ps->touch_state->var[5], ps->touch_state->var[0]);
    LCD_Sym_Setup_Circulate_OffTextValue((bool)ps->touch_state->var[5], ps->touch_state->var[1]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_esc; 
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        ps->touch_state->init = false;
        ps->page_state->page = SetupPage;
      }
      break;

    // ok
    case 0x14:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_ok; 
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        ps->touch_state->init = false;
        MEM_EEPROM_WriteVar(ON_phosphor, ps->touch_state->var[0]);
        MEM_EEPROM_WriteVar(OFF_phosphor, ps->touch_state->var[1]);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->phosphor_state->init_flag = false;
        LCD_Auto_Phosphor_Init(ps);
        ps->page_state->page = SetupPage;
      }
      break;

    // minus
    case 0x23:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->var[5]){ ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 0); }
      else{ ps->touch_state->var[1] = Basic_LimitDec(ps->touch_state->var[1], 0); }
      break;

    // plus
    case 0x24:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      if(ps->touch_state->var[5]){ ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 60); }
      else{ ps->touch_state->var[1] = Basic_LimitAdd(ps->touch_state->var[1], 60); }
      break;

    // on
    case 0x21:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_on; ps->touch_state->var[5] = 0; LCD_Sym_Setup_Circulate_OffTextValue(true, ps->touch_state->var[1]);  }
      break;

    // off
    case 0x22:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_off; ps->touch_state->var[5] = 1; LCD_Sym_Setup_Circulate_OnTextValue(true, ps->touch_state->var[0]); }
      break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // off or on value
  if(ps->touch_state->touched){ (ps->touch_state->var[5] ? LCD_Sym_Setup_Circulate_OffValue(true, ps->touch_state->var[0]) : LCD_Sym_Setup_Circulate_OnValue(true, ps->touch_state->var[1])); }
}


/* ------------------------------------------------------------------*
 *            setup inflow pump linker
 * ------------------------------------------------------------------*/

void Touch_Setup_InflowPumpLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(T_IP_off_h);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(OFF_inflowPump);
    ps->touch_state->var[2] = MEM_EEPROM_ReadVar(ON_inflowPump);
    ps->touch_state->var[3] = MEM_EEPROM_ReadVar(PUMP_inflowPump);
    ps->touch_state->var[4] = MEM_EEPROM_ReadVar(SENSOR_outTank);
    ps->touch_state->var[5] = 4;

    LCD_Sym_Setup_Pump(ps->touch_state->var[3]);
    LCD_Sym_Setup_InflowPump_Sensor((bool)ps->touch_state->var[4]);

    // initialize text and symbols
    LCD_Sym_Setup_InflowPump_Text(0b10110100);
    LCD_Sym_Setup_InflowPump_Values(0b00110100, ps->touch_state->var);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // off h
    case 0x12:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->var[5] = 1;
        LCD_Sym_Setup_InflowPump_Text(0b01101001);
        LCD_Sym_Setup_InflowPump_Values(0b01100001, ps->touch_state->var);
      }
      break;

    // esc
    case 0x13: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_esc;
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        ps->touch_state->init = false; 
        ps->page_state->page = SetupPage;
      }
      break;

    // okay
    case 0x14:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_ok;
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        ps->touch_state->init = false; 
        MEM_EEPROM_WriteVar(ON_inflowPump, ps->touch_state->var[2]);
        MEM_EEPROM_WriteVar(OFF_inflowPump, ps->touch_state->var[1]);
        MEM_EEPROM_WriteVar(T_IP_off_h, ps->touch_state->var[0]);
        MEM_EEPROM_WriteVar(PUMP_inflowPump, ps->touch_state->var[3]);
        MEM_EEPROM_WriteVar(SENSOR_outTank, ps->touch_state->var[4]);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->inflow_pump_state->init_flag = false;
        LCD_Auto_InflowPump_Init(ps);
        ps->page_state->page = SetupPage;
      }
      break;

    // minus
    case 0x23: 
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->var[5] == 1){ ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 0); }
      else if(ps->touch_state->var[5] == 2){ ps->touch_state->var[1] = Basic_LimitDec(ps->touch_state->var[1], 0); }
      else if(ps->touch_state->var[5] == 4){ ps->touch_state->var[2] = Basic_LimitDec(ps->touch_state->var[2], 0); }
      break;

    // plus
    case 0x24:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      if(ps->touch_state->var[5] == 1){ ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 99); }
      else if(ps->touch_state->var[5] == 2){ ps->touch_state->var[1] = Basic_LimitAdd(ps->touch_state->var[1], 59); }
      else if(ps->touch_state->var[5] == 4){ ps->touch_state->var[2] = Basic_LimitAdd(ps->touch_state->var[2], 60); }
      break;

    case 0x21:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->var[5] = 4;
        LCD_Sym_Setup_InflowPump_Text(0b10110100);
        LCD_Sym_Setup_InflowPump_Values(0b00110100, ps->touch_state->var);
      }
      break;

    // off min
    case 0x22:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->var[5] = 2;
        LCD_Sym_Setup_InflowPump_Text(0b01011010);
        LCD_Sym_Setup_InflowPump_Values(0b01010010, ps->touch_state->var);
      }
      break;

    // sensor
    case 0x31:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_sensor;
        // mask and toggle
        ps->touch_state->var[4] &= (1 << 0);
        ps->touch_state->var[4] ^= (1 << 0);
        LCD_Sym_Setup_InflowPump_Sensor((bool)ps->touch_state->var[4]);
      }
      break;

    // mammoth pump
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[3] = 0; LCD_Sym_Setup_Pump(0); } break;

    // electrical pump
    case 0x33: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[3] = 1; LCD_Sym_Setup_Pump(1); } break;

    // two electrical pumps
    case 0x34: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[3] = 2; LCD_Sym_Setup_Pump(2); } break;

    // nothing
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  switch(ps->touch_state->var[5])
  {
    // off h, off min, on min
    case 1: LCD_Sym_Setup_InflowPump_Values(ps->touch_state->var[5], ps->touch_state->var); break;
    case 2: LCD_Sym_Setup_InflowPump_Values(ps->touch_state->var[5], ps->touch_state->var); break;
    case 4: LCD_Sym_Setup_InflowPump_Values(ps->touch_state->var[5], ps->touch_state->var); break;
    default: break;
  }

}


/* ------------------------------------------------------------------*
 *            setup calibration linker
 * ------------------------------------------------------------------*/

void Touch_Setup_CalLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(CAL_Redo_on);
    ps->touch_state->int_var[0] = ((MEM_EEPROM_ReadVar(CAL_H_druck) << 8) | (MEM_EEPROM_ReadVar(CAL_L_druck)));

    // open ventil
    ps->touch_state->var[1] = 0;
  }

  // read mpx
  int mpx_value = MPX_Read();

  // new value
  if(mpx_value != 0xFF00)
  {
    // print every 10th Time
    if(ps->frame_counter->frame == TC_FPS_HALF)
    {
      mpx_value -= ps->touch_state->int_var[0];
      LCD_Sym_Setup_Cal_MPX_Value(mpx_value);
    }
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_esc;
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        if(ps->page_state->page == SetupCalPressure){ OUT_Clr_Air(ps); }
        ps->page_state->page = SetupPage;
      }
      break;

    // okay
    case 0x14: 
      if(!ps->touch_state->touched && ps->page_state->page == SetupCal)
      { 
        ps->touch_state->touched = _ctrl_pos_ok;
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        MEM_EEPROM_WriteSetupEntry(ps);
        MEM_EEPROM_WriteVar(CAL_Redo_on, ps->touch_state->var[0]);
        if(MEM_EEPROM_ReadVar(SONIC_on) && ps->sonic_state->level_cal)
        {
          MEM_EEPROM_WriteVar(SONIC_L_LV, ps->sonic_state->level_cal & 0x00FF);
          MEM_EEPROM_WriteVar(SONIC_H_LV, ((ps->sonic_state->level_cal >> 8) & 0x00FF));
        }
        else{ MPX_LevelCal_SaveToEEPROM(ps); }
        ps->page_state->page = SetupPage;
      }
      break;

    // open all valves
    case 0x23: 
      if(!ps->touch_state->touched && ps->page_state->page == SetupCal)
      { 
        ps->touch_state->touched = _ctrl_open_valve;
        if(!ps->touch_state->var[1]){ ps->touch_state->var[1] = 1; LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_ventil, 0); OUT_Valve_Action(ps, OPEN_All); }
      }
      break;

    // cal
    // calibration for setting pressure to zero level
    case 0x24: 
      if(!ps->touch_state->touched && ps->page_state->page == SetupCal)
      { 
        ps->touch_state->touched = _ctrl_cal;
        LCD_Sym_Setup_Cal_Button(true);

        // calibration try couple of time until it hopefully worked
        ps->touch_state->int_var[0] = 0;
        for(unsigned char i = 0; i < 6; i++)
        {
          // new calibration
          ps->touch_state->int_var[0] = MPX_ReadAverage_UnCal(ps->mpx_state);
          if(!(ps->touch_state->int_var[0] == 0xFF00)){ break; }
          // wait for next cal
          TCC0_wait_ms(100);
        }

        // write to memory
        MEM_EEPROM_WriteVar(CAL_L_druck, (ps->touch_state->int_var[0] & 0x00FF));
        MEM_EEPROM_WriteVar(CAL_H_druck, ((ps->touch_state->int_var[0] >> 8) & 0x00FF));
      }
      break;

    // level measure
    case 0x31: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_level;
        LCD_Sym_Setup_Cal_Level_Sym(true);
        if(ps->touch_state->var[1]){ ps->touch_state->var[1] = 0; LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_ventil, 1); OUT_Valve_Action(ps, CLOSE_All); } 
        ps->page_state->page = SetupCalPressure; 
      }
      break;

    // redo
    // calibration redo with pressure -> auto zone page
    case 0x34: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_redo;
        if(!MEM_EEPROM_ReadVar(SONIC_on))
        {
          ps->touch_state->var[0] &= (1 << 0);
          ps->touch_state->var[0] ^= (1 << 0);
          LCD_Sym_Setup_Cal_MPX_Redo((bool)ps->touch_state->var[0]);
        }
      }
      break;

    // no ps->touch_state->touched
    case 0x00:
      if(ps->touch_state->touched)
      {
        if(ps->touch_state->touched == _ctrl_cal){ LCD_Sym_Setup_Cal_Button(false); }
        ps->touch_state->touched = _ctrl_zero;
      }
      break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // calibration close valves
  if(ps->page_state->page != SetupCal && ps->page_state->page != SetupCalPressure)
  {
    ps->touch_state->init = 0;
    if(ps->touch_state->var[1]){ ps->touch_state->var[1] = 0; OUT_Valve_Action(ps, CLOSE_All); }
  }
}


/* ------------------------------------------------------------------*
 *            setup alarm linker
 * ------------------------------------------------------------------*/

void Touch_Setup_AlarmLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(ALARM_sensor);
    ps->touch_state->var[1] = MEM_EEPROM_ReadVar(ALARM_comp);
    ps->touch_state->var[2] = MEM_EEPROM_ReadVar(ALARM_temp);

    LCD_Sym_Setup_Alarm_Sensor((bool)ps->touch_state->var[0]);
    LCD_Sym_Setup_Alarm_Compressor((bool)ps->touch_state->var[1]);
    LCD_Sym_Setup_Alarm_TempValue(true, ps->touch_state->var[2]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_esc; LCD_Sym_ControlButtons(_ctrl_neg_esc); }
      ps->touch_state->init = false;
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_ok; LCD_Sym_ControlButtons(_ctrl_neg_ok); }
      ps->touch_state->init = false;
      MEM_EEPROM_WriteVar(ALARM_sensor, ps->touch_state->var[0]);
      MEM_EEPROM_WriteVar(ALARM_comp, ps->touch_state->var[1]);
      MEM_EEPROM_WriteVar(ALARM_temp, ps->touch_state->var[2]);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->touched){ ps->touch_state->var[2] = Basic_LimitDec(ps->touch_state->var[2], 15); }
      break;

    // plus
    case 0x24:
      if(!ps->touch_state->touched){ LCD_Sym_ControlButtons(_ctrl_neg_plus); ps->touch_state->touched = _ctrl_pos_plus; }
      if(ps->touch_state->touched){ ps->touch_state->var[2] = Basic_LimitAdd(ps->touch_state->var[2], 99); }
      break;

    // sensor
    case 0x31:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_sensor;
        ps->touch_state->var[0] &= (1 << 0);
        ps->touch_state->var[0] ^= (1 << 0);
        LCD_Sym_Setup_Alarm_Sensor((bool)ps->touch_state->var[0]);
      }
      break;

    // compressor
    case 0x32:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_compressor;
        ps->touch_state->var[1] &= (1 << 0);
        ps->touch_state->var[1] ^= (1 << 0);
        LCD_Sym_Setup_Alarm_Compressor((bool)ps->touch_state->var[1]);
      }
      break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  if(ps->touch_state->touched){ LCD_Sym_Setup_Alarm_TempValue(true, ps->touch_state->var[2]); }
}


/* ------------------------------------------------------------------*
 *            setup watch linker
 * ------------------------------------------------------------------*/

void Touch_Setup_WatchLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MCP7941_ReadTime(ps->twi_state, TIC_HOUR); TCC0_wait_us(25);
    ps->touch_state->var[1] = MCP7941_ReadTime(ps->twi_state, TIC_MIN); TCC0_wait_us(25);
    ps->touch_state->var[2] = MCP7941_ReadTime(ps->twi_state, TIC_DATE); TCC0_wait_us(25);
    ps->touch_state->var[3] = MCP7941_ReadTime(ps->twi_state, TIC_MONTH); TCC0_wait_us(25);
    ps->touch_state->var[4] = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
    ps->touch_state->var[5] = 0;
    LCD_Sym_Setup_Watch_Mark(_n_h, &ps->touch_state->var[0]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_esc; LCD_Sym_ControlButtons2(_ctrl_neg_esc); } 
      ps->touch_state->init = false; ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_ok; LCD_Sym_ControlButtons2(_ctrl_neg_ok); }
      ps->touch_state->init = false;
      unsigned char time = (((ps->touch_state->var[0]/10) << 4) | (ps->touch_state->var[0] % 10));
      MCP7941_WriteByte(TIC_HOUR, time);
      time = (((ps->touch_state->var[1]/10) << 4) | (ps->touch_state->var[1] % 10));
      MCP7941_WriteByte(TIC_MIN, time);
      time = (((ps->touch_state->var[2]/10) << 4) | (ps->touch_state->var[2] % 10));
      MCP7941_WriteByte(TIC_DATE, time);
      time = (((ps->touch_state->var[3]/10) << 4) | (ps->touch_state->var[3] % 10));
      MCP7941_WriteByte(TIC_MONTH, time);
      time = (((ps->touch_state->var[4]/10) << 4) | (ps->touch_state->var[4] % 10));
      MCP7941_WriteByte(TIC_YEAR, time);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // plus
    case 0x24: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons2(_ctrl_neg_plus); }
      switch(ps->touch_state->var[5])
      {
        case 0: ps->touch_state->var[0] = Basic_LimitAdd(ps->touch_state->var[0], 23); break;
        case 1: ps->touch_state->var[1] = Basic_LimitAdd(ps->touch_state->var[1], 59); break;
        case 2: ps->touch_state->var[2] = Basic_LimitAdd(ps->touch_state->var[2], 31); break;
        case 3: ps->touch_state->var[3] = Basic_LimitAdd(ps->touch_state->var[3], 12); break;
        case 4: ps->touch_state->var[4] = Basic_LimitAdd(ps->touch_state->var[4], 99); break;
        default: break;
      } break;

    // minus
    case 0x34: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons2(_ctrl_neg_minus); }
      switch(ps->touch_state->var[5])
      {
        case 0: ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 0); break;
        case 1: ps->touch_state->var[1] = Basic_LimitDec(ps->touch_state->var[1], 0); break;
        case 2: ps->touch_state->var[2] = Basic_LimitDec(ps->touch_state->var[2], 1); break;
        case 3: ps->touch_state->var[3] = Basic_LimitDec(ps->touch_state->var[3], 1); break;
        case 4: ps->touch_state->var[4] = Basic_LimitDec(ps->touch_state->var[4], 0); break;
        default: break;
      } break;

    // hours
    case 0x21: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_h, &ps->touch_state->var[0]); ps->touch_state->var[5] = 0; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // min
    case 0x22: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_min, &ps->touch_state->var[0]); ps->touch_state->var[5] = 1; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // days
    case 0x31: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_day, &ps->touch_state->var[0]); ps->touch_state->var[5] = 2; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // month
    case 0x32: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_month, &ps->touch_state->var[0]); ps->touch_state->var[5] = 3; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // year
    case 0x33: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_year, &ps->touch_state->var[0]); ps->touch_state->var[5] = 4; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons2(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // write date time
  if(ps->touch_state->touched)
  {
    switch(ps->touch_state->var[5])
    {
      case 0: LCD_Sym_Setup_Watch_DateTime(_n_h, ps->touch_state->var[0]); break;
      case 1: LCD_Sym_Setup_Watch_DateTime(_n_min, ps->touch_state->var[1]); break;
      case 2: LCD_Sym_Setup_Watch_DateTime(_n_day, ps->touch_state->var[2]); break;
      case 3: LCD_Sym_Setup_Watch_DateTime(_n_month, ps->touch_state->var[3]); break;
      case 4: LCD_Sym_Setup_Watch_DateTime(_n_year, ps->touch_state->var[4]); break;
      default: break;
    }
  }
}



/* ------------------------------------------------------------------*
 *            setup Zone linker
 * ------------------------------------------------------------------*/

void Touch_Setup_ZoneLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = MEM_EEPROM_ReadVar(SONIC_on);
    ps->touch_state->var[1] = 0;
    ps->touch_state->int_var[0] = (MEM_EEPROM_ReadVar(TANK_H_Circ) << 8) | MEM_EEPROM_ReadVar(TANK_L_Circ);
    ps->touch_state->int_var[1] = (MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | MEM_EEPROM_ReadVar(TANK_L_O2);

    LCD_Sym_Setup_Zone_Sonic(ps->touch_state->var[0]);
    LCD_Sym_Setup_Zone_Level_Air(!(bool)ps->touch_state->var[1], ps->touch_state->int_var[0]);
    LCD_Sym_Setup_Zone_Level_SetDown((bool)ps->touch_state->var[1], ps->touch_state->int_var[1]);
    LCD_Sym_Setup_Zone_Air(!(bool)ps->touch_state->var[1]);
    LCD_Sym_Setup_Zone_SetDown((bool)ps->touch_state->var[1]);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // sonic
    case 0x12: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_sonic;
        ps->touch_state->var[0] &= (1 << 0); 
        ps->touch_state->var[0] ^= (1 << 0); 
        LCD_Sym_Setup_Zone_Sonic(ps->touch_state->var[0]);
      } break;

    // esc
    case 0x13: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_esc; 
        ps->touch_state->init = false;
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        ps->page_state->page = SetupPage;
      }
      break;

    // okay
    case 0x14: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_ok; 
        ps->touch_state->init = false; 
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        unsigned char h = (ps->touch_state->int_var[0] & 0x00FF);
        MEM_EEPROM_WriteVar(TANK_L_O2, h);
        h = ((ps->touch_state->int_var[0] >> 8) & 0x00FF);
        MEM_EEPROM_WriteVar(TANK_H_O2, h);
        h = (ps->touch_state->int_var[1] & 0x00FF);
        MEM_EEPROM_WriteVar(TANK_L_Circ, h);
        h = ((ps->touch_state->int_var[1] >> 8) & 0x00FF);
        MEM_EEPROM_WriteVar(TANK_H_Circ, h);
        MEM_EEPROM_WriteVar(SONIC_on, ps->touch_state->var[0]);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->page_state->page = SetupPage;
      }
      break;

    // minus
    case 0x23:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Sym_ControlButtons(_ctrl_neg_minus); }
      if(ps->touch_state->var[1]){ ps->touch_state->int_var[1] = Basic_LimitDec(ps->touch_state->int_var[1], 0); }
      else{ ps->touch_state->int_var[0] = Basic_LimitDec(ps->touch_state->int_var[0], ps->touch_state->int_var[1]); }
      break;

    // plus
    case 0x24:
      if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Sym_ControlButtons(_ctrl_neg_plus); }
      if(ps->touch_state->var[1]){ ps->touch_state->int_var[1] = Basic_LimitAdd(ps->touch_state->int_var[1], ps->touch_state->int_var[0]); }
      else{ ps->touch_state->int_var[0] = Basic_LimitAdd(ps->touch_state->int_var[0], 999); }
      break;

    // air level
    case 0x21:
    case 0x22:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->var[1] = 0;
        LCD_Sym_Setup_Zone_Level_Air(!(bool)ps->touch_state->var[1], ps->touch_state->int_var[0]);
        LCD_Sym_Setup_Zone_Level_SetDown((bool)ps->touch_state->var[1], ps->touch_state->int_var[1]);
        LCD_Sym_Setup_Zone_Air(!(bool)ps->touch_state->var[1]);
        LCD_Sym_Setup_Zone_SetDown((bool)ps->touch_state->var[1]);
      }
      break;

    // set down level
    case 0x31:
    case 0x32:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->var[1] = 1;
        LCD_Sym_Setup_Zone_Level_Air(!(bool)ps->touch_state->var[1], ps->touch_state->int_var[0]);
        LCD_Sym_Setup_Zone_Level_SetDown((bool)ps->touch_state->var[1], ps->touch_state->int_var[1]);
        LCD_Sym_Setup_Zone_Air(!(bool)ps->touch_state->var[1]);
        LCD_Sym_Setup_Zone_SetDown((bool)ps->touch_state->var[1]);
      }
      break;

    // nothing
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Matrix_MainLinker(ps, touch_matrix);

  // circulate and o2
  if(ps->touch_state->touched){ (ps->touch_state->var[1] ? LCD_Sym_Setup_Zone_Level_SetDown(true, ps->touch_state->int_var[1]) : LCD_Sym_Setup_Zone_Level_Air(true, ps->touch_state->int_var[0])); }
}


/*-------------------------------------------------------------------*
 *            data linker
 * ------------------------------------------------------------------*/

void Touch_Data_Linker(struct PlantState *ps)
{
  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // auto
    case 0x21: LCD_Write_TextButton(9, 0, TEXT_BUTTON_auto, 0); ps->page_state->page = DataAuto; break;
    case 0x22: LCD_Write_TextButton(9, 40, TEXT_BUTTON_manual, 0); ps->page_state->page = DataManual; break;
    case 0x23: LCD_Write_TextButton(9, 80, TEXT_BUTTON_setup, 0); ps->page_state->page = DataSetup; break;
    case 0x24: LCD_Write_TextButton(9, 120, TEXT_BUTTON_sonic, 0); ps->page_state->page = DataSonic; break;
    default: break;
  }

  // data main linker
  Touch_Matrix_MainLinker_Data(ps, touch_matrix);
}


/* ------------------------------------------------------------------*
 *            data auto linker
 * ------------------------------------------------------------------*/

void Touch_Data_AutoLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    ps->touch_state->var[1] = 0;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // arrow up
    case 0x14:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_arrow_up;
        if(ps->touch_state->var[1]){ ps->touch_state->var[1]--; }
        LCD_Sym_Data_Arrow_Up(true);
      } 
      break;

    // arrow down
    case 0x34:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_arrow_down;
        if(ps->touch_state->var[1] < DATA_PAGE_NUM_AUTO){ ps->touch_state->var[1]++; }
        LCD_Sym_Data_Arrow_Down(true);
      } 
      break;

    // nothing
    case 0x00:
      if(ps->touch_state->touched)
      {
        if(ps->touch_state->touched == _ctrl_arrow_up)
        { 
          LCD_Sym_Data_Arrow_Up(false);
          LCD_Data_WriteAutoEntryPage(ps->touch_state->var[1]);
        }
        else if(ps->touch_state->touched == _ctrl_arrow_down)
        {
          LCD_Sym_Data_Arrow_Down(false);
          LCD_Data_WriteAutoEntryPage(ps->touch_state->var[1]);
        } 
        ps->touch_state->touched = _ctrl_zero;
      }
      break;

    default: break;
  }

  // linker
  Touch_Matrix_MainLinker_Data(ps, touch_matrix);
}


/* ------------------------------------------------------------------*
 *            data manual linker
 * ------------------------------------------------------------------*/

void Touch_Data_ManualLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = 0;
    ps->touch_state->var[1] = 0;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {

    // arrow up
    case 0x14:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_arrow_up; 
        if(ps->touch_state->var[1]){ ps->touch_state->var[1]--; }
        LCD_Sym_Data_Arrow_Up(true);
      } 
      break;

    // arrow down
    case 0x34:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_arrow_down; 
        if(ps->touch_state->var[1] < DATA_PAGE_NUM_MANUAL){ ps->touch_state->var[1]++; }
        LCD_Sym_Data_Arrow_Down(true);
      } 
      break;

    // nothing
    case 0x00:
      if(ps->touch_state->touched)
      {
        if(ps->touch_state->touched == _ctrl_arrow_up)
        {
          LCD_Sym_Data_Arrow_Up(false);
          LCD_Data_WriteManualEntryPage(ps->touch_state->var[1]);
        }
        else if(ps->touch_state->touched == _ctrl_arrow_down)
        { 
          LCD_Sym_Data_Arrow_Down(false);
          LCD_Data_WriteManualEntryPage(ps->touch_state->var[1]);
        }
        ps->touch_state->touched = _ctrl_zero;
      }
      break;

    default: break;
  }

  // linker
  Touch_Matrix_MainLinker_Data(ps, touch_matrix);
}


/* ------------------------------------------------------------------*
 *            data setup linker
 * ------------------------------------------------------------------*/

void Touch_Data_SetupLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->var[0] = 0;
    ps->touch_state->var[1] = 0;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // arrow up
    case 0x14:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_arrow_up; 
        if(ps->touch_state->var[1]){ ps->touch_state->var[1]--; }
        LCD_Sym_Data_Arrow_Up(true);
      } 
      break;

    // arrow down
    case 0x34:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_arrow_down; 
        if(ps->touch_state->var[1] < DATA_PAGE_NUM_SETUP){ ps->touch_state->var[1]++; }
        LCD_Sym_Data_Arrow_Down(true);
      } 
      break;

    // nothing
    case 0x00:
      if(ps->touch_state->touched)
      {
        if(ps->touch_state->touched == _ctrl_arrow_up)
        {
          LCD_Sym_Data_Arrow_Up(false);
          LCD_Data_WriteSetupEntryPage(ps->touch_state->var[1]);
        }

        else if(ps->touch_state->touched == _ctrl_arrow_down)
        {
          LCD_Sym_Data_Arrow_Down(false);
          LCD_Data_WriteSetupEntryPage(ps->touch_state->var[1]);
        }
        ps->touch_state->touched = _ctrl_zero;
      }
      break;

    default: break;
  }

  // linker
  Touch_Matrix_MainLinker_Data(ps, touch_matrix);
}


/* ------------------------------------------------------------------*
 *            data sonic linker
 * ------------------------------------------------------------------*/

void Touch_Data_SonicLinker(struct PlantState *ps)
{
  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // shot
    case 0x11:
      if(!ps->touch_state->touched && ps->page_state->page != DataSonicBoot)
      { 
        ps->touch_state->touched = _ctrl_shot;
        if(ps->page_state->page != DataSonic){ LCD_Sym_Data_Sonic_ClearRecording(ps); }
        LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, 0);
        Sonic_Data_Shot(ps);
        ps->page_state->page = DataSonic;
      }
      break;

    // auto shot
    case 0x21:
      if(!ps->touch_state->touched && ps->page_state->page != DataSonicBoot)
      { 
        ps->touch_state->touched = _ctrl_auto;
        LCD_Sym_Data_Sonic_ClearRecording(ps);
        LCD_Sym_Data_Sonic_AutoText();
        Sonic_Query_Temp_Init(ps);
        ps->page_state->page = DataSonicAuto;
      }
      break;

    // bootloader
    case 0x31:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_bootloader;
        LCD_Sym_Data_Sonic_ClearRecording(ps);
        LCD_Write_TextButton(16, 0, TEXT_BUTTON_boot, 0);
        LCD_Write_TextButton(4, 120, TEXT_BUTTON_read, 1);
        LCD_Write_TextButton(10, 120, TEXT_BUTTON_write, 1);

        if(ps->page_state->page == DataSonicBoot)
        {
          LCD_Write_TextButton(16, 0, TEXT_BUTTON_boot, 1);
          LCD_Sym_Data_Sonic_ClearRecording(ps);
          Sonic_Data_Boot_Off(ps);
          ps->page_state->page = DataSonic; 
          break;
        }

        Sonic_Data_Boot_On(ps);
        ps->page_state->page = DataSonicBoot;
      }
      break;

    case 0x14:
      if(!ps->touch_state->touched && ps->page_state->page == DataSonicBoot){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Write_TextButton(4, 120, TEXT_BUTTON_read, 0); ps->page_state->page = DataSonicBootR; }
      break;

    case 0x24:
      if(!ps->touch_state->touched && ps->page_state->page == DataSonicBoot){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Write_TextButton(10, 120, TEXT_BUTTON_write, 0); ps->page_state->page = DataSonicBootW; }
      break;

    // reset buttons
    case 0x00:
      if(ps->touch_state->touched)
      {
        if(ps->touch_state->touched == _ctrl_shot){  LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, 1); }
        else if(ps->touch_state->touched == _ctrl_auto){ LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, 1); }
        ps->touch_state->touched = _ctrl_zero;
      }
      break;

    // main linker
    case 0x41: ps->touch_state->touched = _ctrl_zero; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = AutoPage; break;
    case 0x42: ps->touch_state->touched = _ctrl_zero; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: ps->touch_state->touched = _ctrl_zero; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: ps->touch_state->touched = _ctrl_zero; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = DataPage; break;
    default: break;
  }
}


/*-------------------------------------------------------------------*
 *  pin linker
 * ------------------------------------------------------------------*/

void Touch_Pin_Linker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;

    // pin code save vars
    ps->touch_state->var[0] = 0;
    ps->touch_state->var[1] = 0;
    ps->touch_state->var[2] = 0;
    ps->touch_state->var[3] = 0;

    // code index
    ps->touch_state->var[4] = 0;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // 1
    case 0x11: 
      if(!(ps->touch_state->int_var[0] & (1 << 1)))
      { 
        ps->touch_state->int_var[0] |= (1 << 1);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 1); }
        else{ LCD_Sym_Pin_WriteDigit(1, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 1; ps->touch_state->var[4]++; }
      }
      break;

    // 2
    case 0x12: 
      if(!(ps->touch_state->int_var[0] & (1 << 2)))
      { 
        ps->touch_state->int_var[0] |= (1 << 2);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 2); }
        else{ LCD_Sym_Pin_WriteDigit(2, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 2; ps->touch_state->var[4]++; }
      } break;

    // 3
    case 0x13: 
      if(!(ps->touch_state->int_var[0] & (1 << 3)))
      { 
        ps->touch_state->int_var[0] |= (1 << 3);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 3); }
        else{ LCD_Sym_Pin_WriteDigit(3, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 3; ps->touch_state->var[4]++; }
      } break;

    // 4
    case 0x21: 
      if(!(ps->touch_state->int_var[0] & (1 << 4)))
      { 
        ps->touch_state->int_var[0] |= (1 << 4);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 4); }
        else{ LCD_Sym_Pin_WriteDigit(4, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 4; ps->touch_state->var[4]++; }
      }
      break;

    // 5
    case 0x22: 
      if(!(ps->touch_state->int_var[0] & (1 << 5)))
      { 
        ps->touch_state->int_var[0] |= (1 << 5);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 5); }
        else{ LCD_Sym_Pin_WriteDigit(5, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 5; ps->touch_state->var[4]++; }
      }
      break;

    // 6
    case 0x23: 
      if(!(ps->touch_state->int_var[0] & (1 << 6)))
      { 
        ps->touch_state->int_var[0] |= (1 << 6);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 6); }
        else{ LCD_Sym_Pin_WriteDigit(6, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 6; ps->touch_state->var[4]++; }
      }
      break;

    // 7
    case 0x31: 
      if(!(ps->touch_state->int_var[0] & (1 << 7)))
      { 
        ps->touch_state->int_var[0] |= (1 << 7);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 7); }
        else{ LCD_Sym_Pin_WriteDigit(7, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 7; ps->touch_state->var[4]++; }
      } 
      break;

    // 8
    case 0x32: 
      if(!(ps->touch_state->int_var[0] & (1 << 8)))
      { 
        ps->touch_state->int_var[0] |= (1 << 8);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 8); }
        else{ LCD_Sym_Pin_WriteDigit(8, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 8; ps->touch_state->var[4]++; }
      }
      break;

    // 9
    case 0x33: 
      if(!(ps->touch_state->int_var[0] & (1 << 9)))
      { 
        ps->touch_state->int_var[0] |= (1 << 9);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 9); }
        else{ LCD_Sym_Pin_WriteDigit(9, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 9; ps->touch_state->var[4]++; }
      }
      break;

    // 0
    case 0x42:
      if(!(ps->touch_state->int_var[0] & (1 << 0)))
      { 
        ps->touch_state->int_var[0] |= (1 << 0);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 0); }
        else{ LCD_Sym_Pin_WriteDigit(0, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 0; ps->touch_state->var[4]++; }
      }
      break;

    // del
    case 0x43:
      if(!(ps->touch_state->int_var[0] & (1 << 10)))
      { 
        ps->touch_state->int_var[0] |= (1 << 10);
        LCD_nPinButtons(10);
        LCD_Sym_Pin_DelDigits();
        ps->touch_state->var[4] = 0;
        if(ps->modem->tele_nr_temp->id)
        {
          // reset temp
          ps->modem->temp_digit_pos = 0;
          for(unsigned char i = 0; i < 16; i++){ ps->modem->tele_nr_temp->nr[i] = 11; }
          LCD_Sym_Pin_PrintWholeTelNumber(ps->modem->tele_nr_temp);
        }
      }
      break;

    // esc
    case 0x41: 
      if(!(ps->touch_state->int_var[0] & (1 << 11)))
      {
        ps->touch_state->int_var[0] |= (1 << 11);
        LCD_nPinButtons(11);
        ps->touch_state->var[4] = 0;
        ps->modem->tele_nr_temp->id = 0; 
        ps->modem->temp_digit_pos = 0;
        for(unsigned char i = 0; i < 16; i++){ ps->modem->tele_nr_temp->nr[i] = 11; }
        ps->page_state->page = DataPage;
        ps->touch_state->init = false;
      }
      break;

    // okay tel
    case 0x44:
      if(!(ps->touch_state->int_var[0] & (1 << 12)) && ps->modem->tele_nr_temp->id)
      {
        ps->touch_state->int_var[0] |= (1 << 12);
        LCD_Sym_Pin_OkButton(1);
        AT24C_TeleNr_Write(ps->modem->tele_nr_temp);
        AT24C_TeleNr_ReadToModem(ps);

        // test new number
        if(SMS_ON){ Modem_SMS(ps, ps->modem->tele_nr_temp->id, "Hello from your wastewater treatment plant, your number is added."); }
        else{ Modem_Call(ps, ps->modem->tele_nr_temp->id); }

        // reset temp
        ps->modem->tele_nr_temp->id = 0;
        for(unsigned char i = 0; i < 16; i++){ ps->modem->tele_nr_temp->nr[i] = 11; }
        ps->modem->temp_digit_pos = 0;
      }
      break;

    // no touch
    case 0x00:

      // unmark and reset
      for(unsigned char i = 0; i < 12; i++){ if(ps->touch_state->int_var[0] & (1 << i)){ LCD_pPinButtons(i); } }
      if(ps->touch_state->int_var[0] & (1 << 12)){ LCD_Sym_Pin_ClearPinCode(); }
      ps->touch_state->int_var[0] = 0;
      break;

    default: break;
  }

  // secret check
  if(ps->touch_state->var[4] > 3 && !ps->modem->tele_nr_temp->id)
  {
    ps->touch_state->var[4] = 0;
    LCD_Sym_Pin_ClearPinCode();

    // manual / setup, check secret
    if((ps->touch_state->var[0] == PIN_SECRET_ENTER_0) && (ps->touch_state->var[1] == PIN_SECRET_ENTER_1) && (ps->touch_state->var[2] == PIN_SECRET_ENTER_2) && (ps->touch_state->var[3] == PIN_SECRET_ENTER_3))
    {
      LCD_Sym_Pin_RightMessage();
      switch(ps->page_state->page)
      {
        case PinManual: ps->page_state->page = ManualPage; break;
        case PinSetup: ps->page_state->page = SetupPage; break;
        default: break;
      }
      ps->touch_state->init = false;
    }

    // reset compressor hours
    else if((ps->touch_state->var[0] == PIN_SECRET_COMP_0) && (ps->touch_state->var[1] == PIN_SECRET_COMP_1) && (ps->touch_state->var[2] == PIN_SECRET_COMP_2) && (ps->touch_state->var[3] == PIN_SECRET_COMP_3))
    {
      MCP7941_Write_Comp_OpHours(0);
      LCD_Sym_Pin_OpHoursMessage();
    }

    // enter telephone 1
    else if((ps->touch_state->var[0] == PIN_SECRET_TEL1_0) && (ps->touch_state->var[1] == PIN_SECRET_TEL1_1) && (ps->touch_state->var[2] == PIN_SECRET_TEL1_2) && (ps->touch_state->var[3] == PIN_SECRET_TEL1_3))
    {
      ps->modem->tele_nr_temp->id = 1;
      ps->modem->temp_digit_pos = 0;
      AT24C_TeleNr_ReadToModem(ps);
      LCD_Sym_Pin_PrintWholeTelNumber(ps->modem->tele_nr1);
      LCD_Sym_Pin_OkButton(0);
    }

    // enter telephone 2
    else if((ps->touch_state->var[0] == PIN_SECRET_TEL2_0) && (ps->touch_state->var[1] == PIN_SECRET_TEL2_1) && (ps->touch_state->var[2] == PIN_SECRET_TEL2_2) && (ps->touch_state->var[3] == PIN_SECRET_TEL2_3))
    {
      ps->modem->tele_nr_temp->id = 2;
      ps->modem->temp_digit_pos = 0;
      AT24C_TeleNr_ReadToModem(ps);
      LCD_Sym_Pin_PrintWholeTelNumber(ps->modem->tele_nr2);
      LCD_Sym_Pin_OkButton(0);
    }

    // wrong 
    else{ LCD_Sym_Pin_WrongMessage(); }
    LCD_Sym_Pin_DelDigits();
  }

  // stop
  if(ps->modem->temp_digit_pos > 14)
  {
    // tel written
    ps->modem->tele_nr_temp->id = 0;
    ps->modem->temp_digit_pos = 0;
    ps->touch_state->var[4] = 0;
    LCD_Sym_Pin_ClearPinCode();
  }
  if(ps->modem->tele_nr_temp->id){ Modem_ReadSLED(PinModem); }
}


/*-------------------------------------------------------------------*
 *  pin linker
 * ------------------------------------------------------------------*/

void Touch_Pin_Linker_TeleTemp_AddDigit(struct PlantState *ps, unsigned char digit)
{
  ps->modem->tele_nr_temp->nr[ps->modem->temp_digit_pos] = digit; 
  LCD_Sym_Pin_PrintOneTelNumberDigit(digit, ps->modem->temp_digit_pos); 
  ps->modem->temp_digit_pos++;
}


/* ------------------------------------------------------------------*
 *            main linker embedding
 * ------------------------------------------------------------------*/

void Touch_Matrix_MainLinker(struct PlantState *ps, unsigned char touch_matrix)
{
  switch(touch_matrix)
  {
    // main linker
    case 0x41: if(!ps->touch_state->touched){ ps->touch_state->init = false; ps->page_state->page = AutoPage; } break;
    case 0x42: if(!ps->touch_state->touched){ ps->touch_state->init = false; ps->page_state->page = ManualPage; } break;
    case 0x43: if(!ps->touch_state->touched){ ps->touch_state->init = false; ps->page_state->page = SetupPage; } break;
    case 0x44: if(!ps->touch_state->touched){ ps->touch_state->init = false; ps->page_state->page = DataPage; } break;
    default: break;
  }
}

void Touch_Matrix_MainLinker_Data(struct PlantState *ps, unsigned char touch_matrix)
{
  switch(touch_matrix)
  {
    // main linker
    case 0x41: ps->touch_state->init = false; ps->page_state->page = AutoPage; break;
    case 0x42: ps->touch_state->init = false; ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: ps->touch_state->init = false; ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: ps->touch_state->init = false; ps->page_state->page = DataPage; break;
    default: break;
  }
}