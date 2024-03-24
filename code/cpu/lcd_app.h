// --
// EADOGXL160-7 Display applications

// include guard
#ifndef LCD_APP_H   
#define LCD_APP_H

#include <stdbool.h>
#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            functions
 * ------------------------------------------------------------------*/

void LCD_DisplayRefresh(struct PlantState *ps);
unsigned char LCD_PageCountDown(struct PlantState *ps);

// auto pages
void LCD_AutoPage_Init(struct PlantState *ps);
void LCD_AutoPage(struct PlantState *ps);
void LCD_Auto_CountDownEndAction(struct PlantState *ps);

// air state for manual and auto
void LCD_AirState_Init(struct PlantState *ps);
void LCD_AirState_Update(struct PlantState *ps);

// inflow pump
void LCD_Auto_InflowPump_Init(struct PlantState *ps);
void LCD_Auto_InflowPump_Update(struct PlantState *ps);

// phosphor
void LCD_Auto_Phosphor_Init(struct PlantState *ps);
void LCD_Auto_Phosphor_Update(struct PlantState *ps);

// manual pages
void LCD_ManualPage(struct PlantState *ps);

// setup pages
void LCD_SetupPage(struct PlantState *ps);

// data pages
void LCD_DataPage(struct PlantState *ps);
void LCD_Data_WriteLogEntries(unsigned char entry_page, t_eeprom_memory_section eeprom_mem_section);

// pin page
void LCD_PinPage_Main(struct PlantState *ps);

// calibration (only in init)
void LCD_Calibration(void);

#endif