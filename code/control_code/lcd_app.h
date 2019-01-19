/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Display-App-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
	Application-File Header for the EADOGXL160-7 Display
* ------------------------------------------------------------------
*	Date:			    13.07.2011
* lastChanges:  25.12.2015
\**********************************************************************/


/* ==================================================================*
 * 						Enumerations
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Pages
 * ------------------------------------------------------------------*/
typedef enum
{	AutoPage, 			ManualPage,			SetupPage,		DataPage,
	PinManual,			PinSetup, 			PinModem,     AutoZone,

	AutoSetDown,	AutoMud,
	AutoCirc,     AutoCircOff,  AutoPumpOff,
	AutoAir,			AutoAirOff,

	SetupMain,			  SetupCirculate,
	SetupAir, 			  SetupSetDown,
	SetupPumpOff,		  SetupMud,
	SetupCompressor,	SetupPhosphor,
	SetupInflowPump,	SetupCal,
	SetupCalPressure,	SetupAlarm,
	SetupWatch,			  SetupZone,

	ManualMain,			ManualPumpOff_On,
	ManualCirc,	    ManualCircOff,
	ManualAir,
	ManualSetDown,	ManualPumpOff,
	ManualMud,			ManualCompressor,
	ManualPhosphor,	ManualInflowPump,

	DataMain,			  DataAuto,			  DataManual,			DataSetup,
	DataSonic,      DataSonicAuto,
	DataSonicBoot,  DataSonicBootR, DataSonicBootW,

	ErrorMPX,   ErrorTreat
}t_page;


/* ------------------------------------------------------------------*
 * 						EEPages
 * ------------------------------------------------------------------*/

typedef enum
{	Page1,				Page2,				Page3,			Page4,
	Page5,				Page6,				Page7,			Page8,
}t_eeDataPage;



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Main
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage		(t_page autoPage);
t_page LCD_ManualPage	(t_page manualPage);
t_page LCD_SetupPage	(t_page setupPage);
t_page LCD_DataPage		(t_page dataPage);
t_page LCD_PinPage		(t_page pinPage);


/* ------------------------------------------------------------------*
 * 						Auto Pages
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_Zone		(int *p_min, int *p_sec);
t_page LCD_AutoPage_SetDown	(int *p_min, int *p_sec);
t_page LCD_AutoPage_PumpOff	(int *p_min, int *p_sec);
t_page LCD_AutoPage_Mud		  (int *p_min, int *p_sec);
t_page LCD_AutoPage_Circ    (t_page page, int *p_min, int *p_sec);
t_page LCD_AutoPage_Air		  (t_page page, int *p_min, int *p_sec);

t_page LCD_Write_AirVar(t_page	page, int sec, t_FuncCmd cmd);
t_FuncCmd LCD_Auto_InflowPump(t_page page, int sec, t_FuncCmd cmd);
void LCD_Auto_Phosphor(int sec, t_FuncCmd cmd);


/* ------------------------------------------------------------------*
 * 						Manual Pages
 * ------------------------------------------------------------------*/

t_page LCD_ManualCD(t_page page, int *p_min, int *p_sec);
t_page LCD_ManualPage_Main(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_SetDown(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_PumpOff(t_page page, int *p_Min, int *p_Sec);
t_page LCD_ManualPage_Mud(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_Compressor(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_Phosphor(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_InflowPump(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_Air(int *p_Min, int *p_Sec);
t_page LCD_ManualPage_Circ(t_page page, int *p_Min, int *p_Sec);



/* ------------------------------------------------------------------*
 * 						Setup Pages
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Main			  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Circulate  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Air			  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_SetDown	  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_PumpOff	  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Mud			  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Compressor (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Phosphor	  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_InflowPump	(int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Cal			  (int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Alarm			(int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Watch			(int *p_Min, int *p_Sec);
t_page LCD_SetupPage_Zone			  (int *p_Min, int *p_Sec);


/* ------------------------------------------------------------------*
 * 						Data Pages
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Main(int *p_Min, int *p_Sec);
t_page LCD_DataPage_Auto(int *p_Min, int *p_Sec);
t_page LCD_DataPage_Manual(int *p_Min, int *p_Sec);
t_page LCD_DataPage_Setup(int *p_Min, int *p_Sec);
t_page LCD_DataPage_Sonic(t_page page, int *p_Min, int *p_Sec);


/* ------------------------------------------------------------------*
 * 						Set up Pages
 * ------------------------------------------------------------------*/

void LCD_AutoSet		    (t_page aPage, int *p_Min, int *p_Sec);
void LCD_AutoSet_Symbol	(t_page aPage, int aMin, int aSec);

void LCD_ManualSet	(t_page manPage, int *p_Min, int *p_Sec);
void LCD_SetupSet		(t_page setPage, int *p_Min, int *p_Sec);
void LCD_DataSet		(t_page setPage, int *p_Min, int *p_Sec);


/* ------------------------------------------------------------------*
 * 						Data Page write Entrys
 * ------------------------------------------------------------------*/

void LCD_Entry_Clr(void);
void LCD_WriteAutoEntryPage	  (t_eeDataPage page);
void LCD_WriteManualEntryPage (t_eeDataPage page);
void LCD_WriteSetupEntryPage	(t_eeDataPage page);

void LCD_wPage(t_textButtons data, unsigned char eep, unsigned char entry);
unsigned char LCD_eep_minus	(unsigned char eep, unsigned char cnt);


/* ------------------------------------------------------------------*
 * 						Calibration Init
 * ------------------------------------------------------------------*/

void LCD_Calibration(void);







/**********************************************************************\
 * End of file
\**********************************************************************/
