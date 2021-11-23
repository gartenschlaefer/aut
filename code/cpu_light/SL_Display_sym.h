/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Display-Symbol-HeaderFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Symbol Header File, this Symbols are called in SL_Display_app.c
* ------------------------------------------------------------------														
*	Date:			26.07.2011	
* 	lastChanges:													
\**********************************************************************/


/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Auto
 * -------------------------------------------------------------------*/

void 			LCD_AutoText			(void);
void 			LCD_AutoCountDown		(unsigned char min, unsigned char sec);
void 			LCD_WriteAuto_IP_pSym	(void);
void 			LCD_WriteAuto_IP_nSym	(void);
void 			LCD_WriteAuto_IP_Sensor	(void);

void 			LCD_WriteAutoVar		(int min, int sec);
void 			LCD_WriteAutoVar_Comp	(int min, int sec);
t_page 			LCD_Write_AirVar		(t_page	page, int sec, t_FuncCmd cmd);
unsigned char 	LCD_AutoRead_StartTime	(t_page page, unsigned char on);

/* -------------------------------------------------------------------*
 * 						Manual
 * -------------------------------------------------------------------*/

void LCD_ManualText		(unsigned char min, unsigned char sec);
void LCD_ManualSet_Page	(unsigned char min, unsigned char sec);
void LCD_WriteManualVar	(unsigned char min, unsigned char sec);

/* -------------------------------------------------------------------*
 * 						Setup
 * -------------------------------------------------------------------*/

void LCD_SetupSymbols		(t_SetupSym sym);
void LCD_DateTime			(t_DateTime time, unsigned char value);


/* -------------------------------------------------------------------*
 * 						Data
 * -------------------------------------------------------------------*/

void LCD_WriteDataHeader_Auto	(void);
void LCD_WriteDataHeader_Manual	(void);
void LCD_WriteDataHeader_Setup	(void);

void LCD_WriteAutoEntry		(unsigned char pa, unsigned char eePage, unsigned char entry);
void LCD_WriteManualEntry	(unsigned char pa, unsigned char eePage, unsigned char entry);
void LCD_WriteSetupEntry	(unsigned char pa, unsigned char eePage, unsigned char entry);


/* -------------------------------------------------------------------*
 * 						Pin
 * -------------------------------------------------------------------*/

void LCD_pPinButtons	(unsigned char pPin);
void LCD_nPinButtons	(unsigned char nPin);


/* -------------------------------------------------------------------*
 * 						Control
 * -------------------------------------------------------------------*/

void LCD_TextButton			(t_textSymbols text, unsigned char pos);
void LCD_ControlButtons		(t_CtrlButtons ctrl);
void LCD_ControlButtons2	(t_CtrlButtons ctrl);


/* -------------------------------------------------------------------*
 * 						Misc
 * -------------------------------------------------------------------*/

void LCD_MarkTextButton		(t_textSymbols text);
void LCD_Mark_ManualSymbol	(t_SetupSym sym);

void LCD_WriteCtrlButton	(void);	
void LCD_WriteCtrlButton2	(void);

void LCD_OnValue			(unsigned char value);
void LCD_OffValue			(unsigned char value);
void LCD_OnValueNeg			(unsigned char value);
void LCD_OffValueNeg		(unsigned char value);






/* ===================================================================*
 * 						FUNCTIONS - API Set Pages
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Auto
 * -------------------------------------------------------------------*/

void LCD_AutoSet_Page		(void);
void LCD_AutoSet_Zone		(unsigned char min, unsigned char sec);
void LCD_AutoSet_SetDown	(unsigned char min, unsigned char sec);
void LCD_AutoSet_PumpOff	(unsigned char min, unsigned char sec);

void LCD_AutoSet_Mud		(unsigned char min, unsigned char sec);
void LCD_AutoSet_Circulate	(unsigned char min, unsigned char sec);
void LCD_AutoSet_Air		(unsigned char min, unsigned char sec);


/* -------------------------------------------------------------------*
 * 						Setup
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Page			(void);
void LCD_SetupSet_Circulate		(void);
void LCD_SetupSet_Air			(void);
void LCD_SetupSet_SetDown		(void);
void LCD_SetupSet_PumpOff		(void);
void LCD_SetupSet_Mud			(void);
void LCD_SetupSet_Compressor	(void);
void LCD_SetupSet_Phosphor		(void);
void LCD_SetupSet_InflowPump	(void);
void LCD_SetupSet_Cal			(void);
void LCD_SetupSet_Alarm			(void);
void LCD_SetupSet_Watch			(void);
void LCD_SetupSet_Zone			(void);


/* -------------------------------------------------------------------*
 * 						Data
 * -------------------------------------------------------------------*/

void LCD_DataSet_Page		(void);
void LCD_DataSet_Auto		(void);
void LCD_DataSet_Manual		(void);
void LCD_DataSet_Setup		(void);


/* -------------------------------------------------------------------*
 * 						PIN
 * -------------------------------------------------------------------*/

void LCD_PinSet_Page		(void);








/**********************************************************************\
 * END of xmA_Display_app.h
\**********************************************************************/

