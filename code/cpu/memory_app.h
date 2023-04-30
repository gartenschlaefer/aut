// --
//  internal memory applications

// include guard
#ifndef MEMORY_APP_H   
#define MEMORY_APP_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

// atxmega128a1
// num of pages: 64

// start and end page of data section
#define MEM_DATA_START_SECTION 2
#define MEM_DATA_END_SECTION 50

// 3 manual pages
#define MEM_MANUAL_START_SECTION 2
#define MEM_MANUAL_END_SECTION 6

// 3 setup pages
#define MEM_SETUP_START_SECTION 7
#define MEM_SETUP_END_SECTION 11

// 16 auto pages
#define MEM_AUTO_START_SECTION 12
#define MEM_AUTO_END_SECTION 42

// amount of data pages
#define DATA_PAGE_NUM_MANUAL (3 - 1)
#define DATA_PAGE_NUM_SETUP (3 - 1)
#define DATA_PAGE_NUM_AUTO (16 - 1)


/* ------------------------------------------------------------------*
 *            function headers
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault(void);
unsigned char MEM_EEPROM_ReadVar(t_eeprom_var var);
void MEM_EEPROM_WriteVar(t_eeprom_var var, unsigned char eeData);

unsigned char MEM_EEPROM_ReadData(unsigned char page, unsigned char entry, t_data var);
void MEM_EEPROM_LoadData(unsigned char entry, t_data byte, unsigned char eeData);

void MEM_EEPROM_WriteAutoEntry(struct PlantState *ps);
void MEM_EEPROM_WriteManualEntry(struct PlantState *ps);
void MEM_EEPROM_WriteSetupEntry(struct PlantState *ps);

void MEM_EEPROM_SetZero(void);
void MEM_EEPROM_WriteVarDefault_Short(void);

struct MemoryEntryPos MEM_FindNoEntry(t_text_buttons data);
struct MemoryEntryPos MEM_FindOldestEntry(t_text_buttons data);
struct MemoryEntryPos MEM_FindLatestEntry(t_text_buttons data);

#endif