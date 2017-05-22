/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    USART-function-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceFile for USART Communication
* ------------------------------------------------------------------
*	Date:			    17.08.2013
* lastChanges:
\**********************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "usart_func.h"




/* ==================================================================*
 * 						Interrupts
 * ==================================================================*/

ISR(USARTF1_RXC_vect)
{
	USART_Rx_Buffer(_add, USARTF1.DATA);
}



/* ==================================================================*
 * 						FUNCTIONS Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						USART Init
 * ------------------------------------------------------------------*
 *	Description:
 *		DetTime-Modem -> 9,22 ms
 *		115200 8N1  -> 107chars
 *   57600 8N1   ->  54chars
 *   9600 8N1    ->   9chars
 *		8N1	->	8DataBits, noParityBit, 1StopBit
 *		9600bps		Bscale=3 	  Bsel=12
 *		57600bps	Bscale=-3 	Bsel=131
 *		115200bps	Bscale=-4 	Bsel=123
 * ------------------------------------------------------------------*/

void USART_Init(void)
{
	USARTF1.CTRLA = USART_RXCINTLVL_MED_gc;		//RX-Interrupt-Enabled

	USARTF1.CTRLB = USART_RXEN_bm	|					  //RX-enabled
						      USART_TXEN_bm;						//TX-enabled

	USARTF1.CTRLC =	USART_CMODE_ASYNCHRONOUS_gc	|		//AsynchronousMode
						      USART_PMODE_DISABLED_gc		  |		//NoParity
						      //USART_SBMODE_bm			|		//2StopBits
						      USART_CHSIZE_8BIT_gc;				//8Bit

	//*------------------------------------------*
	USARTF1.BAUDCTRLA =	123;								//115200 - Bsel=123
	USARTF1.BAUDCTRLB =	((1<<7) | (4<<4));	//115200 - Bscale=-4
	//------------------------------------------*/
	/*------------------------------------------*
	USARTF1.BAUDCTRLA =	131;								//57600 - Bsel=131
	USARTF1.BAUDCTRLB =	((1<<7) | (3<<4));	//57600 - Bscale=-3
	//------------------------------------------*/
	/*------------------------------------------/
	USARTF1.BAUDCTRLA =	12;									//9600 - Bsel=12
	USARTF1.BAUDCTRLB =	(3<<4);							//9600 - Bscale=3
	//------------------------------------------*/
}


/* ------------------------------------------------------------------*
 * 						USART Read Byte
 * ------------------------------------------------------------------*/

int USART_ReadByte(void)
{
	unsigned char read=0;

	if(USARTF1.STATUS & USART_RXCIF_bm)
	{
		read= USARTF1.DATA;						//ReadDataReg
		USARTF1.STATUS |= USART_RXCIF_bm;		//ResetFlag
		return (0x0100 | read);					//ReturnData
	}

	return 0x0000;				//NoMessage
}


/* ------------------------------------------------------------------*
 * 						USART RX Buffer
 * ------------------------------------------------------------------*/

unsigned char *USART_Rx_Buffer(t_FuncCmd cmd, char c)
{
	static unsigned char rx_buffer[50]={0};
	static unsigned char pos=0;

	if(cmd == _add)
	{
		rx_buffer[pos+1] = c;			//store char
		pos++;
	}

	else if(cmd == _read)
	{
		rx_buffer[0]= pos;
		return &rx_buffer[0];
	}

	else if(cmd == _clear)
	{
		for(pos=0; pos<50; pos++)
		{
			rx_buffer[pos] = 0x00;
		}
		pos=0;
	}

	return &rx_buffer[0];
}


/* ------------------------------------------------------------------*
 * 						Write Byte
 * ------------------------------------------------------------------*/

void USART_WriteByte(char write)
{
	while(!(USARTF1.STATUS & USART_DREIF_bm));	//DataRegEmpty?
	USARTF1_DATA= write;						//ReadDataReg
	while(!(USARTF1.STATUS & USART_TXCIF_bm));	//waitUntilSent
	USARTF1.STATUS |= USART_TXCIF_bm;			//ResetFlag
}


/* ------------------------------------------------------------------*
 * 						Write Stirng
 * ------------------------------------------------------------------*/

void USART_WriteString(char write[])
{
	unsigned char i=0;

	while(write[i]!=0)
	{
		USART_WriteByte(write[i]);
		i++;
	}
}



/**********************************************************************\
 * End of file
\**********************************************************************/
