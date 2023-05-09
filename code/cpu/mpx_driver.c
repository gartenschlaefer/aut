// --
// mpx pressure sensor

#include <avr/io.h>

#include "mpx_driver.h"

#include "lcd_driver.h"
#include "lcd_sym.h"
#include "memory_app.h"
#include "tc_func.h"
#include "error_func.h"
#include "basic_func.h"


/*-------------------------------------------------------------------*
 *            init mpx
 * ------------------------------------------------------------------*/

void MPX_Init(struct PlantState *ps)
{
  ps->mpx_state->level_cal = ((MEM_EEPROM_ReadVar(TANK_H_MinP) << 8) | (MEM_EEPROM_ReadVar(TANK_L_MinP)));
}


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
 *            MPX - read calibrated
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
 *  MPX read average value: wait until conversion complete and return data
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_Update(struct PlantState *ps)
{
  // read preassure
  ps->mpx_state->mpx_values[ps->mpx_state->mpx_count] = MPX_ReadCal();
  ps->mpx_state->mpx_count++;

  // print
  if(ps->mpx_state->mpx_count > 9)
  {
    ps->mpx_state->mpx_count = 0;
    int mpx_sum = 0;
    for(unsigned char a = 0; a < 10; a++){ mpx_sum += ps->mpx_state->mpx_values[a]; }
    int mpx_av = f_round_int(mpx_sum / 10.0);

    // symbols
    LCD_Sym_MPX_AverageValue(ps->page_state->page, mpx_av);
    return mpx_av;
  }
  return 0xFF00;
}


/* ------------------------------------------------------------------*
 *            average not calibrated
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
 *            average water level
 * ------------------------------------------------------------------*/

void MPX_LevelCal_New(struct PlantState *ps)
{
  // clean data
  ps->mpx_state->mpx_count = 0;
  for(unsigned char a = 0; a < 10; a++){ ps->mpx_state->mpx_values[a] = 0x00; }

  // collect values
  for(unsigned char i = 0; i < 10; i++)
  {
    int av = MPX_ReadAverage_Update(ps);
    if(!(av == 0xFF00))
    {
      ps->mpx_state->level_cal = av;
      break;
    }
    TCC0_wait_ms(100);
  }
}


/* ------------------------------------------------------------------*
 *            save to eeprom
 * ------------------------------------------------------------------*/

void MPX_LevelCal_SaveToEEPROM(struct PlantState *ps)
{
  MEM_EEPROM_WriteVar(TANK_L_MinP, ps->mpx_state->level_cal & 0x00FF);
  MEM_EEPROM_WriteVar(TANK_H_MinP, ((ps->mpx_state->level_cal >> 8) & 0x00FF));
}


/*-------------------------------------------------------------------*
 *            MPX read tank level via pressure
 * ------------------------------------------------------------------*/

void MPX_ReadTank(struct PlantState *ps)
{
  // return if Ultrasonic
  if(MEM_EEPROM_ReadVar(SONIC_on)){ return; }

  // handles
  t_page p = ps->page_state->page;

  // disabled read tank (also not use it in pump off and mud cycle)
  if(!(MEM_EEPROM_ReadVar(SENSOR_inTank)) || p == AutoPumpOff || p == AutoMud)
  {
    // manual
    if(p == ManualCircOn || p == ManualCircOff || p == ManualAir){ return; }

    // auto zone
    else if(p == AutoZone){ ps->page_state->page = AutoCircOn; }
    LCD_Sym_MPX_Auto_DisabledLevelMeasure();
    return;
  }

  // read variables
  int hO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | (MEM_EEPROM_ReadVar(TANK_L_O2)));
  int hCirc = ((MEM_EEPROM_ReadVar(TANK_H_Circ) << 8) | (MEM_EEPROM_ReadVar(TANK_L_Circ)));
  int minP = ((MEM_EEPROM_ReadVar(TANK_H_MinP) << 8) | (MEM_EEPROM_ReadVar(TANK_L_MinP)));

  // read pressure
  MPX_LevelCal_New(ps);
  if(p == ManualCircOn){ return; }
  LCD_Sym_MPX_Auto_MbarValue(ps->mpx_state->level_cal);

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
    // save calibration
    MPX_LevelCal_SaveToEEPROM(ps);

    // difference, todo: check this
    if((ps->mpx_state->level_cal < (minP - 5)) || (ps->mpx_state->level_cal > (minP + 5))) { ps->page_state->page = AutoCircOn; }
    if(ps->mpx_state->level_cal >= (hO2 + minP)){ ps->page_state->page = AutoSetDown; }
    if(ps->mpx_state->level_cal >= (hCirc + minP)){ ps->page_state->page = AutoAirOn; }
    else { ps->page_state->page = AutoCircOn; }
  }
}