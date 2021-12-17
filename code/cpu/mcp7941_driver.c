// --
// MCP7941-Timer-IC

#include <avr/io.h>

#include "defines.h"
#include "lcd_driver.h"

#include "mcp7941_driver.h"
#include "twi_func.h"
#include "tc_func.h"


/* ==================================================================*
 *            FUNCTIONS Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Init
 * ------------------------------------------------------------------*/

void MCP7941_Init(void)
{
  // external battery
  MCP7941_WriteByte(TIC_DAY,  (DEF_DAY | VBATEN));

  // start timer
  MCP7941_WriteByte(TIC_SEC,  TIC_ST);
}


/* ------------------------------------------------------------------*
 *            Init Time Default
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
 *            Write Data i
 * ------------------------------------------------------------------*/

void MCP7941_Write(unsigned char *send, unsigned char i)
{
  // control byte | data bytes | count of bytes
  TWI2_Master_WriteString(WRITE_RTC_RAM, send, i );
}


/* ------------------------------------------------------------------*
 *            Read Data i
 * ------------------------------------------------------------------*/

unsigned char *MCP7941_Read(unsigned char *addr, unsigned char i)
{
  static unsigned char *mcpRec;
  TWI2_Master_WriteString(WRITE_RTC_RAM, addr, i);
  mcpRec = TWI2_Master_ReadString(READ_RTC_RAM, 1);
  return mcpRec;
}



/* ==================================================================*
 *            FUNCTIONS Write and Read
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Write Byte
 * ------------------------------------------------------------------*/

void MCP7941_WriteByte(unsigned char addr, unsigned char sData)
{
  unsigned char send[] = {addr, sData};
  MCP7941_Write(send, 2);
}


/* ------------------------------------------------------------------*
 *            Receive Byte
 * ------------------------------------------------------------------*/

unsigned char MCP7941_ReadByte(unsigned char addr)
{
  unsigned char send[] = {addr};
  unsigned char *rec;
  unsigned char rData;

  rec = MCP7941_Read(send, 1);
  rec++;
  rData = *rec;

  return rData;
}



/* ==================================================================*
 *            Operation Hours
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Write Compressor OpHours2RAM
 * ------------------------------------------------------------------*/

void MCP7941_Write_Comp_OpHours(int hours)
{
  unsigned char h = 0;
  unsigned char l = 0;

  l = (hours & 0x00FF);
  h = ((hours >> 8) & 0x00FF);

  MCP7941_WriteByte(RAM_OP_ADDR_H, h);
  MCP7941_WriteByte(RAM_OP_ADDR_L, l);
}


/* ------------------------------------------------------------------*
 *            Read Compressor OpHoursFromRAM
 * ------------------------------------------------------------------*/

int MCP7941_Read_Comp_OpHours(void)
{
  unsigned char h = 0;
  unsigned char l = 0;

  h = MCP7941_ReadByte(RAM_OP_ADDR_H);
  TCC0_wait_us(25);
  l = MCP7941_ReadByte(RAM_OP_ADDR_L);
  TCC0_wait_us(25);

  return ((h << 8) | l);
}



/* ==================================================================*
 *            Time and Date
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Read Time
 * ------------------------------------------------------------------*/

unsigned char MCP7941_ReadTime(unsigned char cmd)
{
  unsigned char time= 0;
  unsigned char wTime= 0;
  switch(cmd)
  {
    case TIC_SEC:
      time = MCP7941_ReadByte(TIC_SEC);
      wTime = (((time >> 4) & 0x07) * 10 + (time & 0x0F)); break;

    case TIC_MIN:
      time = MCP7941_ReadByte(TIC_MIN);
      wTime = (((time >> 4) & 0x07) * 10 + (time & 0x0F)); break;

    case TIC_HOUR:
      time = MCP7941_ReadByte(TIC_HOUR);
      wTime = (((time >> 4) & 0x03) * 10 + (time & 0x0F)); break;

    case TIC_DATE:
      time = MCP7941_ReadByte(TIC_DATE);
      wTime = (((time >> 4) & 0x03) * 10 + (time & 0x0F)); break;

    case TIC_MONTH:
      time = MCP7941_ReadByte(TIC_MONTH);
      wTime = (((time >> 4) & 0x01) * 10 + (time & 0x0F)); break;

    case TIC_YEAR:
      time = MCP7941_ReadByte(TIC_YEAR);
      wTime = (((time >> 4) & 0x0F) * 10 + (time & 0x0F)); break;
  }
  TCC0_wait_us(25);
  return wTime;
}


/* ------------------------------------------------------------------*
 *            Write Time2LCD
 * ------------------------------------------------------------------*/

void MCP7941_LCD_WriteTime(t_FuncCmd cmd)
{
  unsigned char time=0;

  switch(cmd)
  {
    case _init:

      // : symbol
      LCD_WriteAnyFont(f_4x6_p, 2, 128, 10);
      LCD_WriteAnyFont(f_4x6_p, 2, 140, 10);

      // time
      LCD_WriteAnyValue(f_4x6_p, 2, 2,120, MCP7941_ReadTime(TIC_HOUR));
      LCD_WriteAnyValue(f_4x6_p, 2, 2,132, MCP7941_ReadTime(TIC_MIN));
      LCD_WriteAnyValue(f_4x6_p, 2, 2,144, MCP7941_ReadTime(TIC_SEC));
      break;

    case _exe:

      // sec
      time = MCP7941_ReadTime(TIC_SEC);
      LCD_WriteAnyValue(f_4x6_p, 2, 2,144, time);

      // min
      if(!time)
      {
        time= MCP7941_ReadTime(TIC_MIN);
        LCD_WriteAnyValue(f_4x6_p, 2, 2,132, time);
      }
      else break;

      // hour
      if(!time)
      {
        time= MCP7941_ReadTime(TIC_HOUR);
        LCD_WriteAnyValue(f_4x6_p, 2, 2,132, time);
      }
      else break;
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Write Date2LCD
 * ------------------------------------------------------------------*/

void MCP7941_LCD_WriteDate(void)
{
  // - symbol
  LCD_WriteAnyFont(f_4x6_p, 0, 128, 11);
  LCD_WriteAnyFont(f_4x6_p, 0, 140, 11);

  // year 20xx
  LCD_WriteAnyStringFont(f_4x6_p, 0, 144, "20");

  // date
  LCD_WriteAnyValue(f_4x6_p, 2, 0,120, MCP7941_ReadTime(TIC_DATE));
  LCD_WriteAnyValue(f_4x6_p, 2, 0,132, MCP7941_ReadTime(TIC_MONTH));
  LCD_WriteAnyValue(f_4x6_p, 2, 0,152, MCP7941_ReadTime(TIC_YEAR));
}