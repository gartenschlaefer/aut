// --
// MCP7941-Timer-IC

#include <avr/io.h>

#include "mcp7941_driver.h"

#include "config.h"
#include "lcd_driver.h"
#include "twi_func.h"
#include "tc_func.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void MCP7941_Init(void)
{
  // external battery
  MCP7941_WriteByte(TIC_DAY, (DEF_DAY | VBATEN));

  // start timer
  MCP7941_WriteByte(TIC_SEC, TIC_ST);
}


/* ------------------------------------------------------------------*
 *            init time default
 * ------------------------------------------------------------------*/

void MCP7941_InitDefault(void)
{
  MCP7941_Write_Comp_OpHours(0);

  // control register
  MCP7941_WriteByte(TIC_CTRL, 0);
  MCP7941_WriteByte(TIC_SEC, DEF_SEC);
  MCP7941_WriteByte(TIC_MIN, DEF_MIN);
  MCP7941_WriteByte(TIC_HOUR, DEF_HOUR);
  MCP7941_WriteByte(TIC_DATE, DEF_DATE);
  MCP7941_WriteByte(TIC_MONTH, DEF_MONTH);
  MCP7941_WriteByte(TIC_YEAR, DEF_YEAR);
}


/* ------------------------------------------------------------------*
 *            write data i
 * ------------------------------------------------------------------*/

void MCP7941_WriteString(unsigned char *send, unsigned char i)
{
  // control byte | data bytes | count of bytes
  TWI_D_Master_WriteString(WRITE_RTC_RAM, send, i );
}


/* ------------------------------------------------------------------*
 *            read data i
 * ------------------------------------------------------------------*/

unsigned char *MCP7941_ReadString(struct TWIState *twi_state, unsigned char *addr, unsigned char i)
{
  TWI_D_Master_WriteString(WRITE_RTC_RAM, addr, i);
  unsigned char *mcpRec = TWI_D_Master_ReadString(twi_state, READ_RTC_RAM, 1);
  return mcpRec;
}


/* ------------------------------------------------------------------*
 *            write byte
 * ------------------------------------------------------------------*/

void MCP7941_WriteByte(unsigned char addr, unsigned char sData)
{
  unsigned char send[] = { addr, sData };
  MCP7941_WriteString(send, 2);
}


/* ------------------------------------------------------------------*
 *            receive byte
 * ------------------------------------------------------------------*/

unsigned char MCP7941_ReadByte(struct TWIState *twi_state, unsigned char addr)
{
  unsigned char send[] = { addr };
  unsigned char *rec = MCP7941_ReadString(twi_state, send, 1);
  rec++;
  return *rec;
}


/* ------------------------------------------------------------------*
 *            write compressor operation hours to RAM
 * ------------------------------------------------------------------*/

void MCP7941_Write_Comp_OpHours(int hours)
{
  unsigned char l = (hours & 0x00FF);
  unsigned char h = ((hours >> 8) & 0x00FF);

  MCP7941_WriteByte(RAM_OP_ADDR_H, h);
  MCP7941_WriteByte(RAM_OP_ADDR_L, l);
}


/* ------------------------------------------------------------------*
 *            read compressor operation hours from RAM
 * ------------------------------------------------------------------*/

int MCP7941_Read_Comp_OpHours(struct TWIState *twi_state)
{
  unsigned char h = MCP7941_ReadByte(twi_state, RAM_OP_ADDR_H);
  TCC0_wait_us(25);
  unsigned char l = MCP7941_ReadByte(twi_state, RAM_OP_ADDR_L);
  TCC0_wait_us(25);

  return ((h << 8) | l);
}


/* ------------------------------------------------------------------*
 *            read time
 * ------------------------------------------------------------------*/

unsigned char MCP7941_ReadTime(struct TWIState *twi_state, unsigned char cmd)
{
  unsigned char time = 0;
  unsigned char wTime = 0;
  switch(cmd)
  {
    case TIC_SEC:
      time = MCP7941_ReadByte(twi_state, TIC_SEC);
      wTime = (((time >> 4) & 0x07) * 10 + (time & 0x0F)); break;

    case TIC_MIN:
      time = MCP7941_ReadByte(twi_state, TIC_MIN);
      wTime = (((time >> 4) & 0x07) * 10 + (time & 0x0F)); break;

    case TIC_HOUR:
      time = MCP7941_ReadByte(twi_state, TIC_HOUR);
      wTime = (((time >> 4) & 0x03) * 10 + (time & 0x0F)); break;

    case TIC_DATE:
      time = MCP7941_ReadByte(twi_state, TIC_DATE);
      wTime = (((time >> 4) & 0x03) * 10 + (time & 0x0F)); break;

    case TIC_MONTH:
      time = MCP7941_ReadByte(twi_state, TIC_MONTH);
      wTime = (((time >> 4) & 0x01) * 10 + (time & 0x0F)); break;

    case TIC_YEAR:
      time = MCP7941_ReadByte(twi_state, TIC_YEAR);
      wTime = (((time >> 4) & 0x0F) * 10 + (time & 0x0F)); break;
  }
  TCC0_wait_us(25);
  return wTime;
}