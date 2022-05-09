// --
// ADC Setup

#include <avr/io.h>
#include <stddef.h>
#include <avr/pgmspace.h>

#include "adc_func.h"
#include "modem_driver.h"


/* ------------------------------------------------------------------*
 *            ADC - init
 * ------------------------------------------------------------------*/

void ADC_Init(void)
{
  //Calibration
  ADCA.CALL = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
  ADCA.CALH = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));
  ADCB.CALL = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0));
  ADCB.CALH = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1));

  // ADC-A
  ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;
  ADCA.REFCTRL = ADC_REFSEL_AREFA_gc;
  ADCA.PRESCALER = ADC_PRESCALER_DIV16_gc;
  ADCA.CTRLA = ADC_ENABLE_bm;

  // ADC-B
  ADCB.CTRLB = ADC_RESOLUTION_12BIT_gc;
  ADCB.REFCTRL = ADC_REFSEL_AREFA_gc;
  ADCB.PRESCALER = ADC_PRESCALER_DIV8_gc;
  ADCB.CTRLA = ADC_ENABLE_bm;

  // ADC channels init
  ADC_Touch_Ch();
  ADC_MPX_Ch();
  ADC_USV_Ch();
}


/* ------------------------------------------------------------------*
 *            ReadCalibrationByte
 * ------------------------------------------------------------------*/

unsigned char ReadCalibrationByte(unsigned char index)
{
  unsigned char result;

  NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
  result = pgm_read_byte(index);

  // cleanup NVM command register
  NVM_CMD = NVM_CMD_NO_OPERATION_gc;

  return(result);
}


/*-------------------------------------------------------------------*
 *  ADC_Touch_Ch - ADC Channel
 * --------------------------------------------------------------
 *  Set TOP   and BOTTOM  as Output, TOP=1, BOTTOM=0
 *  Set LEFT  and RIGHT   as Input, Read at LEFT (ADC0)
 *  ADC_Init
 * ------------------------------------------------------------------*/

void ADC_Touch_Ch(void)
{
  // left
  ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc | ADC_CH_GAIN_1X_gc;
  ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc;

  // top
  ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc | ADC_CH_GAIN_1X_gc;
  ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc;
}


/*-------------------------------------------------------------------*
 *  ADC_MPX_Ch - ADCB Channel
 * --------------------------------------------------------------
 *  Set up ADC for pressure sensor MPX
 *  MPX -> AD8555 = MPX * Gain -> single-ended PORTB PIN6
 *  Sensitivity:  0.4mV/kPa
 * ------------------------------------------------------------------*/

void ADC_MPX_Ch(void)
{
  // MPX channel
  ADCB.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc | ADC_CH_GAIN_1X_gc;
  ADCB.CH0.MUXCTRL= ADC_CH_MUXPOS_PIN6_gc;
}


/*-------------------------------------------------------------------*
 *  ADC_USV_CH - ADCA Channel
 * --------------------------------------------------------------
 *  Set up ADC for VoltageSupply and USV check
 *  Input-Pin:  PA7 - ADC7 on ADCA
 * ------------------------------------------------------------------*/

void ADC_USV_Ch(void)
{
  // usv channel
  ADCA.CH2.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc | ADC_CH_GAIN_1X_gc;
  ADCA.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN7_gc;
}


/*-------------------------------------------------------------------*
 *  USV - voltage surveillance and error if USV active
 * ------------------------------------------------------------------*/

void ADC_USV_Check(unsigned int *p_c)
{
  int c = *p_c;

  // start conversion
  ADCA.CTRLA |= ADC_CH2START_bm;

  // blocking wait until finished
  while(!(ADCA.INTFLAGS & (1 << ADC_CH2IF_bp)));

  // reset flag
  ADCA.INTFLAGS |= (1 << ADC_CH2IF_bp);

  // data
  int data = ADCA.CH2RES;

  // check if supply voltage is under 24V: ~3110 = 24V
  if(data < 2500)
  {
    c++;
    if(c == 3)  Modem_Alert("Error: USV is active");
    if(c > 720) c = 0;
  }
  else c = 0;

  *p_c = c;
}