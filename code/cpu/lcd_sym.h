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

// logo
void LCD_Sym_Logo(void);
void LCD_Sym_Clr_InfoSpace(void);
void LCD_Sym_Clr_DataEntrySpace(void);


/* ------------------------------------------------------------------*
 *            auto
 * ------------------------------------------------------------------*/

// set symbols
void LCD_Sym_Auto_SetManager(struct PlantState *ps);

// time and text
void LCD_Sym_Auto_PageTime_Print(struct Tms *tms);
void LCD_Sym_Auto_AirTime_Print(struct Tms *tms);
void LCD_Sym_Auto_PageTime_Update(struct PlantState *ps);
void LCD_Sym_Auto_Text(struct PlantState *ps);

// cycle general
void LCD_Sym_Auto_Main(struct PlantState *ps);
void LCD_Sym_Auto_CompressorNeg(void);
void LCD_Sym_Auto_CompressorPos(void);
void LCD_Sym_Auto_ClrActualCycleSpace(void);

// main cycles
void LCD_Sym_Auto_Zone(void);
void LCD_Sym_Auto_SetDown(void);
void LCD_Sym_Auto_PumpOff(void);
void LCD_Sym_Auto_Mud(void);
void LCD_Sym_Auto_CircOn(void);
void LCD_Sym_Auto_CircOff(void);
void LCD_Sym_Auto_AirOn(void);
void LCD_Sym_Auto_AirOff(void);
void LCD_Sym_Auto_AirPageSelect(t_page page);

// inflow pump
void LCD_Sym_Auto_Ip_Base(struct PlantState *ps);
void LCD_Sym_Auto_Ip_Time(unsigned char cho, struct Thms *t_hms);

// phosphor
void LCD_Sym_Auto_Ph(struct PlantState *ps);
void LCD_Sym_Auto_Ph_Time(struct Tms *tms);
void LCD_Sym_Auto_Ph_Time_Min(int min);
void LCD_Sym_Auto_Ph_Time_Sec(int sec);

// sonic
void LCD_Sym_Auto_SonicVal(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            manual
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Manual_GetSymbolPosition(t_any_symbol sym);
void LCD_Sym_Manual_Draw(t_any_symbol sym);
void LCD_Sym_Manual_AllSymbols(void);

void LCD_Sym_Manual_Main(struct PlantState *ps);
void LCD_Sym_Manual_Text(struct PlantState *ps);
void LCD_Sym_Manual_PageTime_Min(int min);
void LCD_Sym_Manual_PageTime_Sec(int sec);
void LCD_Sym_Manual_PageTime_Print(struct PlantState *ps);
void LCD_Sym_Manual_PageTime_Update(struct PlantState *ps);
void LCD_Sym_Manual_PumpOff_OkButton(bool p_sym);
void LCD_Sym_Manual_PumpOff_OkButton_Clr(void);


/* ------------------------------------------------------------------*
 *            setup
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Setup_GetSymbolPosition(t_any_symbol sym);
struct RowColPos LCD_Sym_Setup_GetSymbolPosition_Sub(t_any_symbol sym);
void LCD_Sym_Setup_Draw(t_any_symbol sym);
void LCD_Sym_Setup_Draw_Sub(t_any_symbol sym);
void LCD_Sym_Setup_AllSymbols(void);

void LCD_Sym_Setup_Page(void);
void LCD_Sym_Setup_Circulate(void);


void LCD_Sym_Setup_Circulate_OnText(bool positive);
void LCD_Sym_Setup_Circulate_OnValue(bool positive, int value);
void LCD_Sym_Setup_Circulate_OnTextValue(bool positive, int value);
void LCD_Sym_Setup_Circulate_OffText(bool positive);
void LCD_Sym_Setup_Circulate_OffValue(bool positive, int value);
void LCD_Sym_Setup_Circulate_OffTextValue(bool positive, int value);
void LCD_Sym_Setup_Circulate_TimeText(bool positive);
void LCD_Sym_Setup_Circulate_TimeValue(bool positive, int value);
void LCD_Sym_Setup_Circulate_TimeTextValue(bool positive, int value);
void LCD_Sym_Setup_Circulate_Change_Sensor(unsigned char sensor);
void LCD_Sym_Setup_Circulate_Change_Values(unsigned char on, unsigned char *p_var);


void LCD_Sym_Setup_Air(void);
void LCD_Sym_Setup_Air_Change_Values(unsigned char on, unsigned char *p_var);
void LCD_Sym_Setup_SetDown(void);
void LCD_Sym_Setup_PumpOff(void);
void LCD_Sym_Setup_PumpOff_Compressor(bool positive);
void LCD_Sym_Setup_PumpOff_Pump(bool positive);
void LCD_Sym_Setup_Mud(void);
void LCD_Sym_Setup_Compressor(void);
void LCD_Sym_Setup_Phosphor(void);
void LCD_Sym_Setup_InflowPump(void);
void LCD_Sym_Setup_InflowPump_Values(unsigned char cho, unsigned char *val);
void LCD_Sym_Setup_InflowPump_Text(unsigned char select);
void LCD_Sym_Setup_Cal(struct PlantState *ps);
void LCD_Sym_Setup_Cal_Level_Sym(bool positive);
void LCD_Sym_Setup_Cal_Level_Sonic(int level_cal);
void LCD_Sym_Setup_Cal_Level_MPX(int level_cal);
void LCD_Sym_Setup_Cal_MPXCountDown(int sec);
void LCD_Sym_Setup_Cal_Clr_MPXCountDown(void);
void LCD_Sym_Setup_Alarm(struct PlantState *ps);
void LCD_Sym_Setup_Watch_Mark(t_DateTime time, unsigned char *p_dT);
void LCD_Sym_Setup_Watch_DateTime(t_DateTime time, unsigned char value);
void LCD_Sym_Setup_Watch(void);
void LCD_Sym_Setup_Zone(void);
void LCD_Sym_Setup_Pump(unsigned char mark);



/* ------------------------------------------------------------------*
 *            data
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_Page(void);
void LCD_Sym_Data_ActualPageNum(int page_num);
void LCD_Sym_Data_TotalPageNum(int page_num);
void LCD_Sym_Data_Auto(void);
void LCD_Sym_Data_Manual(void);
void LCD_Sym_Data_Setup(void);

void LCD_Sym_DataArrows(void);

void LCD_Sym_Data_Sonic(struct PlantState *ps);
void LCD_Sym_Data_Sonic_ReadSversion(struct PlantState *ps);
void LCD_Sym_Data_Sonic_ClearRecording(struct PlantState *ps);
void LCD_Sym_Data_Sonic_SingleShot(struct PlantState *ps);
void LCD_Sym_Data_Sonic_SequentialShots(struct PlantState *ps);
void LCD_Sym_Data_Sonic_SingleTemp(struct PlantState *ps);
void LCD_Sym_Data_Sonic_SequentialTemp(struct PlantState *ps);
void LCD_Sym_Data_Sonic_AutoText(void);
void LCD_Sym_Data_Sonic_NoUs(void);
void LCD_Sym_Data_Sonic_BootOk(void);
void LCD_Sym_Data_Sonic_BootNoData(void);
void LCD_Sym_Data_Sonic_BootError(void);
void LCD_Sym_Data_Sonic_BootNone(void);
void LCD_Sym_Data_Sonic_BootSucess(void);

void LCD_Sym_Data_WriteAutoEntry(unsigned char pa, unsigned char eePage, unsigned char entry);
void LCD_Sym_Data_WriteManualEntry(unsigned char pa, unsigned char eePage, unsigned char entry);
void LCD_Sym_Data_WriteSetupEntry(unsigned char pa, unsigned char eePage, unsigned char entry);


/* ------------------------------------------------------------------*
 *            MPX
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX_AverageValue(t_page page, int av_value);
void LCD_Sym_MPX_Auto_DisabledLevelMeasure(void);
void LCD_Sym_MPX_Auto_MbarValue(int value);
void LCD_Sym_MPX_LevelPerc(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            pin
 * ------------------------------------------------------------------*/

void LCD_Sym_PinPage(void);
void LCD_Sym_Pin_RightMessage(void);
void LCD_Sym_Pin_WrongMessage(void);
void LCD_Sym_Pin_OpHoursMessage(void);
void LCD_Sym_Pin_ClearPinCode(void);
void LCD_Sym_Pin_PrintWholeTelNumber(struct TeleNr *tele_nr);
void LCD_Sym_Pin_PrintOneTelNumberDigit(unsigned char digit, unsigned char pos);
void LCD_pPinButtons(unsigned char pPin);
void LCD_nPinButtons(unsigned char nPin);
void LCD_Sym_Pin_DelDigits(void);
void LCD_Sym_Pin_WriteDigit(unsigned char pin, unsigned char code_pos);


/* ------------------------------------------------------------------*
 *            control
 * ------------------------------------------------------------------*/

void LCD_Sym_TextButton(t_text_buttons text, unsigned char pos);
void LCD_Sym_ControlButtons(t_CtrlButtons ctrl);
void LCD_Sym_ControlButtons2(t_CtrlButtons ctrl);
void LCD_Sym_Pin_OkButton(unsigned char on);


/* ------------------------------------------------------------------*
 *            Misc
 * ------------------------------------------------------------------*/

void LCD_Sym_MarkTextButton(t_text_buttons text);
void LCD_Sym_WriteCtrlButton(void);
void LCD_Sym_WriteCtrlButton2(void);


/* ------------------------------------------------------------------*
 *            CAN
 * ------------------------------------------------------------------*/

void LCD_Sym_CAN_MCPFail(void);


/* ------------------------------------------------------------------*
 *            sonic
 * ------------------------------------------------------------------*/

struct RowColPos LCD_Sym_Sonic_NoUS_Pos(t_page page);
void LCD_Sym_Sonic_NoUS_Clear(struct PlantState *ps);
void LCD_Sym_Sonic_NoUS_Message(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            error
 * ------------------------------------------------------------------*/

void LCD_Sym_Error(unsigned char err);


/* ------------------------------------------------------------------*
 *            timer ic
 * ------------------------------------------------------------------*/

void LCD_Sym_Auto_WorldTime(struct PlantState *ps);
void LCD_Sym_Auto_Date(struct PlantState *ps);


/* ------------------------------------------------------------------*
 *            other
 * ------------------------------------------------------------------*/

void LCD_Sym_Data_EndText(void);
t_any_symbol LCD_Sym_GetAntiSymbol(t_any_symbol sym);

#endif