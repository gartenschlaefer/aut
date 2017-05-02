/*
 */

#include<avr/io.h>

/* ------------------------------------------------------------------*
 * 						Output Init
 * ------------------------------------------------------------------*/

void OUT_init(void)
{
	// Signal lamp
	DDRA |= (1 << PA0);
	PORTA &= ~(1 << PA0);

	// S5 - door switch - PB4 open - PB5 close
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
