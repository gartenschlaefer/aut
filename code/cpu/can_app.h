// --
// CAN-Bus applications over MCP2515 ic

// include guard
#ifndef CAN_APP_H   
#define CAN_APP_H

#include "enums.h"

/* ------------------------------------------------------------------*
 *            Defines
 * ------------------------------------------------------------------*/

//--------------------------------USCREG1 - UltraSonic Control Register1
#define   USADR0  (1 << 0)    //Addr0
#define   USADR1  (1 << 1)    //Addr1
#define   USADR2  (1 << 2)    //Addr2
#define   USADR3  (1 << 3)    //Addr3
#define   USEN    (1 << 4)    //UltraSonic Enable
#define   USMOD1  (1 << 5)    //UltraSonic Modus1
#define   USMOD2  (1 << 6)    //UltraSonic Modus2
#define   USMOD3  (1 << 7)    //UltraSonic Modus3

//--------------------------------USCREG2 - UltraSonic Control Register2
#define   RDIS  (1 << 0)    //Read Distance
#define   RTEMP (1 << 1)    //Read Temp

//--------------------------------USSREG - UltraSonic Status Register
#define   DISA  (1 << 0)    //Data Distance available
#define   TEMPA (1 << 1)    //Temp available
#define   MDIS  (1 << 2)    //MeasureDistance
#define   MTEMP (1 << 3)    //MeasureTemp
#define   USRDY (1 << 4)    //UltraSonic ReadyBit
#define   ERR   (1 << 5)    //Error


/* ------------------------------------------------------------------*
 *            function headers
 * ------------------------------------------------------------------*/

void CAN_Init(void);
void CAN_RxB0_Init(void);
unsigned char *CAN_RxB0_Read(void);
t_UScmd CAN_RxACK(void);
unsigned char CAN_TxB0_Write(unsigned char *txB0);
void CAN_TxCmd(t_UScmd cmd);
void CAN_TxUSSREG(unsigned char reg);

unsigned char CAN_LiveCheck(unsigned char addr);
unsigned char *CAN_SonicVersion(t_FuncCmd cmd);
unsigned char *CAN_SonicQuery(t_FuncCmd cmd, t_UScmd us);
unsigned char CAN_SonicReadProgram(t_FuncCmd cmd);
unsigned char CAN_SonicWriteProgram(t_FuncCmd cmd);

#endif