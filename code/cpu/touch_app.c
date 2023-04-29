// --
// touch applications

#include "touch_app.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_sym.h"
#include "lcd_app.h"
#include "eval_app.h"
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


/*-------------------------------------------------------------------*
 *  Touch_Matrix
 * --------------------------------------------------------------
 *  return 0xYX
 *  Touch Matrix:
 *    1.Row: 11, 12, 13 ,14
 *    2.Row: 21, 22, 23 ,24, usw.
 * ------------------------------------------------------------------*/

unsigned char Touch_Matrix(void)
{
  static int x[2] = {0, 0};
  static int y[2] = {0, 0};
  int xAv = 0;
  int yAv = 0;
  unsigned char lx = 0;
  unsigned char hy = 0;
  static unsigned char state = 0;
  unsigned char *p_touch = Touch_Read();

  // first state, read data
  if(p_touch[0] == _ready && (state == 0 || state == 1))
  {
    y[state] = p_touch[1];
    x[state] = p_touch[2];
    state++;
  }

  // interpret data
  if(p_touch[0] == _ready && state == 2)
  {
    state = 0;

    // too much diffs, reject sample
    if(x[0] < x[1] - 10 || x[0] > x[1] + 10) return 0;
    if(y[0] < y[1] - 10 || y[0] > y[1] + 10) return 0;

    // calibrate
    yAv = Touch_Cal_Y_Value(((y[0] + y[1]) / 2));
    xAv = Touch_Cal_X_Value(((x[0] + x[1]) / 2));

    // x-matrix
    if (xAv < 16) lx = 0;
    else if (xAv > 16 && xAv < 35)  lx = 1;
    else if (xAv > 47 && xAv < 70)  lx = 2;
    else if (xAv > 80 && xAv < 105) lx = 3;
    else if (xAv > 115 && xAv < 145) lx = 4;
    else lx = 5;

    // y-matrix
    if (yAv < 16) hy = 0;
    else if (yAv > 18 && yAv < 37) hy = 1;
    else if (yAv > 45 && yAv < 58) hy = 2;
    else if (yAv > 65 && yAv < 77) hy = 3;
    else if (yAv > 85 && yAv < 105) hy = 4;
    else hy = 5;

    //*** debug touch matrix
    if (DEBUG)
    {
      LCD_WriteAnyValue(f_4x6_p, 2, 22, 152, hy);
      LCD_WriteAnyValue(f_4x6_p, 2, 24, 152, lx);
    }
  }

  // other, this is important
  else
  {
    hy = 6; 
    lx = 6;
  }

  // safety state
  if (state > 2) state = 0;

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
  static unsigned char bug = 0;
  static unsigned char touch = 0;

  switch(Touch_Matrix())
  {
    // backlight
    case 0x11: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x12: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x13: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x14: PORT_Backlight_On(ps->backlight); bug = 0; break;

    // secret code
    case 0x21:
      if(!touch)
      { 
        touch = 1;
        if(bug == 0){ bug = 1; }
        else if(bug == 1){ bug = 2; }
        else{ bug = 0; }
        PORT_Backlight_On(ps->backlight);
      }
      break;

    case 0x22: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x23: PORT_Backlight_On(ps->backlight); bug = 0; break;

    // secret code
    case 0x24:
      if(!touch)
      { 
        touch = 1;
        if(bug == 2) bug = 3;
        else if(bug == 3) bug = 4;
        else bug = 0;
        PORT_Backlight_On(ps->backlight);
      }
      break;

    case 0x31: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x32: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x33: PORT_Backlight_On(ps->backlight); bug = 0; break;
    case 0x34: PORT_Backlight_On(ps->backlight); bug = 0; break;

    // main linker
    case 0x41: 
      PORT_Backlight_On(ps->backlight);
      Error_Off(ps);
      if(bug == 4){ bug = 5; }
      else{ bug = 0; }
      break;

    // manual
    case 0x42: 
      PORT_Backlight_On(ps->backlight);
      Error_Off(ps); bug = 0;
      LCD_Sym_MarkTextButton(TEXT_BUTTON_manual);
      ps->page_state->page = PinManual; LCD_PinPage_Init(ps);
      break;

    // setup
    case 0x43: 
      PORT_Backlight_On(ps->backlight);
      Error_Off(ps); bug = 0;
      LCD_Sym_MarkTextButton(TEXT_BUTTON_setup);
      ps->page_state->page = PinSetup; LCD_PinPage_Init(ps);
      break;

    // data
    case 0x44: 
      PORT_Backlight_On(ps->backlight);
      Error_Off(ps); bug = 0;
      LCD_Sym_MarkTextButton(TEXT_BUTTON_data);
      ps->page_state->page = DataPage; break;

    case 0x00: if(touch) { touch = 0; } break;
    default: break;
  }

  // bug
  if(bug == 5)
  {
    ps->page_state->page_time->min = 0;
    ps->page_state->page_time->sec = 5;
    LCD_Sym_Auto_CountDown(ps->page_state->page_time);
    bug = 0;
  }

}


/* ------------------------------------------------------------------*
 *            manual linker
 * ------------------------------------------------------------------*/

void Touch_Manual_Linker(struct PlantState *ps)
{
  switch(Touch_Matrix())
  {
    case 0x11: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_circulate); ps->page_state->page = ManualCircOn; break;
    case 0x12: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_air); ps->page_state->page = ManualAir; break;
    case 0x13: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_setDown); ps->page_state->page = ManualSetDown; break;
    case 0x14: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_pumpOff); ps->page_state->page = ManualPumpOff; break;

    case 0x21: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_mud); ps->page_state->page = ManualMud; break;
    case 0x22: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_compressor); ps->page_state->page = ManualCompressor; break;
    case 0x23: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_phosphor); ps->page_state->page = ManualPhosphor; break;
    case 0x24: PORT_Backlight_On(ps->backlight); LCD_Sym_Manual_Select(_n_inflowPump); ps->page_state->page = ManualInflowPump; break;

    case 0x33: if(ps->page_state->page == ManualPumpOff){ LCD_WriteAnySymbol(s_19x19, 15, 85, _n_ok); ps->page_state->page = ManualPumpOff_On; } break;

    // main linker
    case 0x41: PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_auto); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = AutoPage; break;
    case 0x42: PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_manual); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = ManualPage; break;
    case 0x43: PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_setup); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = SetupPage; break;
    case 0x44: PORT_Backlight_On(ps->backlight); LCD_Sym_MarkTextButton(TEXT_BUTTON_data); MEM_EEPROM_WriteManualEntry(ps); ps->page_state->page = DataPage; break;

    default: break;
  }
}


/*-------------------------------------------------------------------*
 *  setup linker
 * ------------------------------------------------------------------*/

void Touch_Setup_Linker(struct PlantState *ps)
{
  switch(Touch_Matrix())
  {
    case 0x11: ps->page_state->page = SetupCirculate; break;
    case 0x12: ps->page_state->page = SetupAir; break;
    case 0x13: ps->page_state->page = SetupSetDown; break;
    case 0x14: ps->page_state->page = SetupPumpOff; break;

    case 0x21: ps->page_state->page = SetupMud; break;
    case 0x22: ps->page_state->page = SetupCompressor; break;
    case 0x23: ps->page_state->page = SetupPhosphor; break;
    case 0x24: ps->page_state->page = SetupInflowPump; break;

    case 0x31: ps->page_state->page = SetupCal; break;
    case 0x32: ps->page_state->page = SetupAlarm; break;
    case 0x33: ps->page_state->page = SetupWatch; break;
    case 0x34: ps->page_state->page = SetupZone; break;

    // main linker
    case 0x41: ps->page_state->page = AutoPage; break;
    case 0x42: ps->page_state->page = ManualPage; break;
    case 0x43: ps->page_state->page = SetupPage; break;
    case 0x44: ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            setup circulate linker
 * ------------------------------------------------------------------*/

void Touch_Setup_CirculateLinker(struct PlantState *ps)
{
  static unsigned char circ[4] = {0};
  static unsigned char sensor = 0;

  static int time1 = 0;
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;

  unsigned char *p_circ = circ;

  if(!init)
  {
    init = 1;
    on = 0;
    circ[0] = MEM_EEPROM_ReadVar(ON_circ);
    circ[1] = MEM_EEPROM_ReadVar(OFF_circ);
    circ[2] = MEM_EEPROM_ReadVar(TIME_L_circ);
    circ[3] = MEM_EEPROM_ReadVar(TIME_H_circ);
    sensor = MEM_EEPROM_ReadVar(SENSOR_inTank);
    time1 = ((circ[3] << 8) | circ[2]);
    LCD_Sym_Setup_CircSensor(sensor);
    LCD_Sym_Setup_CircText(on, p_circ);
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_circ, circ[0]);
      MEM_EEPROM_WriteVar(OFF_circ, circ[1]);
      MEM_EEPROM_WriteVar(TIME_L_circ, circ[2]);
      MEM_EEPROM_WriteVar(TIME_H_circ, circ[3]);
      MEM_EEPROM_WriteVar(SENSOR_inTank, sensor);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      switch(on)
      {
        // circulate on min / max
        case 0: circ[0] = Basic_LimitDec(circ[0], 2); break;
        case 1: circ[1] = Basic_LimitDec(circ[1], 5); break;
        case 2: time1 = Basic_LimitDec(time1, 0); break;
        default: break;
      } break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      switch(on)
      {
        // circulate on min / max
        case 0: circ[0] = Basic_LimitAdd(circ[0], 30); break;
        case 1: circ[1] = Basic_LimitAdd(circ[1], 60); break;
        case 2: time1 = Basic_LimitAdd(time1, 999); break;
        default: break;
      } break;

    // on
    case 0x21: if(!touch){ on = 0; touch = 5; LCD_Sym_Setup_CircText(on, p_circ); } break;

    // off
    case 0x22: if(!touch){ on = 1; touch = 5; LCD_Sym_Setup_CircText(on, p_circ); } break;

    // sensor
    case 0x31: if(!touch){ touch = 5; sensor = 1; LCD_Sym_Setup_CircSensor(1); } break;

    // time
    case 0x32: if(!touch){ touch = 5; sensor = 0; on = 2; LCD_Sym_Setup_CircSensor(0); LCD_Sym_Setup_CircText(on, p_circ); } break;

    // no touch
    case 0x00: if(touch){ circ[2] = (time1 & 0x00FF); circ[3] = ((time1>>8) & 0x00FF); LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  if(touch)
  {
    switch (on)
    {
      case 0: LCD_Sym_Setup_OnValueNeg(circ[0]); break;
      case 1: LCD_Sym_Setup_OffValueNeg(circ[1]); break;
      case 2: LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, time1); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            setup air linker
 * ------------------------------------------------------------------*/

void Touch_Setup_AirLinker(struct PlantState *ps)
{
  static unsigned char air[4] = {0};
  static int time2 = 0;
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;
  unsigned char *pointer_air = air;

  if(!init)
  {
    init = 1;
    on = 0;
    air[0] = MEM_EEPROM_ReadVar(ON_air);
    air[1] = MEM_EEPROM_ReadVar(OFF_air);
    air[2] = MEM_EEPROM_ReadVar(TIME_L_air);
    air[3] = MEM_EEPROM_ReadVar(TIME_H_air);
    time2 = ((air[3] << 8) | air[2]);
    LCD_Sym_Setup_AirText(on, pointer_air);
    LCD_ClrSpace(15, 39, 3, 31);
    LCD_WriteAnyStringFont(f_6x8_p, 16, 40, "Time:");
  }

  switch(Touch_Matrix())
  { 
    // esc
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_air, air[0]);
      MEM_EEPROM_WriteVar(OFF_air, air[1]);
      MEM_EEPROM_WriteVar(TIME_L_air, air[2]);
      MEM_EEPROM_WriteVar(TIME_H_air, air[3]);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      switch(on)
      {
        // on min, off min, time min
        case 0: air[0] = Basic_LimitDec(air[0], 10); break;
        case 1: air[1] = Basic_LimitDec(air[1], 5); break;
        case 2: time2 = Basic_LimitDec(time2, 0); break;
        default: break;
      } break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      switch(on)
      {
        // on max, off max, time max
        case 0: air[0] = Basic_LimitAdd(air[0], 60); break;
        case 1: air[1] = Basic_LimitAdd(air[1], 55); break;
        case 2: time2 = Basic_LimitAdd(time2, 999); break;
        default: break;
      } break;

    // on
    case 0x21: if(!touch){ on = 0; touch = 5; LCD_Sym_Setup_AirText(on, pointer_air); } break;

    // off
    case 0x22: if(!touch){ on = 1; touch = 5; LCD_Sym_Setup_AirText(on, pointer_air); } break;

    // time
    case 0x32: if(!touch){ touch = 5; on = 2; LCD_Sym_Setup_AirText(on, pointer_air); }
      LCD_FillSpace (15, 39, 4, 31);
      LCD_WriteAnyStringFont(f_6x8_n, 16, 40,"Time:"); 
      break;

    // no touch
    case 0x00: if(touch){ air[2] = (time2 & 0x00FF); air[3] = ((time2>>8) & 0x00FF); LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  if(touch)
  {
    switch (on)
    {
      case 0: LCD_Sym_Setup_OnValueNeg(air[0]); break;
      case 1: LCD_Sym_Setup_OffValueNeg(air[1]); break;
      case 2: LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, time2); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            setup set down linker
 * ------------------------------------------------------------------*/

void Touch_Setup_SetDownLinker(struct PlantState *ps)
{
  static unsigned char setDown = 0;

  static unsigned char init = 0;
  static unsigned char touch = 0;

  if(!init)
  {
    init = 1;
    setDown = MEM_EEPROM_ReadVar(TIME_setDown);
    LCD_WriteAnyValue(f_6x8_n, 3, 10, 30, setDown);
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(TIME_setDown, setDown);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      setDown = Basic_LimitDec(setDown, 50); break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      setDown = Basic_LimitAdd(setDown, 90); break;

    case 0x00: if(touch){ LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  // time config
  if(touch){ LCD_WriteAnyValue(f_6x8_n, 3, 10, 30, setDown); }
}


/* ------------------------------------------------------------------*
 *            setup Pump-Off linker
 * ------------------------------------------------------------------*/

void Touch_Setup_PumpOffLinker(struct PlantState *ps)
{
  static unsigned char pumpOn = 0;
  static unsigned char pump = 0;

  static unsigned char init = 0;
  static unsigned char touch = 0;

  if(!init)
  {
    init = 1;
    pumpOn = MEM_EEPROM_ReadVar(ON_pumpOff);
    pump = MEM_EEPROM_ReadVar(PUMP_pumpOff);

    LCD_Sym_Setup_OnValueNeg(pumpOn);

    if(!pump){ LCD_WriteAnySymbol(s_29x17, 15, 0, _n_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, _p_pump); }
    else{ LCD_WriteAnySymbol(s_29x17, 15, 0, _p_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, _n_pump); }
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_pumpOff, pumpOn);
      MEM_EEPROM_WriteVar(PUMP_pumpOff, pump);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(touch){ pumpOn = Basic_LimitDec(pumpOn, 5); } break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      if(touch){ pumpOn = Basic_LimitAdd(pumpOn, 60); } break;

    // mammoth pump
    case 0x31: if(!touch){ touch = 5; pump = 0; LCD_WriteAnySymbol(s_29x17, 15, 0, _n_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, _p_pump); } break;

    // electrical pump
    case 0x32: if(!touch){ touch = 5; pump = 1; LCD_WriteAnySymbol(s_29x17, 15, 0, _p_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, _n_pump); } break;

    // no touch
    case 0x00: if(touch){ LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  // output
  if(touch){ LCD_Sym_Setup_OnValueNeg(pumpOn); }
}


/* ------------------------------------------------------------------*
 *            setup mud linker
 * ------------------------------------------------------------------*/

void Touch_Setup_MudLinker(struct PlantState *ps)
{
  static unsigned char mudMin = 0;
  static unsigned char mudSec = 0;
  static unsigned char onM = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;

  if(!init)
  {
    init = 1;
    mudMin = MEM_EEPROM_ReadVar(ON_MIN_mud);
    mudSec = MEM_EEPROM_ReadVar(ON_SEC_mud);
    LCD_WriteAnyStringFont(f_6x8_p, 11, 7,"ON:");
    if(!onM){ LCD_WriteAnyValue(f_6x8_p, 2, 11, 40, mudMin); LCD_WriteAnyValue(f_6x8_n, 2, 16, 40, mudSec); }
    else{ LCD_WriteAnyValue(f_6x8_n, 2, 11, 40, mudMin); LCD_WriteAnyValue(f_6x8_p, 2, 16, 40, mudSec); }
    LCD_WriteAnyStringFont(f_6x8_p, 11,55,"min");
    LCD_WriteAnyStringFont(f_6x8_p, 16,55,"sec");
  }

  switch(Touch_Matrix())
  {
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_MIN_mud, mudMin);
      MEM_EEPROM_WriteVar(ON_SEC_mud, mudSec);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(onM) mudMin = Basic_LimitDec(mudMin, 0);
      else mudSec = Basic_LimitDec(mudSec, 0);
      break;

    // plus
    case 0x24: if(!touch){ LCD_ControlButtons(_setup_neg_sym_plus); touch = 4; }
      if(onM) mudMin = Basic_LimitAdd(mudMin, 20);
      else mudSec = Basic_LimitAdd(mudSec, 59);
      break;

    // min
    case 0x22: if(!touch){onM = 1; LCD_WriteAnyValue(f_6x8_p, 2, 16, 40, mudSec); touch = 5; } break;
    
    // sec
    case 0x32: if(!touch){onM = 0; LCD_WriteAnyValue(f_6x8_p, 2, 11, 40, mudMin); touch = 5; } break;
    case 0x00: if(touch){ LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  // min max
  if(onM && touch){ LCD_WriteAnyValue(f_6x8_n, 2, 11, 40, mudMin); }
  if(!onM && touch){ LCD_WriteAnyValue(f_6x8_n, 2, 16, 40, mudSec); }
}





/* ------------------------------------------------------------------*
 *            setup compressor linker
 * ------------------------------------------------------------------*/

void Touch_Setup_CompressorLinker(struct PlantState *ps)
{
  static int druckMin = 0;
  static int druckMax = 0;
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;

  unsigned char h = 0;
  unsigned char l = 0;

  if(!init)
  {
    init = 1;
    on = 0;
    h = MEM_EEPROM_ReadVar(MAX_H_druck);
    l = MEM_EEPROM_ReadVar(MAX_L_druck);
    druckMax = ((h << 8) | l);

    h = MEM_EEPROM_ReadVar(MIN_H_druck);
    l = MEM_EEPROM_ReadVar(MIN_L_druck);
    druckMin = ((h << 8) | l);

    // min max
    if(on){ LCD_WriteAnyValue(f_6x8_p, 3, 16, 7, druckMax); LCD_WriteAnyValue(f_6x8_n, 3, 11, 7, druckMin); }
    if(!on){ LCD_WriteAnyValue(f_6x8_p, 3, 11, 7, druckMin); LCD_WriteAnyValue(f_6x8_n, 3, 16, 7, druckMax); }
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: 
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      l = (druckMax & 0x00FF);
      h = ((druckMax >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(MAX_H_druck, h);
      MEM_EEPROM_WriteVar(MAX_L_druck, l);

      l = (druckMin & 0x00FF);
      h = ((druckMin >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(MIN_H_druck, h);
      MEM_EEPROM_WriteVar(MIN_L_druck, l);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(on) druckMin = Basic_LimitDec(druckMin, 0);
      else druckMax = Basic_LimitDec(druckMax, 0); 
      break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      if(on) druckMin = Basic_LimitAdd(druckMin, 999);
      else druckMax = Basic_LimitAdd(druckMax, 999); 
      break;

    // min
    case 0x21: if(!touch){on = 1; LCD_WriteAnyValue(f_6x8_p, 3, 16, 7, druckMax); touch = 5; } break;

    // max
    case 0x31: if(!touch){on = 0; LCD_WriteAnyValue(f_6x8_p, 3, 11, 7, druckMin); touch = 5; } break;

    // nothing
    case 0x00: if(touch){LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  // min max
  if(on && touch){ LCD_WriteAnyValue(f_6x8_n, 3, 11, 7, druckMin); }
  if(!on && touch){ LCD_WriteAnyValue(f_6x8_n, 3, 16, 7, druckMax); }
}


/* ------------------------------------------------------------------*
 *            setup phosphor linker
 * ------------------------------------------------------------------*/

void Touch_Setup_PhosphorLinker(struct PlantState *ps)
{
  static unsigned char pOn = 10;
  static unsigned char pOff = 10;
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;

  // init
  if(!init)
  {
    init = 1;
    pOff = MEM_EEPROM_ReadVar(OFF_phosphor);
    pOn = MEM_EEPROM_ReadVar(ON_phosphor);
    if(on)
    { 
      LCD_Sym_Setup_OnValueNeg(pOn); 
      LCD_Sym_Setup_OffValue(pOff);
    }
    else if(!on)
    {
      LCD_Sym_Setup_OnValue(pOn); 
      LCD_Sym_Setup_OffValueNeg(pOff);
    }
  }

  switch(Touch_Matrix())
  {
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;MEM_EEPROM_WriteVar(ON_phosphor, pOn);
      MEM_EEPROM_WriteVar(OFF_phosphor, pOff);
      MEM_EEPROM_WriteSetupEntry(ps);
      LCD_Auto_Phosphor_Init(ps);
      ps->page_state->page = SetupPage;
      break;

    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(on) pOn = Basic_LimitDec(pOn, 0);
      else pOff= Basic_LimitDec(pOff, 0);
      break;

    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      if(on) pOn = Basic_LimitAdd(pOn, 60);
      else pOff= Basic_LimitAdd(pOff, 60);
      break;

    // on
    case 0x21:
      if(!touch){on = 1; LCD_Sym_Setup_OffValue(pOff); touch = 5; }
      break;

    // off
    case 0x22:
      if(!touch){on = 0; LCD_Sym_Setup_OnValue(pOn); touch = 5; }
      break;

    // no touch
    case 0x00: if(touch){LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  if(on && touch){ LCD_Sym_Setup_OnValueNeg(pOn); }
  if(!on && touch){ LCD_Sym_Setup_OffValueNeg(pOff); }
}


/* ------------------------------------------------------------------*
 *            setup inflow pump linker
 * ------------------------------------------------------------------*/

void Touch_Setup_InflowPumpLinker(struct PlantState *ps)
{
  static unsigned char t_ipVal[3] = {0, 0, 0};
  static unsigned char iPump = 0;
  static unsigned char sensor = 0;

  static unsigned char cho = 4;
  static unsigned char init = 0;
  static unsigned char touch = 0;

  if(!init)
  {
    init = 1;
    t_ipVal[0] = MEM_EEPROM_ReadVar(T_IP_off_h);
    t_ipVal[1] = MEM_EEPROM_ReadVar(OFF_inflowPump);
    t_ipVal[2] = MEM_EEPROM_ReadVar(ON_inflowPump);
    iPump = MEM_EEPROM_ReadVar(PUMP_inflowPump);
    sensor = MEM_EEPROM_ReadVar(SENSOR_outTank);

    LCD_Sym_Setup_Pump(iPump);
    if(sensor) LCD_WriteAnySymbol(s_29x17, 15, 5, _n_sensor);
    else LCD_WriteAnySymbol(s_29x17, 15, 5, _p_sensor);

    // initialize text and symbols
    LCD_Sym_Setup_InflowPump_Text(0b10110100);
    LCD_Sym_Setup_InflowPump_Values(0b00110100, &t_ipVal[0]);
  }

  switch(Touch_Matrix())
  {
    // off h
    case 0x12:
      if(!touch)
      { 
        cho = 1; touch = 5;
        LCD_Sym_Setup_InflowPump_Text(0b01101001);
        LCD_Sym_Setup_InflowPump_Values(0b01100001, &t_ipVal[0]);
      }
      break;

    // esc
    case 0x13: 
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0; 
      MEM_EEPROM_WriteVar(ON_inflowPump, t_ipVal[2]);
      MEM_EEPROM_WriteVar(OFF_inflowPump, t_ipVal[1]);
      MEM_EEPROM_WriteVar(T_IP_off_h, t_ipVal[0]);
      MEM_EEPROM_WriteVar(PUMP_inflowPump, iPump);
      MEM_EEPROM_WriteVar(SENSOR_outTank, sensor);
      MEM_EEPROM_WriteSetupEntry(ps);
      LCD_Auto_InflowPump_Init(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: 
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(cho == 1){ t_ipVal[0] = Basic_LimitDec(t_ipVal[0], 0); }
      else if(cho == 2){ t_ipVal[1] = Basic_LimitDec(t_ipVal[1], 0); }
      else if(cho ==4){ t_ipVal[2] = Basic_LimitDec(t_ipVal[2], 0); }
      break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      if(cho == 1){ t_ipVal[0] = Basic_LimitAdd(t_ipVal[0], 99); }
      else if(cho == 2){ t_ipVal[1] = Basic_LimitAdd(t_ipVal[1], 59); }
      else if(cho ==4){ t_ipVal[2] = Basic_LimitAdd(t_ipVal[2], 60); }
      break;

    case 0x21:
      if(!touch)
      { 
        cho = 4; touch = 5;
        LCD_Sym_Setup_InflowPump_Text(0b10110100);
        LCD_Sym_Setup_InflowPump_Values(0b00110100, &t_ipVal[0]);
      }
      break;

    // off min
    case 0x22:
      if(!touch)
      {
        cho = 2; touch = 5;
        LCD_Sym_Setup_InflowPump_Text(0b01011010);
        LCD_Sym_Setup_InflowPump_Values(0b01010010, &t_ipVal[0]);
      }
      break;

    // sensor
    case 0x31:
      if(sensor && !touch)
      { 
        touch = 5; sensor = 0;
        LCD_WriteAnySymbol(s_29x17, 15, 5, _p_sensor);
      }
      if(!sensor && !touch)
      { 
        touch = 5; sensor = 1;
        LCD_WriteAnySymbol(s_29x17, 15, 5, _n_sensor);
      }
      break;

    // mammoth pump
    case 0x32: iPump = 0; LCD_Sym_Setup_Pump(0); break;

    // electrical pump
    case 0x33: iPump = 1; LCD_Sym_Setup_Pump(1); break;

    // two electrical pumps
    case 0x34: iPump = 2; LCD_Sym_Setup_Pump(2); break;

    // nothing
    case 0x00: if(touch){ LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  switch(cho)
  {
    // off h, off min, on min
    case 1: LCD_Sym_Setup_InflowPump_Values(cho, &t_ipVal[0]); break;
    case 2: LCD_Sym_Setup_InflowPump_Values(cho, &t_ipVal[0]); break;
    case 4: LCD_Sym_Setup_InflowPump_Values(cho, &t_ipVal[0]); break;
    default: break;
  }

}


/* ------------------------------------------------------------------*
 *            setup calibration linker
 * ------------------------------------------------------------------*/

void Touch_Setup_CalLinker(struct PlantState *ps)
{
  static unsigned char touch = 0;
  int read = 0;
  static int cal = 0;
  static unsigned char iniCal = 0;
  static unsigned char calRedo = 0;
  static unsigned char i = 0;
  static unsigned char openV = 0;

  if(!iniCal)
  {
    i = 0;
    iniCal = 1;
    cal = ((MEM_EEPROM_ReadVar(CAL_H_druck) << 8) | (MEM_EEPROM_ReadVar(CAL_L_druck)));
    calRedo = MEM_EEPROM_ReadVar(CAL_Redo_on);
  }

  read = MPX_Read();
  if(read != 0xFF00)
  {
    i++;

    // print every 10th Time
    if(i > 10)
    {
      i = 0;
      read = read - cal;

      // negative / positive pressure
      if(read < 0){ read = -read; LCD_WriteAnyStringFont(f_6x8_p, 10, 34, "-"); }
      else LCD_ClrSpace(10, 34, 2, 5);
      LCD_WriteAnyValue(f_6x8_p, 3, 10, 40, read);
    }
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: 
      if(!touch)
      { 
        touch = 6;
        LCD_ControlButtons(_setup_neg_sym_esc);
        if(ps->page_state->page == SetupCalPressure){ OUT_Clr_Air(ps); }
        ps->page_state->page = SetupPage;
      } break;

    // okay
    case 0x14: 
      if(!touch && ps->page_state->page == SetupCal)
      { 
        touch = 7;
        LCD_ControlButtons(_setup_neg_sym_ok);
        MEM_EEPROM_WriteSetupEntry(ps);
        MEM_EEPROM_WriteVar(CAL_Redo_on, calRedo);
        if(MEM_EEPROM_ReadVar(SONIC_on) && ps->sonic_state->level_cal)
        {
          MEM_EEPROM_WriteVar(SONIC_L_LV, ps->sonic_state->level_cal & 0x00FF);
          MEM_EEPROM_WriteVar(SONIC_H_LV, ((ps->sonic_state->level_cal >> 8) & 0x00FF));
        }
        else MPX_LevelCal(ps, _save);
        ps->page_state->page = SetupPage;
      } break;

    case 0x23: 
      if(!touch && ps->page_state->page == SetupCal)
      { 
        touch = 8;
        if(!openV){ openV = 1; LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_ventil, 0); PORT_Valve_OpenAll(ps); }
      } break;

    // calibration for setting pressure to zero level
    case 0x24: 
      if(!touch && ps->page_state->page == SetupCal)
      { 
        touch = 4;
        LCD_WriteAnySymbol(s_29x17, 9, 125, _n_cal);

        // calibration try couple of time until it hopefully worked
        cal = 0;
        for(unsigned char i = 0; i < 6; i++)
        {
          // new calibration
          cal = MPX_ReadAverage_UnCal(ps->mpx_state);
          if(!(cal == 0xFF00)) break;
          // wait for next cal
          TCC0_wait_ms(100);
        }

        // write to memory
        MEM_EEPROM_WriteVar(CAL_L_druck, (cal & 0x00FF));
        MEM_EEPROM_WriteVar(CAL_H_druck, ((cal >> 8) & 0x00FF));
      } break;

    // level measure
    case 0x31: 
      if(!touch)
      { 
        touch = 5;
        LCD_WriteAnySymbol(s_29x17, 15,1, _n_level);
        if(openV){ openV = 0; LCD_Write_TextButton(9, 80, TEXT_BUTTON_open_ventil, 1); OUT_Valve_CloseAll(ps); } 
        ps->page_state->page = SetupCalPressure; 
      } break;

    // calibration redo with pressure -> auto zone page
    case 0x34: 
      if(!touch)
      { 
        touch = 3;
        if(!MEM_EEPROM_ReadVar(SONIC_on))
        {
          if(calRedo){ calRedo = 0; LCD_WriteAnySymbol(s_19x19, 15, 130, _p_arrow_redo); }
          else{ calRedo = 1; LCD_WriteAnySymbol(s_19x19, 15, 130, _n_arrow_redo); }
        }
      }
      break;

    // no touch
    case 0x00:
      if(touch)
      {
        if(touch == 4) LCD_WriteAnySymbol(s_29x17, 9,125, _p_cal);
        touch = 0;
      } break;

    // main linker
    case 0x41: ps->page_state->page = AutoPage; break;
    case 0x42: ps->page_state->page = ManualPage; break;
    case 0x43: ps->page_state->page = SetupPage; break;
    case 0x44: ps->page_state->page = DataPage; break;
    default: break;
  }
  if(ps->page_state->page != SetupCal && ps->page_state->page != SetupCalPressure)
  {
    iniCal = 0;
    if(openV){ openV = 0; OUT_Valve_CloseAll(ps); }
  }
}


/* ------------------------------------------------------------------*
 *            setup alarm linker
 * ------------------------------------------------------------------*/

void Touch_Setup_AlarmLinker(struct PlantState *ps)
{
  static unsigned char sensor = 0;
  static unsigned char comp = 0;
  static unsigned char temp = 0;

  static unsigned char init = 0;
  static unsigned char touch = 0;

  if(!init)
  {
    init = 1;
    sensor = MEM_EEPROM_ReadVar(ALARM_sensor);
    comp = MEM_EEPROM_ReadVar(ALARM_comp);
    temp = MEM_EEPROM_ReadVar(ALARM_temp);

    LCD_WriteAnyValue(f_6x8_n, 3, 10,15, temp);

    if(comp) LCD_WriteAnySymbol(s_29x17, 15, 40, _n_compressor);
    else LCD_WriteAnySymbol(s_29x17, 15, 40, _p_compressor);
    if(sensor) LCD_WriteAnySymbol(s_29x17, 15, 0, _n_sensor);
    else LCD_WriteAnySymbol(s_29x17, 15, 0, _p_sensor);
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ALARM_comp, comp);
      MEM_EEPROM_WriteVar(ALARM_sensor, sensor);
      MEM_EEPROM_WriteVar(ALARM_temp, temp);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(touch){ temp = Basic_LimitDec(temp, 15); }
      break;

    // plus
    case 0x24:
      if(!touch){ LCD_ControlButtons(_setup_neg_sym_plus); touch = 4; }
      if(touch){ temp = Basic_LimitAdd(temp, 99); }
      break;

    // sensor
    case 0x31:
      if(!touch)
      {
        touch = 5;
        if(sensor){ sensor = 0; LCD_WriteAnySymbol(s_29x17, 15, 0, _p_sensor); }
        else{ sensor = 1; LCD_WriteAnySymbol(s_29x17, 15, 0, _n_sensor); }
      }
      break;

    // compressor
    case 0x32:
      if(!touch)
      { 
        touch = 5;
        if(comp){ comp = 0; LCD_WriteAnySymbol(s_29x17, 15, 40, _p_compressor); }
        else{ comp = 1; LCD_WriteAnySymbol(s_29x17, 15, 40, _n_compressor); }
      }
      break;

    // no touch
    case 0x00: if(touch){ LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  if(touch){ LCD_WriteAnyValue(f_6x8_n, 3, 10,15, temp); }
}


/* ------------------------------------------------------------------*
 *            setup watch linker
 * ------------------------------------------------------------------*/

void Touch_Setup_WatchLinker(struct PlantState *ps)
{
  static unsigned char dT[5] = {0};
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;
  unsigned char time = 0;

  if(!init)
  {
    init = 1;
    dT[0] = MCP7941_ReadTime(ps->twi_state, TIC_HOUR); TCC0_wait_us(25);
    dT[1] = MCP7941_ReadTime(ps->twi_state, TIC_MIN); TCC0_wait_us(25);
    dT[2] = MCP7941_ReadTime(ps->twi_state, TIC_DATE); TCC0_wait_us(25);
    dT[3] = MCP7941_ReadTime(ps->twi_state, TIC_MONTH); TCC0_wait_us(25);
    dT[4] = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
    LCD_Sym_Setup_Watch_Mark(_n_h, &dT[0]);
    on = 0;
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons2(_setup_neg_sym_esc); } 
      init = 0; ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons2(_setup_neg_sym_ok); }
      init = 0;
      time = (((dT[0]/10) << 4) | (dT[0]%10));
      MCP7941_WriteByte(TIC_HOUR, time);
      time = (((dT[1]/10) << 4) | (dT[1]%10));
      MCP7941_WriteByte(TIC_MIN, time);
      time = (((dT[2]/10) << 4) | (dT[2]%10));
      MCP7941_WriteByte(TIC_DATE, time);
      time = (((dT[3]/10) << 4) | (dT[3]%10));
      MCP7941_WriteByte(TIC_MONTH, time);
      time = (((dT[4]/10) << 4) | (dT[4]%10));
      MCP7941_WriteByte(TIC_YEAR, time);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons2(_setup_neg_sym_plus); }
      switch(on)
      {
        case 0: dT[0] = Basic_LimitAdd(dT[0], 23); break;
        case 1: dT[1] = Basic_LimitAdd(dT[1], 59); break;
        case 2: dT[2] = Basic_LimitAdd(dT[2], 31); break;
        case 3: dT[3] = Basic_LimitAdd(dT[3], 12); break;
        case 4: dT[4] = Basic_LimitAdd(dT[4], 99); break;
        default: break;
      } break;

    // minus
    case 0x34: if(!touch){ touch = 5; LCD_ControlButtons2(_setup_neg_sym_minus); }
      switch(on)
      {
        case 0: dT[0] = Basic_LimitDec(dT[0], 0); break;
        case 1: dT[1] = Basic_LimitDec(dT[1], 0); break;
        case 2: dT[2] = Basic_LimitDec(dT[2], 1); break;
        case 3: dT[3] = Basic_LimitDec(dT[3], 1); break;
        case 4: dT[4] = Basic_LimitDec(dT[4], 0); break;
        default: break;
      } break;

    // hours
    case 0x21: if(!touch){ LCD_Sym_Setup_Watch_Mark(_n_h, &dT[0]); on = 0; touch = 5; } break;

    // min
    case 0x22: if(!touch){ LCD_Sym_Setup_Watch_Mark(_n_min, &dT[0]); on = 1; touch = 5; } break;

    // days
    case 0x31: if(!touch){ LCD_Sym_Setup_Watch_Mark(_n_day, &dT[0]); on = 2; touch = 5; } break;

    // month
    case 0x32: if(!touch){ LCD_Sym_Setup_Watch_Mark(_n_month, &dT[0]); on = 3; touch = 5; } break;

    // year
    case 0x33: if(!touch){ LCD_Sym_Setup_Watch_Mark(_n_year, &dT[0]); on = 4; touch = 5; } break;

    // no touch
    case 0x00: if(touch){ LCD_ControlButtons2(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  // write date time
  if(touch)
  {
    switch(on)
    {
      case 0: LCD_Sym_Setup_Watch_DateTime(_n_h, dT[0]); break;
      case 1: LCD_Sym_Setup_Watch_DateTime(_n_min, dT[1]); break;
      case 2: LCD_Sym_Setup_Watch_DateTime(_n_day, dT[2]); break;
      case 3: LCD_Sym_Setup_Watch_DateTime(_n_month, dT[3]); break;
      case 4: LCD_Sym_Setup_Watch_DateTime(_n_year, dT[4]); break;
      default: break;
    }
  }
}



/* ------------------------------------------------------------------*
 *            setup Zone linker
 * ------------------------------------------------------------------*/

void Touch_Setup_ZoneLinker(struct PlantState *ps)
{
  static int lvO2 = 0;
  static int lvCirc = 0;
  static int sonic = 0;
  static unsigned char onM = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;
  unsigned char h = 0;

  if(!init)
  {
    init = 1;
    lvO2 = (MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | MEM_EEPROM_ReadVar(TANK_L_O2);
    lvCirc = (MEM_EEPROM_ReadVar(TANK_H_Circ) << 8) | MEM_EEPROM_ReadVar(TANK_L_Circ);

    sonic = MEM_EEPROM_ReadVar(SONIC_on);
    if(sonic){ LCD_WriteAnySymbol(s_19x19, 3, 47, _n_sonic); }
    else{ LCD_WriteAnySymbol(s_19x19, 3, 47, _p_sonic); }

    if(!onM)
    { 
      LCD_WriteAnyValue(f_6x8_p, 3, 11, 40, lvCirc);
      LCD_WriteAnyValue(f_6x8_n, 3, 16, 40, lvO2);
      LCD_WriteAnySymbol(s_29x17, 9, 0, _p_air);
      LCD_WriteAnySymbol(s_29x17, 14, 0, _n_setDown);
    }
    else
    { 
      LCD_WriteAnyValue(f_6x8_n, 3, 11, 40, lvCirc);
      LCD_WriteAnyValue(f_6x8_p, 3, 16, 40, lvO2);
      LCD_WriteAnySymbol(s_29x17, 9, 0, _n_air);
      LCD_WriteAnySymbol(s_29x17, 14, 0, _p_setDown); 
    }

    // cm
    LCD_WriteAnyStringFont(f_6x8_p, 11,60,"cm");
    LCD_WriteAnyStringFont(f_6x8_p, 16,60,"cm");
  }

  switch(Touch_Matrix())
  {
    case 0x12: 
      if(!touch)
      { 
        touch = 8;
        if(sonic){ sonic = 0; LCD_WriteAnySymbol(s_19x19, 3, 47, _p_sonic); }
        else{ sonic = 1; LCD_WriteAnySymbol(s_19x19, 3, 47, _n_sonic); }
      } break;

    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons(_setup_neg_sym_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(_setup_neg_sym_ok); }
      init = 0; h = (lvO2 & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_L_O2, h);
      h = ((lvO2 >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_O2, h);
      h = (lvCirc & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_L_Circ, h);
      h = ((lvCirc >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_Circ, h);
      MEM_EEPROM_WriteVar(SONIC_on, sonic);
      MEM_EEPROM_WriteSetupEntry(ps);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(_setup_neg_sym_minus); }
      if(onM) lvCirc = Basic_LimitDec(lvCirc, 0);
      else lvO2 = Basic_LimitDec(lvO2, lvCirc);
      break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(_setup_neg_sym_plus); }
      if(onM) lvCirc = Basic_LimitAdd(lvCirc, lvO2);
      else lvO2 = Basic_LimitAdd(lvO2, 999);
      break;

    // circulate
    case 0x21:
    case 0x22:
      if(!touch)
      {
        onM = 1; touch = 5;
        LCD_WriteAnyValue(f_6x8_p, 3, 16, 40, lvO2);
        LCD_WriteAnySymbol(s_29x17, 9, 0, _n_air);
        LCD_WriteAnySymbol(s_29x17, 14, 0, _p_setDown);
      }
      break;

    // o2
    case 0x31:
    case 0x32:
      if(!touch)
      {
        onM = 0; touch = 5;
        LCD_WriteAnyValue(f_6x8_p, 3, 11, 40, lvCirc);
        LCD_WriteAnySymbol(s_29x17, 9, 0, _p_air);
        LCD_WriteAnySymbol(s_29x17, 14, 0, _n_setDown);
      }
      break;

    // nothing
    case 0x00: if(touch){ LCD_ControlButtons(touch); touch = 0; } break;

    // main linker
    case 0x41: init = 0; ps->page_state->page = AutoPage; break;
    case 0x42: init = 0; ps->page_state->page = ManualPage; break;
    case 0x43: init = 0; ps->page_state->page = SetupPage; break;
    case 0x44: init = 0; ps->page_state->page = DataPage; break;
    default: break;
  }

  // circulate and o2
  if(onM && touch){ LCD_WriteAnyValue(f_6x8_n, 3, 11, 40, lvCirc); }
  if(!onM && touch){ LCD_WriteAnyValue(f_6x8_n, 3, 16, 40, lvO2); }
}


/*-------------------------------------------------------------------*
 *            data linker
 * ------------------------------------------------------------------*/

void Touch_Data_Linker(struct PlantState *ps)
{
  switch(Touch_Matrix())
  {
    // auto
    case 0x21: LCD_Write_TextButton(9, 0, TEXT_BUTTON_auto, 0); ps->page_state->page = DataAuto; break;
    case 0x22: LCD_Write_TextButton(9, 40, TEXT_BUTTON_manual, 0); ps->page_state->page = DataManual; break;
    case 0x23: LCD_Write_TextButton(9, 80, TEXT_BUTTON_setup, 0); ps->page_state->page = DataSetup; break;
    case 0x24: LCD_Write_TextButton(9, 120, TEXT_BUTTON_sonic, 0); ps->page_state->page = DataSonic; break;

    // main linker
    case 0x41: ps->page_state->page = AutoPage; break;
    case 0x42: ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            data auto linker
 * ------------------------------------------------------------------*/

void Touch_Data_AutoLinker(struct PlantState *ps)
{
  static unsigned char mark = 0;
  static unsigned char iData = 0;

  switch(Touch_Matrix())
  {
    // arrow up
    case 0x14:
      if(!mark)
      { 
        mark = 1; 
        if(iData) iData--;
        LCD_WriteAnySymbol(s_19x19, 3, 140, _n_arrow_up);
      } 
      break;

    // arrow down
    case 0x34:
      if(!mark)
      { 
        mark = 2; 
        if(iData < DATA_PAGE_NUM_AUTO) iData++;
        LCD_WriteAnySymbol(s_19x19, 14, 140, _n_arrow_down);
      } 
      break;

    // nothing
    case 0x00:
      if(mark == 1)
      { 
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 3, 140, _p_arrow_up);
        LCD_WriteAutoEntryPage(iData);
      }

      else if(mark == 2)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 14, 140, _p_arrow_down);
        LCD_WriteAutoEntryPage(iData);
      } 
      break;

    // main linker
    case 0x41: iData = 0; ps->page_state->page = AutoPage; break;
    case 0x42: iData = 0; ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: iData = 0; ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: iData = 0; ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            data manual linker
 * ------------------------------------------------------------------*/

void Touch_Data_ManualLinker(struct PlantState *ps)
{
  static unsigned char mark = 0;
  static unsigned char iData = 0;

  switch(Touch_Matrix())
  {

    // arrow up
    case 0x14:
      if(!mark)
      { 
        mark = 1; 
        if(iData) iData--;
        LCD_WriteAnySymbol(s_19x19, 3, 140, _n_arrow_up);
      } 
      break;

    // arrow down
    case 0x34:
      if(!mark)
      { 
        mark = 2; 
        if(iData < DATA_PAGE_NUM_MANUAL) iData++;
        LCD_WriteAnySymbol(s_19x19, 14, 140, _n_arrow_down);
      } 
      break;

    // nothing
    case 0x00:
      if(mark == 1)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 3, 140, _p_arrow_up);
        LCD_WriteManualEntryPage(iData);
      }

      else if(mark == 2)
      { 
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 14, 140, _p_arrow_down);
        LCD_WriteManualEntryPage(iData);
      }   
      break;

    // main linker
    case 0x41: iData = 0; ps->page_state->page = AutoPage; break;
    case 0x42: iData = 0; ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: iData = 0; ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: iData = 0; ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            data setup linker
 * ------------------------------------------------------------------*/

void Touch_Data_SetupLinker(struct PlantState *ps)
{
  static unsigned char mark = 0;
  static unsigned char iData = 0;

  switch(Touch_Matrix())
  {
    // arrow up
    case 0x14:
      if(!mark)
      { 
        mark = 1; 
        if(iData) iData--;
        LCD_WriteAnySymbol(s_19x19, 3, 140, _n_arrow_up);
      } 
      break;

    // arrow down
    case 0x34:
      if(!mark)
      { 
        mark = 2; 
        if(iData < DATA_PAGE_NUM_SETUP) iData++;
        LCD_WriteAnySymbol(s_19x19, 14, 140, _n_arrow_down);
      } 
      break;

    // nothing
    case 0x00:
      if(mark == 1)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 3, 140, _p_arrow_up);
        LCD_WriteSetupEntryPage(iData);
      }

      else if(mark == 2)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 14, 140, _p_arrow_down);
        LCD_WriteSetupEntryPage(iData);
      }
      break;

    // main linker
    case 0x41: iData = 0; ps->page_state->page = AutoPage; break;
    case 0x42: iData = 0; ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: iData = 0; ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: iData = 0; ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            data sonic linker
 * ------------------------------------------------------------------*/

void Touch_Data_SonicLinker(struct PlantState *ps)
{
  static unsigned char touch = 0;

  switch(Touch_Matrix())
  {
    // shot
    case 0x11:
      if(!touch && ps->page_state->page != DataSonicBoot)
      { 
        touch = 1;
        if(ps->page_state->page != DataSonic){ LCD_Sym_Data_Sonic_ClearRecording(ps); }
        LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, 0);
        Sonic_Data_Shot(ps);
        ps->page_state->page = DataSonic;
      }
      break;

    // auto shot
    case 0x21:
      if(!touch && ps->page_state->page != DataSonicBoot)
      { 
        touch = 2;
        LCD_Sym_Data_Sonic_ClearRecording(ps);
        LCD_Sym_Data_Sonic_AutoText();
        Sonic_Query_Temp_Init(ps);
        ps->page_state->page = DataSonicAuto;
      }
      break;

    // bootloader
    case 0x31:
      if(!touch)
      { 
        touch = 3;
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
      if(!touch && ps->page_state->page == DataSonicBoot){ touch = 4; LCD_Write_TextButton(4, 120, TEXT_BUTTON_read, 0); ps->page_state->page = DataSonicBootR; }
      break;

    case 0x24:
      if(!touch && ps->page_state->page == DataSonicBoot){ touch = 5; LCD_Write_TextButton(10, 120, TEXT_BUTTON_write, 0); ps->page_state->page = DataSonicBootW; }
      break;

    // reset buttons
    case 0x00:
      if(touch == 1){ touch = 0; LCD_Write_TextButton(4, 0, TEXT_BUTTON_shot, 1); }
      else if(touch == 2){ touch = 0; LCD_Write_TextButton(10, 0, TEXT_BUTTON_auto, 1); }
      else{ touch = 0; }
      break;

    // main linker
    case 0x41: touch = 0; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = AutoPage; break;
    case 0x42: touch = 0; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = PinManual; LCD_PinPage_Init(ps); break;
    case 0x43: touch = 0; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = PinSetup; LCD_PinPage_Init(ps); break;
    case 0x44: touch = 0; LCD_Sym_Data_Sonic_ClearRecording(ps); ps->page_state->page = DataPage; break;
    default: break;
  }
}


/*-------------------------------------------------------------------*
 *  pin linker
 * ------------------------------------------------------------------*/

void Touch_Pin_Linker(struct PlantState *ps)
{
  static unsigned char touch[12] = {0x00};
  static unsigned char in[4] = {0x00};

  // pin-codes
  unsigned char secret[4] = {2, 5, 8, 0};
  unsigned char compH[4] = {1, 5, 9, 3};
  unsigned char tel1[4] = {0, 0, 0, 0};
  unsigned char tel2[4] = {0, 0, 0, 1};

  unsigned char i = 0;
  static unsigned char cp = 0;
  static struct TelNr nr;

  switch(Touch_Matrix())
  {
    // 1
    case 0x11: 
      if(!touch[1])
      { 
        touch[1] = 0x11;
        if(nr.id){ nr.tel = 1; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(1, cp); in[cp] = 1; cp++; }
      } break;

    // 2
    case 0x12: 
      if(!touch[2])
      { 
        touch[2] = 0x12;
        if(nr.id){ nr.tel = 2; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(2, cp); in[cp] = 2; cp++; }
      } break;

    // 3
    case 0x13: 
      if(!touch[3])
      { 
        touch[3] = 0x13;
        if(nr.id){ nr.tel = 3; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(3, cp); in[cp] = 3; cp++; }
      } break;

    // 4
    case 0x21: 
      if(!touch[4])
      { 
        touch[4] = 0x14;
        if(nr.id){ nr.tel = 4; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(4, cp); in[cp] = 4; cp++; }
      } break;

    // 5
    case 0x22: 
      if(!touch[5])
      { 
        touch[5] = 0x15;
        if(nr.id){ nr.tel = 5; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(5, cp); in[cp] = 5; cp++; }
      } break;

    // 6
    case 0x23: 
      if(!touch[6])
      { 
        touch[6] = 0x16;
        if(nr.id){ nr.tel = 6; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(6, cp); in[cp] = 6; cp++; }
      } break;

    // 7
    case 0x31: 
      if(!touch[7])
      { 
        touch[7] = 0x17;
        if(nr.id){ nr.tel = 7; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(7, cp); in[cp] = 7; cp++; }
      } break;

    // 8
    case 0x32: 
      if(!touch[8])
      { 
        touch[8] = 0x18;
        if(nr.id){ nr.tel = 8; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(8, cp); in[cp] = 8; cp++; }
      } break;

    // 9
    case 0x33: 
      if(!touch[9])
      { 
        touch[9] = 0x19;
        if(nr.id){ nr.tel = 9; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(9, cp); in[cp] = 9; cp++; }
      } break;

    // 0
    case 0x42:
      if(!touch[0])
      { 
        touch[0] = 0x10;
        if(nr.id){ nr.tel = 0; Modem_TelNr(ps, _write, nr); LCD_Sym_Pin_PrintOneTelNumberDigit(nr); nr.pos++; }
        else{ LCD_Sym_Pin_WriteDigit(0, cp); in[cp] = 0; cp++; }
      } break;

    // del
    case 0x43: 
      LCD_nPinButtons(10);
      LCD_Sym_Pin_DelDigits();
      touch[10] = 0x20;
      cp = 0;
      if(nr.id)
      {
        nr.pos = 0;
        Modem_TelNr(ps, _reset, nr);
        LCD_Sym_Pin_PrintWholeTelNumber(ps, nr);
      }
      break;

    // esc
    case 0x41: 
      LCD_nPinButtons(11); 
      cp = 0;
      nr.id = 0; nr.pos = 0;
      ps->page_state->page = DataPage;
      break;

    // okay tel
    case 0x44:
      if(!touch[11] && nr.id)
      {
        touch[11] = 1;
        LCD_Sym_Pin_OkButton(1);
        Modem_TelNr(ps, _save, nr);

        // test new number
        if(SMS_ON){ Modem_SMS(ps, nr, "Hello from your wastewater treatment plant, your number is added."); }
        else{ Modem_Call(ps, nr); }
        nr.id = 0;
        nr.pos = 0;
      }
      break;

    // no touch
    case 0x00:
      // unmark
      for(unsigned char i = 0; i < 11; i++){ if(touch[i]){ LCD_pPinButtons(i); } }
      for(i = 0; i < 10; i++){ touch[i] = 0; }
      if(touch[11]){ touch[11] = 0; LCD_Sym_Pin_Clear(); }
      break;

    default: break;
  }

  if(cp > 3 && !nr.id)
  {
    cp = 0;
    LCD_Sym_Pin_Clear();

    // manual / setup, check secret
    if((in[0] == secret[0]) && (in[1] == secret[1]) && (in[2] == secret[2]) && (in[3] == secret[3]))
    {
      LCD_Sym_Pin_RightMessage();
      switch(ps->page_state->page)
      {
        case PinManual: ps->page_state->page = ManualPage; break;
        case PinSetup: ps->page_state->page = SetupPage; break;
        default: break;
      }
    }

    // reset compressor hours
    else if((in[0] == compH[0]) && (in[1] == compH[1]) && (in[2] == compH[2]) && (in[3] == compH[3]))
    {
      MCP7941_Write_Comp_OpHours(0);
      LCD_Sym_Pin_OpHoursMessage();
    }

    // enter telephone 1
    else if((in[0] == tel1[0]) && (in[1] == tel1[1]) && (in[2] == tel1[2]) && (in[3] == tel1[3]))
    {
      nr.id = 1; nr.pos = 0;
      Modem_TelNr(ps, _init, nr);
      LCD_Sym_Pin_PrintWholeTelNumber(ps, nr);
      LCD_Sym_Pin_OkButton(0);
    }

    // enter telephone 2
    else if((in[0] == tel2[0]) && (in[1] == tel2[1]) && (in[2] == tel2[2]) && (in[3] == tel2[3]))
    {
      nr.id = 2; nr.pos = 0;
      Modem_TelNr(ps, _init, nr);
      LCD_Sym_Pin_PrintWholeTelNumber(ps, nr);
      LCD_Sym_Pin_OkButton(0);
    }

    // wrong 
    else{ LCD_Sym_Pin_WrongMessage(); }
    LCD_Sym_Pin_DelDigits();
  }

  // stop
  if(nr.pos > 14)
  {
    // tel written
    nr.id = 0;
    nr.pos = 0;
    cp = 0;
    LCD_Sym_Pin_Clear();
  }
  if(nr.id) Modem_ReadSLED(PinModem);
}