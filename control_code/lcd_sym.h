/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Display-Symbol-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Symbol Header File, this Symbols are called in SL_Display_app.c
* ------------------------------------------------------------------
*	Date:			    26.07.2011
* lastChanges:  10.08.2015
\**********************************************************************/


/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Auto
 * ------------------------------------------------------------------*/

void LCD_AutoSet_Page		(void);
void LCD_AutoSet_Zone		(unsigned char min, unsigned char sec);
void LCD_AutoSet_SetDown(unsigned char min, unsigned char sec);
void LCD_AutoSet_PumpOff(unsigned char min, unsigned char sec);
void LCD_AutoSet_Mud		(unsigned char min, unsigned char sec);
void LCD_SymbolAuto_IP  (t_page page, t_FuncCmd cmd);
void LCD_WriteAutoVar_IP(unsigned char cho, unsigned char *t_ip);
void LCD_SymbolAuto_Ph(t_FuncCmd state);
void LCD_WriteAutoVar_Ph(unsigned char min, unsigned char sec);

void LCD_AutoText(void);
void LCD_Auto_SonicVal(t_page page, int sonic);
void LCD_AutoCountDown(unsigned char min, unsigned char sec);
void LCD_WriteAuto_IP_Sensor(void);
void LCD_WriteAutoVar(int min, int sec);
void LCD_WriteAutoVar_Comp(int min, int sec);

int LCD_AutoRead_StartTime(t_page page);
void LCD_AutoAirSym        (t_page page);

/* ------------------------------------------------------------------*
 * 						Manual
 * ------------------------------------------------------------------*/

void LCD_ManualText		  (int min, unsigned char sec);
void LCD_ManualSet_Page	(int min, unsigned char sec);
void LCD_WriteManualVar(int min, unsigned char sec);

/* ------------------------------------------------------------------*
 * 						Setup
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Page			(void);
void LCD_SetupSet_Circulate	(void);
void LCD_SetupSet_Air			  (void);
void LCD_SetupSet_SetDown		(void);
void LCD_SetupSet_PumpOff		(void);
void LCD_SetupSet_Mud			  (void);
void LCD_SetupSet_Compressor(void);
void LCD_SetupSet_Phosphor	(void);
void LCD_SetupSet_InflowPump(void);
void LCD_SetupSet_Cal			  (void);
void LCD_SetupSet_Alarm			(void);
void LCD_SetupSet_Watch			(void);
void LCD_SetupSet_Zone			(void);

void LCD_SetupSymbols (t_SetupSym sym);
void LCD_DateTime			(t_DateTime time, unsigned char value);

void LCD_Setup_IPValues (unsigned char cho, unsigned char *val);
void LCD_Setup_IPText		(unsigned char select);

/* ------------------------------------------------------------------*
 * 						Data
 * ------------------------------------------------------------------*/

void LCD_DataSet_Page		(void);
void LCD_DataSet_Auto		(void);
void LCD_DataSet_Manual	(void);
void LCD_DataSet_Setup	(void);
void LCD_DataSet_Sonic	(void);

void LCD_WriteDataHeader_Auto	  (void);
void LCD_WriteDataHeader_Manual (void);
void LCD_WriteDataHeader_Setup	(void);
void LCD_WriteDataArrows		    (void);
void LCD_Data_SonicWrite			    (t_FuncCmd cmd, int shot);

void LCD_WriteAutoEntry		(unsigned char pa, unsigned char eePage,
unsigned char entry);
void LCD_WriteManualEntry	(unsigned char pa, unsigned char eePage,
unsigned char entry);
void LCD_WriteSetupEntry	(unsigned char pa, unsigned char eePage,
unsigned char entry);

/* ------------------------------------------------------------------*
 * 						MPX
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX(t_FuncCmd cmd, int value);


/* ------------------------------------------------------------------*
 * 						Pin
 * ------------------------------------------------------------------*/

void LCD_PinSet_Page	(void);
void LCD_Pin_Write    (t_FuncCmd cmd, TelNr nr);
void LCD_pPinButtons	(unsigned char pPin);
void LCD_nPinButtons	(unsigned char nPin);


/* ------------------------------------------------------------------*
 * 						Control
 * ------------------------------------------------------------------*/

void LCD_TextButton			  (t_textButtons text, unsigned char pos);
void LCD_ControlButtons		(t_CtrlButtons ctrl);
void LCD_ControlButtons2	(t_CtrlButtons ctrl);
void LCD_Pin_WriteOK      (unsigned char on);


/* ------------------------------------------------------------------*
 * 						Misc
 * ------------------------------------------------------------------*/

void LCD_MarkTextButton		(t_textButtons text);
void LCD_Mark_ManualSymbol(t_SetupSym sym);

void LCD_WriteCtrlButton	(void);
void LCD_WriteCtrlButton2	(void);

void LCD_OnValue		(unsigned char value);
void LCD_OffValue		(unsigned char value);
void LCD_OnValueNeg	(unsigned char value);
void LCD_OffValueNeg(unsigned char value);


/* ------------------------------------------------------------------*
 * 						Sonic
 * ------------------------------------------------------------------*/

unsigned char LCD_Sym_NoUS(t_page page, t_FuncCmd cmd);




/**********************************************************************\
 * END of file
\**********************************************************************/
