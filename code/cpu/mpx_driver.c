// --
// mpx pressure sensor

#include <avr/io.h>

#include "mpx_driver.h"

#include "lcd_driver.h"
#include "lcd_sym.h"
#include "memory_app.h"
#include "tc_func.h"
#include "error_func.h"


/*-------------------------------------------------------------------*
 *  wait until the conversion is complete and return data
 * ------------------------------------------------------------------*/

int MPX_Read(void)
{
  int data = 0;

  // start Conversion
  ADCB.CTRLA |= ADC_CH0START_bm;
  while(!(ADCB.INTFLAGS & (1 << ADC_CH0IF_bp)));

  // reset INT-Flag
  ADCB.INTFLAGS |= (1 << ADC_CH0IF_bp);

  // get data
  data = ADCB.CH0RES;
  data = data >> 2;

  return data;
}


/* ------------------------------------------------------------------*
 *            MPX - Read Calibrated
 * ------------------------------------------------------------------*/

int MPX_ReadCal(void)
{
  int cal = 0;
  int data = 0;

  data = MPX_Read();
  cal = ((MEM_EEPROM_ReadVar(CAL_H_druck) << 8) | (MEM_EEPROM_ReadVar(CAL_L_druck)));
  data = data - cal;
  if(data < 0) data = 0;
  if(data > 999) data = 999;

  return data;
}


/*-------------------------------------------------------------------*
 *  MPX Read Average Value: Wait until Conversion Complete and return Data
 * ------------------------------------------------------------------*/

int MPX_ReadAverage(t_textButtons page, t_FuncCmd cmd)
{
  static unsigned char mpxCount = 0;
  static int mpx[10] = {0x00};
  int add = 0;
  unsigned char a = 0;

  //--------------------------------------------------------Clean
  if(cmd == _clean)
  {
    mpxCount = 0;
    for(a = 0; a < 10; a++) mpx[a] = 0x00;
  }

  //--------------------------------------------------------exe
  else if(cmd == _exe)
  {
    // read preassure
    mpx[mpxCount] = MPX_ReadCal();
    mpxCount++;

    // print
    if(mpxCount > 9)
    {
      mpxCount = 0;
      for(a = 0; a < 10; a++) add = add + mpx[a];
      add = add / 10;

      switch(page)
      {
        case Auto:    
          LCD_WriteAnyValue(f_4x6_p, 3, 13,43, add);
          return add;

        case Manual:  
          LCD_WriteAnyValue(f_6x8_p, 3, 17,42, add);
          return add;

        case Data: return add;
        default: break;
      }
    }
  }
  return 0xFF00;
}


/* ------------------------------------------------------------------*
 *            Average Value only
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_Value(void)
{
  unsigned char i = 0;
  int avAge = 0;

  // clean data
  MPX_ReadAverage(Data, _clean);

  for(i = 0; i < 10; i++)
  {
    avAge = MPX_ReadAverage(Data, _exe);
    TCC0_wait_ms(100);
    if(!(avAge == 0xFF00)) return avAge;
  }

  return avAge;
}


/* ------------------------------------------------------------------*
 *            Average notCalibrated
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_UnCal(void)
{
  static unsigned char mpxCount = 0;
  static int mpx[10] = {0x00};
  int unCal = 0;
  unsigned char a = 0;

  // collect pressure values
  mpx[mpxCount] = MPX_Read();
  mpxCount++;

  // average values
  if(mpxCount > 4)
  {
    mpxCount = 0;
    for(a = 0; a < 5; a++) unCal += mpx[a];
    unCal = unCal / 5;
    return unCal;
  }
  TCC0_wait_ms(100);

  return 0xFF00;
}


/* ------------------------------------------------------------------*
 *            Average UnCal
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_UnCal_Value(void)
{
  unsigned char i = 0;
  int avAge = 0;

  for(i = 0; i < 6; i++)
  {
    // average values
    avAge = MPX_ReadAverage_UnCal();
    TCC0_wait_ms(100);
    if(!(avAge == 0xFF00)) return avAge;
  }
  return avAge;
}


/* ------------------------------------------------------------------*
 *            Average Waterlevel
 * ------------------------------------------------------------------*/

int MPX_LevelCal(t_FuncCmd cmd)
{
  static int level = 0;

  switch(cmd)
  {
    // init: read from EEPROM
    case _init:
      level = ((MEM_EEPROM_ReadVar(TANK_H_MinP) << 8) | (MEM_EEPROM_ReadVar(TANK_L_MinP)));
      LCD_WriteAnyValue(f_6x8_p, 3, 17, 40, level);
      break;

    // save to EEPROM
    case _save:
      MEM_EEPROM_WriteVar(TANK_L_MinP, level & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_MinP, ((level >> 8) & 0x00FF));
      break;

    // measure
    case _new:
      level = MPX_ReadAverage_Value();
      break;

    // write
    case _write: LCD_WriteAnyValue(f_6x8_p, 3, 17, 40, level); break;
    default: break;
  }
  return level;
}


/*-------------------------------------------------------------------*
 *  MPX Read Tank via pressure
 * --------------------------------------------------------------
 *  Reads the position of Water, call only in air times
 *  gives back page to go next, or stay in same
 * ------------------------------------------------------------------*/

void MPX_ReadTank(struct PlantState *ps, t_FuncCmd cmd)
{
  static int mpxAv = 0;
  static unsigned char error = 0;


  // return if Ultrasonic
  if(MEM_EEPROM_ReadVar(SONIC_on)) return;

  // variables
  int perP = 0;
  t_page p = ps->page_state->page;

  // disabled read tank
  if(!(MEM_EEPROM_ReadVar(SENSOR_inTank)) || p == AutoPumpOff || p == AutoMud)
  {
    // manual
    if(p == ManualCirc || p == ManualCircOff || p == ManualAir){ LCD_Sym_MPX(_mmbar, perP); }

    // auto zone
    else if(p == AutoZone){ LCD_Sym_MPX(_notav, 0); ps->page_state->page = AutoCirc; }
    LCD_Sym_MPX(_notav, 0);
    return;
  }

  // read variables
  int hO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | (MEM_EEPROM_ReadVar(TANK_L_O2)));
  int hCirc = ((MEM_EEPROM_ReadVar(TANK_H_Circ) << 8) | (MEM_EEPROM_ReadVar(TANK_L_Circ)));
  int minP = ((MEM_EEPROM_ReadVar(TANK_H_MinP) << 8) | (MEM_EEPROM_ReadVar(TANK_L_MinP)));

  // exe
  if(cmd == _exe)
  {
    // read pressure
    mpxAv = MPX_LevelCal(_new);
    if(p == ManualCirc) return;
    LCD_Sym_MPX(_mbar, mpxAv);

    // error
    if(mpxAv >= (hO2 + minP))
    {
      error++;
      //ps->page_state->page = AutoSetDown;
      if(error >= 2){ ps->error_state->pending_err_code |= E_IT; error = 0; }
    }

    // circulate
    if(p == AutoCirc)
    {
      if(mpxAv >= (hCirc + minP)){ ps->page_state->page = AutoAir; }
    }

    // air
    else if(p == AutoAir)
    {
      if(mpxAv >= (hO2 + minP)){ ps->page_state->page = AutoSetDown; }
    }

    // zone
    else if(p == AutoZone)
    {
      // difference, todo: check this
      if((mpxAv < (minP - 5)) || (mpxAv > (minP + 5))) { ps->page_state->page = AutoCirc; }

      MPX_LevelCal(_save);
      if(mpxAv >= (hO2 + minP)){ ps->page_state->page = AutoSetDown; }
      if(mpxAv >= (hCirc + minP)){ ps->page_state->page = AutoAir; }
      else { ps->page_state->page = AutoCirc; }
    }
  }

  // write
  else if(cmd == _write)
  {
    // calculate percentage
    perP = mpxAv - minP;
    if(perP <= 0){ perP = 0; }
    perP = ((perP * 100) / hO2);

    //ManualWrite
    if(p == ManualCirc){ LCD_Sym_MPX(_mmbar, perP); return; }
    LCD_Sym_MPX(_debug, perP);
  }

  // error
  //else if(cmd == _error) { if(error >= 2) return ErrorMPX; }
  //else if(cmd == _error) { if(error >= 2) ps->error_state->; }
  //return page;
}