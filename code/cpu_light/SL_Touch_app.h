/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Touchpanel-App-HeaderFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Application-Header-File for the Touchpanel of 
	the EADOGXL160-7 Display
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:											
\**********************************************************************/



/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Touch Matrix
 * -------------------------------------------------------------------*/
 
unsigned char Touch_Matrix(void);


/* -------------------------------------------------------------------*
 * 						Main Linker
 * -------------------------------------------------------------------*/
 
t_page Touch_AutoLinker		(unsigned char matrix, t_page page, int *p_min);
t_page Touch_ManualLinker	(unsigned char matrix, t_page page);
t_page Touch_SetupLinker	(unsigned char matrix, t_page page);
t_page Touch_DataLinker		(unsigned char matrix, t_page page);
t_page Touch_PinLinker		(unsigned char matrix, t_page page);


/* -------------------------------------------------------------------*
 * 						Manual Linker
 * -------------------------------------------------------------------*/

void Touch_ManualPumpOffLinker	(unsigned char matrix);


/* -------------------------------------------------------------------*
 * 						Setup Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupCirculateLinker	(unsigned char matrix, t_page page);
t_page Touch_SetupAirLinker			(unsigned char matrix, t_page page);
t_page Touch_SetupWatchLinker		(unsigned char matrix, t_page page);
t_page Touch_SetupSetDownLinker		(unsigned char matrix, t_page page);
t_page Touch_SetupPumpOffLinker		(unsigned char matrix, t_page page);
t_page Touch_SetupMudLinker			(unsigned char matrix, t_page page);
t_page Touch_SetupCompressorLinker	(unsigned char matrix, t_page page);
t_page Touch_SetupPhosphorLinker	(unsigned char matrix, t_page page);
t_page Touch_SetupInflowPumpLinker	(unsigned char matrix, t_page page);
t_page Touch_SetupCalLinker			(unsigned char matrix, t_page page);
t_page Touch_SetupAlarmLinker		(unsigned char matrix, t_page page);
t_page Touch_SetupZoneLinker		(unsigned char matrix, t_page page);

/* -------------------------------------------------------------------*
 * 						Data Linker
 * -------------------------------------------------------------------*/

t_page Touch_DataAutoLinker			(unsigned char matrix, t_page pa);
t_page Touch_DataManualLinker		(unsigned char matrix, t_page pa);
t_page Touch_DataSetupLinker		(unsigned char matrix, t_page pa);





/**********************************************************************\
 * END of xmA_Touch_app.h
\**********************************************************************/

