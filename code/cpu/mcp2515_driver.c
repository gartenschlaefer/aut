// --
// MCP2525-CAN-Controller-IC

#include <avr/io.h>

#include "mcp2515_driver.h"

#include "spi_func.h"
#include "tc_func.h"


/* ------------------------------------------------------------------*
 *            init 100kps
 * ------------------------------------------------------------------*/

void MCP2515_Init(void)
{
  SPI_Init();
  TCC0_wait_ms(200);

  // reset output -> off
  PORTE.DIRSET = PIN2_bm;
  PORTE.OUTSET = PIN2_bm;
  TCC0_wait_ms(200);
  MCP2515_SWReset();
  TCC0_wait_ms(10);

  MCP2515_WriteReg(CNF1, 0x04);   //BRP-16Mhz/(4+1)*2, SJW=1
  MCP2515_WriteReg(CNF2, 0xDE);   //PRSEG=6+1 PHSEG1=3+1 BTLMode 3xSample
  MCP2515_WriteReg(CNF3, 0x03);   //PHSEG2=3+1
}


/* ------------------------------------------------------------------*
 *            Reset software
 * ------------------------------------------------------------------*/

void MCP2515_SWReset(void)
{
  CS_CLR;
  SPI_WriteByte(0xA0);
  CS_SET;
}


/* ------------------------------------------------------------------*
 *            Reset hardware
 * ------------------------------------------------------------------*/

// void MCP2515_HWReset(void)
// {
//   TCC0_wait_ms(200);
//   PORTE.OUTCLR = PIN2_bm;
//   TCC0_wait_ms(200);
//   PORTE.OUTSET = PIN2_bm;
// }


/* ------------------------------------------------------------------*
 *            Write Register
 * ------------------------------------------------------------------*/

void MCP2515_WriteReg(unsigned char addr, unsigned char data)
{
  CS_CLR;
  SPI_WriteByte(0x02);
  SPI_WriteByte(addr);
  SPI_WriteByte(data);
  CS_SET;
}


/* ------------------------------------------------------------------*
 *            Read Register
 * ------------------------------------------------------------------*/

unsigned char MCP2515_ReadReg(unsigned char addr)
{
  CS_CLR;
  SPI_WriteByte(0x03);
  SPI_WriteByte(addr);
  unsigned char rReg = SPI_ReadByte();
  CS_SET;
  return rReg;
}


/* ------------------------------------------------------------------*
 *            Bit Modify
 * ------------------------------------------------------------------*/

// void MCP2515_BitModify(unsigned char addr, unsigned char mask, unsigned char data)
// {
//   CS_CLR;
//   SPI_WriteByte(addr);
//   SPI_WriteByte(mask);
//   SPI_WriteByte(data);
//   CS_SET;
// }


/* ------------------------------------------------------------------*
 *            Read RxB0 or RxB1 -> macros
 * ------------------------------------------------------------------*/

// unsigned char MCP2515_ReadRxB(unsigned char cmd)
// {
//   CS_CLR;
//   SPI_WriteByte(cmd);
//   unsigned char rReg = SPI_ReadByte();
//   CS_SET;
//   return rReg;
// }


/* ------------------------------------------------------------------*
 *            Load TxBuffer -> macros
 * ------------------------------------------------------------------*/

// void MCP2515_LoadTxBuffer(unsigned char cmd, unsigned char buffer)
// {
//   CS_CLR;
//   SPI_WriteByte(cmd);
//   SPI_WriteByte(buffer);
//   CS_SET;
// }