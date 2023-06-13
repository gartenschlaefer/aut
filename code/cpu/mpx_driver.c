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
  ;
}


/*-------------------------------------------------------------------*
 *            update function
 * ------------------------------------------------------------------*/

void MPX_Update(struct PlantState *ps)
{
  // check each third frame
  if(ps->frame_counter->frame % 3)
  {
    // read average
    int av = MPX_ReadAverage_Update(ps);

    // new mpx value
    if(!(av == 0xFF00))
    {
      t_page p = ps->page_state->page;

      // limit
      if(p != SetupCal && p != SetupCalPressure)
      {
        if(av < 0){ av = 0; }
        else if(av > 999){ av = 999; }
      }

      // page dependent handling
      switch(p)
      {
        case AutoSetDown: case AutoMud: case AutoPumpOff:
        case AutoCircOn: case AutoAirOn: case AutoZone: case AutoCircOff: case AutoAirOff:
          LCD_Sym_Auto_MPX_AverageValue(av);
          // read tank
          if(p == AutoCircOn || p == AutoAirOn || p == AutoZone){ MPX_ReadTank(ps); }
          // level perc
          if(!ps->settings->settings_zone->sonic_on){ LCD_Sym_Auto_Tank_LevelPerc((bool)(p != AutoPumpOff && p != AutoMud), ps->mpx_state->actual_level_perc); }
          break;

        case ManualMain: case ManualPumpOff_On: case ManualCircOn: case ManualCircOff: case ManualAir: case ManualSetDown: case ManualPumpOff:
        case ManualMud: case ManualCompressor: case ManualPhosphor: case ManualInflowPump: 
          LCD_Sym_Manual_MPX_AverageValue(av);
          break;

        case SetupCal: case SetupCalPressure: LCD_Sym_Setup_Cal_MPX_AverageValue(av); break;
        default: break;  
      }
    }
  }
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

int MPX_ReadCal(struct SettingsCalibration *settings_calibration)
{
  int data = MPX_Read();
  data -= settings_calibration->zero_offset_pressure;
  return data;
}


/*-------------------------------------------------------------------*
 *  MPX read average value: wait until conversion complete and return data
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_Update(struct PlantState *ps)
{
  // read preassure
  ps->mpx_state->mpx_values[ps->mpx_state->mpx_idx] = MPX_ReadCal(ps->settings->settings_calibration);
  ps->mpx_state->mpx_idx++;

  // print
  if(ps->mpx_state->mpx_idx > 9)
  {
    ps->mpx_state->mpx_idx = 0;
    int mpx_sum = 0;
    for(unsigned char a = 0; a < 10; a++){ mpx_sum += ps->mpx_state->mpx_values[a]; }
    int mpx_av = f_round_int(mpx_sum / 10.0);

    // update actual value
    ps->mpx_state->actual_mpx_av = mpx_av;
    return mpx_av;
  }
  return 0xFF00;
}


/* ------------------------------------------------------------------*
 *            uncalibrated average value (for zero calibration)
 * ------------------------------------------------------------------*/

int MPX_UnCal_Average_New(struct MPXState *mpx_state)
{
  int mpx_uncal_av = 0;
  unsigned char num_samples = 4;

  for(unsigned char i = 0; i < num_samples; i++)
  {
    mpx_uncal_av += MPX_Read();
    TCC0_wait_ms(100);
  }
  mpx_uncal_av /= num_samples;
  return mpx_uncal_av;
}


/* ------------------------------------------------------------------*
 *            average water level
 * ------------------------------------------------------------------*/

void MPX_LevelCal_New(struct PlantState *ps)
{
  // clean data
  ps->mpx_state->mpx_idx = 0;
  for(unsigned char a = 0; a < 10; a++){ ps->mpx_state->mpx_values[a] = 0x00; }

  // collect values
  for(unsigned char i = 0; i < 10; i++)
  {
    int av = MPX_ReadAverage_Update(ps);
    if(!(av == 0xFF00))
    {
      if(av < 0){ av = 0; }
      else if(av > 999){ av = 999; }
      ps->settings->settings_calibration->tank_level_min_pressure = av;
      break;
    }
    TCC0_wait_ms(100);
  }
}


/*-------------------------------------------------------------------*
 *            MPX read tank level via pressure
 * ------------------------------------------------------------------*/

void MPX_ReadTank(struct PlantState *ps)
{
  // handles
  t_page p = ps->page_state->page;

  // // disabled read tank (also not use it in pump off and mud cycle)
  if(!ps->settings->settings_circulate->sensor_in_tank)
  {
    // auto zone
    if(p == AutoZone){ ps->page_state->page = AutoCircOn; }
    return;
  }

  // shortcuts
  int zero = ps->settings->settings_calibration->tank_level_min_pressure;

  // calculate level percentage
  int level_perc = ps->mpx_state->actual_mpx_av - zero;
  if(level_perc <= 0){ level_perc = 0; }
  level_perc = ((level_perc * 100) / ps->settings->settings_zone->level_to_set_down);
  ps->mpx_state->actual_level_perc = level_perc;

  // circulate
  if(p == AutoCircOn)
  {
    if(ps->mpx_state->actual_mpx_av >= (zero + ps->settings->settings_zone->level_to_air)){ ps->page_state->page = AutoAirOn; }
  }

  // air
  else if(p == AutoAirOn)
  {
    if(ps->mpx_state->actual_mpx_av >= (zero + ps->settings->settings_zone->level_to_set_down)){ ps->page_state->page = AutoSetDown; }
  }

  // zone
  else if(p == AutoZone)
  {
    if(ps->mpx_state->actual_mpx_av >= (zero + ps->settings->settings_zone->level_to_set_down)){ ps->page_state->page = AutoSetDown; }
    else if(ps->mpx_state->actual_mpx_av >= (zero + ps->settings->settings_zone->level_to_air)){ ps->page_state->page = AutoAirOn; }
    else{ ps->page_state->page = AutoCircOn; }
  }
}