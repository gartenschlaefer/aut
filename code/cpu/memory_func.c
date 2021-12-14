// --
//  internal memory functions

#include<avr/io.h>

#include "defines.h"
#include "memory_app.h"
#include "memory_func.h"


/* ==================================================================*
 *            FUNCTIONS Basics
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  execute command
 * ------------------------------------------------------------------*/

void MEM_EEPROM_ExecuteCMD(void)
{
  // protection
  CCP = 0xD8;

  // execute command
  NVM.CTRLA = NVM_CMDEX_bm;
  while(NVM.STATUS & NVM_NVMBUSY_bm);
}


/*-------------------------------------------------------------------*
 *  load page buffer with one byte
 * ------------------------------------------------------------------*/

void MEM_EEPROM_LoadPageBuffer(unsigned char byte, unsigned char eeData)
{
  NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
  NVM.ADDR0 = byte;
  NVM.DATA0 = eeData;
}


/*-------------------------------------------------------------------*
 *  erase page buffer of EEPROM
 * ------------------------------------------------------------------*/

void MEM_EEPROM_ErasePageBuffer(void)
{
  NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
  MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 *  erase page
 * ------------------------------------------------------------------*/

void MEM_EEPROM_PageErase(unsigned char page)
{
  NVM.CMD = NVM_CMD_ERASE_EEPROM_PAGE_gc;

  NVM.ADDR0 = ((page << 5) & 0xE0);
  NVM.ADDR1 = (page >> 3);

  MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 *  write page
 * ------------------------------------------------------------------*/

void MEM_EEPROM_PageWrite(unsigned char page)
{
  NVM.CMD = NVM_CMD_WRITE_EEPROM_PAGE_gc; //CMD Load
  NVM.ADDR0 = ((page << 5) & 0xE0);
  NVM.ADDR1 = (page >> 3);
  MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 *  erase and write page
 * ------------------------------------------------------------------*/

void MEM_EEPROM_PageEraseWrite(unsigned char page)
{
  NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
  NVM.ADDR0 = ((page << 5) & 0xE0);
  NVM.ADDR1 = (page >> 3);
  MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 *  erase whole EEPROM parts, which are dedicated in Page Buffer
 * ------------------------------------------------------------------*/

void MEM_EEPROM_Erase(void)
{
  NVM.CMD = NVM_CMD_ERASE_EEPROM_gc;
  MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 *  reads one byte from dedicated address in EEPROM
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_Read(unsigned char page, unsigned char byte)
{
  NVM.CMD = NVM_CMD_READ_EEPROM_gc;

  // page0 + byte | page1
  NVM.ADDR0 = ((page << 5) & 0xE0) | (byte & 0x1F);
  NVM.ADDR1 = (page >> 3);

  // protection
  CCP = 0xD8;

  // execute command
  NVM.CTRLA =  NVM_CMDEX_bm;

  return NVM.DATA0;
}



/* ==================================================================*
 *            EEPROM APPS
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  write byte
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteByte(unsigned char page, unsigned char byte, unsigned char eeData)
{
  // buffer loaded -> erase
  if(NVM.STATUS & NVM_EELOAD_bm) MEM_EEPROM_ErasePageBuffer();

  // load new data | write
  MEM_EEPROM_LoadPageBuffer(byte, eeData);
  MEM_EEPROM_PageEraseWrite(page);
}


/*-------------------------------------------------------------------*
 *  MEM_EEPROM_Entire Page Erase (32Bytes)
 * ------------------------------------------------------------------*/

void MEM_EEPROM_EntPageErase(unsigned char page)
{
  unsigned char i = 0;

  // dummy bytes
  for(i = 0; i < 32; i++)
  {
    MEM_EEPROM_LoadPageBuffer(i, 0xFF);
  }
  MEM_EEPROM_PageErase(page);
}


/*-------------------------------------------------------------------*
 *  Memory EEPROM - Entire Erase
 * ------------------------------------------------------------------*/

void MEM_EEPROM_EntErase(void)
{
  unsigned char i = 0;

  // dummy bytes
  for(i = 0; i < 32; i++)
  {
    MEM_EEPROM_LoadPageBuffer(i, 0xFF);
  }

  MEM_EEPROM_Erase();
}