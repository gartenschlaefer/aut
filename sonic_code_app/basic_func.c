/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    basic_func.c
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

#include "defines.h"
#include "basic_func.h"
#include "tc_func.h"
#include "adc_func.h"
#include "can_func.h"


/* ==================================================================*
 * 						FUNCTIONS
 * ==================================================================*/

void Basic_Init(void)
{
  Watchdog_Init();            //WatchdogInig
  Clock_Init();               //CLKInit
  TC0_16MHzWait_msWhile(5);   //wait
  PORT_Init();                //PortInit
  ADC_ADC1_Init();            //ADCInit
  CAN_Init();                 //CANInit
  TC0_16MHzWait_msWhile(5);   //wait
  TC0_16MHz_Stop();           //StopTimer

  CAN_RXMOb_SetAddr(0x01);              //SetCANAdress
  TC0_16MHzWait_msQuery(_init, 20000);  //RuntimeTimer
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
	//CLKPR= (1<<CLKPS0);		//clk/2
	//CLKPR= (1<<CLKPS1);		//clk/4
}


/* ------------------------------------------------------------------*
 * 						Watchdog Init
 * ------------------------------------------------------------------*/

void Watchdog_Init(void)
{
	WDTCR |=  (1<<WDE) | (1<<WDCE);					      //Write Protection
	WDTCR =   (1<<WDE) | (1<<WDP1) | (1<<WDP2);		//1s Timer
}


/* ------------------------------------------------------------------*
 * 						Runtime
 * ------------------------------------------------------------------*/

t_UScmd Basic_Runtime(t_UScmd state)
{
  if(state == _wait)
  {
    if(TC0_16MHzWait_msQuery(_exe, 20000))
      state = _readTemp;                       //ReadTemp
  }
  return state;
}



/* ===================================================================*
 * 						FUNCTIONS Jump
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Jump to Application
 * -------------------------------------------------------------------*/

void Boot_Jump2App(void)
{
	asm volatile("jmp 0x0000");			//Jump Instruction
}


/* -------------------------------------------------------------------*
 * 						Jump to Bootloader
 * -------------------------------------------------------------------*/

void Boot_Jump2Bootloader(void)
{
	asm volatile("jmp 0x3000");			//Jump Instruction
}




/*********************************************************************\
 * End of file
\**********************************************************************/
