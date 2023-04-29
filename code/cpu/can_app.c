// --
// CAN-Bus applications over MCP2515 ic

#include <avr/io.h>

#include "can_app.h"
#include "lcd_sym.h"
#include "mcp2515_driver.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void CAN_Init(void)
{
  MCP2515_Init();
  CAN_RxB0_Init();

  // config to normal
  MCP2515_WriteReg(CANCTRL, 0x05); 
}


/* ------------------------------------------------------------------*
 *            Receive Buffer init
 * ------------------------------------------------------------------*/

void CAN_RxB0_Init(void)
{
  // use filters
  MCP2515_WriteReg(RXB0CTRL, 0x00);

  // filter id register
  MCP2515_WriteReg(RXF0SIDH, 0xFF);
  MCP2515_WriteReg(RXF0SIDL, 0xFF);

  // filter mask register
  MCP2515_WriteReg(RXM0SIDH, 0x00);
  MCP2515_WriteReg(RXM0SIDL, 0x00);

  // clear RxB0D0
  MCP2515_WriteReg(RXB0D0, 0x00);
}


/* ------------------------------------------------------------------*
 *            CAN update
 * ------------------------------------------------------------------*/

void CAN_Update(struct CANState *can_state)
{
  // read rxb0
  CAN_RxB0_Read(can_state);

  // okay flag
  if(!can_state->mcp2525_ok_flag){ LCD_Sym_CAN_MCPFail(); }
}


/*-------------------------------------------------------------------*
 *  can read buffer
 * --------------------------------------------------------------
 *  return[0]: dlc  - 0: noMessage  - 1: Message
 *  return[1]: addr - Address High
 *  return[2]: cmd  - Command
 *  return[3]: data - following DataBytes   - return[3..10]
 * ------------------------------------------------------------------*/

void CAN_RxB0_Read(struct CANState *can_state)
{
  // RxB0 filled?
  if(MCP2515_ReadReg(CANINTF) & RX0IF_bm)
  {
    // read dlc (data length code)
    unsigned char dlc = (MCP2515_ReadReg(RXB0DLC) & 0x0F);

    // save dlc, adress, and command byte
    can_state->rxb0_buffer[0] = dlc;
    can_state->rxb0_buffer[1] = MCP2515_ReadReg(RXB0SIDH);
    can_state->rxb0_buffer[2] = MCP2515_ReadReg(RXB0D0);

    // read databytes
    for(unsigned char i = 3; i < (dlc + 2); i++){ can_state->rxb0_buffer[i] = MCP2515_ReadReg(0x64 + i); }

    // reset flag
    MCP2515_WriteReg(CANINTF, 0x00);

    // indicate data available
    can_state->rxb0_data_av = true;
  }
}


/* ------------------------------------------------------------------*
 *            clear read buffer
 * ------------------------------------------------------------------*/

void CAN_RxB0_Clear(struct CANState *can_state)
{
  for(unsigned char i = 0; i < 10; i++){ can_state->rxb0_buffer[i] = 0x00; }
  can_state->rxb0_data_av = false;
}


/*-------------------------------------------------------------------*
 *  CAN_TxB0_Write
 * --------------------------------------------------------------
 *  return:       - 1: Error      - 0: succeed
 *  txB0[0]: dlc  - 0: noMessage  - 1: Message
 *  txB0[1]: addr - Address High
 *  txB0[2]: cmd  - Command
 *  txB0[3]: data - following DataBytes   - return[3..10]
 * ------------------------------------------------------------------*/

unsigned char CAN_TxB0_Write(struct CANState *can_state, unsigned char *txB0)
{
  int err = 0;
  unsigned char i = 0;
  unsigned char dlc = txB0[0];

  // dlc and address h and l
  MCP2515_WriteReg(TXB0DLC, dlc);
  MCP2515_WriteReg(TXB0SIDH, txB0[1]);
  MCP2515_WriteReg(TXB0SIDL, 0x00);

  // write data to buffer register
  while(dlc)
  {
    dlc--;
    MCP2515_WriteReg(0x36 + i, txB0[2 + i]);
    i++;
  }

  // start transmission
  MCP2515_WriteReg(TXB0CTRL, TXREQ);

  // wait until done
  while(!(MCP2515_ReadReg(CANINTF) & TX0IF_bm))
  {
    // can error
    if((MCP2515_ReadReg(TXB0CTRL) & 0xF0))
    {
      err = 0;
      return 1;
    }

    // MCP2515 failed
    if(err > 1000)
    {
      err = 0;
      can_state->mcp2525_ok_flag = false;
      return 2;
    }
    err++;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            TX - Command2UltraSonic
 * -------------------------------------------------------------------*
 *  txB0[0]: dlc  - 0:noMessage - 1:Message
 *  txB0[1]: addr - Address High
 *  txB0[2]: cmd  - Command
 * ------------------------------------------------------------------*/

void CAN_TxCmd(struct CANState *can_state, t_can_cmd cmd)
{
  unsigned char tx[3] = {1, 0x01, cmd};
  CAN_TxB0_Write(can_state, &tx[0]);
}


/* ------------------------------------------------------------------*
 *            TX - UltraSonicStatusRegister command
 * ------------------------------------------------------------------*/

// void CAN_TxUSSREG(unsigned char reg)
// {
//   unsigned char tx[4] = {1, 0x01, CAN_CMD_sonic_write_USSREG, reg};
//   CAN_TxB0_Write(&tx[0]);
// }


/* ------------------------------------------------------------------*
 *            RX - ACK
 * ------------------------------------------------------------------*/

unsigned char CAN_RxB0_Ack(struct CANState *can_state)
{
  if(can_state->rxb0_data_av){ return can_state->rxb0_buffer[2]; }
  return CAN_CMD_sonic_wait;
}
