/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Watchdog-Funktionen-SourceFile	
* ------------------------------------------------------------------
*	Âµ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Watchdog Funktions
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:										
\**********************************************************************/

#include<avr/io.h>

#include "SL_Watchdog_func.h"


/* ===================================================================*
 * 						FUNCTIONS 
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Watchdog_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Watchdog intialization
 * -------------------------------------------------------------------*/
 
void Watchdog_Init(void)
{
	CCP= CCP_IOREG_gc;						//IO Protection

	WDT.CTRL=	WDT_CEN_bm			|		//Watchdog writeEnable
				WDT_PER_8KCLK_gc	|		//1s open
				WDT_ENABLE_bm;				//Watchdog Enable
}



/*--------------------------------------------------------------------*
 * 	Watchdog_Restart
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Watchdog intialization
 * -------------------------------------------------------------------*/
 
void Watchdog_Restart(void)
{
	WDT_RESET;
}
 
 


/**********************************************************************\
 * END of xmA_Watchdog_func.c
\**********************************************************************/

