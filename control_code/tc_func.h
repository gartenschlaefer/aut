/*********************************************************************\
*	Author:			  Christian Walter
*	Project:		  Timer Counter Funktionen
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
	Header zu TC_func.c
* ------------------------------------------------------------------
*	Date:			    21.04.2011
* lastChanges:	08.02.2016
\**********************************************************************/


/* ==================================================================*
 * 						defines
 * ==================================================================*/

#define TC_CAN_MS   100


/* ==================================================================*
 * 						FUNCTIONS - API - Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCC0 - Main Timer
 * ------------------------------------------------------------------*/

void TCC0_WaitMicroSec_Init(int microSec);
void TCC0_WaitMilliSec_Init(int milliSec);
void TCC0_WaitSec_Init(int Sec);
void TCC0_Stop(void);
unsigned char TCC0_Wait_Query(void);

void TCC0_wait_us(int us);
void TCC0_wait_ms(int ms);
void TCC0_wait_sec(int sec);
void TCC0_wait_3s5(void);

void TCC0_Main_Wait(void);
void TCC0_DisplayManual_Wait(void);
void TCC0_DisplaySetup_Wait(void);
void TCC0_DisplayData_Wait(void);
void TCC0_Touch_Wait(void);


/* ------------------------------------------------------------------*
 * 						TCC1 - TWI Timer
 * ------------------------------------------------------------------*/

void TCC1_WaitMilliSec_Init(int milliSec);
unsigned char TCC1_Wait_Query(void);
void TCC1_Stop(void);

/* ------------------------------------------------------------------*
 * 						TCD0 - Touch Timer
 * ------------------------------------------------------------------*/

void TCD0_WaitMilliSec_Init(int milliSec);
void TCD0_WaitSec_Init(int Sec);
unsigned char TCD0_Wait_Query(void);
void TCD0_Stop(void);

/* ------------------------------------------------------------------*
 * 						TCD1 - Safety Timer
 * ------------------------------------------------------------------*/

void TCD1_WaitMilliSec_Init(int milliSec);
unsigned char TCD1_Wait_Query(void);
void TCD1_Stop(void);
unsigned char TCD1_MainAuto_SafetyTC(t_FuncCmd cmd);

/* ------------------------------------------------------------------*
 * 						TCE0 - Error Timer
 * ------------------------------------------------------------------*/

void 			      TCE0_WaitSec_Init(int Sec);
void            TCE0_WaitMilliSec_Init(int milliSec);
unsigned char 	TCE0_Wait_Query	(void);
void 			      TCE0_Stop			  (void);
unsigned char 	TCE0_ErrorTimer			  (t_FuncCmd error);

/* ------------------------------------------------------------------*
 * 						TCE0 - CAN Timer 1
 * ------------------------------------------------------------------*/

void 			      TCE1_WaitMilliSec_Init	(int milliSec);
unsigned char 	TCE1_Wait_Query		(void);
void 			      TCE1_Stop				(void);

/* ------------------------------------------------------------------*
 * 						TCF0 - RunTime
 * ------------------------------------------------------------------*/

void 			    TCF0_WaitSec_Init		  (int Sec);
void 			    TCF0_WaitMilliSec_Init	(int milliSec);
unsigned char TCF0_Wait_Query		    (void);
void 			    TCF0_Stop				      (void);


/* ------------------------------------------------------------------*
 * 						TCF1 - Modem
 * ------------------------------------------------------------------*/

void 			    TCF1_WaitSec_Init		  (int Sec);
void 			    TCF1_WaitMilliSec_Init	(int milliSec);
unsigned char TCF1_Wait_Query		    (void);
void 			    TCF1_Stop				      (void);






/*********************************************************************\
 * End of SL_TC_func.h
\**********************************************************************/
