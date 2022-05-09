// --
//  MCP7941-Timer-IC

// include guard
#ifndef MCP_7941_DRIVER_H   
#define MCP_7941_DRIVER_H

#include "enums.h"


/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define WRITE_RTC_RAM (0xDE >> 1)
#define READ_RTC_RAM  (0xDF >> 1)

#define VBATEN        (1 << 3)
#define VBAT          (1 << 4)
#define OSCON         (1 << 5)


/* ------------------------------------------------------------------*
 *            Register / BitPosition
 * ------------------------------------------------------------------*/

#define TIC_SEC     0x00
#define TIC_MIN     0x01
#define TIC_HOUR    0x02
#define TIC_DAY     0x03
#define TIC_DATE    0x04
#define TIC_MONTH   0x05
#define TIC_YEAR    0x06
#define TIC_CTRL    0x07
#define TIC_CAL     0x08
#define TIC_UNLOCK  0x09

#define TIC_EXTOSC  (1 << 3)    //ExternalOszillator->TIC_CTRL
#define TIC_ST      (1 << 7)    //Start->TIC_SEC


/* ------------------------------------------------------------------*
 *            SRAM
 * ------------------------------------------------------------------*/

#define RAM_OP_ADDR_L   (0x20)
#define RAM_OP_ADDR_H   (0x21)



/* ==================================================================*
 *            FUCNTION API
 * ==================================================================*/

void MCP7941_Init(void);
void MCP7941_InitDefault(void);

void MCP7941_Write(unsigned char *send, unsigned char i);
unsigned char *MCP7941_Read(unsigned char *addr, unsigned char i);

void MCP7941_WriteByte(unsigned char addr, unsigned char sData);
unsigned char MCP7941_ReadByte(unsigned char addr);

void MCP7941_Write_Comp_OpHours(int hours);
int MCP7941_Read_Comp_OpHours(void);

unsigned char MCP7941_ReadTime(unsigned char cmd);
void MCP7941_LCD_WriteTime(t_FuncCmd cmd);
void MCP7941_LCD_WriteDate(void);

#endif