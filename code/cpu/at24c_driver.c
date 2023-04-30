// --
// EEPROM AT24C512 driver

#include "at24c_driver.h"
#include "twi_func.h"
#include "tc_func.h"


/* ------------------------------------------------------------------*
 *            memory reset
 * ------------------------------------------------------------------*/

void AT24C_MemoryReset(void)
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
 *            write byte
 * ------------------------------------------------------------------*/

void AT24C_WriteByte(int addr, unsigned char sData)
{
  unsigned char send[] = {0, 0, 0};

  send[0] = ((addr >> 8) & 0x00FF);
  send[1] = addr & 0x00FF;
  send[2] = sData;

  TWI_D_Master_WriteString(AT24C_ADDR_WRITE, send, 3);
}


/* ------------------------------------------------------------------*
 *            write page - 128byte
 * ------------------------------------------------------------------*/

void AT24C_WritePage(int addr, unsigned char *sData)
{
  unsigned char send[130];
  unsigned char i = 0;

  send[0] = ((addr >> 8) & 0x00FF);
  send[1] = addr & 0x00FF;
  for(i = 0; i < 128; i++){ send[i + 2] = sData[i]; }
  TWI_D_Master_WriteString(AT24C_ADDR_WRITE, send, 130);
}


/* ------------------------------------------------------------------*
 *            read byte
 * ------------------------------------------------------------------*/

unsigned char AT24C_ReadByte(struct TWIState *twi_state, int addr)
{
  unsigned char send[] = {0, 0};
  unsigned char *rec;
  unsigned char rData;

  send[0] = ((addr >> 8) & 0x00FF);
  send[1] = addr & 0x00FF;

  TWI_D_Master_WriteString(AT24C_ADDR_WRITE, send, 2);
  rec = TWI_D_Master_ReadString(twi_state, AT24C_ADDR_READ, 1);
  rec++;
  rData = *rec;

  return rData;
}


/* ------------------------------------------------------------------*
 *            read 8 bytes
 * ------------------------------------------------------------------*/

unsigned char *AT24C_Read8Byte(struct TWIState *twi_state, int addr)
{
  unsigned char send[] = {0, 0};
  unsigned char *rec;

  send[0] = ((addr >> 8) & 0x00FF);
  send[1] = addr & 0x00FF;

  TWI_D_Master_WriteString(AT24C_ADDR_WRITE, send, 2);
  rec = TWI_D_Master_ReadString(twi_state, AT24C_ADDR_READ, 8);
  rec++;

  return rec;
}