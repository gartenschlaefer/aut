// --
// EADOGXL160-7 Display applications

// include guard
#ifndef LCD_APP_H   
#define LCD_APP_H

#include <stdbool.h>
#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            main
 * ------------------------------------------------------------------*/

void LCD_AutoPage(struct PlantState *ps);
void LCD_ManualPage(struct PlantState *ps);
void LCD_SetupPage(struct PlantState *ps);
void LCD_DataPage(struct PlantState *ps);

// pin page
void LCD_PinPage_Main(struct PlantState *ps);
void LCD_PinPage_Init(struct PlantState *ps);



/* ------------------------------------------------------------------*
 *            states and symbols
 * ------------------------------------------------------------------*/

void LCD_Auto_SetStateTime(struct PlantState *ps);
void LCD_Auto_SetStateOutput(struct PlantState *ps);
void LCD_Auto_ResetAutoSavePageState(struct PlantState *ps);
void LCD_Auto_CountDownEndAction(struct PlantState *ps);

// air state (for circulate and air cycle)
void LCD_AirState_Manager(struct PlantState *ps);

// inflow pump
void LCD_Auto_InflowPump_Init(struct PlantState *ps);
void LCD_Auto_InflowPump_Update(struct PlantState *ps);

// phosphor
void LCD_Auto_Phosphor_Init(struct PlantState *ps);
void LCD_Auto_Phosphor_Update(struct PlantState *ps);

void LCD_Manual_SetState(struct PlantState *ps);
void LCD_Manual_ResetState(struct PlantState *ps, t_page save_page);

void LCD_Setup_Symbols(struct PlantState *ps);
void LCD_Data_Symbols(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            data page write entries
 * ------------------------------------------------------------------*/

void LCD_Entry_Clr(void);
void LCD_WriteAutoEntryPage(unsigned char page);
void LCD_WriteManualEntryPage(unsigned char page);
void LCD_WriteSetupEntryPage(unsigned char page);

void LCD_wPage(t_text_buttons data, unsigned char eep, unsigned char entry, bool half);
unsigned char LCD_eep_minus(t_text_buttons data, unsigned char eep, unsigned char cnt);
void LCD_Sym_Data_EndText(void);

void LCD_AirState_SetAutoStartTime(struct PlantState *ps);

/* ------------------------------------------------------------------*
 *            calibration init
 * ------------------------------------------------------------------*/

void LCD_Calibration(void);

#endif