/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Display-App-HeaderFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Application-File Header for the EADOGXL160-7 Display
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:											
\**********************************************************************/



/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/


/* ===================================================================*
 * 						Pages
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Main
 * -------------------------------------------------------------------*/

t_page 			LCD_AutoPage			(t_page autoPage);
t_page 			LCD_ManualPage			(t_page manualPage);
t_page 			LCD_SetupPage			(t_page setupPage);
t_page			LCD_DataPage			(t_page dataPage);
t_page 			LCD_PinPage				(t_page pinPage);


/* -------------------------------------------------------------------*
 * 						Auto Pages
 * -------------------------------------------------------------------*/

t_page 			LCD_AutoPage_Zone		(int *p_min, int *p_sec);
t_page			LCD_AutoPage_SetDown	(int *p_min, int *p_sec);
t_page			LCD_AutoPage_PumpOff	(int *p_min, int *p_sec);	
t_page 			LCD_AutoPage_Mud		(int *p_min, int *p_sec);
t_page 			LCD_AutoPage_Circulate	(int *p_min, int *p_sec);
t_page 			LCD_AutoPage_Air		(int *p_min, int *p_sec);

t_FuncCmd 		LCD_Auto_InflowPump		(unsigned char rSec, t_FuncCmd cmd);
void 			LCD_Auto_Phosphor		(unsigned char rSec, t_FuncCmd cmd);


/* -------------------------------------------------------------------*
 * 						Manual Pages
 * -------------------------------------------------------------------*/

t_page 			LCD_ManualPage_Main			(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_ManualPage_Circulate	(unsigned char *p_Min, unsigned char *p_Sec);	
t_page 			LCD_ManualPage_Air			(unsigned char *p_Min, unsigned char *p_Sec);	
t_page			LCD_ManualPage_SetDown		(unsigned char *p_Min, unsigned char *p_Sec); 	
t_page 			LCD_ManualPage_PumpOff		(unsigned char *p_Min, unsigned char *p_Sec);	
t_page			LCD_ManualPage_Mud			(unsigned char *p_Min, unsigned char *p_Sec); 		
t_page			LCD_ManualPage_Compressor	(unsigned char *p_Min, unsigned char *p_Sec); 
t_page			LCD_ManualPage_Phosphor		(unsigned char *p_Min, unsigned char *p_Sec); 	
t_page 			LCD_ManualPage_InflowPump	(unsigned char *p_Min, unsigned char *p_Sec); 


/* -------------------------------------------------------------------*
 * 						Setup Pages
 * -------------------------------------------------------------------*/

t_page 			LCD_SetupPage_Main			(unsigned char *p_Min, unsigned char *p_Sec);
t_page 			LCD_SetupPage_Circulate		(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_SetupPage_Air			(unsigned char *p_Min, unsigned char *p_Sec);
t_page 			LCD_SetupPage_SetDown		(unsigned char *p_Min, unsigned char *p_Sec);
t_page 			LCD_SetupPage_PumpOff		(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_SetupPage_Mud			(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_SetupPage_Compressor	(unsigned char *p_Min, unsigned char *p_Sec);
t_page 			LCD_SetupPage_Phosphor		(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_SetupPage_InflowPump	(unsigned char *p_Min, unsigned char *p_Sec);
t_page 			LCD_SetupPage_Cal			(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_SetupPage_Alarm			(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_SetupPage_Watch			(unsigned char *p_Min, unsigned char *p_Sec);
t_page 			LCD_SetupPage_Zone			(unsigned char *p_Min, unsigned char *p_Sec);

/* -------------------------------------------------------------------*
 * 						Data Pages
 * -------------------------------------------------------------------*/

t_page			LCD_DataPage_Main	(unsigned char *p_Min, unsigned char *p_Sec);
t_page			LCD_DataPage_Auto	(unsigned char *p_Min, unsigned char *p_Sec);	
t_page 			LCD_DataPage_Manual	(unsigned char *p_Min, unsigned char *p_Sec);	
t_page 			LCD_DataPage_Setup	(unsigned char *p_Min, unsigned char *p_Sec); 	




/* ===================================================================*
 * 						Set up Pages
 * ===================================================================*/

void LCD_AutoSet		(t_page aPage, int *p_Min, int *p_Sec);
void LCD_AutoSet_Symbol	(t_page aPage, int aMin, int aSec);

void LCD_ManualSet		(t_page manPage, unsigned char *p_Min, unsigned char *p_Sec);
void LCD_SetupSet		(t_page setPage, unsigned char *p_Min, unsigned char *p_Sec);
void LCD_DataSet		(t_page setPage, unsigned char *p_Min, unsigned char *p_Sec);




/* ===================================================================*
 * 						Data Page write Entrys
 * ===================================================================*/

void 			LCD_WriteAutoEntryPage	(t_eeDataPage page);
void 			LCD_WriteManualEntryPage(t_eeDataPage page);
void 			LCD_WriteSetupEntryPage	(t_eeDataPage page);

void 			LCD_wPage		(t_textSymbols data, unsigned char eep, unsigned char entry);
unsigned char 	LCD_eep_minus	(unsigned char eep, unsigned char cnt);




/* ===================================================================*
 * 						Calibration Init
 * ===================================================================*/

void 			LCD_Calibration(void);







/**********************************************************************\
 * END of xmA_Display_app.h
\**********************************************************************/

