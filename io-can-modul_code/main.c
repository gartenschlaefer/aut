/*
 */

#include <avr/io.h>
#include "defines.h"
#include "basic.h"
#include "output.h"
#include "tc.h"

int main(void)
{
  Clock_Init();
  OUT_init();

  struct Gate Gate1;
  Gate_init(&Gate1);

  TC0_16MHzWait_msWhile(10);

  while(1)
  {
    // Open the Gate
    if(S5_GATE_OPEN && !Gate1.switch_open && !Gate1.end_cycle
       && !Gate1.switch_close)
    {
      OUT_setSignalLamp();
      Gate1.switch_open = 1;
      Gate1.time_wait = 0;
      TC0_16MHzWait_msQuery(_init, 1000);
    }
    else if(!S5_GATE_OPEN && Gate1.switch_open && !Gate1.move_open)
    {
      OUT_resetSignalLamp();
      Gate1.switch_open = 0;
      Gate1.time_wait = 0;
    }
    else if(!S5_GATE_OPEN && Gate1.move_open)
    {
      FU1_RESET_OPEN;
      Gate1.switch_open = 0;
      Gate1.move_open = 0;
      Gate1.end_cycle = 1;
      TC0_16MHzWait_msQuery(_init, 1000);
    }

    // time till moving of the gate
    if(Gate1.switch_open && !Gate1.move_open)
    {
      // 100ms passed
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1.time_wait++;
      }
      if(Gate1.time_wait >= TIME_GATE_TO_MOVE)
      {
        FU1_SET_OPEN;
        Gate1.move_open = 1;
        Gate1.time_wait = 0;
      }
    }


    // Close the Gate
    if(S5_GATE_CLOSE && !Gate1.switch_close && !Gate1.end_cycle
      && !Gate1.switch_open)
    {
      OUT_setSignalLamp();
      Gate1.switch_close = 1;
      Gate1.time_wait = 0;
      TC0_16MHzWait_msQuery(_init, 1000);
    }
    else if(!S5_GATE_CLOSE && Gate1.switch_close && !Gate1.move_close)
    {
      OUT_resetSignalLamp();
      Gate1.switch_close = 0;
      Gate1.time_wait = 0;
    }
    else if(!S5_GATE_CLOSE && Gate1.move_close)
    {
      FU2_RESET_CLOSE;
      Gate1.switch_close = 0;
      Gate1.move_close = 0;
      Gate1.end_cycle = 1;
      TC0_16MHzWait_msQuery(_init, 1000);
    }

    // time till moving of the gate
    if(Gate1.switch_close && !Gate1.move_close)
    {
      // 100ms passed
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1.time_wait++;
      }
      if(Gate1.time_wait >= TIME_GATE_TO_MOVE)
      {
        FU2_SET_CLOSE;
        Gate1.move_close = 1;
        Gate1.time_wait = 0;
      }
    }

    // end of cycle, set lamp off and reset FU on
    if(Gate1.end_cycle == 1)
    {
      // 100ms passed
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1.time_wait++;
      }
      if(Gate1.time_wait >= TIME_GATE_TO_MOVE)
      {
        OUT_resetSignalLamp();
        FU4_SET_RST;
        Gate1.time_wait = 0;
        Gate1.end_cycle = 2;
      }
    }
    // Reset FU off
    else if(Gate1.end_cycle == 2)
    {
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        FU4_RESET_RST;
        Gate1.end_cycle = 3;
      }
    }
    // wait
    else if(Gate1.end_cycle == 3)
    {
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1.time_wait++;
      }
      if(Gate1.time_wait >= TIME_GATE_TO_MOVE)
      {
        Gate1.time_wait = 0;
        Gate1.end_cycle = 0;
      }
    }
  }

  return 0;
}
