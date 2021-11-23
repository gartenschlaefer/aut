/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Main-File
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Main-File of Steuerung Light SL
* ------------------------------------------------------------------														
*	Date:			27.05.2011  	
* 	lastChanges:											
\**********************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>

/* ===================================================================*
 * 						HeaderFiles
 * ===================================================================*/

#include "SL_Define_sym.h"

#include "SL_Display_driver.h"
#include "SL_Touch_driver.h"

#include "SL_Output_app.h"
#include "SL_Display_app.h"
#include "SL_Touch_app.h"
#include "SL_Eval_app.h"
#include "SL_Memory_app.h"

#include "SL_MPX_driver.h"
#include "SL_DS1302_driver.h"
#include "SL_MCP9800_driver.h"
#include "SL_AD8555_driver.h"

#include "SL_TC_func.h"
#include "SL_PORT_func.h"
#include "SL_TWI_func.h"
#include "SL_Clock_func.h"
#include "SL_Watchdog_func.h"
#include "SL_Memory_func.h"


/* ===================================================================*
 * 						Main
 * ===================================================================*/

int main(void)
{
	t_page 			page= ManualPage;		//StartPage

	//Init
	Clock_Init();							//Init Clock
	PORT_Init();							//Init PORT
	ADC_Init();								//Init ADC-Channels
	
	TWI_Master_Init();						//Init TWI
	LCD_HardwareRst();						//Display Reset
	TC_Main_Wait();							//Wait 200ms
	
	LCD_Init();                          	//Init LCD 
	TC_Main_Wait();							//Wait 200ms
	LCD_Clean();							//Display Clean
	
	Watchdog_Init();						//Init Watchdog
	AD8555_Init();							//Init Amplifier
	DS1302_Init();							//Init DS1302

	/*--------------------------------------------------Default-Init-Main
	LCD_Backlight(Backlight_ON);	//Bachlight ON
	DS1302_WriteDefault();			//DS1302
	MEM_EEPROM_Init();				//Memory
	Watchdog_Restart();				//Watchdog restart
	LCD_Calibration();				//Touch-CAL+while(1)
	//---------------------------------------------------*/

	LCD_Backlight(Backlight_ON);
	TC_Main_Wait();							//Wait 200ms
		
	TC_MainAuto_SafetyTC(_init);			//Safety Timer if DS1302 fail

	while(1)
	{
		Watchdog_Restart();
		PORT_Bootloader();
		PORT_RunTime();

		//--------------------------------------------------------------GreatLinker--
		switch(page)
		{
			case AutoPage:			page= LCD_AutoPage(page);		break;
			case ManualPage:		page= LCD_ManualPage(page);		break;
			case SetupPage:			page= LCD_SetupPage(page);		break;
			case DataPage:			page= LCD_DataPage(page);		break;

			case PinManual:			page= LCD_PinPage(page);		break;	
			case PinSetup: 			page= LCD_PinPage(page);		break;
			
			case AutoZone:			page= LCD_AutoPage(page);
			case AutoSetDown:		page= LCD_AutoPage(page);	 	break;	
			case AutoPumpOff:		page= LCD_AutoPage(page);		break;
			case AutoMud:			page= LCD_AutoPage(page);		break;
			case AutoCirculate:		page= LCD_AutoPage(page);		break;
			case AutoAir:			page= LCD_AutoPage(page);		break;
			
			case ManualMain:		page= LCD_ManualPage(page);		break;
			case ManualCirculate:	page= LCD_ManualPage(page);		break;
			case ManualAir: 		page= LCD_ManualPage(page);		break;	
			case ManualSetDown:		page= LCD_ManualPage(page);		break;
			case ManualPumpOff:		page= LCD_ManualPage(page);		break;
			case ManualPumpOff_On:	page= LCD_ManualPage(page);		break;
			case ManualMud:			page= LCD_ManualPage(page);		break;
			case ManualCompressor:	page= LCD_ManualPage(page);		break;
			case ManualPhosphor:	page= LCD_ManualPage(page);		break;	
			case ManualInflowPump:	page= LCD_ManualPage(page);		break;
			
			case SetupMain:			page= LCD_SetupPage(page);		break;
			case SetupCirculate:	page= LCD_SetupPage(page);		break;	
			case SetupAir: 			page= LCD_SetupPage(page);		break;
			case SetupSetDown:		page= LCD_SetupPage(page);		break;
			case SetupPumpOff:		page= LCD_SetupPage(page);		break;
			case SetupMud:			page= LCD_SetupPage(page);		break;
			case SetupCompressor:	page= LCD_SetupPage(page);		break;
			case SetupPhosphor:		page= LCD_SetupPage(page);		break;
			case SetupInflowPump:	page= LCD_SetupPage(page);		break;
			case SetupCal:			page= LCD_SetupPage(page);		break;
			case SetupCalPressure:	page= LCD_SetupPage(page);		break;
			case SetupAlarm:		page= LCD_SetupPage(page);		break;
			case SetupWatch:		page= LCD_SetupPage(page);		break;
			case SetupZone:			page= LCD_SetupPage(page);		break;			

			case DataMain:			page= LCD_DataPage(page);		break;
			case DataAuto:			page= LCD_DataPage(page);		break;
			case DataManual:		page= LCD_DataPage(page);		break;	
			case DataSetup:			page= LCD_DataPage(page);		break;

			default:				page= AutoPage;					break;
		}
	}
}








/*********************************************************************\
 * End of Main.c
\**********************************************************************/




