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

int MPX_ReadAverage(struct PlantState *ps, t_FuncCmd cmd)
{
  // clean
  if(cmd == _clean)
  {
    ps->mpx_state->mpx_count = 0;
    for(unsigned char a = 0; a < 10; a++) ps->mpx_state->mpx_values[a] = 0x00;
  }

  // execute
  else if(cmd == _exe)
  {
    // read preassure
    ps->mpx_state->mpx_values[ps->mpx_state->mpx_count] = MPX_ReadCal();
    ps->mpx_state->mpx_count++;

    // print
    if(ps->mpx_state->mpx_count > 9)
    {
      ps->mpx_state->mpx_count = 0;
      int add = 0;
      for(unsigned char a = 0; a < 10; a++) add += ps->mpx_state->mpx_values[a];
      add = add / 10;

      switch(ps->page_state->page)
      {
        case AutoPage:    
          LCD_WriteAnyValue(f_4x6_p, 3, 13,43, add);
          return add;

        case ManualPage: 
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
 *            Average notCalibrated
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_UnCal(struct MPXState *mpx_state)
{
  // collect pressure values
  mpx_state->mpx_values[mpx_state->mpx_count] = MPX_Read();
  mpx_state->mpx_count++;

  // average values
  if(mpx_state->mpx_count > 4)
  {
    int unCal = 0;
    mpx_state->mpx_count = 0;
    for(unsigned char a = 0; a < 5; a++) unCal += mpx_state->mpx_values[a];
    return unCal / 5;
  }

  // failed return message
  return 0xFF00;
}


/* ------------------------------------------------------------------*
 *            Average Waterlevel
 * ------------------------------------------------------------------*/

void MPX_LevelCal(struct PlantState *ps, t_FuncCmd cmd)
{
  switch(cmd)
  {
    // init: read from EEPROM
    case _init:
      ps->mpx_state->level_cal = ((MEM_EEPROM_ReadVar(TANK_H_MinP) << 8) | (MEM_EEPROM_ReadVar(TANK_L_MinP)));
      LCD_WriteAnyValue(f_6x8_p, 3, 17, 40, ps->mpx_state->level_cal);
      break;

    // save to EEPROM
    case _save:
      MEM_EEPROM_WriteVar(TANK_L_MinP, ps->mpx_state->level_cal & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_MinP, ((ps->mpx_state->level_cal >> 8) & 0x00FF));
      break;

    // measure
    case _new:

      // clean data
      MPX_ReadAverage(ps, _clean);

      // try some times
      for(unsigned char i = 0; i < 10; i++)
      {
        int av = MPX_ReadAverage(ps, _exe);
        if(!(av == 0xFF00))
        {
          ps->mpx_state->level_cal = av;
          break;
        }
        TCC0_wait_ms(100);
      }
      break;

    // write
    case _write: LCD_WriteAnyValue(f_6x8_p, 3, 17, 40, ps->mpx_state->level_cal); break;
    default: break;
  }
}


/*-------------------------------------------------------------------*
 *  MPX Read Tank via pressure
 * --------------------------------------------------------------
 *  Reads the position of Water, call only in air times
 *  gives back page to go next, or stay in same
 * ------------------------------------------------------------------*/

void MPX_ReadTank(struct PlantState *ps, t_FuncCmd cmd)
{
  // return if Ultrasonic
  if(MEM_EEPROM_ReadVar(SONIC_on)) return;

  // variables
  int perP = 0;
  t_page p = ps->page_state->page;

  // disabled read tank
  if(!(MEM_EEPROM_ReadVar(SENSOR_inTank)) || p == AutoPumpOff || p == AutoMud)
  {
    // manual
    if(p == ManualCircOn || p == ManualCircOff || p == ManualAir){ LCD_Sym_MPX(_mmbar, perP); }

    // auto zone
    else if(p == AutoZone){ LCD_Sym_MPX(_notav, 0); ps->page_state->page = AutoCircOn; }
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
    MPX_LevelCal(ps, _new);
    if(p == ManualCircOn) return;
    LCD_Sym_MPX(_mbar, ps->mpx_state->level_cal);

    // error
    if(ps->mpx_state->level_cal >= (hO2 + minP))
    {
      ps->mpx_state->error_counter++;
      if(ps->mpx_state->error_counter >= 2){ ps->error_state->pending_err_code |= E_IT; ps->mpx_state->error_counter = 0; }
    }

    // circulate
    if(p == AutoCircOn)
    {
      if(ps->mpx_state->level_cal >= (hCirc + minP)){ ps->page_state->page = AutoAirOn; }
    }

    // air
    else if(p == AutoAirOn)
    {
      if(ps->mpx_state->level_cal >= (hO2 + minP)){ ps->page_state->page = AutoSetDown; }
    }

    // zone
    else if(p == AutoZone)
    {
      // difference, todo: check this
      if((ps->mpx_state->level_cal < (minP - 5)) || (ps->mpx_state->level_cal > (minP + 5))) { ps->page_state->page = AutoCircOn; }

      MPX_LevelCal(ps, _save);
      if(ps->mpx_state->level_cal >= (hO2 + minP)){ ps->page_state->page = AutoSetDown; }
      if(ps->mpx_state->level_cal >= (hCirc + minP)){ ps->page_state->page = AutoAirOn; }
      else { ps->page_state->page = AutoCircOn; }
    }
  }

  // write
  else if(cmd == _write)
  {
    // calculate percentage
    perP = ps->mpx_state->level_cal - minP;
    if(perP <= 0){ perP = 0; }
    perP = ((perP * 100) / hO2);

    //ManualWrite
    if(p == ManualCircOn){ LCD_Sym_MPX(_mmbar, perP); return; }
    LCD_Sym_MPX(_debug, perP);
  }
}