/*********************************************************************\
*	Author:			Red_Calcifer
*	Projekt:		Timer Counter Funktionen
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Header zu TC_func.c
* ------------------------------------------------------------------														
*	Date:			21.04.2011  	
* 	lastChanges:	27.07.2011											
\**********************************************************************/



/* ===================================================================*
 * 						FUNCTIONS 16MHz
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Main Timer TCC0
 * -------------------------------------------------------------------*/

void 			TCC0_16MHzWaitMicroSec_Init	(int microSec);
void 			TCC0_16MHzWaitMilliSec_Init	(int milliSec);
void 			TCC0_16MHzWaitSec_Init		(int Sec);
unsigned char 	TCC0_16MHzWait_Query		(void);
void 			TCC0_16MHz_Stop				(void);


/* -------------------------------------------------------------------*
 * 						TWI Timer TCC1
 * -------------------------------------------------------------------*/

void 			TCC1_16MHzWaitMilliSec_Init	(int milliSec);
unsigned char 	TCC1_16MHzWait_Query		(void);
void 			TCC1_16MHz_Stop				(void);


/* -------------------------------------------------------------------*
 * 						Touch Timer TCD0
 * -------------------------------------------------------------------*/

void 			TCD0_16MHzWaitMilliSec_Init	(int milliSec);
void 			TCD0_16MHzWaitSec_Init		(int Sec);
unsigned char 	TCD0_16MHzWait_Query		(void);
void 			TCD0_16MHz_Stop				(void);

/* -------------------------------------------------------------------*
 * 						DS1302 Timer TCD1
 * -------------------------------------------------------------------*/

void			TCD1_16MHzWaitMicroSec_Init	(int microSec);
unsigned char 	TCD1_16MHzWait_Query		(void);
void 			TCD1_16MHz_Stop				(void);


/* -------------------------------------------------------------------*
 * 						Output Timer TCE0
 * -------------------------------------------------------------------*/

void 			TCE0_16MHzWaitSec_Init		(int Sec);
void 			TCE0_16MHzWaitMilliSec_Init	(int milliSec);
unsigned char 	TCE0_16MHzWait_Query		(void);
void 			TCE0_16MHz_Stop				(void);


/* -------------------------------------------------------------------*
 * 						Display Timer TCE1
 * -------------------------------------------------------------------*/

void 			TCE1_16MHzWaitMilliSec_Init	(int milliSec);
unsigned char 	TCE1_16MHzWait_Query		(void);
void 			TCE1_16MHz_Stop				(void);


/* -------------------------------------------------------------------*
 * 						AD8555 Timer TCF0
 * -------------------------------------------------------------------*/

void 			TCF0_16MHzWaitMicroSec_Init	(int microSec);
unsigned char 	TCF0_16MHzWait_Query		(void);
void 			TCF0_16MHz_Stop				(void);



/* -------------------------------------------------------------------*
 * 						Apps
 * -------------------------------------------------------------------*/

void TC_Main_Wait			(void);
void TC_Touch_Wait			(void);
void TC_DS1302_Wait			(void);

void TC_Ventil_Wait_3s5		(void);
void TC_Ventil_Wait_3s		(void);
void TC_Ventil_Wait_2s		(void);
void TC_Ventil_Wait_1s		(void);
void TC_Ventil_Wait_500ms	(void);

void TC_DisplayManual_Wait	(void);
void TC_DisplaySetup_Wait	(void);
void TC_DisplayData_Wait	(void);


unsigned char TC_MainAuto_SafetyTC(t_FuncCmd cmd);






/*********************************************************************\
 * End of SL_TC_func.h
\**********************************************************************/
