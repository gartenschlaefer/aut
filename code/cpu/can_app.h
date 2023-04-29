// --
// CAN-Bus applications over MCP2515 ic

// include guard
#ifndef CAN_APP_H   
#define CAN_APP_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            Defines
 * ------------------------------------------------------------------*/

// USCREG1 - sonic Control Register1

// address register
#define   USADR0  (1 << 0)
#define   USADR1  (1 << 1)
#define   USADR2  (1 << 2)
#define   USADR3  (1 << 3)

// sonic enable and modes
#define   USEN    (1 << 4)
#define   USMOD1  (1 << 5)
#define   USMOD2  (1 << 6)
#define   USMOD3  (1 << 7)

// USCREG2 - sonic Control Register2

// read distance or temp
#define   RDIS  (1 << 0)
#define   RTEMP (1 << 1)

// USSREG - sonic Status Register

// data or temp available
#define   DISA  (1 << 0)
#define   TEMPA (1 << 1)

// measure distance or temp
#define   MDIS  (1 << 2)
#define   MTEMP (1 << 3)

// ready bit and error
#define   USRDY (1 << 4)
#define   ERR   (1 << 5)


/* ------------------------------------------------------------------*
 *            function headers
 * ------------------------------------------------------------------*/

void CAN_Init(void);
void CAN_RxB0_Init(void);
void CAN_Update(struct CANState *can_state);
void CAN_RxB0_Read(struct CANState *can_state);
void CAN_RxB0_Clear(struct CANState *can_state);
unsigned char CAN_RxB0_Ack(struct CANState *can_state);
unsigned char CAN_TxB0_Write(struct CANState *can_state, unsigned char *txB0);
void CAN_TxCmd(struct CANState *can_state, t_can_cmd cmd);
//void CAN_TxUSSREG(unsigned char reg);

#endif