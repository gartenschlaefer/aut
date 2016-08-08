/**********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Interception ICT
*	Name:			    ADC-func-Sourcefile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	ADC Setup
* ------------------------------------------------------------------
*	Date:			    21.02.2015
* lastChanges:
\**********************************************************************/


#include <avr/io.h>
#include <stddef.h>
#include <avr/pgmspace.h>

#include "defines.h"
#include "tc_func.h"
#include "adc_func.h"
#include "lcd_driver.h"
#include "modem_driver.h"


/* ==================================================================*
 * 						FUNCTIONS
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						ADC - Init
 * ------------------------------------------------------------------*/

void ADC_Init(void)
{
	ADCA.CALL = ReadCalibrationByte(offsetof(
	  NVM_PROD_SIGNATURES_t, ADCACAL0));	    //Calibration Low
	ADCA.CALH = ReadCalibrationByte(offsetof(
	  NVM_PROD_SIGNATURES_t, ADCACAL1));	    //Calibration High
	ADCB.CALL = ReadCalibrationByte(offsetof(
	  NVM_PROD_SIGNATURES_t, ADCBCAL0));	    //Calibration Low
	ADCB.CALH = ReadCalibrationByte(offsetof(
	  NVM_PROD_SIGNATURES_t, ADCBCAL1));	    //Calibration High

	ADCA.CTRLB=		  ADC_RESOLUTION_12BIT_gc;	//Resolution 12BIT right
	ADCA.REFCTRL= 	ADC_REFSEL_AREFA_gc;		  //ReferenceVoltage
	ADCA.PRESCALER= ADC_PRESCALER_DIV16_gc;		//Prescaler clk/16
	ADCA.CTRLA= 	  ADC_ENABLE_bm;				    //ADC ENABLE

  ADCB.CTRLB=		  ADC_RESOLUTION_12BIT_gc;	//Resolution 12BIT right
  ADCB.REFCTRL= 	ADC_REFSEL_AREFA_gc;			//ReferenceVoltage
  ADCB.PRESCALER= ADC_PRESCALER_DIV8_gc;		//Prescaler-clk/8
  ADCB.CTRLA=     ADC_ENABLE_bm;						//ADC ENABLE

	ADC_Touch_Ch();							//ADC Channel Touch-panel
	ADC_MPX_Ch();								//ADC Channel PressureSensor MPX
  ADC_USV_Ch();               //ADC Channel USVCheck
}



/* ------------------------------------------------------------------*
 * 						ReadCalibrationByte
 * ------------------------------------------------------------------*/

unsigned char ReadCalibrationByte(unsigned char index)
{
	unsigned char result;

	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(index);
	NVM_CMD = NVM_CMD_NO_OPERATION_gc; 			//CleanUp NVM CommandReg

	return(result);
}



/*-------------------------------------------------------------------*
 * 	ADC_Touch_Ch - ADC Channel
 * --------------------------------------------------------------
 *	Set TOP 	and BOTTOM 	as Output, TOP=1, BOTTOM=0
 *	Set LEFT 	and RIGHT 	as Input, Read at LEFT (ADC0)
 *	ADC_Init
 * ------------------------------------------------------------------*/

void ADC_Touch_Ch(void)
{
	ADCA.CH0.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|	//LEFT
						        ADC_CH_GAIN_1X_gc;
	ADCA.CH0.MUXCTRL= ADC_CH_MUXPOS_PIN3_gc;

	ADCA.CH1.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|	//TOP
						        ADC_CH_GAIN_1X_gc;
	ADCA.CH1.MUXCTRL= ADC_CH_MUXPOS_PIN2_gc;
}



/*-------------------------------------------------------------------*
 * 	ADC_MPX_Ch - ADCB Channel
 * --------------------------------------------------------------
 *	Set up ADC for Drucksensor MPX
 * 	MPX -> AD8555 = MPX * Gain -> Singleended PORTB PIN6
 * 	Sensitivity:	0.4mV/kPa
 * ------------------------------------------------------------------*/

void ADC_MPX_Ch(void)
{
	ADCB.CH0.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|	  //MPXS
                    ADC_CH_GAIN_1X_gc;

	ADCB.CH0.MUXCTRL= ADC_CH_MUXPOS_PIN6_gc;
}



/*-------------------------------------------------------------------*
 * 	ADC_USV_CH - ADCA Channel
 * --------------------------------------------------------------
 *	Set up ADC for VoltageSupply and USV check
 *  Input-Pin:  PA7 - ADC7 on ADCA
 * ------------------------------------------------------------------*/

void ADC_USV_Ch(void)
{
	ADCA.CH2.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|   //MPXS
                    ADC_CH_GAIN_1X_gc;

	ADCA.CH2.MUXCTRL= ADC_CH_MUXPOS_PIN7_gc;
}



/*-------------------------------------------------------------------*
 * 	ADC_USV_Check
 * --------------------------------------------------------------
 *	Voltage surveillance and error if USV active
 * ------------------------------------------------------------------*/

void ADC_USV_Check(void)
{
  int data = 0;
  static int c = 0;

  ADCA.CTRLA |= ADC_CH2START_bm;					        //Start Conversion

  while(!(ADCA.INTFLAGS & (1<<ADC_CH2IF_bp)));	  //Wait
  ADCA.INTFLAGS |= (1<<ADC_CH2IF_bp);				      //Reset INT-Flag
  data = ADCA.CH2RES;								              //Data at LEFT
  if(data < 2500)      //~3110 = 24V
  {
    c++;
    if(c == 3)  Modem_CallAllNumbers();
    if(c > 720) c = 0;
  }
  else c = 0;
}




/**********************************************************************\
 * End of file
\**********************************************************************/


