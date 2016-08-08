/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    90can_main.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Bootloader Programm
* ------------------------------------------------------------------
*	Date:			    05.10.2011
* lastChanges:
\*********************************************************************/

#include<avr/io.h>

/* ==================================================================*
 * 						Header
 * ==================================================================*/

#include "90can__defines.h"
#include "90can_Basic_func.h"
#include "90can_CAN_app.h"
#include "90can_Boot_app.h"

/* ==================================================================*
 * 						Main
 * ==================================================================*/

int main(void)
{
	t_UScmd state = _wait;               //BeginState
  Basic_Init();                         //InitFunc

	while(1)
	{
		WDT_RESET;                        //WatchdogReset
		state = CAN_RxParser(state);      //CANParser

		switch(state)
		{
			case _program:      state = Boot_Program_App(); break;
			case _readProgram:  state = Boot_Read_App();    break;
			default:									                      break;
		}
	}
}





/*********************************************************************\
 * End of file
\**********************************************************************/
