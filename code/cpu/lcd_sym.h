// --
// lcd symbolds

// include guard
#ifndef LCD_SYM_H   
#define LCD_SYM_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Main(void);
void LCD_Sym_Auto_Zone(struct Tms *tms);
void LCD_Sym_Auto_SetDown(struct Tms *tms);
void LCD_Sym_Auto_PumpOff(struct Tms *tms);
void LCD_Sym_Auto_Mud(struct Tms *tms);
void LCD_Sym_Auto_IP(struct PlantState *ps);
void LCD_Sym_Auto_IP_Time(unsigned char cho, struct Thms *t_hms);
void LCD_Sym_Auto_Ph(struct PlantState *ps);

void LCD_Sym_Auto_Text(struct PlantState *ps);
void LCD_Sym_Auto_SonicVal(t_page page, int sonic);
void LCD_Sym_Auto_CountDown(struct Tms *tms);

//void LCD_Sym_WriteAutoVar(struct PageState *ps);
//void LCD_Sym_WriteAutoVar_Comp(struct Tms *tms);

void LCD_Sym_Auto_PageTime(struct PlantState *ps, struct Tms *tms);

void LCD_Sym_AutoAir(t_page page);
void LCD_Sym_Logo(void);


/* ------------------------------------------------------------------*
 *            Manual
 * ------------------------------------------------------------------*/

void LCD_Sym_Manual_Text(struct PlantState *ps);
void LCD_Sym_Manual_PageTime(struct Tms *tms);


/* ------------------------------------------------------------------*
 *            Setup
 * ------------------------------------------------------------------*/

void LCD_Sym_Setup_Page(void);
void LCD_Sym_Setup_Circulate(void);
void LCD_Sym_Setup_Air(void);
void LCD_Sym_Setup_SetDown(void);
void LCD_Sym_Setup_PumpOff(void);
void LCD_Sym_Setup_Mud(void);
void LCD_Sym_Setup_Compressor(void);
void LCD_Sym_Setup_Phosphor(void);
void LCD_Sym_Setup_InflowPump(void);
void LCD_Sym_Setup_Cal(struct PlantState *ps);
void LCD_Sym_Setup_Alarm(void);
void LCD_Sym_Setup_Watch(void);
void LCD_Sym_Setup_Zone(void);

void LCD_Sym_SetupSymbols(t_SetupSym sym);
void LCD_DateTime(t_DateTime time, unsigned char value);

void LCD_Setup_IPValues(unsigned char cho, unsigned char *val);
void LCD_Setup_IPText(unsigned char select);


/* ------------------------------------------------------------------*
 *            Data
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Page(void);
void LCD_Sym_Data_Auto(void);
void LCD_Sym_Data_Manual(void);
void LCD_Sym_Data_Setup(void);
void LCD_Sym_Data_Sonic(void);

void LCD_Sym_DataArrows(void);
void LCD_Data_SonicWrite(t_FuncCmd cmd, int shot);

void LCD_WriteAutoEntry(unsigned char pa, unsigned char eePage, unsigned char entry);
void LCD_WriteManualEntry(unsigned char pa, unsigned char eePage, unsigned char entry);
void LCD_WriteSetupEntry(unsigned char pa, unsigned char eePage, unsigned char entry);


/* ------------------------------------------------------------------*
 *            MPX
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX(t_FuncCmd cmd, int value);


/* ------------------------------------------------------------------*
 *            Pin
 * ------------------------------------------------------------------*/

void LCD_PinSet_Page(void);
void LCD_Pin_Write(t_FuncCmd cmd, struct TelNr nr);
void LCD_pPinButtons(unsigned char pPin);
void LCD_nPinButtons(unsigned char nPin);


/* ------------------------------------------------------------------*
 *            Control
 * ------------------------------------------------------------------*/

void LCD_TextButton(t_textButtons text, unsigned char pos);
void LCD_ControlButtons(t_CtrlButtons ctrl);
void LCD_ControlButtons2(t_CtrlButtons ctrl);
void LCD_Pin_WriteOK(unsigned char on);


/* ------------------------------------------------------------------*
 *            Misc
 * ------------------------------------------------------------------*/

void LCD_Sym_MarkTextButton(t_textButtons text);
void LCD_Sym_Mark_ManualSymbol(t_SetupSym sym);

void LCD_WriteCtrlButton(void);
void LCD_WriteCtrlButton2(void);

void LCD_OnValue(unsigned char value);
void LCD_OffValue(unsigned char value);
void LCD_OnValueNeg(unsigned char value);
void LCD_OffValueNeg(unsigned char value);


/* ------------------------------------------------------------------*
 *            Sonic
 * ------------------------------------------------------------------*/

unsigned char LCD_Sym_NoUS(t_page page, t_FuncCmd cmd);


/* ------------------------------------------------------------------*
 *            error
 * ------------------------------------------------------------------*/

void LCD_Sym_Error(unsigned char err);


/* ------------------------------------------------------------------*
 *            timer ic
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_Time(t_FuncCmd cmd);
void LCD_Sym_Auto_Date(void);


/* ------------------------------------------------------------------*
 *            other
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_EndText(void);

#endif