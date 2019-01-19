/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    can_app.h
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	CAN Application Header-File for CAN-Participiants
* ------------------------------------------------------------------
*	Date:			    08.04.2013
* lastChanges:  13.08.2015
\**********************************************************************/


/* ==================================================================*
 * 						Defines
 * ==================================================================*/

//--------------------------------USCREG1 - UltraSonicControlRegister1
#define		USADR0	(1 << 0)		//Addr0
#define		USADR1	(1 << 1)		//Addr1
#define		USADR2	(1 << 2)		//Addr2
#define		USADR3	(1 << 3)		//Addr3
#define 	USEN	  (1 << 4)		//UltraSonic Enable
#define		USMOD1	(1 << 5)		//UltraSonic Modus1
#define		USMOD2	(1 << 6)		//UltraSonic Modus2
#define		USMOD3	(1 << 7)		//UltraSonic Modus3

//--------------------------------USCREG2 - UltraSonicControlRegister2
#define		RDIS	(1 << 0)		//ReadDistance
#define		RTEMP	(1 << 1)		//ReadTemp

//--------------------------------USSREG - UltraSonic Status Register
#define		DISA	(1 << 0)		//Data Distance available
#define		TEMPA	(1 << 1)		//Temp available
#define		MDIS	(1 << 2)		//MeasureDistance
#define		MTEMP	(1 << 3)		//MeasureTemp
#define		USRDY	(1 << 4)		//UltraSonic ReadyBit
#define		ERR		(1 << 5)		//ErrorBit



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void CAN_TXMOB_Working(void);
void CAN_TXMOB_ACK(void);
void CAN_TXMOB_ACK_OneShot(void);
void CAN_TXMOB_ACK_5Shots(void);
void CAN_TXMOB_ACK_Temp(void);
void CAN_TXMOB_ACK_Boot(void);
void CAN_TXMOB_USSREG(void);
void CAN_TXMOB_USDDREG(void);
void CAN_TXMOB_USDTREG(void);
void CAN_TXMOB_sVersion(void);

t_UScmd CAN_RxParser(t_UScmd state);

unsigned char CAN_USSREG(t_FuncCmd cmd, unsigned char write);
int CAN_USDDREG(t_FuncCmd cmd, int write);
int CAN_USDTREG(t_FuncCmd cmd, int write);







/*********************************************************************\
 * End of can_app.h
\**********************************************************************/
