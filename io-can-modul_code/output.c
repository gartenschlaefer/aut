/*
 */

#include<avr/io.h>
#include "output.h"
#include "defines.h"
#include "tc.h"


/* ------------------------------------------------------------------*
 * 						Output Init
 * ------------------------------------------------------------------*/

void OUT_init(void)
{
	// Signal lamp
	DDRA |= (1 << PA0);
  // FU
  DDRA |= (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4);
  PORTA = 0x00;

  // S4 - PB6 Manual - PB7 Auto
  // S5 - Gate switch - PB5 close - PB4 open
  // S6 - PB3 Emergency Stop
  // S7 - PB2 Reserved
  DDRB &= ~(  (1 << PB7) | (1 << PB6) |
              (1 << PB5) | (1 << PB4) |
              (1 << PB3) | (1 << PB2) );

  // S1 - PC2 End switch
  // S2 - PC1 End switch
  // S3 - PC0 Reserved
  DDRC &= ~(  (1 << PC0) | (1 << PC1) | (1 << PC2) );

  // Enable pull ups
  PORTB |=  (1 << PB7) | (1 << PB6) |
            (1 << PB5) | (1 << PB4) |
            (1 << PB3) | (1 << PB2);

  PORTC |=  (1 << PC0) | (1 << PC1) | (1 << PC2);

}

/* ------------------------------------------------------------------*
 * 						Signal lamp
 * ------------------------------------------------------------------*/

void OUT_setSignalLamp(void)
{
  PORTA |= (1 << PA0);
}

void OUT_resetSignalLamp(void)
{
	PORTA &= ~(1 << PA0);
}

/* ------------------------------------------------------------------*
 * 						FU Gate Open
 * ------------------------------------------------------------------*/

void OUT_setFuGateOpen(void)
{
  PORTA |= (1 << PA1);
}

void OUT_resetFuGateOpen(void)
{
	PORTA &= ~(1 << PA1);
}

/* ------------------------------------------------------------------*
 * 						Gate Struct Init
 * ------------------------------------------------------------------*/

void Gate_init(struct Gate *MyGate)
{
   MyGate->switch_open = 0;
   MyGate->switch_close = 0;
   MyGate->move_open = 0;
   MyGate->move_close = 0;
   MyGate->end_cycle = 0;
   MyGate->time_wait = 0;
}

/* ------------------------------------------------------------------*
 *            Gate Open Close algorithm
 * ------------------------------------------------------------------*/

void Gate_algorithm(struct Gate *Gate1)
{
// Open the Gate
    if(S5_GATE_OPEN && !Gate1->switch_open && !Gate1->end_cycle
       && !Gate1->switch_close)
    {
      OUT_setSignalLamp();
      Gate1->switch_open = 1;
      Gate1->time_wait = 0;
      TC0_16MHzWait_msQuery(_init, 1000);
    }
    else if(!S5_GATE_OPEN && Gate1->switch_open && !Gate1->move_open)
    {
      OUT_resetSignalLamp();
      Gate1->switch_open = 0;
      Gate1->time_wait = 0;
    }
    else if(!S5_GATE_OPEN && Gate1->move_open)
    {
      FU1_RESET_OPEN;
      Gate1->switch_open = 0;
      Gate1->move_open = 0;
      Gate1->end_cycle = 1;
      TC0_16MHzWait_msQuery(_init, 1000);
    }

    // time till moving of the gate
    if(Gate1->switch_open && !Gate1->move_open)
    {
      // 100ms passed
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1->time_wait++;
      }
      if(Gate1->time_wait >= TIME_GATE_TO_MOVE)
      {
        FU1_SET_OPEN;
        Gate1->move_open = 1;
        Gate1->time_wait = 0;
      }
    }


    // Close the Gate
    if(S5_GATE_CLOSE && !Gate1->switch_close && !Gate1->end_cycle
      && !Gate1->switch_open)
    {
      OUT_setSignalLamp();
      Gate1->switch_close = 1;
      Gate1->time_wait = 0;
      TC0_16MHzWait_msQuery(_init, 1000);
    }
    else if(!S5_GATE_CLOSE && Gate1->switch_close && !Gate1->move_close)
    {
      OUT_resetSignalLamp();
      Gate1->switch_close = 0;
      Gate1->time_wait = 0;
    }
    else if(!S5_GATE_CLOSE && Gate1->move_close)
    {
      FU2_RESET_CLOSE;
      Gate1->switch_close = 0;
      Gate1->move_close = 0;
      Gate1->end_cycle = 1;
      TC0_16MHzWait_msQuery(_init, 1000);
    }

    // time till moving of the gate
    if(Gate1->switch_close && !Gate1->move_close)
    {
      // 100ms passed
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1->time_wait++;
      }
      if(Gate1->time_wait >= TIME_GATE_TO_MOVE)
      {
        FU2_SET_CLOSE;
        Gate1->move_close = 1;
        Gate1->time_wait = 0;
      }
    }

    // end of cycle, set lamp off and reset FU on
    if(Gate1->end_cycle == 1)
    {
      // 100ms passed
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1->time_wait++;
      }
      if(Gate1->time_wait >= TIME_GATE_TO_MOVE)
      {
        OUT_resetSignalLamp();
        FU4_SET_RST;
        Gate1->time_wait = 0;
        Gate1->end_cycle = 2;
      }
    }
    // Reset FU off
    else if(Gate1->end_cycle == 2)
    {
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        FU4_RESET_RST;
        Gate1->end_cycle = 3;
      }
    }
    // wait
    else if(Gate1->end_cycle == 3)
    {
      if(TC0_16MHzWait_msQuery(_exe, 1000))
      {
        Gate1->time_wait++;
      }
      if(Gate1->time_wait >= TIME_GATE_TO_MOVE)
      {
        Gate1->time_wait = 0;
        Gate1->end_cycle = 0;
      }
    }
}











