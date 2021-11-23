/*********************************************************************\
* Author:       Christian Walter
* ------------------------------------------------------------------
* Project:      UltraSonic
* Name:         90can_main.c
* ------------------------------------------------------------------
* �-Controler:  AT90CAN128/32
* Compiler:     avr-gcc (WINAVR 2010)
* Description:
* ------------------------------------------------------------------
* Testprogrmme for Evaluation-Board
* ------------------------------------------------------------------
* Date:         05.10.2011
* lastChanges:
\*********************************************************************/

#include<avr/io.h>

/* ==================================================================*
 *            Header
 * ==================================================================*/

#include "defines.h"
#include "basic_func.h"
#include "can_app.h"
#include "sonic_app.h"


/* ==================================================================*
 *            Main
 * ==================================================================*/

int main(void)
{
  t_UScmd state = _readTemp;            //BeginState
  Basic_Init();                         //InitFunc

  while(1)
  {
    WDT_RESET;                          //WatchdogRestart
    state = Basic_Runtime(state);       //RuntimeTC0
    state = CAN_RxParser(state);        //CANParser

    switch(state)
    {
      case _oneShot:  state = Sonic_OneShot();      break;
      case _5Shots:   state = Sonic_5Shots(_exe);   break;

      case _readTemp: state = Sonic_Temp();         break;
      case _wait:                                   break;
      default:                                      break;
    }
  }
}





/*********************************************************************\
 * End of file
\**********************************************************************/
