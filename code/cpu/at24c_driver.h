// --
// EEPROM AT24C512 driver

// include guard
#ifndef AT24C_DRIVER_H   
#define AT24C_DRIVER_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define AT24C_ADDR_READ   (0xA1 >> 1)
#define AT24C_ADDR_WRITE  (0xA0 >> 1)

#define AT24C_BOOT_PAGE_OS  1

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void AT24C_MemoryReset(void);
void AT24C_WriteByte(int addr, unsigned char sData);
void AT24C_WritePage(int addr, unsigned char *sData);
unsigned char AT24C_ReadByte(struct TWIState *twi_state, int addr);
unsigned char *AT24C_Read8Byte(struct TWIState *twi_state, int addr);

#endif