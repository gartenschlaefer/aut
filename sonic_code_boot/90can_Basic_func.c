/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    90can_Basic_func.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	PORT, Clock, Watchdog, Runtime
* ------------------------------------------------------------------
*	Date:			    12.04.2015
* lastChanges:  12.04.2015
\**********************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>

/* ==================================================================*
 * 						Header
 * ==================================================================*/

#include "90can__defines.h"
#include "90can_Basic_func.h"
#include "90can_TC_func.h"
#include "90can_CAN_func.h"


/* ==================================================================*
 * 						FUNCTIONS
 * ==================================================================*/

void Basic_Init(void)
{
  Watchdog_Init();            //WatchdogInig
  Clock_Init();               //CLKInit
  TC0_Wait_msWhile(5);        //wait
  PORT_Init();                //PortInit
  CAN_Init();                 //CANInit
  TC0_Wait_msWhile(5);        //wait
  TC0_Stop();                 //StopTimer

  CAN_RXMOb_SetAddr(0x01);    //SetCANAdress
}


/* ------------------------------------------------------------------*
 * 						PORT Init
 * ------------------------------------------------------------------*/

void PORT_Init(void)
{
	DDRB |= (1 << PB5);			//Output Sonic Pulse
	DDRD |= (1 << PD4);			//Output STB-CAN
	DDRE |= (1 << PE3);			//Output Enable
	DDRF = 0x00;					  //ADC-All-Inputs

	PORTB &= ~(1 << PB5);		//Pulse-Low
	PORTD &= ~(1 << PD4);		//STB-CAN-Low=HighSpeedCAN
	PORTE |= (1 << PE3);		//Enable-High=DisablePulse
}


/* ------------------------------------------------------------------*
 * 						Clock Init
 * ------------------------------------------------------------------*/

void Clock_Init(void)
{
	cli();						      //Interrupt Disable
	CLKPR = (1 << CLKPCE);	//Change Enable
	CLKPR = 0x00;				    //no Prescaler
}


/* ------------------------------------------------------------------*
 * 						Watchdog Init
 * ------------------------------------------------------------------*/

void Watchdog_Init(void)
{
	WDTCR |=  (1<<WDE) | (1<<WDCE);					      //Write Protection
	WDTCR =   (1<<WDE) | (1<<WDP1) | (1<<WDP2);		//1s Timer
}







/*********************************************************************\
 * End of file
\**********************************************************************/
