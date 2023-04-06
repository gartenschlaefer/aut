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
    yAv = Touch_Y_Cal(((y[0] + y[1]) / 2));
    xAv = Touch_X_Cal(((x[0] + x[1]) / 2));

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
    // manual pages
    case ManualMain: case ManualPumpOff_On:
    case ManualCirc: case ManualCircOff:
    case ManualAir: case ManualSetDown:
    case ManualPumpOff: case ManualMud:
    case ManualCompressor: case ManualPhosphor:
    case ManualInflowPump: Touch_ManualLinker(ps); break;

    // setup pages
    case SetupMain: Touch_SetupLinker(ps); break;
    case SetupCirculate: Touch_SetupCirculateLinker(ps); break;
    case SetupAir: Touch_SetupAirLinker(ps); break;
    case SetupSetDown: Touch_SetupSetDownLinker(ps); break;
    case SetupPumpOff: Touch_SetupPumpOffLinker(ps); break;
    case SetupMud: Touch_SetupMudLinker(ps); break;
    case SetupCompressor: Touch_SetupCompressorLinker(ps); break;
    case SetupPhosphor: Touch_SetupPhosphorLinker(ps); break;
    case SetupInflowPump: Touch_SetupInflowPumpLinker(ps); break;
    case SetupCal:
    case SetupCalPressure: Touch_SetupCalLinker(ps); break;
    case SetupAlarm: Touch_SetupAlarmLinker(ps); break;
    case SetupWatch: Touch_SetupWatchLinker(ps); break;
    case SetupZone: Touch_SetupZoneLinker(ps); break;

    // data pages
    case DataMain: Touch_DataLinker(ps); break;
    case DataAuto: Touch_DataAutoLinker(ps); break;
    case DataManual: Touch_DataManualLinker(ps); break;
    case DataSetup: Touch_DataSetupLinker(ps); break;
    case DataSonic: case DataSonicAuto: case DataSonicBoot: case DataSonicBootR: case DataSonicBootW: Touch_DataSonicLinker(ps); break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *  Touch_AutoLinker
 * ------------------------------------------------------------------*/

void Touch_AutoLinker(struct PlantState *ps)
{
  static unsigned char bug = 0;
  static unsigned char touch = 0;

  switch(Touch_Matrix())
  {
    //------------------------------------------------BacklightON
    case 0x11: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x12: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x13: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x14: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;

    case 0x21:
      if(!touch)
      { 
        touch = 1;
        if(bug == 0) bug = 1;
        else if(bug == 1) bug = 2;
        else bug = 0;
        LCD_Backlight(_on, ps->lcd_backlight);
      }
      break;

    case 0x22: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x23: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;

    case 0x24:
      if(!touch)
      { 
        touch = 1;
        if(bug == 2) bug = 3;
        else if(bug == 3) bug = 4;
        else bug = 0;
        LCD_Backlight(_on, ps->lcd_backlight);
      }
      break;

    case 0x31: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x32: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x33: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;
    case 0x34: LCD_Backlight(_on, ps->lcd_backlight); bug = 0; break;

    // main linker
    case 0x41: 
      LCD_Backlight(_on, ps->lcd_backlight); Error_OFF(ps->lcd_backlight);
      if(bug == 4) bug = 5;
      else bug = 0;
      break;

    // manual
    case 0x42: 
      LCD_Backlight(_on, ps->lcd_backlight); Error_OFF(ps->lcd_backlight); bug = 0;
      LCD_Sym_MarkTextButton(Manual);
      OUT_Valve_AutoClose(ps->page_state->page);
      ps->page_state->page = PinManual; break;

    // setup
    case 0x43: 
      LCD_Backlight(_on, ps->lcd_backlight); Error_OFF(ps->lcd_backlight); bug = 0;
      LCD_Sym_MarkTextButton(Setup);
      OUT_Valve_AutoClose(ps->page_state->page);
      ps->page_state->page = PinSetup; break;

    // data
    case 0x44: 
      LCD_Backlight(_on, ps->lcd_backlight); Error_OFF(ps->lcd_backlight); bug = 0;
      LCD_Sym_MarkTextButton(Data);
      OUT_Valve_AutoClose(ps->page_state->page);
      ps->page_state->page = DataPage; break;

    case 0x00: if(touch) { touch = 0; } break;
    default: break;
  }

  // bug
  if(bug == 5)
  {
    ps->page_state->page_time->min = 0;
    ps->page_state->page_time->sec = 5;
    // *p_min = 0;
    // *p_sec = 5;
    //LCD_Sym_Auto_CountDown(*p_min, *p_min);
    LCD_Sym_Auto_CountDown(ps->page_state->page_time);
    bug = 0;
  }

}



/* ------------------------------------------------------------------*
 *            Manual Linker
 * ------------------------------------------------------------------*/

void Touch_ManualLinker(struct PlantState *ps)
{
  //t_page sel_page = NoPage;

  switch(Touch_Matrix())
  {
    case 0x11: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_circulate); ps->page_state->page = ManualCirc; break;
    case 0x12: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_air); ps->page_state->page = ManualAir; break;
    case 0x13: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_setDown); ps->page_state->page = ManualSetDown; break;
    case 0x14: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_pumpOff); ps->page_state->page = ManualPumpOff; break;

    case 0x21: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_mud); ps->page_state->page = ManualMud; break;
    case 0x22: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_compressor); ps->page_state->page = ManualCompressor; break;
    case 0x23: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_phosphor); ps->page_state->page = ManualPhosphor; break;
    case 0x24: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_Mark_ManualSymbol(sn_inflowPump); ps->page_state->page = ManualInflowPump; break;

    case 0x33: if(ps->page_state->page == ManualPumpOff){ LCD_WriteAnySymbol(s_19x19, 15, 85, n_ok); ps->page_state->page = ManualPumpOff_On; } break;

    // main linker
    case 0x41: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_MarkTextButton(Auto); MEM_EEPROM_WriteManualEntry(0, 0, _write); ps->page_state->page = AutoPage; break;
    case 0x42: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_MarkTextButton(Manual); MEM_EEPROM_WriteManualEntry(0, 0, _write); ps->page_state->page = ManualPage; break;
    case 0x43: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_MarkTextButton(Setup); MEM_EEPROM_WriteManualEntry(0, 0, _write); ps->page_state->page = SetupPage; break;
    case 0x44: LCD_Backlight(_on, ps->lcd_backlight); LCD_Sym_MarkTextButton(Data); MEM_EEPROM_WriteManualEntry(0, 0, _write); ps->page_state->page = DataPage; break;

    default: break;
  }

  // switch(Touch_Matrix())
  // {
  //   case 0x11: sel_page = ManualCirc; break;
  //   case 0x12: sel_page = ManualAir; break;
  //   case 0x13: sel_page = ManualSetDown; break;
  //   case 0x14: sel_page = ManualPumpOff; break;

  //   case 0x21: sel_page = ManualMud; break;
  //   case 0x22: sel_page = ManualCompressor; break;
  //   case 0x23: sel_page = ManualPhosphor; break;
  //   case 0x24: sel_page = ManualInflowPump; break;

  //   case 0x33: if(page == ManualPumpOff){ sel_page = ManualPumpOff_On} break;

  //   // main linker
  //   case 0x41: sel_page = AutoPage
  //   case 0x42: sel_page = ManualPage
  //   case 0x43: sel_page = SetupPage
  //   case 0x44: sel_page = DataPage

  //   default: break;
  // }

  // if(sel_page != NoPage)
  // {
  //   LCD_Backlight(_on, ps->lcd_backlight);
  //   LCD_Sym_Mark_ManualSymbol(sn_circulate);
  // }
}


/*-------------------------------------------------------------------*
 *  setup linker
 * ------------------------------------------------------------------*/

void Touch_SetupLinker(struct PlantState *ps)
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
 *            Setup Circulate Linker
 * ------------------------------------------------------------------*/

void Touch_SetupCirculateLinker(struct PlantState *ps)
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
    if(sensor) Eval_SetupCircSensorMark(1);
    else Eval_SetupCircSensorMark(0);
    Eval_SetupCircTextMark(on, p_circ);
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_circ, circ[0]);
      MEM_EEPROM_WriteVar(OFF_circ, circ[1]);
      MEM_EEPROM_WriteVar(TIME_L_circ, circ[2]);
      MEM_EEPROM_WriteVar(TIME_H_circ, circ[3]);
      MEM_EEPROM_WriteVar(SENSOR_inTank, sensor);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      switch(on)
      {
        // circulate on min / max
        case 0: circ[0] = Eval_SetupMinus(circ[0], 2); break;
        case 1: circ[1] = Eval_SetupMinus(circ[1], 5); break;
        case 2: time1 = Eval_SetupMinus(time1, 0); break;
        default: break;
      } break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      switch(on)
      {
        // circulate on min / max
        case 0: circ[0] = Eval_SetupPlus(circ[0], 30); break;
        case 1: circ[1] = Eval_SetupPlus(circ[1], 60); break;
        case 2: time1 = Eval_SetupPlus(time1, 999); break;
        default: break;
      } break;

    // on
    case 0x21: if(!touch){ on = 0; touch = 5; Eval_SetupCircTextMark(on, p_circ); } break;

    // off
    case 0x22: if(!touch){ on = 1; touch = 5; Eval_SetupCircTextMark(on, p_circ); } break;

    // sensor
    case 0x31: if(!touch){ touch = 5; sensor = 1; Eval_SetupCircSensorMark(1); } break;

    // time
    case 0x32: if(!touch){ touch = 5; sensor = 0; on = 2; Eval_SetupCircSensorMark(0); Eval_SetupCircTextMark(on, p_circ); } break;

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
      case 0: LCD_OnValueNeg(circ[0]); break;
      case 1: LCD_OffValueNeg(circ[1]); break;
      case 2: LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, time1); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            Setup Air Linker
 * ------------------------------------------------------------------*/

void Touch_SetupAirLinker(struct PlantState *ps)
{
  static unsigned char air[4] = {0};
  static int time2 = 0;
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;
  unsigned char *p_air = air;

  if(!init)
  {
    init = 1;
    on = 0;
    air[0] = MEM_EEPROM_ReadVar(ON_air);
    air[1] = MEM_EEPROM_ReadVar(OFF_air);
    air[2] = MEM_EEPROM_ReadVar(TIME_L_air);
    air[3] = MEM_EEPROM_ReadVar(TIME_H_air);
    time2 = ((air[3] << 8) | air[2]);
    Eval_SetupAirTextMark(on, p_air);
    LCD_ClrSpace(15, 39, 3, 31);
    LCD_WriteAnyStringFont(f_6x8_p, 16, 40, "Time:");
  }

  switch(Touch_Matrix())
  { 
    // esc
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_air, air[0]);
      MEM_EEPROM_WriteVar(OFF_air, air[1]);
      MEM_EEPROM_WriteVar(TIME_L_air, air[2]);
      MEM_EEPROM_WriteVar(TIME_H_air, air[3]);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      switch(on)
      {
        // on min, off min, time min
        case 0: air[0] = Eval_SetupMinus(air[0], 10); break;
        case 1: air[1] = Eval_SetupMinus(air[1], 5); break;
        case 2: time2 = Eval_SetupMinus(time2, 0); break;
        default: break;
      } break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      switch(on)
      {
        // on max, off max, time max
        case 0: air[0] = Eval_SetupPlus(air[0], 60); break;
        case 1: air[1] = Eval_SetupPlus(air[1], 55); break;
        case 2: time2 = Eval_SetupPlus(time2, 999); break;
        default: break;
      } break;

    // on
    case 0x21: if(!touch){ on = 0; touch = 5; Eval_SetupAirTextMark(on, p_air); } break;

    // off
    case 0x22: if(!touch){ on = 1; touch = 5; Eval_SetupAirTextMark(on, p_air); } break;

    // time
    case 0x32: if(!touch){ touch = 5; on = 2; Eval_SetupAirTextMark(on, p_air); }
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
      case 0: LCD_OnValueNeg(air[0]); break;
      case 1: LCD_OffValueNeg(air[1]); break;
      case 2: LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, time2); break;
      default: break;
    }
  }
}


/* ------------------------------------------------------------------*
 *            Setup Set-Down Linker
 * ------------------------------------------------------------------*/

void Touch_SetupSetDownLinker(struct PlantState *ps)
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
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(TIME_setDown, setDown);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      setDown = Eval_SetupMinus(setDown, 50); break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      setDown = Eval_SetupPlus(setDown, 90); break;

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
 *            Setup Pump-Off Linker
 * ------------------------------------------------------------------*/

void Touch_SetupPumpOffLinker(struct PlantState *ps)
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

    LCD_OnValueNeg(pumpOn);

    if(!pump){ LCD_WriteAnySymbol(s_29x17, 15, 0, n_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, p_pump); }
    else{ LCD_WriteAnySymbol(s_29x17, 15, 0, p_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, n_pump); }
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_pumpOff, pumpOn);
      MEM_EEPROM_WriteVar(PUMP_pumpOff, pump);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(touch){ pumpOn = Eval_SetupMinus(pumpOn, 5); } break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      if(touch){ pumpOn = Eval_SetupPlus(pumpOn, 60); } break;

    // mammoth pump
    case 0x31: if(!touch){ touch = 5; pump = 0; LCD_WriteAnySymbol(s_29x17, 15, 0, n_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, p_pump); } break;

    // electrical pump
    case 0x32: if(!touch){ touch = 5; pump = 1; LCD_WriteAnySymbol(s_29x17, 15, 0, p_compressor); LCD_WriteAnySymbol(s_19x19, 14, 50, n_pump); } break;

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
  if(touch){ LCD_OnValueNeg(pumpOn); }
}


/* ------------------------------------------------------------------*
 *            Setup Mud Linker
 * ------------------------------------------------------------------*/

void Touch_SetupMudLinker(struct PlantState *ps)
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
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ON_MIN_mud, mudMin);
      MEM_EEPROM_WriteVar(ON_SEC_mud, mudSec);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(onM) mudMin = Eval_SetupMinus(mudMin, 0);
      else mudSec = Eval_SetupMinus(mudSec, 0);
      break;

    // plus
    case 0x24: if(!touch){ LCD_ControlButtons(sn_plus); touch = 4; }
      if(onM) mudMin = Eval_SetupPlus(mudMin, 20);
      else mudSec = Eval_SetupPlus(mudSec, 59);
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
 *            Setup Compressor Linker
 * ------------------------------------------------------------------*/

void Touch_SetupCompressorLinker(struct PlantState *ps)
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
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      l = (druckMax & 0x00FF);
      h = ((druckMax >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(MAX_H_druck, h);
      MEM_EEPROM_WriteVar(MAX_L_druck, l);

      l = (druckMin & 0x00FF);
      h = ((druckMin >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(MIN_H_druck, h);
      MEM_EEPROM_WriteVar(MIN_L_druck, l);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(on) druckMin = Eval_SetupMinus(druckMin, 0);
      else druckMax = Eval_SetupMinus(druckMax, 0); 
      break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      if(on) druckMin = Eval_SetupPlus(druckMin, 999);
      else druckMax = Eval_SetupPlus(druckMax, 999); 
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
 *            Setup Phosphor Linker
 * ------------------------------------------------------------------*/

void Touch_SetupPhosphorLinker(struct PlantState *ps)
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
      LCD_OnValueNeg(pOn); 
      LCD_OffValue(pOff);
    }
    else if(!on)
    {
      LCD_OnValue(pOn); 
      LCD_OffValueNeg(pOff);
    }
  }

  switch(Touch_Matrix())
  {
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;MEM_EEPROM_WriteVar(ON_phosphor, pOn);
      MEM_EEPROM_WriteVar(OFF_phosphor, pOff);
      MEM_EEPROM_WriteSetupEntry();
      LCD_Auto_Phosphor(ps, _init);
      ps->page_state->page = SetupPage;
      break;

    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(on) pOn = Eval_SetupMinus(pOn, 0);
      else pOff= Eval_SetupMinus(pOff, 0);
      break;

    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      if(on) pOn = Eval_SetupPlus(pOn, 60);
      else pOff= Eval_SetupPlus(pOff, 60);
      break;

    // on
    case 0x21:
      if(!touch){on = 1; LCD_OffValue(pOff); touch = 5; }
      break;

    // off
    case 0x22:
      if(!touch){on = 0; LCD_OnValue(pOn); touch = 5; }
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

  if(on && touch){ LCD_OnValueNeg(pOn); }
  if(!on && touch){ LCD_OffValueNeg(pOff); }
}


/* ------------------------------------------------------------------*
 *            Setup Inflow-Pump Linker
 * ------------------------------------------------------------------*/

void Touch_SetupInflowPumpLinker(struct PlantState *ps)
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

    Eval_SetupPumpMark(iPump);
    if(sensor) LCD_WriteAnySymbol(s_29x17, 15, 5, n_sensor);
    else LCD_WriteAnySymbol(s_29x17, 15, 5, p_sensor);

    // initialize text and symbols
    LCD_Setup_IPText(0b10110100);
    LCD_Setup_IPValues(0b00110100, &t_ipVal[0]);
  }

  switch(Touch_Matrix())
  {
    // off h
    case 0x12:
      if(!touch)
      { 
        cho = 1; touch = 5;
        LCD_Setup_IPText(0b01101001);
        LCD_Setup_IPValues(0b01100001, &t_ipVal[0]);
      }
      break;

    // esc
    case 0x13: 
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0; 
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0; 
      MEM_EEPROM_WriteVar(ON_inflowPump, t_ipVal[2]);
      MEM_EEPROM_WriteVar(OFF_inflowPump, t_ipVal[1]);
      MEM_EEPROM_WriteVar(T_IP_off_h, t_ipVal[0]);
      MEM_EEPROM_WriteVar(PUMP_inflowPump, iPump);
      MEM_EEPROM_WriteVar(SENSOR_outTank, sensor);
      MEM_EEPROM_WriteSetupEntry();
      LCD_Auto_InflowPump(ps, _init);
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23: 
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(cho == 1) t_ipVal[0] = Eval_SetupMinus(t_ipVal[0], 0);
      else if(cho == 2) t_ipVal[1] = Eval_SetupMinus(t_ipVal[1], 0);
      else if(cho ==4) t_ipVal[2] = Eval_SetupMinus(t_ipVal[2], 0); 
      break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      if(cho == 1) t_ipVal[0] = Eval_SetupPlus(t_ipVal[0], 99);
      else if(cho == 2) t_ipVal[1] = Eval_SetupPlus(t_ipVal[1], 59);
      else if(cho ==4) t_ipVal[2] = Eval_SetupPlus(t_ipVal[2], 60); 
      break;

    case 0x21:
      if(!touch)
      { 
        cho = 4; touch = 5;
        LCD_Setup_IPText(0b10110100);
        LCD_Setup_IPValues(0b00110100, &t_ipVal[0]);
      }
      break;

    // off min
    case 0x22:
      if(!touch)
      {
        cho = 2; touch = 5;
        LCD_Setup_IPText(0b01011010);
        LCD_Setup_IPValues(0b01010010, &t_ipVal[0]);
      }
      break;

    // sensor
    case 0x31:
      if(sensor && !touch)
      { 
        touch = 5; sensor = 0;
        LCD_WriteAnySymbol(s_29x17, 15, 5, p_sensor);
      }
      if(!sensor && !touch)
      { 
        touch = 5; sensor = 1;
        LCD_WriteAnySymbol(s_29x17, 15, 5, n_sensor);
      }
      break;

    // mammoth pump
    case 0x32: iPump = 0; Eval_SetupPumpMark(0); break;

    // electrical pump
    case 0x33: iPump = 1; Eval_SetupPumpMark(1); break;

    // two electrical pumps
    case 0x34: iPump = 2; Eval_SetupPumpMark(2); break;

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
    case 1: LCD_Setup_IPValues(cho, &t_ipVal[0]); break;
    case 2: LCD_Setup_IPValues(cho, &t_ipVal[0]); break;
    case 4: LCD_Setup_IPValues(cho, &t_ipVal[0]); break;
    default: break;
  }

}


/* ------------------------------------------------------------------*
 *            Setup Cal Linker
 * ------------------------------------------------------------------*/

void Touch_SetupCalLinker(struct PlantState *ps)
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
        LCD_ControlButtons(sn_esc);
        if(ps->page_state->page == SetupCalPressure) OUT_Clr_Air();
        ps->page_state->page = SetupPage;
      } break;

    // okay
    case 0x14: 
      if(!touch && ps->page_state->page == SetupCal)
      { 
        touch = 7;
        LCD_ControlButtons(sn_ok);
        MEM_EEPROM_WriteSetupEntry();
        MEM_EEPROM_WriteVar(CAL_Redo_on, calRedo);
        if(MEM_EEPROM_ReadVar(SONIC_on)) Sonic_LevelCal(_save);
        else MPX_LevelCal(ps, _save);
        ps->page_state->page = SetupPage;
      } break;

    case 0x23: 
      if(!touch && ps->page_state->page == SetupCal)
      { 
        touch = 8;
        if(!openV){ openV = 1; LCD_Write_TextButton(9, 80, OpenV, 0); PORT_Valve_OpenAll(); }
      } break;

    // calibration for setting pressure to zero level
    case 0x24: 
      if(!touch && ps->page_state->page == SetupCal)
      { 
        touch = 4;
        LCD_WriteAnySymbol(s_29x17, 9, 125, n_cal);

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
        LCD_WriteAnySymbol(s_29x17, 15,1, n_level);
        if(openV){ openV = 0; LCD_Write_TextButton(9, 80, OpenV, 1); OUT_CloseAllValves(); } 
        ps->page_state->page = SetupCalPressure; 
      } break;

    // calibration redo with pressure -> Auto Zone page
    case 0x34: 
      if(!touch)
      { 
        touch = 3;
        if(!MEM_EEPROM_ReadVar(SONIC_on))
        {
          if(calRedo){ calRedo = 0; LCD_WriteAnySymbol(s_19x19, 15, 130, p_arrowRedo); }
          else{ calRedo = 1; LCD_WriteAnySymbol(s_19x19, 15, 130, n_arrowRedo); }
        }
      }
      break;

    // no touch
    case 0x00:
      if(touch)
      {
        if(touch == 4) LCD_WriteAnySymbol(s_29x17, 9,125, p_cal);
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
    if(openV){ openV = 0; OUT_CloseAllValves(); }
  }
}


/* ------------------------------------------------------------------*
 *            Setup Alarm Linker
 * ------------------------------------------------------------------*/

void Touch_SetupAlarmLinker(struct PlantState *ps)
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

    if(comp) LCD_WriteAnySymbol(s_29x17, 15, 40, n_compressor);
    else LCD_WriteAnySymbol(s_29x17, 15, 40, p_compressor);
    if(sensor) LCD_WriteAnySymbol(s_29x17, 15, 0, n_sensor);
    else LCD_WriteAnySymbol(s_29x17, 15, 0, p_sensor);
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13:
      if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14:
      if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0;
      MEM_EEPROM_WriteVar(ALARM_comp, comp);
      MEM_EEPROM_WriteVar(ALARM_sensor, sensor);
      MEM_EEPROM_WriteVar(ALARM_temp, temp);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(touch){ temp = Eval_SetupMinus(temp, 15); }
      break;

    // plus
    case 0x24:
      if(!touch){ LCD_ControlButtons(sn_plus); touch = 4; }
      if(touch){ temp = Eval_SetupPlus(temp, 99); }
      break;

    // sensor
    case 0x31:
      if(!touch)
      {
        touch = 5;
        if(sensor){ sensor = 0; LCD_WriteAnySymbol(s_29x17, 15, 0, p_sensor); }
        else{ sensor = 1; LCD_WriteAnySymbol(s_29x17, 15, 0, n_sensor); }
      }
      break;

    // compressor
    case 0x32:
      if(!touch)
      { touch = 5;
        if(comp){ comp = 0; LCD_WriteAnySymbol(s_29x17, 15, 40, p_compressor); }
        else{ comp = 1; LCD_WriteAnySymbol(s_29x17, 15, 40, n_compressor); }
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
 *            Setup Watch Linker
 * ------------------------------------------------------------------*/

void Touch_SetupWatchLinker(struct PlantState *ps)
{
  static unsigned char dT[5] = {0};
  static unsigned char on = 0;
  static unsigned char init = 0;
  static unsigned char touch = 0;
  unsigned char time = 0;

  if(!init)
  {
    init = 1;
    dT[0] = MCP7941_ReadTime(TIC_HOUR); TCC0_wait_us(25);
    dT[1] = MCP7941_ReadTime(TIC_MIN); TCC0_wait_us(25);
    dT[2] = MCP7941_ReadTime(TIC_DATE); TCC0_wait_us(25);
    dT[3] = MCP7941_ReadTime(TIC_MONTH); TCC0_wait_us(25);
    dT[4] = MCP7941_ReadTime(TIC_YEAR);
    Eval_SetupWatchMark(n_h, &dT[0]);
    on = 0;
  }

  switch(Touch_Matrix())
  {
    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons2(sn_esc); } 
      init = 0; ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons2(sn_ok); }
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
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // plus
    case 0x24: if(!touch){ touch = 4; LCD_ControlButtons2(sn_plus); }
      switch(on)
      {
        case 0: dT[0] = Eval_SetupPlus(dT[0], 23); break;
        case 1: dT[1] = Eval_SetupPlus(dT[1], 59); break;
        case 2: dT[2] = Eval_SetupPlus(dT[2], 31); break;
        case 3: dT[3] = Eval_SetupPlus(dT[3], 12); break;
        case 4: dT[4] = Eval_SetupPlus(dT[4], 99); break;
        default: break;
      } break;

    // minus
    case 0x34: if(!touch){ touch = 5; LCD_ControlButtons2(sn_minus); }
      switch(on)
      {
        case 0: dT[0] = Eval_SetupMinus(dT[0], 0); break;
        case 1: dT[1] = Eval_SetupMinus(dT[1], 0); break;
        case 2: dT[2] = Eval_SetupMinus(dT[2], 1); break;
        case 3: dT[3] = Eval_SetupMinus(dT[3], 1); break;
        case 4: dT[4] = Eval_SetupMinus(dT[4], 0); break;
        default: break;
      } break;

    // hours
    case 0x21: if(!touch){ Eval_SetupWatchMark(n_h, &dT[0]); on = 0; touch = 5; } break;

    // min
    case 0x22: if(!touch){ Eval_SetupWatchMark(n_min, &dT[0]); on = 1; touch = 5; } break;

    // days
    case 0x31: if(!touch){ Eval_SetupWatchMark(n_day, &dT[0]); on = 2; touch = 5; } break;

    // month
    case 0x32: if(!touch){ Eval_SetupWatchMark(n_month, &dT[0]); on = 3; touch = 5; } break;

    // year
    case 0x33: if(!touch){ Eval_SetupWatchMark(n_year, &dT[0]); on = 4; touch = 5; } break;

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
      case 0: LCD_DateTime(n_h, dT[0]); break;
      case 1: LCD_DateTime(n_min, dT[1]); break;
      case 2: LCD_DateTime(n_day, dT[2]); break;
      case 3: LCD_DateTime(n_month, dT[3]); break;
      case 4: LCD_DateTime(n_year, dT[4]); break;
      default: break;
    }
  }
}



/* ------------------------------------------------------------------*
 *            Setup Zone Linker
 * ------------------------------------------------------------------*/

void Touch_SetupZoneLinker(struct PlantState *ps)
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
    if(sonic) LCD_WriteAnySymbol(s_19x19, 3, 47, n_sonic);
    else LCD_WriteAnySymbol(s_19x19, 3, 47, p_sonic);

    if(!onM)
    { 
      LCD_WriteAnyValue(f_6x8_p, 3, 11, 40, lvCirc);
      LCD_WriteAnyValue(f_6x8_n, 3, 16, 40, lvO2);
      LCD_WriteAnySymbol(s_29x17, 9, 0, p_air);
      LCD_WriteAnySymbol(s_29x17, 14, 0, n_setDown);
    }
    else
    { 
      LCD_WriteAnyValue(f_6x8_n, 3, 11, 40, lvCirc);
      LCD_WriteAnyValue(f_6x8_p, 3, 16, 40, lvO2);
      LCD_WriteAnySymbol(s_29x17, 9, 0, n_air);
      LCD_WriteAnySymbol(s_29x17, 14, 0, p_setDown); 
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
        if(sonic){ sonic = 0; LCD_WriteAnySymbol(s_19x19, 3, 47, p_sonic); }
        else{ sonic = 1; LCD_WriteAnySymbol(s_19x19, 3, 47, n_sonic); }
      } break;

    // esc
    case 0x13: if(!touch){ touch = 6; LCD_ControlButtons(sn_esc); }
      init = 0;
      ps->page_state->page = SetupPage;
      break;

    // okay
    case 0x14: if(!touch){ touch = 7; LCD_ControlButtons(sn_ok); }
      init = 0; h = (lvO2 & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_L_O2, h);
      h = ((lvO2 >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_O2, h);
      h = (lvCirc & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_L_Circ, h);
      h = ((lvCirc >> 8) & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_Circ, h);
      MEM_EEPROM_WriteVar(SONIC_on, sonic);
      MEM_EEPROM_WriteSetupEntry();
      ps->page_state->page = SetupPage;
      break;

    // minus
    case 0x23:
      if(!touch){ touch = 5; LCD_ControlButtons(sn_minus); }
      if(onM) lvCirc = Eval_SetupMinus(lvCirc, 0);
      else lvO2 = Eval_SetupMinus(lvO2, lvCirc);
      break;

    // plus
    case 0x24:
      if(!touch){ touch = 4; LCD_ControlButtons(sn_plus); }
      if(onM) lvCirc = Eval_SetupPlus(lvCirc, lvO2);
      else lvO2 = Eval_SetupPlus(lvO2, 999);
      break;

    // circulate
    case 0x21:
    case 0x22:
      if(!touch)
      {
        onM = 1; touch = 5;
        LCD_WriteAnyValue(f_6x8_p, 3, 16, 40, lvO2);
        LCD_WriteAnySymbol(s_29x17, 9, 0, n_air);
        LCD_WriteAnySymbol(s_29x17, 14, 0, p_setDown);
      }
      break;

    // o2
    case 0x31:
    case 0x32:
      if(!touch)
      {
        onM = 0; touch = 5;
        LCD_WriteAnyValue(f_6x8_p, 3, 11, 40, lvCirc);
        LCD_WriteAnySymbol(s_29x17, 9, 0, p_air);
        LCD_WriteAnySymbol(s_29x17, 14, 0, n_setDown);
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
 *  Touch_DataLinker
 * ------------------------------------------------------------------*/

void Touch_DataLinker(struct PlantState *ps)
{
  switch(Touch_Matrix())
  {
    // auto
    case 0x21: LCD_Write_TextButton(9, 0, Auto, 0); ps->page_state->page = DataAuto; break;
    case 0x22: LCD_Write_TextButton(9, 40, Manual, 0); ps->page_state->page = DataManual; break;
    case 0x23: LCD_Write_TextButton(9, 80, Setup, 0); ps->page_state->page = DataSetup; break;
    case 0x24: LCD_Write_TextButton(9, 120, Sonic, 0); ps->page_state->page = DataSonic; break;

    // main linker
    case 0x41: ps->page_state->page = AutoPage; break;
    case 0x42: ps->page_state->page = PinManual; break;
    case 0x43: ps->page_state->page = PinSetup; break;
    case 0x44: ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Data Auto Linker
 * ------------------------------------------------------------------*/

void Touch_DataAutoLinker(struct PlantState *ps)
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
        LCD_WriteAnySymbol(s_19x19, 3, 140, n_arrowUp);
      } 
      break;

    // arrow down
    case 0x34:
      if(!mark)
      { 
        mark = 2; 
        if(iData < DATA_PAGE_NUM_AUTO) iData++;
        LCD_WriteAnySymbol(s_19x19, 14, 140, n_arrowDown);
      } 
      break;

    // nothing
    case 0x00:
      if(mark == 1)
      { 
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 3, 140, p_arrowUp);
        LCD_WriteAutoEntryPage(iData);
      }

      else if(mark == 2)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 14, 140, p_arrowDown);
        LCD_WriteAutoEntryPage(iData);
      } 
      break;

    // main linker
    case 0x41: iData = 0; ps->page_state->page = AutoPage; break;
    case 0x42: iData = 0; ps->page_state->page = PinManual; break;
    case 0x43: iData = 0; ps->page_state->page = PinSetup; break;
    case 0x44: iData = 0; ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Data Manual Linker
 * ------------------------------------------------------------------*/

void Touch_DataManualLinker(struct PlantState *ps)
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
        LCD_WriteAnySymbol(s_19x19, 3, 140, n_arrowUp);
      } 
      break;

    // arrow down
    case 0x34:
      if(!mark)
      { 
        mark = 2; 
        if(iData < DATA_PAGE_NUM_MANUAL) iData++;
        LCD_WriteAnySymbol(s_19x19, 14, 140, n_arrowDown);
      } 
      break;

    // nothing
    case 0x00:
      if(mark == 1)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 3, 140, p_arrowUp);
        LCD_WriteManualEntryPage(iData);
      }

      else if(mark == 2)
      { 
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 14, 140, p_arrowDown);
        LCD_WriteManualEntryPage(iData);
      }   
      break;

    // main linker
    case 0x41: iData = 0; ps->page_state->page = AutoPage; break;
    case 0x42: iData = 0; ps->page_state->page = PinManual; break;
    case 0x43: iData = 0; ps->page_state->page = PinSetup; break;
    case 0x44: iData = 0; ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Data Setup Linker
 * ------------------------------------------------------------------*/

void Touch_DataSetupLinker(struct PlantState *ps)
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
        LCD_WriteAnySymbol(s_19x19, 3, 140, n_arrowUp);
      } 
      break;

    // arrow down
    case 0x34:
      if(!mark)
      { 
        mark = 2; 
        if(iData < DATA_PAGE_NUM_SETUP) iData++;
        LCD_WriteAnySymbol(s_19x19, 14, 140, n_arrowDown);
      } 
      break;

    // nothing
    case 0x00:
      if(mark == 1)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 3, 140, p_arrowUp);
        LCD_WriteSetupEntryPage(iData);
      }

      else if(mark == 2)
      {
        mark = 0;
        LCD_WriteAnySymbol(s_19x19, 14, 140, p_arrowDown);
        LCD_WriteSetupEntryPage(iData);
      }
      break;

    // main linker
    case 0x41: iData = 0; ps->page_state->page = AutoPage; break;
    case 0x42: iData = 0; ps->page_state->page = PinManual; break;
    case 0x43: iData = 0; ps->page_state->page = PinSetup; break;
    case 0x44: iData = 0; ps->page_state->page = DataPage; break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Data UltraSonic Linker
 * ------------------------------------------------------------------*/

void Touch_DataSonicLinker(struct PlantState *ps)
{
  static unsigned char touch = 0;

  switch(Touch_Matrix())
  {
    // shot
    case 0x11:
      if(!touch && ps->page_state->page != DataSonicBoot)
      { 
        touch = 1;
        if(ps->page_state->page != DataSonic) LCD_Data_SonicWrite(_clear, 0);
        LCD_Write_TextButton(4, 0, Shot, 0);
        Sonic_Data_Shot();
        ps->page_state->page = DataSonic;
      }
      break;

    // auto shot
    case 0x21:
      if(!touch && ps->page_state->page != DataSonicBoot)
      { 
        touch = 2;
        LCD_Data_SonicWrite(_clear, 0);
        LCD_Data_SonicWrite(_autotext, 0);
        CAN_SonicQuery(_init, _startTemp);
        ps->page_state->page = DataSonicAuto;
      }
      break;

    // bootloader
    case 0x31:
      if(!touch)
      { 
        touch = 3;
        LCD_Data_SonicWrite(_clear, 0);
        LCD_Write_TextButton(16, 0, Boot, 0);
        LCD_Write_TextButton(4, 120, Read, 1);
        LCD_Write_TextButton(10, 120, Write, 1);

        if(ps->page_state->page == DataSonicBoot)
        {
          LCD_Write_TextButton(16, 0, Boot, 1);
          LCD_Data_SonicWrite(_clear, 0);
          Sonic_Data_Boot(_off);
          ps->page_state->page = DataSonic; 
          break;
        }

        Sonic_Data_Boot(_on);
        ps->page_state->page = DataSonicBoot;
      }
      break;

    case 0x14:
      if(!touch && ps->page_state->page == DataSonicBoot){ touch = 4; LCD_Write_TextButton(4, 120, Read, 0); ps->page_state->page = DataSonicBootR; }
      break;

    case 0x24:
      if(!touch && ps->page_state->page == DataSonicBoot){ touch = 5; LCD_Write_TextButton(10, 120, Write, 0); ps->page_state->page = DataSonicBootW; }
      break;

    // reset buttons
    case 0x00:
      if(touch == 1){ touch = 0; LCD_Write_TextButton(4, 0, Shot, 1); }
      else if(touch == 2){ touch = 0; LCD_Write_TextButton(10, 0, Auto, 1); }
      else touch = 0;
      break;

    // main linker
    case 0x41: touch = 0; LCD_Data_SonicWrite(_clear, 0); ps->page_state->page = AutoPage; break;
    case 0x42: touch = 0; LCD_Data_SonicWrite(_clear, 0); ps->page_state->page = PinManual; break;
    case 0x43: touch = 0; LCD_Data_SonicWrite(_clear, 0); ps->page_state->page = PinSetup; break;
    case 0x44: touch = 0; LCD_Data_SonicWrite(_clear, 0); ps->page_state->page = DataPage; break;
    default: break;
  }
}



/* ==================================================================*
 *            Pin Linker
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  Touch_PinLinker
 * ------------------------------------------------------------------*/

void Touch_PinLinker(struct PlantState *ps)
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
        if(nr.id){ nr.tel= 1; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(1, cp); in[cp] = 1; cp++; }
      } break;

    // 2
    case 0x12: 
      if(!touch[2])
      { 
        touch[2] = 0x12;
        if(nr.id){ nr.tel= 2; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(2, cp); in[cp] = 2; cp++; }
      } break;

    // 3
    case 0x13: 
      if(!touch[3])
      { 
        touch[3] = 0x13;
        if(nr.id){ nr.tel= 3; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(3, cp); in[cp] = 3; cp++; }
      } break;

    // 4
    case 0x21: 
      if(!touch[4])
      { 
        touch[4] = 0x14;
        if(nr.id){ nr.tel= 4; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(4, cp); in[cp] = 4; cp++; }
      } break;

    // 5
    case 0x22: 
      if(!touch[5])
      { 
        touch[5] = 0x15;
        if(nr.id){ nr.tel= 5; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(5, cp); in[cp] = 5; cp++; }
      } break;

    // 6
    case 0x23: 
      if(!touch[6])
      { 
        touch[6] = 0x16;
        if(nr.id){ nr.tel= 6; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(6, cp); in[cp] = 6; cp++; }
      } break;

    // 7
    case 0x31: 
      if(!touch[7])
      { 
        touch[7] = 0x17;
        if(nr.id){ nr.tel= 7; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(7, cp); in[cp] = 7; cp++; }
      } break;

    // 8
    case 0x32: 
      if(!touch[8])
      { 
        touch[8] = 0x18;
        if(nr.id){ nr.tel= 8; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(8, cp); in[cp] = 8; cp++; }
      } break;

    // 9
    case 0x33: 
      if(!touch[9])
      { 
        touch[9] = 0x19;
        if(nr.id){ nr.tel= 9; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(9, cp); in[cp] = 9; cp++; }
      } break;

    // 0
    case 0x42:
      if(!touch[0])
      { 
        touch[0] = 0x10;
        if(nr.id){ nr.tel= 0; Modem_TelNr(_write, nr); LCD_Pin_Write(_telnr, nr); nr.pos++; }
        else{ Eval_PinWrite(0, cp); in[cp] = 0; cp++; }
      } break;

    // del
    case 0x43: 
      LCD_nPinButtons(10);
      Eval_PinDel();
      touch[10] = 0x20;
      cp = 0;
      if(nr.id)
      {
        nr.pos = 0;
        Modem_TelNr(_reset, nr);
        LCD_Pin_Write(_write, nr);
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
        LCD_Pin_WriteOK(1);
        Modem_TelNr(_save, nr);

        // test new number
        if(SMS_ON) Modem_SMS(nr, "Hello from your wastewater treatment plant, you are added.");
        else Modem_Call(nr);
        nr.id = 0;
        nr.pos = 0;
      }
      break;

    // no touch
    case 0x00:
      Eval_PinClr(&touch[0]);
      for(i = 0; i < 10; i++) touch[i] = 0;
      if(touch[11]){ touch[11] = 0; LCD_Pin_Write(_clear, nr); }
      break;

    default: break;
  }

  if(cp > 3 && !nr.id)
  {
    cp = 0;
    LCD_Pin_Write(_clear, nr);

    // manual setup
    // check secret
    if((in[0] == secret[0]) && (in[1] == secret[1]) && (in[2] == secret[2]) && (in[3] == secret[3]))
    {
      LCD_Pin_Write(_right, nr);
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
      LCD_Pin_Write(_op, nr);
    }

    // enter telephone 1
    else if((in[0] == tel1[0]) && (in[1] == tel1[1]) && (in[2] == tel1[2]) && (in[3] == tel1[3]))
    {
      nr.id = 1; nr.pos = 0;
      Modem_TelNr(_init, nr);
      LCD_Pin_Write(_write, nr);
      LCD_Pin_WriteOK(0);
    }

    // enter telephone 2
    else if((in[0] == tel2[0]) && (in[1] == tel2[1]) && (in[2] == tel2[2]) && (in[3] == tel2[3]))
    {
      nr.id = 2; nr.pos = 0;
      Modem_TelNr(_init, nr);
      LCD_Pin_Write(_write, nr);
      LCD_Pin_WriteOK(0);
    }

    // wrong 
    else{ LCD_Pin_Write(_wrong, nr); }
    Eval_PinDel();
  }

  // stop
  if(nr.pos > 14)
  {
    // tel written
    nr.id = 0;
    nr.pos = 0;
    cp = 0;
    LCD_Pin_Write(_clear, nr);
  }
  if(nr.id) Modem_ReadSLED(PinModem);
}