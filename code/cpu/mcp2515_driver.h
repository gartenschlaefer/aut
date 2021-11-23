// --
// MCP2525-CAN-Controller-IC

// Include guard
#ifndef MCP2515_DRIVER_H   
#define MCP2515_DRIVER_H

/* ==================================================================*
 *            Defines
 * ==================================================================*/

/* ==================================================================*
 *            SPI Commands
 * ==================================================================*/

#define CMD_RXB0SIDH  0x90
#define CMD_RXB0D0    0x92
#define CMD_RXB1SIDH  0x94
#define CMD_RXB1D0    0x96

#define CMD_TXB0SIDH  0x40
#define CMD_TXB0D0    0x41
#define CMD_TXB1SIDH  0x42
#define CMD_TXB1D0    0x43
#define CMD_TXB2SIDH  0x44
#define CMD_TXB2D0    0x45


/* ==================================================================*
 *            Transmission Register Addresses
 * ==================================================================*/
/* ------------------------------------------------------------------*
 *            Cotnrol Register + BitLocations
 * ------------------------------------------------------------------*/

#define TXB0CTRL  0x30
#define TXB1CTRL  0x40
#define TXB2CTRL  0x50

#define ABTF    (1<<6)
#define MLOA    (1<<5)
#define TXERR   (1<<4)
#define TXREQ   (1<<3)
#define TXP1    (1<<1)
#define TXP0    (1<<0)

/* ------------------------------------------------------------------*
 *            Standard Identifier + Datalength Code
 * ------------------------------------------------------------------*/

#define TXB0SIDH  0x31
#define TXB1SIDH  0x41
#define TXB2SIDH  0x51
#define TXB0SIDL  0x32
#define TXB1SIDL  0x42
#define TXB2SIDL  0x52

#define TXB0DLC   0x35
#define TXB1DLC   0x45
#define TXB2DLC   0x55

/* ------------------------------------------------------------------*
 *            TransmitBuffers
 * ------------------------------------------------------------------*/

#define TXB0D0    0x36
#define TXB0D1    0x37
#define TXB0D2    0x38
#define TXB0D3    0x39
#define TXB0D4    0x3A
#define TXB0D5    0x3B
#define TXB0D6    0x3C
#define TXB0D7    0x3D

#define TXB1D0    0x46
#define TXB1D1    0x47
#define TXB1D2    0x48
#define TXB1D3    0x49
#define TXB1D4    0x4A
#define TXB1D5    0x4B
#define TXB1D6    0x4C
#define TXB1D7    0x4D

#define TXB2D0    0x56
#define TXB2D1    0x57
#define TXB2D2    0x58
#define TXB2D3    0x59
#define TXB2D4    0x5A
#define TXB2D5    0x5B
#define TXB2D6    0x5C
#define TXB2D7    0x5D


/* ==================================================================*
 *            Receiption Register Addresses
 * ==================================================================*/
/* ------------------------------------------------------------------*
 *            Cotnrol Register
 * ------------------------------------------------------------------*/

#define RXB0CTRL  0x60
#define RXB1CTRL  0x70

#define RXM1_bm   (1<<6)
#define RXM0_bm   (1<<5)
#define RXRTR_bm  (1<<3)
#define BUKT_bm   (1<<2)
#define BUKT1_bm  (1<<1)
#define FILHIT0_bm  (1<<0)
#define FILHIT1_bm  (1<<1)
#define FILHIT2_bm  (1<<2)

/* ------------------------------------------------------------------*
 *            Standard Identifier + Datalength Code
 * ------------------------------------------------------------------*/

#define RXB0SIDH  0x61    //SID reiceived from Message
#define RXB1SIDH  0x71
#define RXB0SIDL  0x62
#define RXB1SIDL  0x72

#define RXB0DLC   0x65    //DLC reiceived from Message
#define RXB1DLC   0x75

/* ------------------------------------------------------------------*
 *            ReceiveBuffers
 * ------------------------------------------------------------------*/

#define RXB0D0    0x66
#define RXB0D1    0x67
#define RXB0D2    0x68
#define RXB0D3    0x69
#define RXB0D4    0x6A
#define RXB0D5    0x6B
#define RXB0D6    0x6C
#define RXB0D7    0x6D

#define RXB1D0    0x76
#define RXB1D1    0x77
#define RXB1D2    0x78
#define RXB1D3    0x79
#define RXB1D4    0x7A
#define RXB1D5    0x7B
#define RXB1D6    0x7C
#define RXB1D7    0x7D

/* ------------------------------------------------------------------*
 *            Filter + Mask Register
 * ------------------------------------------------------------------*/

#define RXF0SIDH  0x00
#define RXF1SIDH  0x04
#define RXF2SIDH  0x08
#define RXF3SIDH  0x10
#define RXF4SIDH  0x14
#define RXF5SIDH  0x18

#define RXF0SIDL  0x01
#define RXF1SIDL  0x05
#define RXF2SIDL  0x09
#define RXF3SIDL  0x11
#define RXF4SIDL  0x15
#define RXF5SIDL  0x19

#define RXM0SIDH  0x20
#define RXM1SIDH  0x24
#define RXM0SIDL  0x21
#define RXM1SIDL  0x25


/* ------------------------------------------------------------------*
 *            CAN-Timing + Error Register Addresses
 * ------------------------------------------------------------------*/

#define CNF1    0x2A
#define CNF2    0x29
#define CNF3    0x28

#define TEC     0x1C    //Trasmit ErrorCounter
#define REC     0x1D    //Receive ErrorCounter
#define EFLG    0x2D    //Error Flags


/* ------------------------------------------------------------------*
 *            CAN Interrupt + Bit-Locations
 * ------------------------------------------------------------------*/

#define CANINTE   0x2B    //Register
#define CANINTF   0x2C    //Register

#define MERRF_bm  (1<<7)
#define WAKIF_bm  (1<<6)
#define ERRIF_bm  (1<<5)
#define TX2IF_bm  (1<<4)
#define TX1IF_bm  (1<<3)
#define TX0IF_bm  (1<<2)
#define RX1IF_bm  (1<<1)
#define RX0IF_bm  (1<<0)


/* ------------------------------------------------------------------*
 *            CAN Control + Statur Register Addresses
 * ------------------------------------------------------------------*/

#define CANCTRL   0x0F
#define CANSTAT   0x0E


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

void MCP2515_Init(void);
void MCP2515_HWReset(void);
void MCP2515_SWReset(void);

void MCP2515_WriteReg(unsigned char addr, unsigned char data);
unsigned char MCP2515_ReadReg(unsigned char addr);
void MCP2515_BitModify(unsigned char addr, unsigned char mask, unsigned char data);

unsigned char MCP2515_ReadRxB(unsigned char cmd);
void MCP2515_LoadTxBuffer(unsigned char cmd, unsigned char buffer);

#endif