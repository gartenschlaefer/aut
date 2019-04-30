/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Interception ICT
*	Name:			    AT24C-app-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	HeaderFile for EEPROM AT24C512
* ------------------------------------------------------------------
*	Date:			    30.12.2014
* lastChanges:
\**********************************************************************/

// Include guard
#ifndef AT24C_APP_H   
#define AT24C_APP_H

typedef enum
{
  TEL1_0,   TEL1_1,   TEL1_2,   TEL1_3,
  TEL1_4,   TEL1_5,   TEL1_6,   TEL1_7,
  TEL1_8,   TEL1_9,   TEL1_A,   TEL1_B,
  TEL1_C,   TEL1_D,   TEL1_E,   TEL1_F,   //16var pos:15

  TEL2_0,   TEL2_1,   TEL2_2,   TEL2_3,
  TEL2_4,   TEL2_5,   TEL2_6,   TEL2_7,
  TEL2_8,   TEL2_9,   TEL2_A,  TEL2_B,
  TEL2_C,   TEL2_D,   TEL2_E,   TEL2_F,   //32var pos:31
}t_EEvar;



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void          AT24C_WriteVar(t_EEvar var, unsigned char sData);
unsigned char AT24C_ReadVar (t_EEvar var);

#endif
