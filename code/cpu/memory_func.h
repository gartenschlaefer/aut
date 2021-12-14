// --
//  internal memory functions

// include guard
#ifndef MEMORY_FUNC_H   
#define MEMORY_FUNC_H

/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Basics
 * ------------------------------------------------------------------*/

void MEM_EEPROM_ExecuteCMD(void);

void MEM_EEPROM_LoadPageBuffer(unsigned char byte, unsigned char eeData);
void MEM_EEPROM_ErasePageBuffer(void);

void MEM_EEPROM_PageErase(unsigned char page);
void MEM_EEPROM_PageWrite(unsigned char page);
void MEM_EEPROM_PageEraseWrite(unsigned char page);

void MEM_EEPROM_Erase(void);

unsigned char MEM_EEPROM_Read(unsigned char page, unsigned char byte);
void MEM_EEPROM_WriteByte(unsigned char page, unsigned char byte, unsigned char eeData);

void MEM_EEPROM_EntPageErase(unsigned char page);
void MEM_EEPROM_EntErase(void);

#endif