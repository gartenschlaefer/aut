// --
// mpx pressure sensor

#include <avr/io.h>

#include "mpx_driver.h"

#include "lcd_driver.h"
#include "lcd_sym.h"
#include "memory_app.h"
#include "tc_func.h"
#include "error_func.h"
#include "utils.h"


/*-------------------------------------------------------------------*
 *            init mpx
 * ------------------------------------------------------------------*/

void MPX_Init(struct PlantState *ps)
{
  ps->mpx_state->new_mpx_av_flag = false;
  for(unsigned char a = 0; a < 10; a++){ ps->mpx_state->mpx_values[a] = 0x00; }
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
    int mpx_av = MPX_ReadAverage_Update(ps);

    // new mpx value
    if(!(mpx_av == 0xFF00))
    {
      ps->mpx_state->new_mpx_av_flag = true;
      ps->mpx_state->actual_mpx_av = mpx_av;
    }
    else{ ps->mpx_state->new_mpx_av_flag = false; }
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