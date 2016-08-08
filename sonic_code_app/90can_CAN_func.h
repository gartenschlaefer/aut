/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    90can_CAN_func.h
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	CAN Functions Header-File
* ------------------------------------------------------------------
*	Date:			    10.10.2011
* lastChanges:  13.08.2015
\**********************************************************************/




/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void CAN_Init(void);
void CAN_ClearMOb(void);
void CAN_RXMOb_Init(void);

unsigned char CAN_TXOK(void);
unsigned char CAN_RXOK(void);

void CAN_TXMOB	(unsigned char *object);
unsigned char *CAN_RXMOB	(void);

void CAN_RXMOb_SetAddr(unsigned char addr);




/*********************************************************************\
 * End of file
\**********************************************************************/
