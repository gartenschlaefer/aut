/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			USART-function-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	HeaderFile for USART Communication
* ------------------------------------------------------------------
*	Date:			17.08.2013
* lastChanges:
\**********************************************************************/

// Include guard
#ifndef USART_FUNC_H   
#define USART_FUNC_H

/* ==================================================================*
 * 						Defines
 * ==================================================================*/


/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/


void 	USART_Init			(void);
int 	USART_ReadByte		(void);
void 	USART_WriteByte		(char write);
void 	USART_WriteString	(char write[]);

unsigned char 	*USART_Rx_Buffer(t_FuncCmd cmd, char c);

#endif
