/*
 */

#include<avr/io.h>
#include "output.h"

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

	// S5 - Gate switch - PB4 open - PB5 close
  DDRB &= ~(1 << PB4);
  DDRB &= ~(1 << PB5);
  // Enable pull ups
  PORTB |= (1 << PB4) | (1 << PB5);

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










