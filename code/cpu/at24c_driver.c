// --
// EEPROM AT24C512 driver


#include <avr/io.h>

#include "defines.h"
#include "twi_func.h"
#include "at24c_driver.h"
#include "tc_func.h"


/* ==================================================================*
 *            FUNCTIONS Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Init
 * ------------------------------------------------------------------*/

void AT24C_Init(void)
{
  unsigned char addr = 0;
  for(addr = 0; addr < 32; addr++)
  {
    // model tel nr.
    AT24C_WriteByte(addr, 11);
    TCC0_wait_ms(10);
  }
}


/* ------------------------------------------------------------------*
 *            Write Byte
 * ------------------------------------------------------------------*/

void AT24C_WriteByte(int addr, unsigned char sData)
{
  unsigned char send[]= {0,0,0};

  send[0]= ((addr >> 8) & 0x00FF);
  send[1]= addr & 0x00FF;
  send[2]= sData;

  TWI2_Master_WriteString(AT24C_ADDR_WRITE, send, 3);
}


/* ------------------------------------------------------------------*
 *            Write Page - 128Byte
 * ------------------------------------------------------------------*/

void AT24C_WritePage(int addr, unsigned char *sData)
{
  unsigned char send[130];
  unsigned char i = 0;

  send[0] = ((addr >> 8) & 0x00FF);
  send[1] = addr & 0x00FF;
  for(i = 0; i < 128; i++) send[i+2] = sData[i];
  TWI2_Master_WriteString(AT24C_ADDR_WRITE, send, 130);
}


/* ------------------------------------------------------------------*
 *            Read Byte
 * ------------------------------------------------------------------*/

unsigned char AT24C_ReadByte(int addr)
{
  unsigned char send[] = {0,0};
  unsigned char *rec;
  unsigned char rData;

  send[0]= ((addr >> 8) & 0x00FF);
  send[1]= addr & 0x00FF;

  TWI2_Master_WriteString(AT24C_ADDR_WRITE, send, 2);
  rec = TWI2_Master_ReadString(AT24C_ADDR_READ, 1);   //Read
  rec++;
  rData = *rec;

  return rData;
}


/* ------------------------------------------------------------------*
 *            Read 8Byte
 * ------------------------------------------------------------------*/

unsigned char *AT24C_Read8Byte(int addr)
{
  unsigned char send[] = {0,0};
  unsigned char *rec;

  send[0]= ((addr >> 8) & 0x00FF);
  send[1]= addr & 0x00FF;

  TWI2_Master_WriteString(AT24C_ADDR_WRITE, send, 2);
  rec = TWI2_Master_ReadString(AT24C_ADDR_READ, 8);   //Read
  rec++;

  return rec;
}
