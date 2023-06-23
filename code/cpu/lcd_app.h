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

// auto pages
void LCD_AutoPage(struct PlantState *ps);
void LCD_Auto_SetStateTime(struct PlantState *ps);
void LCD_Auto_SetStateOutput(struct PlantState *ps);
void LCD_Auto_ResetAutoSavePageState(struct PlantState *ps);
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
void LCD_Manual_SetState(struct PlantState *ps);
void LCD_Manual_ResetState(struct PlantState *ps, t_page save_page);

// setup pages
void LCD_SetupPage(struct PlantState *ps);
void LCD_Setup_Symbols(struct PlantState *ps);

// data pages
void LCD_DataPage(struct PlantState *ps);
void LCD_Data_Symbols(struct PlantState *ps);
void LCD_Data_WriteAutoEntryPage(unsigned char page);
void LCD_Data_WriteManualEntryPage(unsigned char page);
void LCD_Data_WriteSetupEntryPage(unsigned char page);

void LCD_Data_wPage(t_text_buttons data, unsigned char eep, unsigned char entry, bool half);
unsigned char LCD_Data_EEP_Minus(t_text_buttons data, unsigned char eep, unsigned char cnt);

// pin page
void LCD_PinPage_Main(struct PlantState *ps);
void LCD_PinPage_Init(struct PlantState *ps);

// calibration (only in init)
void LCD_Calibration(void);

#endif