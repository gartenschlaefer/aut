/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    AT24C-driver-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	HeaderFile for EEPROM AT24C512
* ------------------------------------------------------------------
*	Date:			    31.12.2012
* lastChanges:  09.02.2016
\**********************************************************************/

// Include guard
#ifndef AT24C_DRIVER_H   
#define AT24C_DRIVER_H

/* ==================================================================*
 * 						Defines
 * ==================================================================*/

#define AT24C_ADDR_READ		  (0xA1 >> 1)
#define AT24C_ADDR_WRITE	  (0xA0 >> 1)

#define AT24C_BOOT_PAGE_OS  1

/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/


void AT24C_Init(void);
void AT24C_WriteByte(int addr, unsigned char sData);
void AT24C_WritePage(int addr, unsigned char *sData);
unsigned char AT24C_ReadByte(int addr);
unsigned char *AT24C_Read8Byte(int addr);

#endif
