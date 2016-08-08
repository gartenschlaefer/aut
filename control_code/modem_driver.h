/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		Interception ICT
*	Name:			  Modem-GC864-driver-HeaderFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	HeaderFile for Telit-GSM-Modem GC864Quad_v2
* ------------------------------------------------------------------
*	Date:			    01.12.2012
* lastChanges:  30.12.2014
\**********************************************************************/


/* ==================================================================*
 * 						Defines
 * ==================================================================*/


/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void Modem_Init(void);

unsigned char Modem_TurnOn  (void);
void          Modem_TurnOff (void);

void Modem_ReadPWR(void);
void Modem_ReadSLED(void);
void Modem_ReadCTS(void);
void Modem_ReadRxD(void);

char Modem_TelNr(t_FuncCmd cmd, TelNr nr);

unsigned char Modem_Call(TelNr nr);
void          Modem_CallAllNumbers(void);

void Modem_SendTest(void);
void Modem_Test(void);





/**********************************************************************\
 * End of modem_driver.h
\**********************************************************************/
