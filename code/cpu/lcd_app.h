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
void LCD_PinPage(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            auto pages
 * ------------------------------------------------------------------*/

// void LCD_AutoPage_Zone(struct PlantState *ps);
// void LCD_AutoPage_SetDown(struct PlantState *ps);
// void LCD_AutoPage_PumpOff(struct PlantState *ps);
// void LCD_AutoPage_Mud(struct PlantState *ps);
// void LCD_AutoPage_Circ(struct PlantState *ps);
// void LCD_AutoPage_Air(struct PlantState *ps);



/* ------------------------------------------------------------------*
 *            manual pages
 * ------------------------------------------------------------------*/

//void LCD_ManualCD(struct PlantState *ps);
// void LCD_ManualPage_Main(struct PlantState *ps);
// void LCD_ManualPage_SetDown(struct PlantState *ps);
// void LCD_ManualPage_PumpOff(struct PlantState *ps);
// void LCD_ManualPage_Mud(struct PlantState *ps);
// void LCD_ManualPage_Compressor(struct PlantState *ps);
// void LCD_ManualPage_Phosphor(struct PlantState *ps);
// void LCD_ManualPage_InflowPump(struct PlantState *ps);
// void LCD_ManualPage_Air(struct PlantState *ps);
// void LCD_ManualPage_Circ(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            setup pages
 * ------------------------------------------------------------------*/

// void LCD_SetupPage_Main(struct PlantState *ps);
// void LCD_SetupPage_Circulate(struct PlantState *ps);
// void LCD_SetupPage_Air(struct PlantState *ps);
// void LCD_SetupPage_SetDown(struct PlantState *ps);
// void LCD_SetupPage_PumpOff(struct PlantState *ps);
// void LCD_SetupPage_Mud(struct PlantState *ps);
// void LCD_SetupPage_Compressor(struct PlantState *ps);
// void LCD_SetupPage_Phosphor(struct PlantState *ps);
// void LCD_SetupPage_InflowPump(struct PlantState *ps);
// void LCD_SetupPage_Cal(struct PlantState *ps);
// void LCD_SetupPage_Alarm(struct PlantState *ps);
// void LCD_SetupPage_Watch(struct PlantState *ps);
// void LCD_SetupPage_Zone(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            data pages
 * ------------------------------------------------------------------*/

// void LCD_DataPage_Main(struct PlantState *ps);
// void LCD_DataPage_Auto(struct PlantState *ps);
// void LCD_DataPage_Manual(struct PlantState *ps);
// void LCD_DataPage_Setup(struct PlantState *ps);
// void LCD_DataPage_Sonic(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            states and symbols
 * ------------------------------------------------------------------*/

void LCD_Auto_SetState(struct PlantState *ps);
void LCD_Auto_Symbols(struct PlantState *ps);
void LCD_Auto_CountDownEndAction(struct PlantState *ps);
void LCD_AirState(struct PlantState *ps, t_FuncCmd cmd);
t_FuncCmd LCD_Auto_InflowPump(struct PlantState *ps, t_FuncCmd cmd);
void LCD_Auto_Phosphor(struct PlantState *ps, t_FuncCmd cmd);

void LCD_Manual_SetState(struct PlantState *ps);
void LCD_Manual_ResetState(t_page save_page);
void LCD_Manual_Symbols(struct PlantState *ps);

void LCD_Setup_Symbols(struct PlantState *ps);
void LCD_Data_Symbols(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            data page write entries
 * ------------------------------------------------------------------*/

void LCD_Entry_Clr(void);
void LCD_WriteAutoEntryPage(unsigned char page);
void LCD_WriteManualEntryPage(unsigned char page);
void LCD_WriteSetupEntryPage(unsigned char page);

void LCD_wPage(t_textButtons data, unsigned char eep, unsigned char entry, bool half);
unsigned char LCD_eep_minus(t_textButtons data, unsigned char eep, unsigned char cnt);
void LCD_Sym_Data_EndText(void);

int LCD_AutoRead_StartTime(struct PlantState *ps);

/* ------------------------------------------------------------------*
 *            calibration init
 * ------------------------------------------------------------------*/

void LCD_Calibration(void);

#endif