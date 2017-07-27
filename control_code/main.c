/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Main-File
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Main-File of Control ICT - Interception
* ------------------------------------------------------------------
*	Date:			    27.05.2011
* lastChanges:	12.05.2015
\*********************************************************************/

#include<avr/io.h>

/* ==================================================================*
 * 						HeaderFiles
 * ==================================================================*/

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "modem_driver.h"
#include "port_func.h"
#include "basic_func.h"


/* ==================================================================*
 * 						Main
 * ==================================================================*/

int main(void)
{
	// start page
	t_page 	page = DataPage;
  // Init
	Basic_Init();
  LCD_Backlight(_on);
  // Input
  struct InputHandler input_handler;
  struct Modem modem;
  InputHandler_init(&input_handler);
  Modem_init(&modem);

	while(1)
	{
		Watchdog_Restart();
		PORT_Bootloader();
    Modem_Check(page, &modem);

		//------------------------------------------------GreatLinker
		switch(page)
		{
			case AutoPage:		page = LCD_AutoPage(page);		break;
			case ManualPage:  page = LCD_ManualPage(page);  break;
			case SetupPage:	  page = LCD_SetupPage(page);	  break;
			case DataPage:		page = LCD_DataPage(page);	  break;
      //----------------------------------------------Pin-Pages
			case PinManual:		page = LCD_PinPage(page);		break;
			case PinSetup: 		page = LCD_PinPage(page);		break;
      //----------------------------------------------Auto-Pages
			case AutoZone:
			case AutoSetDown:
			case AutoPumpOff:
			case AutoMud:
			case AutoCirc:
      case AutoCircOff:
			case AutoAir:
			case AutoAirOff:
			  page = LCD_AutoPage(page);
        PORT_RunTime(&input_handler);
        break;
      //----------------------------------------------Manual-Pages
			case ManualMain:
			case ManualCirc:
      case ManualCircOff:
			case ManualAir:
			case ManualSetDown:
			case ManualPumpOff:
			case ManualPumpOff_On:
			case ManualMud:
			case ManualCompressor:
			case ManualPhosphor:
			case ManualInflowPump:
			  page = LCD_ManualPage(page);
			  break;
      //----------------------------------------------Manual-Pages
			case SetupMain:
			case SetupCirculate:
			case SetupAir:
			case SetupSetDown:
			case SetupPumpOff:
			case SetupMud:
			case SetupCompressor:
			case SetupPhosphor:
			case SetupInflowPump:
			case SetupCal:
			case SetupCalPressure:
			case SetupAlarm:
			case SetupWatch:
			case SetupZone:
			  page = LCD_SetupPage(page);
			  break;
      //----------------------------------------------Data-Pages
			case DataMain:
			case DataAuto:
			case DataManual:
			case DataSetup:
			case DataSonic:
			case DataSonicAuto:
      case DataSonicBoot:
      case DataSonicBootR:
      case DataSonicBootW:
        page = LCD_DataPage(page);
        break;

			default: page = AutoPage; break;
    }
	}
}





/*********************************************************************\
 * End of file
\**********************************************************************/
