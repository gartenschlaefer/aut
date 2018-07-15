/*
 */

#include <avr/io.h>
#include "defines.h"
#include "basic.h"
#include "output.h"
#include "tc.h"
#include "can_app.h"

int main(void)
{
  Clock_Init();
  OUT_init();

  // Objects
  struct Gate Gate1;

  // init
  Gate_init(&Gate1);

  t_UScmd state = _readTemp;

  TC0_16MHzWait_msWhile(10);

  while(1)
  {
    //***
    if (DEBUG_FLAG) 
    {
      if (S1_END) SIGLAMP_SET;
      else SIGLAMP_RESET;
    }

    // Gate stuff
    Gate_algorithm(&Gate1);

    // CAN
    state = CAN_RxParser(state);        //CANParser

    switch(state)
    {
      case _readTemp: //state = Sonic_Temp();         break;
      case _wait:                                   break;
      default:                                      break;
    }
  }

  return 0;
}
