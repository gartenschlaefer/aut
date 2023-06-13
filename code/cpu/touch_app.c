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
#include "settings.h"


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
    ps->touch_state->touched = _ctrl_zero;

    // bug
    ps->touch_state->var[0] = 0;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {

    // auto
    case 0x41:
      if(!ps->touch_state->touched)
      {
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        ps->touch_state->var[0]++;
      }
      break;

    // manual
    case 0x42:
      if(!ps->touch_state->touched)
      {
        if(!ps->port_state->valve_action_flag)
        {
          ps->touch_state->init = false;
          PORT_Backlight_On(ps->backlight);
          Error_Off(ps);
          ps->touch_state->var[0] = 0;
          LCD_Sym_MarkTextButton(TEXT_BUTTON_manual);
          ps->page_state->page = PinManual;
          LCD_PinPage_Init(ps);
        }
      }
      break;

    // setup
    case 0x43: 
      if(!ps->port_state->valve_action_flag)
      {
        ps->touch_state->init = false;
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        ps->touch_state->var[0] = 0;
        LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
        ps->page_state->page = PinSetup;
        LCD_PinPage_Init(ps);
      }
      break;

    // data
    case 0x44: 
      if(!ps->port_state->valve_action_flag)
      {
        ps->touch_state->init = false;
        PORT_Backlight_On(ps->backlight);
        Error_Off(ps);
        ps->touch_state->var[0] = 0;
        LCD_Sym_MarkTextButton(TEXT_BUTTON_data);
        ps->page_state->page = DataPage;
      }
      break;

    case 0x00: 
      if(ps->touch_state->touched){ ps->touch_state->touched = _ctrl_zero; }
      break;

    default: PORT_Backlight_On(ps->backlight); if(touch_matrix == 0x55){ ps->touch_state->var[0] = 0; } break;
  }

  // bug code entered
  if(ps->touch_state->var[0] >= 5)
  {
    ps->page_state->page_time->min = 0;
    ps->page_state->page_time->sec = 1;
    LCD_Sym_Auto_PageTime_Print(ps->page_state->page_time);
    ps->touch_state->var[0] = 0;
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
    ps->touch_state->touched = _ctrl_zero;

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
    case 0x14: if(ps->page_state->page != ManualPumpOff){ Touch_Manual_Linker_Select(ps, _n_pump_off); } break;

    case 0x21: if(ps->page_state->page != ManualMud){ Touch_Manual_Linker_Select(ps, _n_mud); } break;
    case 0x22: if(ps->page_state->page != ManualCompressor){ Touch_Manual_Linker_Select(ps, _n_compressor); } break;
    case 0x23: if(ps->page_state->page != ManualPhosphor){ Touch_Manual_Linker_Select(ps, _n_phosphor); } break;
    case 0x24: if(ps->page_state->page != ManualInflowPump){ Touch_Manual_Linker_Select(ps, _n_inflow_pump); } break;

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
      case _n_pump_off: ps->page_state->page = ManualPumpOff; break;
      case _n_mud: ps->page_state->page = ManualMud; break;
      case _n_compressor: ps->page_state->page = ManualCompressor; break;
      case _n_phosphor: ps->page_state->page = ManualPhosphor; break;
      case _n_inflow_pump: ps->page_state->page = ManualInflowPump; break;
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


/* ------------------------------------------------------------------*
 *            setup value limits
 * ------------------------------------------------------------------*/

void Touch_Setup_Matrix_MinusPlus(struct PlantState *ps, unsigned char touch_matrix)
{
  switch(touch_matrix)
  {
    // minus
    case 0x23: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_minus; 
        LCD_Sym_ControlButtons(_ctrl_neg_minus); 
      }
      Basic_LimitDec_VL(ps->touch_state->p_value_setting, ps->touch_state->p_value_limit);
      break;

    // plus
    case 0x24: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_pos_plus;
        LCD_Sym_ControlButtons(_ctrl_neg_plus);
      }
      Basic_LimitAdd_VL(ps->touch_state->p_value_setting, ps->touch_state->p_value_limit);
      break;
      
    default: break;
  }
}



/* ------------------------------------------------------------------*
 *            setup page linker embedding
 * ------------------------------------------------------------------*/

void Touch_Setup_Matrix_PageButtonLinker(struct PlantState *ps, unsigned char touch_matrix)
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
    ps->touch_state->var[0] = NonePage;
    ps->touch_state->var[1] = _none_symbol;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    case 0x11: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupCirculate; ps->touch_state->var[1] = _n_circulate; } break;
    case 0x12: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupAir; ps->touch_state->var[1] = _n_air; } break;
    case 0x13: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupSetDown; ps->touch_state->var[1] = _n_setDown; } break;
    case 0x14: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupPumpOff; ps->touch_state->var[1] = _n_pump_off; } break;

    case 0x21: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupMud; ps->touch_state->var[1] = _n_mud; } break;
    case 0x22: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupCompressor; ps->touch_state->var[1] = _n_compressor; } break;
    case 0x23: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupPhosphor; ps->touch_state->var[1] = _n_phosphor; } break;
    case 0x24: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupInflowPump; ps->touch_state->var[1] = _n_inflow_pump; } break;

    case 0x31: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupCal; ps->touch_state->var[1] = _n_cal; } break;
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupAlarm; ps->touch_state->var[1] = _n_alarm; } break;
    case 0x33: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupWatch; ps->touch_state->var[1] = _n_watch; } break;
    case 0x34: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->touch_state->var[0] = SetupZone; ps->touch_state->var[1] = _n_zone; } break;

    case 0x00: 
      if(ps->touch_state->touched)
      { 
        if(ps->touch_state->var[0] != NonePage){ ps->page_state->page = ps->touch_state->var[0]; }
        ps->touch_state->var[0] = NonePage;
        ps->touch_state->touched = _ctrl_zero;
        ps->touch_state->init = false;
      } 
      break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

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
    Settings_Read_Circulate(ps->settings->settings_circulate);

    // selection
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_circulate->on_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_circulate->val_lim_on_min;
    LCD_Sym_Setup_Circulate_Change_Sensor(ps->settings->settings_circulate->sensor_in_tank);
    LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->select, ps->settings->settings_circulate);
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

        // save settings
        Settings_Save_Circulate(ps->settings->settings_circulate);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->touch_state->init = false;
        ps->page_state->page = SetupPage;
      }
      break;

    // on
    case 0x21: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_on; 
        ps->touch_state->p_value_setting = &ps->settings->settings_circulate->on_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_circulate->val_lim_on_min;
        ps->touch_state->select = 0;
        LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->select, ps->settings->settings_circulate);
      } 
      break;

    // off
    case 0x22: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_off;
        ps->touch_state->p_value_setting = &ps->settings->settings_circulate->off_min; 
        ps->touch_state->p_value_limit = &ps->settings->settings_circulate->val_lim_off_min; 
        ps->touch_state->select = 1; 
        LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->select, ps->settings->settings_circulate);
      }
      break;

    // circ sensor
    case 0x31: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_sensor;
        ps->settings->settings_circulate->sensor_in_tank = 1;
        LCD_Sym_Setup_Circulate_Change_Sensor(1); 
      } 
      break;

    // time
    case 0x32: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->p_value_setting = &ps->settings->settings_circulate->time_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_circulate->val_lim_time_min;
        ps->settings->settings_circulate->sensor_in_tank = 0;
        ps->touch_state->select = 2;
        LCD_Sym_Setup_Circulate_Change_Sensor(0);
        LCD_Sym_Setup_Circulate_Change_Values(ps->touch_state->select, ps->settings->settings_circulate);
      } 
      break;

    // no touch
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // common matrix linkers
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  if(ps->touch_state->touched)
  {
    switch(ps->touch_state->select)
    {
      case 0: LCD_Sym_Setup_General_OnValue(true, ps->settings->settings_circulate->on_min); break;
      case 1: LCD_Sym_Setup_General_OffValue(true, ps->settings->settings_circulate->off_min); break;
      case 2: LCD_Sym_Setup_General_TimeValue(true, ps->settings->settings_circulate->time_min); break;
      default: break;
    }
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
    Settings_Read_Air(ps->settings->settings_air);

    // select
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_air->on_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_air->val_lim_on_min;
    LCD_Sym_Setup_Air_Change_Values(ps->touch_state->select, ps->settings->settings_air);
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
      Settings_Save_Air(ps->settings->settings_air);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // on
    case 0x21: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_on; 
        ps->touch_state->select = 0;
        ps->touch_state->p_value_setting = &ps->settings->settings_air->on_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_air->val_lim_on_min;
        LCD_Sym_Setup_Air_Change_Values(ps->touch_state->select, ps->settings->settings_air);
      } 
      break;

    // off
    case 0x22: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_off;
        ps->touch_state->select = 1; 
        ps->touch_state->p_value_setting = &ps->settings->settings_air->off_min; 
        ps->touch_state->p_value_limit = &ps->settings->settings_air->val_lim_off_min; 
        LCD_Sym_Setup_Air_Change_Values(ps->touch_state->select, ps->settings->settings_air);
      }
      break;

    // time
    case 0x32: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_time;
        ps->touch_state->select = 2;
        ps->touch_state->p_value_setting = &ps->settings->settings_air->time_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_air->val_lim_time_min;
        LCD_Sym_Setup_Air_Change_Values(ps->touch_state->select, ps->settings->settings_air);
      } 
      break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // common touch matrices
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  if(ps->touch_state->touched)
  {
    switch(ps->touch_state->select)
    {
      case 0: LCD_Sym_Setup_General_OnValue(true, ps->settings->settings_air->on_min); break;
      case 1: LCD_Sym_Setup_General_OffValue(true, ps->settings->settings_air->off_min); break;
      case 2: LCD_Sym_Setup_General_TimeValue(true, ps->settings->settings_air->time_min); break;
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
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_SetDown(ps->settings->settings_set_down);

    // selection
    ps->touch_state->p_value_setting = &ps->settings->settings_set_down->time_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_set_down->val_lim_time_min;
    LCD_Sym_Setup_SetDown_TimeValue(ps->settings->settings_set_down->time_min);
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
      Settings_Save_SetDown(ps->settings->settings_set_down);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // time config
  if(ps->touch_state->touched){ LCD_Sym_Setup_SetDown_TimeValue(ps->settings->settings_set_down->time_min); }
}


/* ------------------------------------------------------------------*
 *            setup Pump-Off linker
 * ------------------------------------------------------------------*/

void Touch_Setup_PumpOffLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_PumpOff(ps->settings->settings_pump_off);

    // selection
    ps->touch_state->p_value_setting = &ps->settings->settings_pump_off->on_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_pump_off->val_lim_on_min;

    // on value
    LCD_Sym_Setup_General_OnTextValue(true, ps->settings->settings_pump_off->on_min);

    // symbols
    LCD_Sym_Setup_PumpOff_Compressor(!(bool)ps->settings->settings_pump_off->pump);
    LCD_Sym_Setup_PumpOff_Pump((bool)ps->settings->settings_pump_off->pump);
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
      Settings_Save_PumpOff(ps->settings->settings_pump_off);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->touch_state->init = false;
      ps->page_state->page = SetupPage;
      break;

    // mammoth pump
    case 0x31: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->settings->settings_pump_off->pump = 0; LCD_Sym_Setup_PumpOff_Compressor(true); LCD_Sym_Setup_PumpOff_Pump(false); } break;

    // electrical pump
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->settings->settings_pump_off->pump = 1; LCD_Sym_Setup_PumpOff_Compressor(false); LCD_Sym_Setup_PumpOff_Pump(true); } break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // output
  if(ps->touch_state->touched == _ctrl_pos_minus || ps->touch_state->touched == _ctrl_pos_plus){ LCD_Sym_Setup_General_OnValue(true, ps->settings->settings_pump_off->on_min); }
}


/* ------------------------------------------------------------------*
 *            setup mud linker
 * ------------------------------------------------------------------*/

void Touch_Setup_MudLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_Mud(ps->settings->settings_mud);

    // selection
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_mud->on_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_mud->val_lim_on_min;

    LCD_Sym_Setup_Mud_Min(!(bool)ps->touch_state->select, ps->settings->settings_mud->on_min);
    LCD_Sym_Setup_Mud_Sec((bool)ps->touch_state->select, ps->settings->settings_mud->on_sec);
    LCD_Sym_Setup_Mud_MinText(!(bool)ps->touch_state->select);
    LCD_Sym_Setup_Mud_SecText((bool)ps->touch_state->select);
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
      Settings_Save_Mud(ps->settings->settings_mud);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // min
    case 0x21: 
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 0;
        ps->touch_state->p_value_setting = &ps->settings->settings_mud->on_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_mud->val_lim_on_min;
        LCD_Sym_Setup_Mud_Sec(false, ps->settings->settings_mud->on_sec);
        LCD_Sym_Setup_Mud_MinText(!(bool)ps->touch_state->select);
        LCD_Sym_Setup_Mud_SecText((bool)ps->touch_state->select);
      }
      break;
    
    // sec
    case 0x22: 
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 1;
        ps->touch_state->p_value_setting = &ps->settings->settings_mud->on_sec;
        ps->touch_state->p_value_limit = &ps->settings->settings_mud->val_lim_on_sec;
        LCD_Sym_Setup_Mud_Min(false, ps->settings->settings_mud->on_min);
        LCD_Sym_Setup_Mud_MinText(!(bool)ps->touch_state->select);
        LCD_Sym_Setup_Mud_SecText((bool)ps->touch_state->select);
      }
      break;

    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // min max
  if(ps->touch_state->touched){ (ps->touch_state->select ? LCD_Sym_Setup_Mud_Sec(true, ps->settings->settings_mud->on_sec) : LCD_Sym_Setup_Mud_Min(true, ps->settings->settings_mud->on_min)); }
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
    Settings_Read_Compressor(ps->settings->settings_compressor);

    // selection
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_compressor->min_pressure;
    ps->touch_state->p_value_limit = &ps->settings->settings_compressor->val_lim_min_pressure;

    // min max
    LCD_Sym_Setup_Compressor_MinP(!ps->touch_state->select, ps->settings->settings_compressor->min_pressure);
    LCD_Sym_Setup_Compressor_MaxP(ps->touch_state->select, ps->settings->settings_compressor->max_pressure);
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
        Settings_Save_Compressor(ps->settings->settings_compressor);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->touch_state->init = false;
        ps->page_state->page = SetupPage;
      }
      break;

    // min
    case 0x21:
    case 0x22:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 0;
        ps->touch_state->p_value_setting = &ps->settings->settings_compressor->min_pressure;
        ps->touch_state->p_value_limit = &ps->settings->settings_compressor->val_lim_min_pressure;
        LCD_Sym_Setup_Compressor_MaxP(false, ps->settings->settings_compressor->max_pressure);
      }
      break;

    // max
    case 0x31:
    case 0x32:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 1;
        ps->touch_state->p_value_setting = &ps->settings->settings_compressor->max_pressure;
        ps->touch_state->p_value_limit = &ps->settings->settings_compressor->val_lim_max_pressure;
        LCD_Sym_Setup_Compressor_MinP(false, ps->settings->settings_compressor->min_pressure);
      }
      break;

    // nothing
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // min max value update
  if(ps->touch_state->touched){ (ps->touch_state->select ? LCD_Sym_Setup_Compressor_MaxP(true, ps->settings->settings_compressor->max_pressure) : LCD_Sym_Setup_Compressor_MinP(true, ps->settings->settings_compressor->min_pressure)); }
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
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_Phosphor(ps->settings->settings_phosphor);

    // selection
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_phosphor->on_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_phosphor->val_lim_on_min;

    LCD_Sym_Setup_General_OnTextValue(!(bool)ps->touch_state->select, ps->settings->settings_phosphor->on_min);
    LCD_Sym_Setup_General_OffTextValue((bool)ps->touch_state->select, ps->settings->settings_phosphor->off_min);
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
        Settings_Save_Phosphor(ps->settings->settings_phosphor);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->phosphor_state->init_flag = false;
        LCD_Auto_Phosphor_Init(ps);
        ps->touch_state->init = false;
        ps->page_state->page = SetupPage;
      }
      break;

    // on
    case 0x21:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_on;
        ps->touch_state->select = 0;
        ps->touch_state->p_value_setting = &ps->settings->settings_phosphor->on_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_phosphor->val_lim_on_min;
        LCD_Sym_Setup_General_OnTextValue(!(bool)ps->touch_state->select, ps->settings->settings_phosphor->on_min);
        LCD_Sym_Setup_General_OffTextValue((bool)ps->touch_state->select, ps->settings->settings_phosphor->off_min);
      }
      break;

    // off
    case 0x22:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_off;
        ps->touch_state->select = 1;
        ps->touch_state->p_value_setting = &ps->settings->settings_phosphor->off_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_phosphor->val_lim_off_min;
        LCD_Sym_Setup_General_OnTextValue(!(bool)ps->touch_state->select, ps->settings->settings_phosphor->on_min);
        LCD_Sym_Setup_General_OffTextValue((bool)ps->touch_state->select, ps->settings->settings_phosphor->off_min);
      }
      break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // off or on value
  if(ps->touch_state->touched){ (ps->touch_state->select ? LCD_Sym_Setup_General_OffValue(true, ps->settings->settings_phosphor->off_min) : LCD_Sym_Setup_General_OnValue(true, ps->settings->settings_phosphor->on_min)); }
}


/* ------------------------------------------------------------------*
 *            setup inflow pump linker
 * ------------------------------------------------------------------*/

void Touch_Setup_InflowPumpLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_InflowPump(ps->settings->settings_inflow_pump);

    // selection
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_inflow_pump->on_min;
    ps->touch_state->p_value_limit = &ps->settings->settings_inflow_pump->val_lim_on_min;

    LCD_Sym_Setup_Pump(ps->settings->settings_inflow_pump->pump);
    LCD_Sym_Setup_InflowPump_Sensor((bool)ps->settings->settings_inflow_pump->sensor_out_tank);

    // initialize text and symbols
    LCD_Sym_Setup_InflowPump_Text(ps->touch_state->select);
    LCD_Sym_Setup_InflowPump_Values(ps->touch_state->select, ps->settings->settings_inflow_pump);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // off h
    case 0x12:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 2;
        ps->touch_state->p_value_setting = &ps->settings->settings_inflow_pump->off_hou;
        ps->touch_state->p_value_limit = &ps->settings->settings_inflow_pump->val_lim_off_hou;
        LCD_Sym_Setup_InflowPump_Text(ps->touch_state->select);
        LCD_Sym_Setup_InflowPump_Values(ps->touch_state->select, ps->settings->settings_inflow_pump);
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
        Settings_Save_InflowPump(ps->settings->settings_inflow_pump);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->inflow_pump_state->init_flag = false;
        LCD_Auto_InflowPump_Init(ps);
        ps->touch_state->init = false; 
        ps->page_state->page = SetupPage;
      }
      break;

    // on min
    case 0x21:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 0;
        ps->touch_state->p_value_setting = &ps->settings->settings_inflow_pump->on_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_inflow_pump->val_lim_on_min;
        LCD_Sym_Setup_InflowPump_Text(ps->touch_state->select);
        LCD_Sym_Setup_InflowPump_Values(ps->touch_state->select, ps->settings->settings_inflow_pump);
      }
      break;

    // off min
    case 0x22:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 1;
        ps->touch_state->p_value_setting = &ps->settings->settings_inflow_pump->off_min;
        ps->touch_state->p_value_limit = &ps->settings->settings_inflow_pump->val_lim_off_min;
        LCD_Sym_Setup_InflowPump_Text(ps->touch_state->select);
        LCD_Sym_Setup_InflowPump_Values(ps->touch_state->select, ps->settings->settings_inflow_pump);
      }
      break;

    // sensor out tank
    case 0x31:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_sensor;
        ps->settings->settings_inflow_pump->sensor_out_tank = f_toogle_bool(ps->settings->settings_inflow_pump->sensor_out_tank);
        LCD_Sym_Setup_InflowPump_Sensor((bool)ps->settings->settings_inflow_pump->sensor_out_tank);
      }
      break;

    // mammoth pump
    case 0x32: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->settings->settings_inflow_pump->pump = 0; LCD_Sym_Setup_Pump(ps->settings->settings_inflow_pump->pump); } break;

    // electrical pump
    case 0x33: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->settings->settings_inflow_pump->pump = 1; LCD_Sym_Setup_Pump(ps->settings->settings_inflow_pump->pump); } break;

    // two electrical pumps
    case 0x34: if(!ps->touch_state->touched){ ps->touch_state->touched = _ctrl_none; ps->settings->settings_inflow_pump->pump = 2; LCD_Sym_Setup_Pump(ps->settings->settings_inflow_pump->pump); } break;

    // nothing
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  switch(ps->touch_state->select)
  {
    // off h, off min, on min
    case 0: LCD_Sym_Setup_InflowPump_ValuesOnMin(true, ps->settings->settings_inflow_pump->on_min); break;
    case 1: LCD_Sym_Setup_InflowPump_ValuesOffMin(true, ps->settings->settings_inflow_pump->off_min); break;
    case 2: LCD_Sym_Setup_InflowPump_ValuesOffHou(true, ps->settings->settings_inflow_pump->off_hou); break;
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
    Settings_Read_Calibration(ps->settings->settings_calibration);

    // open ventil
    ps->touch_state->var[0] = false;
    ps->touch_state->var[1] = ZeroPage;
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // esc
    case 0x13: 
      if(!ps->touch_state->touched && !ps->touch_state->var[1])
      { 
        ps->touch_state->touched = _ctrl_pos_esc;
        LCD_Sym_ControlButtons(_ctrl_neg_esc);
        ps->touch_state->var[1] = SetupPage;
      }
      break;

    // okay
    case 0x14: 
      if(!ps->touch_state->touched && ps->page_state->page == SetupCal && !ps->touch_state->var[1])
      { 
        ps->touch_state->touched = _ctrl_pos_ok;
        LCD_Sym_ControlButtons(_ctrl_neg_ok);
        Settings_Save_Calibration(ps->settings->settings_calibration);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->touch_state->var[1] = SetupPage;
      }
      break;

    // open all valves
    case 0x23: 
      if(!ps->touch_state->touched && ps->page_state->page == SetupCal)
      { 
        ps->touch_state->touched = _ctrl_open_valve;
        // toggle valve
        ps->touch_state->var[0] = f_toogle_bool(ps->touch_state->var[0]);
        LCD_Sym_Setup_Cal_OpenValveButton(ps->touch_state->var[0]);
        OUT_Valve_Action(ps, (ps->touch_state->var[0] ? OPEN_All : CLOSE_All));
      }
      break;

    // cal
    // calibration for setting pressure to zero level
    case 0x24: 
      if(!ps->touch_state->touched && ps->page_state->page == SetupCal && !ps->touch_state->var[1])
      { 
        ps->touch_state->touched = _ctrl_cal;
        LCD_Sym_Setup_Cal_Button(true);

        // new calibration
        ps->settings->settings_calibration->zero_offset_pressure = MPX_UnCal_Average_New(ps->mpx_state);
      }
      break;

    // level measure
    case 0x31: 
      if(!ps->touch_state->touched && ps->page_state->page != SetupCalPressure)
      { 
        ps->touch_state->touched = _ctrl_level;
        LCD_Sym_Setup_Cal_Level_Sym(true);

        // only if no movement of valves
        if(!ps->port_state->valve_action_flag)
        {
          // reset open valves
          if(ps->touch_state->var[0])
          {
            ps->touch_state->var[0] = 0;
            LCD_Sym_Setup_Cal_OpenValveButton(false);
            OUT_Valve_Action(ps, CLOSE_All);
          }
          ps->page_state->page = SetupCalPressure;
        }
      }
      break;

    // redo
    // calibration redo with pressure -> auto zone page
    case 0x34: 
      if(!ps->touch_state->touched && !ps->touch_state->var[1])
      { 
        ps->touch_state->touched = _ctrl_redo;
        if(!ps->settings->settings_zone->sonic_on)
        {
          ps->settings->settings_calibration->redo_on = f_toogle_bool(ps->settings->settings_calibration->redo_on);
          LCD_Sym_Setup_Cal_MPX_Redo((bool)ps->settings->settings_calibration->redo_on);
        }
      }
      break;

    // no ps->touch_state->touched
    case 0x00:
      if(ps->touch_state->touched)
      {
        switch(ps->touch_state->touched)
        {
          case _ctrl_cal: LCD_Sym_Setup_Cal_Button(false); break;
          case _ctrl_level: if(ps->page_state->page != SetupCalPressure){ LCD_Sym_Setup_Cal_Level_Sym(false); } break;
          default: break;
        }
        ps->touch_state->touched = _ctrl_zero;
      }
      break;

    // main linker
    case 0x41: if(!ps->touch_state->touched){ LCD_Sym_MarkTextButton(TEXT_BUTTON_auto); ps->touch_state->var[1] = AutoPage; } break;
    case 0x42: if(!ps->touch_state->touched){ LCD_Sym_MarkTextButton(TEXT_BUTTON_manual); ps->touch_state->var[1] = ManualPage; } break;
    case 0x43: if(!ps->touch_state->touched){ LCD_Sym_MarkTextButton(TEXT_BUTTON_setup); ps->touch_state->var[1] = SetupPage; } break;
    case 0x44: if(!ps->touch_state->touched){ LCD_Sym_MarkTextButton(TEXT_BUTTON_data); ps->touch_state->var[1] = DataPage; } break;

    default: break;
  }

  // leave setup page request through esc, ok, or main buttons
  if(ps->touch_state->var[1])
  { 
    // valves are open
    if(ps->touch_state->var[0])
    {
      // request closing
      OUT_Valve_Action(ps, CLOSE_All);
      LCD_Sym_Setup_Cal_OpenValveButton(false);
      ps->touch_state->var[0] = false;
    }

    // calibration of pressure
    if(ps->page_state->page == SetupCalPressure)
    { 
      ps->page_state->page = SetupCal;
      OUT_Clr_Air(ps);
    }

    // still closing
    if(ps->port_state->valve_action_flag)
    { 
      // blink valve button
      if(ps->frame_counter->frame % 10)
      {
        LCD_Sym_Setup_Cal_OpenValveButton((ps->frame_counter->frame < (TC_FPS_HALF - 1) ? true : false));
      }
    }
    // change page
    else
    { 
      ps->touch_state->init = false;
      ps->page_state->page = ps->touch_state->var[1];
      ps->touch_state->var[1] = ZeroPage;
    }
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
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_Alarm(ps->settings->settings_alarm);

    // selection
    ps->touch_state->p_value_setting = &ps->settings->settings_alarm->temp;
    ps->touch_state->p_value_limit = &ps->settings->settings_alarm->val_lim_temp;

    LCD_Sym_Setup_Alarm_Sensor((bool)ps->settings->settings_alarm->sensor);
    LCD_Sym_Setup_Alarm_Compressor((bool)ps->settings->settings_alarm->compressor);
    LCD_Sym_Setup_Alarm_TempValue(true, ps->settings->settings_alarm->temp);
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
      Settings_Save_Alarm(ps->settings->settings_alarm);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // sensor
    case 0x31:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_sensor;
        ps->settings->settings_alarm->sensor = f_toogle_bool(ps->settings->settings_alarm->sensor);
        LCD_Sym_Setup_Alarm_Sensor((bool)ps->settings->settings_alarm->sensor);
      }
      break;

    // compressor
    case 0x32:
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_compressor;
        ps->settings->settings_alarm->compressor = f_toogle_bool(ps->settings->settings_alarm->compressor);
        LCD_Sym_Setup_Alarm_Compressor((bool)ps->settings->settings_alarm->compressor);
      }
      break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  if(ps->touch_state->touched){ LCD_Sym_Setup_Alarm_TempValue(true, ps->settings->settings_alarm->temp); }
}


/* ------------------------------------------------------------------*
 *            setup watch linker
 * ------------------------------------------------------------------*/

void Touch_Setup_WatchLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
    ps->touch_state->var[0] = MCP7941_ReadTime(ps->twi_state, TIC_HOUR); TCC0_wait_us(25);
    ps->touch_state->var[1] = MCP7941_ReadTime(ps->twi_state, TIC_MIN); TCC0_wait_us(25);
    ps->touch_state->var[2] = MCP7941_ReadTime(ps->twi_state, TIC_DATE); TCC0_wait_us(25);
    ps->touch_state->var[3] = MCP7941_ReadTime(ps->twi_state, TIC_MONTH); TCC0_wait_us(25);
    ps->touch_state->var[4] = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
    ps->touch_state->select = 0;
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
      switch(ps->touch_state->select)
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
      switch(ps->touch_state->select)
      {
        case 0: ps->touch_state->var[0] = Basic_LimitDec(ps->touch_state->var[0], 0); break;
        case 1: ps->touch_state->var[1] = Basic_LimitDec(ps->touch_state->var[1], 0); break;
        case 2: ps->touch_state->var[2] = Basic_LimitDec(ps->touch_state->var[2], 1); break;
        case 3: ps->touch_state->var[3] = Basic_LimitDec(ps->touch_state->var[3], 1); break;
        case 4: ps->touch_state->var[4] = Basic_LimitDec(ps->touch_state->var[4], 0); break;
        default: break;
      } break;

    // hours
    case 0x21: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_h, ps->touch_state->var); ps->touch_state->select = 0; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // min
    case 0x22: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_min, ps->touch_state->var); ps->touch_state->select = 1; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // days
    case 0x31: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_day, ps->touch_state->var); ps->touch_state->select = 2; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // month
    case 0x32: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_month, ps->touch_state->var); ps->touch_state->select = 3; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // year
    case 0x33: if(!ps->touch_state->touched){ LCD_Sym_Setup_Watch_Mark(_n_year, ps->touch_state->var); ps->touch_state->select = 4; ps->touch_state->touched = _ctrl_pos_minus; } break;

    // no ps->touch_state->touched
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons2(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // write date time
  if(ps->touch_state->touched)
  {
    switch(ps->touch_state->select)
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
    ps->touch_state->touched = _ctrl_zero;
    Settings_Read_Zone(ps->settings->settings_zone);

    // select
    ps->touch_state->select = 0;
    ps->touch_state->p_value_setting = &ps->settings->settings_zone->level_to_air;
    ps->touch_state->p_value_limit = &ps->settings->settings_zone->val_lim_level_to_air;

    LCD_Sym_Setup_Zone_Sonic(ps->settings->settings_zone->sonic_on);
    LCD_Sym_Setup_Zone_LevelToAir_Value(!(bool)ps->touch_state->select, ps->settings->settings_zone->level_to_air);
    LCD_Sym_Setup_Zone_LevelToSetDown_Value((bool)ps->touch_state->select, ps->settings->settings_zone->level_to_set_down);
    LCD_Sym_Setup_Zone_Air(!(bool)ps->touch_state->select);
    LCD_Sym_Setup_Zone_SetDown((bool)ps->touch_state->select);
  }

  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // sonic
    case 0x12: 
      if(!ps->touch_state->touched)
      { 
        ps->touch_state->touched = _ctrl_sonic;
        ps->settings->settings_zone->sonic_on = f_toogle_bool(ps->settings->settings_zone->sonic_on);
        LCD_Sym_Setup_Zone_Sonic(ps->settings->settings_zone->sonic_on);
      } break;

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
        Settings_Save_Zone(ps->settings->settings_zone);
        MEM_EEPROM_WriteSetupEntry(ps);
        ps->touch_state->init = false; 
        ps->page_state->page = SetupPage;
      }
      break;

    // air level
    case 0x21:
    case 0x22:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 0;
        ps->touch_state->p_value_setting = &ps->settings->settings_zone->level_to_air;
        ps->touch_state->p_value_limit = &ps->settings->settings_zone->val_lim_level_to_air;
        LCD_Sym_Setup_Zone_LevelToSetDown_Value(false, ps->settings->settings_zone->level_to_set_down);
        LCD_Sym_Setup_Zone_Air(!(bool)ps->touch_state->select);
        LCD_Sym_Setup_Zone_SetDown((bool)ps->touch_state->select);
      }
      break;

    // set down level
    case 0x31:
    case 0x32:
      if(!ps->touch_state->touched)
      {
        ps->touch_state->touched = _ctrl_none;
        ps->touch_state->select = 1;
        ps->touch_state->p_value_setting = &ps->settings->settings_zone->level_to_set_down;
        ps->touch_state->p_value_limit = &ps->settings->settings_zone->val_lim_level_to_set_down;
        LCD_Sym_Setup_Zone_LevelToAir_Value(false, ps->settings->settings_zone->level_to_air);
        LCD_Sym_Setup_Zone_Air(!(bool)ps->touch_state->select);
        LCD_Sym_Setup_Zone_SetDown((bool)ps->touch_state->select);
      }
      break;

    // nothing
    case 0x00: if(ps->touch_state->touched){ LCD_Sym_ControlButtons(ps->touch_state->touched); ps->touch_state->touched = _ctrl_zero; } break;

    default: break;
  }

  // matrix main linker
  Touch_Setup_Matrix_MinusPlus(ps, touch_matrix);
  Touch_Setup_Matrix_PageButtonLinker(ps, touch_matrix);

  // circulate and o2
  if(ps->touch_state->touched){ (ps->touch_state->select ? LCD_Sym_Setup_Zone_LevelToSetDown_Value(true, ps->settings->settings_zone->level_to_set_down) : LCD_Sym_Setup_Zone_LevelToAir_Value(true, ps->settings->settings_zone->level_to_air)); }
}


/*-------------------------------------------------------------------*
 *            data matrix
 * ------------------------------------------------------------------*/

void Touch_Data_Matrix_PageButtonLinker(struct PlantState *ps, unsigned char touch_matrix)
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


/*-------------------------------------------------------------------*
 *            data linker
 * ------------------------------------------------------------------*/

void Touch_Data_Linker(struct PlantState *ps)
{
  unsigned char touch_matrix = Touch_Matrix(ps->touch_state);
  switch(touch_matrix)
  {
    // auto
    case 0x21: LCD_Write_TextButton(9, 0, TEXT_BUTTON_auto, true); ps->page_state->page = DataAuto; break;
    case 0x22: LCD_Write_TextButton(9, 40, TEXT_BUTTON_manual, true); ps->page_state->page = DataManual; break;
    case 0x23: LCD_Write_TextButton(9, 80, TEXT_BUTTON_setup, true); ps->page_state->page = DataSetup; break;
    case 0x24: LCD_Write_TextButton(9, 120, TEXT_BUTTON_sonic, true); ps->page_state->page = DataSonic; break;
    default: break;
  }

  // data main linker
  Touch_Data_Matrix_PageButtonLinker(ps, touch_matrix);
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
  Touch_Data_Matrix_PageButtonLinker(ps, touch_matrix);
}


/* ------------------------------------------------------------------*
 *            data manual linker
 * ------------------------------------------------------------------*/

void Touch_Data_ManualLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
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
  Touch_Data_Matrix_PageButtonLinker(ps, touch_matrix);
}


/* ------------------------------------------------------------------*
 *            data setup linker
 * ------------------------------------------------------------------*/

void Touch_Data_SetupLinker(struct PlantState *ps)
{
  if(!ps->touch_state->init)
  {
    ps->touch_state->init = true;
    ps->touch_state->touched = _ctrl_zero;
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
  Touch_Data_Matrix_PageButtonLinker(ps, touch_matrix);
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
        LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, true);
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
        LCD_Write_TextButton(16, 0, TEXT_BUTTON_boot, true);
        LCD_Write_TextButton(4, 120, TEXT_BUTTON_read, false);
        LCD_Write_TextButton(10, 120, TEXT_BUTTON_write, false);

        if(ps->page_state->page == DataSonicBoot)
        {
          LCD_Write_TextButton(16, 0, TEXT_BUTTON_boot, false);
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
      if(!ps->touch_state->touched && ps->page_state->page == DataSonicBoot){ ps->touch_state->touched = _ctrl_pos_plus; LCD_Write_TextButton(4, 120, TEXT_BUTTON_read, true); ps->page_state->page = DataSonicBootR; }
      break;

    case 0x24:
      if(!ps->touch_state->touched && ps->page_state->page == DataSonicBoot){ ps->touch_state->touched = _ctrl_pos_minus; LCD_Write_TextButton(10, 120, TEXT_BUTTON_write, true); ps->page_state->page = DataSonicBootW; }
      break;

    // reset buttons
    case 0x00:
      if(ps->touch_state->touched)
      {
        if(ps->touch_state->touched == _ctrl_shot){  LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, false); }
        else if(ps->touch_state->touched == _ctrl_auto){ LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, false); }
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
    ps->touch_state->touched = _ctrl_zero;

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
      if(!(ps->touch_state->pin_touch & (1 << 1)))
      { 
        ps->touch_state->pin_touch |= (1 << 1);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 1); }
        else{ LCD_Sym_Pin_WriteDigit(1, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 1; ps->touch_state->var[4]++; }
      }
      break;

    // 2
    case 0x12: 
      if(!(ps->touch_state->pin_touch & (1 << 2)))
      { 
        ps->touch_state->pin_touch |= (1 << 2);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 2); }
        else{ LCD_Sym_Pin_WriteDigit(2, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 2; ps->touch_state->var[4]++; }
      } break;

    // 3
    case 0x13: 
      if(!(ps->touch_state->pin_touch & (1 << 3)))
      { 
        ps->touch_state->pin_touch |= (1 << 3);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 3); }
        else{ LCD_Sym_Pin_WriteDigit(3, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 3; ps->touch_state->var[4]++; }
      } break;

    // 4
    case 0x21: 
      if(!(ps->touch_state->pin_touch & (1 << 4)))
      { 
        ps->touch_state->pin_touch |= (1 << 4);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 4); }
        else{ LCD_Sym_Pin_WriteDigit(4, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 4; ps->touch_state->var[4]++; }
      }
      break;

    // 5
    case 0x22: 
      if(!(ps->touch_state->pin_touch & (1 << 5)))
      { 
        ps->touch_state->pin_touch |= (1 << 5);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 5); }
        else{ LCD_Sym_Pin_WriteDigit(5, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 5; ps->touch_state->var[4]++; }
      }
      break;

    // 6
    case 0x23: 
      if(!(ps->touch_state->pin_touch & (1 << 6)))
      { 
        ps->touch_state->pin_touch |= (1 << 6);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 6); }
        else{ LCD_Sym_Pin_WriteDigit(6, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 6; ps->touch_state->var[4]++; }
      }
      break;

    // 7
    case 0x31: 
      if(!(ps->touch_state->pin_touch & (1 << 7)))
      { 
        ps->touch_state->pin_touch |= (1 << 7);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 7); }
        else{ LCD_Sym_Pin_WriteDigit(7, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 7; ps->touch_state->var[4]++; }
      } 
      break;

    // 8
    case 0x32: 
      if(!(ps->touch_state->pin_touch & (1 << 8)))
      { 
        ps->touch_state->pin_touch |= (1 << 8);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 8); }
        else{ LCD_Sym_Pin_WriteDigit(8, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 8; ps->touch_state->var[4]++; }
      }
      break;

    // 9
    case 0x33: 
      if(!(ps->touch_state->pin_touch & (1 << 9)))
      { 
        ps->touch_state->pin_touch |= (1 << 9);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 9); }
        else{ LCD_Sym_Pin_WriteDigit(9, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 9; ps->touch_state->var[4]++; }
      }
      break;

    // 0
    case 0x42:
      if(!(ps->touch_state->pin_touch & (1 << 0)))
      { 
        ps->touch_state->pin_touch |= (1 << 0);
        if(ps->modem->tele_nr_temp->id){ Touch_Pin_Linker_TeleTemp_AddDigit(ps, 0); }
        else{ LCD_Sym_Pin_WriteDigit(0, ps->touch_state->var[4]); ps->touch_state->var[ps->touch_state->var[4]] = 0; ps->touch_state->var[4]++; }
      }
      break;

    // del
    case 0x43:
      if(!(ps->touch_state->pin_touch & (1 << 10)))
      { 
        ps->touch_state->pin_touch |= (1 << 10);
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
      if(!(ps->touch_state->pin_touch & (1 << 11)))
      {
        ps->touch_state->pin_touch |= (1 << 11);
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
      if(!(ps->touch_state->pin_touch & (1 << 12)) && ps->modem->tele_nr_temp->id)
      {
        ps->touch_state->pin_touch |= (1 << 12);
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
      for(unsigned char i = 0; i < 12; i++){ if(ps->touch_state->pin_touch & (1 << i)){ LCD_pPinButtons(i); } }
      if(ps->touch_state->pin_touch & (1 << 12)){ LCD_Sym_Pin_ClearPinCode(); }
      ps->touch_state->pin_touch = 0;
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