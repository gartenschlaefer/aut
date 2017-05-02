/*
 */

#include<avr/io.h>
#include<avr/interrupt.h>

/* ------------------------------------------------------------------*
 * 						Clock Init
 * ------------------------------------------------------------------*/

void Clock_Init(void)
{
	cli();						      //Interrupt Disable
	CLKPR = (1 << CLKPCE);	//Change Enable

	CLKPR = 0x00;				    //no Prescaler
	//CLKPR= (1<<CLKPS0);		//clk/2
	//CLKPR= (1<<CLKPS1);		//clk/4
}
