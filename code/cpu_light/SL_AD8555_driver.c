/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			AD8555-driver-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Amplifier for Pressure Sensor MPX
* ------------------------------------------------------------------														
*	Date:			25.07.2011  	
* 	lastChanges:											
\**********************************************************************/


#include <avr/io.h>

#include "SL_Define_sym.h"

#include "SL_TC_func.h"
#include "SL_AD8555_driver.h"




/* ===================================================================*
 * 						FUNCTIONS Basics
 * ===================================================================*/

void AD8555_Write_Bit(unsigned char bit)
{
	AD8555_OUTPUT;

	if(!bit)			//LOW Output	
	{
		AD8555_HIGH;
		TCF0_16MHzWaitMicroSec_Init(2);
		while(!TCF0_16MHzWait_Query());
		AD8555_LOW;
	}
	else				//HIGH Output	
	{
		AD8555_HIGH;
		TCF0_16MHzWaitMicroSec_Init(60);
		while(!TCF0_16MHzWait_Query());
		AD8555_LOW;
	}

	TCF0_16MHzWaitMicroSec_Init(20);
	while(!TCF0_16MHzWait_Query());			//wait
}


/* -------------------------------------------------------------------*
 * 						Write Function Code
 * -------------------------------------------------------------------*/

void AD8555_Write_FC_SimFirstGain(void)
{
	unsigned char write=0;
	unsigned char bit=0;
	unsigned char i=0;

	write= (AD8555_FC_SIM<<4) | (AD8555_FC_1GAIN<<2) | (AD8555_FC_DUMMY);
	
	for(i=0; i<6; i++)
	{
		bit= ((write>>i) & 0x01);
		AD8555_Write_Bit(bit);
	}
}

void AD8555_Write_FC_SimSecondGain(void)
{
	unsigned char write=0;
	unsigned char bit=0;
	unsigned char i=0;

	write= (AD8555_FC_SIM<<4) | (AD8555_FC_2GAIN<<2) | (AD8555_FC_DUMMY);
	
	for(i=0; i<6; i++)
	{
		bit= ((write>>i) & 0x01);
		AD8555_Write_Bit(bit);
	}
}


/* -------------------------------------------------------------------*
 * 						Write Data
 * -------------------------------------------------------------------*/

void AD8555_Write_Data_SimFirstGain(void)
{
	unsigned char write=0;
	unsigned char bit=0;
	unsigned char i=0;

	write= AD8555_DATA_1GAIN;
	
	for(i=0; i<8; i++)
	{
		bit= ((write<<i) & 0x80);
		AD8555_Write_Bit(bit);
	}
}

void AD8555_Write_Data_SimSecondGain(void)
{
	unsigned char write=0;
	unsigned char bit=0;
	unsigned char i=0;

	write= AD8555_DATA_2GAIN;
	
	for(i=0; i<8; i++)
	{
		bit= ((write<<i) & 0x80);
		AD8555_Write_Bit(bit);
	}
}


/* -------------------------------------------------------------------*
 * 						Write Start and End Packet
 * -------------------------------------------------------------------*/

void AD8555_Write_StartPacket(void)
{
	int 	start= 0b100000000001;
	int 	i=0;
	int 	bit=0;

	for(i=0; i<12; i++)
	{
		bit= ((start>>i) & 0x01);
		AD8555_Write_Bit(bit);
	}
}

void AD8555_Write_EndPacket(void)
{
	int 			end= 0b011111111110;
	unsigned char 	i=0;
	unsigned char 	bit=0;

	for(i=0; i<12; i++)
	{
		bit= ((end>>i) & 0x01);
		AD8555_Write_Bit(bit);
	}
}




/* ===================================================================*
 * 						APPs
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Set Simulation
 * -------------------------------------------------------------------*/

void AD8555_Init(void)
{
	AD8555_SetSim_FirstGain();
	AD8555_SetSim_SecondGain();
}

void AD8555_SetSim_FirstGain(void)
{
	AD8555_Write_StartPacket();
	AD8555_Write_FC_SimFirstGain();
	AD8555_Write_Data_SimFirstGain();
	AD8555_Write_EndPacket();
}

void AD8555_SetSim_SecondGain(void)
{
	AD8555_Write_StartPacket();
	AD8555_Write_FC_SimSecondGain();
	AD8555_Write_Data_SimSecondGain();
	AD8555_Write_EndPacket();
}




/**********************************************************************\
 * End of SL_AD8555_driver.c
\**********************************************************************/


