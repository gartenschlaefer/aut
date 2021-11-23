/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Clock-Functions-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	SourceCode of Clock Source Config and Selection for SL
* ------------------------------------------------------------------														
*	Date:			01.06.2011  	
* 	lastChanges:											
\**********************************************************************/

#include<avr/io.h>

#include "SL_Clock_func.h"


/* ===================================================================*
 * 						FUNCTIONS 
 * ===================================================================*/
 
void Clock_Init(void)
{
	//CCP = 0xD8;
	//OSC.XOSCFAIL |=	OSC_XOSCFDEN_bm;
	
	OSC.XOSCCTRL= 	OSC_FRQRANGE_12TO16_gc	|		//Frequenz
					OSC_XOSCSEL_XTAL_16KCLK_gc;		//Start-Up
	
	OSC.CTRL = 		OSC_XOSCEN_bm;					//Enable

	while(!(OSC.STATUS & OSC_XOSCRDY_bm));			//Wait
	
	CCP = 0xD8;										//Protection
	CLK.CTRL = CLK_SCLKSEL_XOSC_gc;					//Selection
	
	//CCP = 0xD8;									//Protection
	//CLK.PSCTRL= CLK_PSADIV_2_gc;					//Division by two
}



/**********************************************************************\
 * End of SL_Clock_func.c
\**********************************************************************/
