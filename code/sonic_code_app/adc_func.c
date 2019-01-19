/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    adc_func.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	ADC Functions Source-File
* ------------------------------------------------------------------
*	Date:			    05.12.2012
* lastChanges:  10.04.2015
\**********************************************************************/

#include<avr/io.h>

#include "adc_func.h"


/* ==================================================================*
 * 						FUNCTIONS - Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						ADC Reset
 * ------------------------------------------------------------------*/

void ADC_Reset(void)
{
	if(ADCSRA & (1 << ADIF))  ADCSRA |= (1 << ADIF);		//Reset Flag

	ADCSRA = 0x00;		//ADC disable
	ADCSRB = 0x00;
	ADMUX = 0x00;
}


/* ------------------------------------------------------------------*
 * 						ADC1 Init - SonicReception
 * ------------------------------------------------------------------*/

void ADC_ADC1_Init(void)
{
	DDRF &= ~(1 << DDF1);		  //Input

	ADMUX =	(1 << MUX0)	  |	  //ADC1 - SonicCompare
				  (1 << REFS0)	|	  //AVCC CAP@AREF
				  (1 << ADLAR);		  //left adjusted

	ADCSRA =  (1 << ADEN)	  |	//ADC enable
            (1 << ADPS1)	|	//ADC clk/4
            (1 << ADATE);		//TriggerSource Enable/Freerunning

	ADCSRA |=	(1 << ADSC);		//start first converstion
}


/* ------------------------------------------------------------------*
 * 						ADC1 - TurnOff
 * ------------------------------------------------------------------*/

void ADC_ADC1_TurnOff(void)
{
	ADC_Reset();				    //ADC disable
	DDRF |=   (1 << DDF1);	//PF1(ADC1) Output
	PORTF &= ~(1 << PF1);		//SetLow
}


/* ------------------------------------------------------------------*
 * 						Init ADC3 - Temperature
 * ------------------------------------------------------------------*/

void ADC_ADC3_Init(void)
{
	ADMUX = (1 << MUX0)	  |	  //-
				  (1 << MUX1)	  |	  //ADC3 - Temp
				  (1 << REFS0)	|	  //-
				  (1 << REFS1);		  //Internal-2.56V-CAP@AREF

	ADCSRA = 	(1 << ADEN)	|	  //ADC enable
				    (1 << ADPS1);		//ADC clk/4
}


/* ------------------------------------------------------------------*
 * 						ADC Read Byte
 * ------------------------------------------------------------------*/

unsigned char ADC_ReadByte(void)
{
	unsigned char adc = 0;

	if(ADCSRA & (1 << ADIF))
	{
		adc = ADCL;               //ReadFirstLow
		adc = ADCH;               //ReadHighThen
		ADCSRA |= (1 << ADIF);		//Reset Flag
		return adc;			          //ReturnValue
	}
	return 0;
}


/* ------------------------------------------------------------------*
 * 						ADC Read Byte While
 * ------------------------------------------------------------------*/

unsigned char ADC_ReadByteComplete(void)
{
	unsigned char adc = 0;

	while(!(ADCSRA & (1 << ADIF)));		//wait until ADC complete
  adc = ADCL;                       //ReadFirstLow
  adc = ADCH;                       //ReadHighThen
	ADCSRA |=	(1 << ADIF);				    //Reset Flag
	return adc;
}


/* ------------------------------------------------------------------*
 * 						ADC Read 10 Bit
 * ------------------------------------------------------------------*/

int ADC_Read10Bit(void)
{
	unsigned char adc_L=0;
	unsigned char adc_H=0;
	if(ADCSRA & (1 << ADIF))
	{
		adc_L = ADCL;
		adc_H = ADCH;
		ADCSRA |= (1 << ADIF);				    //Reset Flag
		return ((adc_H << 8) | adc_L);	  //ReturnValue
	}
	return 0x8000;
}






/**********************************************************************\
 * End of adc_func.c
\**********************************************************************/
