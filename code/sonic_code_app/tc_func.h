/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    tc_func.h
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	TimerCounter Functions Header-File
* ------------------------------------------------------------------
*	Date:			    06.10.2011
* lastChanges:  11.04.2015
\**********************************************************************/


/* ==================================================================*
 * 						FUNCTIONS 16MHz Timer
 * ==================================================================*/

void TC_Wait_1clk(void);


/* ------------------------------------------------------------------*
 * 						TC0 - 8Bit-MainWaitTimer
 * ------------------------------------------------------------------*/

void TC0_16MHzWait_1ms_Init(void);
unsigned char TC0_16MHzWait_1ms_Query(void);
void TC0_16MHz_Stop(void);

void TC0_16MHzWait_msWhile(int mSec);
unsigned char TC0_16MHzWait_msQuery(t_FuncCmd cmd, int mSec);


/* ------------------------------------------------------------------*
 * 						TC2 - 8Bit-SecnodWaitTimer
 * ------------------------------------------------------------------*/

void TC2_16MHzWait_1ms_Init(void);
unsigned char TC2_16MHzWait_1ms_Query(void);
void TC2_16MHz_Stop(void);

void TC2_16MHzWait_msWhile(int mSec);
unsigned char TC2_16MHzWait_msQuery(t_FuncCmd cmd, int mSec);


/* ------------------------------------------------------------------*
 * 						TC1 - 16bit-Timer Reserved4Sonic Pulses
 * ------------------------------------------------------------------*/

/* ------------------------------------------------------------------*
 * 						TC3 - 16bit-Timer-Reserved
 * ------------------------------------------------------------------*/

void TC3_Sonic_StartTimer	(void);
void TC3_Sonic_StopTimer	(void);
void TC3_Sonic_ResetTimer	(void);
void TC3_Sonic_Init(void);
void TC3_Sonic_Send(void);




/*********************************************************************\
 * End of tc_func.h
\**********************************************************************/
