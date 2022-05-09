// --
//  internal memory applications

// include guard
#ifndef MEMORY_APP_H   
#define MEMORY_APP_H

#include "enums.h"


/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

// atxmega128a1
// num of pages: 64

// start and end page of data section
#define DATA_START_PAGE 2
#define DATA_END_PAGE 50

// 3 Manual pages
#define MANUAL_START_PAGE 2
#define MANUAL_END_PAGE 6

// 3 Setup pages
#define SETUP_START_PAGE 7
#define SETUP_END_PAGE 11

// 16 Auto pages
#define AUTO_START_PAGE 12
#define AUTO_END_PAGE 42

// amount of data pages
#define DATA_PAGE_NUM_MANUAL (3 - 1)
#define DATA_PAGE_NUM_SETUP (3 - 1)
#define DATA_PAGE_NUM_AUTO (16 - 1)


/* ------------------------------------------------------------------*
 *            function headers
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault(void);
unsigned char MEM_EEPROM_ReadVar(t_var var);
void MEM_EEPROM_WriteVar(t_var var, unsigned char eeData);

unsigned char MEM_EEPROM_ReadData(unsigned char page, unsigned char entry, t_data var);
void MEM_EEPROM_LoadData(unsigned char entry, t_data byte, unsigned char eeData);

void MEM_EEPROM_WriteAutoEntry(int o2, unsigned char error, t_AutoEntry write);
void MEM_EEPROM_WriteManualEntry(unsigned char h, unsigned char min, t_FuncCmd cmd);
void MEM_EEPROM_WriteSetupEntry(void);

void MEM_EEPROM_SetZero(void);
void MEM_EEPROM_WriteVarDefault_Short(void);

#endif