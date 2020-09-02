/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		Interception ICT
*	Name:			  Modem-GC864-driver-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	HeaderFile for Telit-GSM-Modem GC864Quad_v2
* ------------------------------------------------------------------
*	Date:			    01.12.2012
* lastChanges:  30.12.2014
\**********************************************************************/

// Include guard
#ifndef MODEM_DRIVER_H
#define MODEM_DRIVER_H

/* ==================================================================*
 * 						Defines
 * ==================================================================*/

#define MO_HANG_UP_TIME   25
#define MO_STARTUP_DELAY  200

#define MO_PW_OFF    (!(PORTF.IN & PIN2_bm))
#define MO_PORT_ON   (PORTF.OUTSET = PIN0_bm)
#define MO_PORT_OFF  (PORTF.OUTCLR = PIN0_bm)

#define CHAR_CR (0x0D)
#define CHAR_LF (0x0A)
#define CHAR_ESC (0x1B)
#define CHAR_CTR_Z (0x1A)


struct Modem {
  unsigned char turned_on;
  unsigned char turn_on_state;
  unsigned char turn_on_error;
  int startup_delay;
};


/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void Modem_init(struct Modem *mo);
void Modem_Port_Init(void);

unsigned char Modem_Check(t_page page, struct Modem *mo);
unsigned char Modem_TurnOn(struct Modem *mo);
void Modem_TurnOff (void);

void Modem_ReadPWR(void);
void Modem_ReadSLED(t_page page);
unsigned char Modem_CTS_ready(void);
void Modem_ReadCTS(void);

char Modem_TelNr(t_FuncCmd cmd, TelNr nr);

unsigned char Modem_Call(TelNr nr);
void Modem_CallAllNumbers(void);

void Modem_SendTest(void);
void Modem_WriteSMS(void);
void Modem_DialNumber(void);
void Modem_GetSoftwareversion(void);
void Modem_Test(void);

#endif
