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

t_page LCD_AutoPage(t_page page, struct PlantState *ps);
t_page LCD_ManualPage(t_page manualPage, struct PlantState *ps);
t_page LCD_SetupPage(t_page setupPage, struct PlantState *ps);
t_page LCD_DataPage(t_page dataPage, struct PlantState *ps);
t_page LCD_PinPage(t_page pinPage, struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            auto pages
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_Zone(int *p_min, int *p_sec, struct PlantState *ps);
t_page LCD_AutoPage_SetDown(int *p_min, int *p_sec, struct PlantState *ps);
t_page LCD_AutoPage_PumpOff(int *p_min, int *p_sec, struct PlantState *ps);
t_page LCD_AutoPage_Mud(int *p_min, int *p_sec, struct PlantState *ps);
t_page LCD_AutoPage_Circ(t_page page, int *p_min, int *p_sec, struct PlantState *ps);
t_page LCD_AutoPage_Air(t_page page, int *p_min, int *p_sec, struct PlantState *ps);

t_page LCD_Write_AirVar(t_page  page, int sec, t_FuncCmd cmd);
t_FuncCmd LCD_Auto_InflowPump(t_page page, int sec, t_FuncCmd cmd);
void LCD_Auto_Phosphor(int sec, t_FuncCmd cmd);


/* ------------------------------------------------------------------*
 *            manual pages
 * ------------------------------------------------------------------*/

t_page LCD_ManualCD(t_page page, int *p_min, int *p_sec, struct PlantState *ps);
t_page LCD_ManualPage_Main(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_SetDown(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_PumpOff(t_page page, int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_Mud(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_Compressor(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_Phosphor(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_InflowPump(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_Air(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_ManualPage_Circ(t_page page, int *p_Min, int *p_Sec, struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            setup pages
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Main(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Circulate(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Air(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_SetDown(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_PumpOff(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Mud(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Compressor(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Phosphor(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_InflowPump(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Cal(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Alarm(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Watch(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_SetupPage_Zone(int *p_Min, int *p_Sec, struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            data pages
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Main(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_DataPage_Auto(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_DataPage_Manual(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_DataPage_Setup(int *p_Min, int *p_Sec, struct PlantState *ps);
t_page LCD_DataPage_Sonic(t_page page, int *p_Min, int *p_Sec, struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            set up pages
 * ------------------------------------------------------------------*/

void LCD_AutoSet(t_page aPage, int *p_Min, int *p_Sec, struct PlantState *ps);
void LCD_AutoSet_Symbol(t_page aPage, int aMin, int aSec);

void LCD_ManualSet(t_page manPage, int *p_Min, int *p_Sec, struct PlantState *ps);
void LCD_SetupSet(t_page setPage, int *p_Min, int *p_Sec, struct PlantState *ps);
void LCD_DataSet(t_page setPage, int *p_Min, int *p_Sec, struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            data page write entries
 * ------------------------------------------------------------------*/

void LCD_Entry_Clr(void);
void LCD_WriteAutoEntryPage(unsigned char page);
void LCD_WriteManualEntryPage(unsigned char page);
void LCD_WriteSetupEntryPage(unsigned char page);

void LCD_wPage(t_textButtons data, unsigned char eep, unsigned char entry, bool half);
unsigned char LCD_eep_minus(t_textButtons data, unsigned char eep, unsigned char cnt);
void LCD_Data_EndText(void);


/* ------------------------------------------------------------------*
 *            calibration init
 * ------------------------------------------------------------------*/

void LCD_Calibration(void);

#endif