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
	t_page 	page = DataPage;	//StartPage

	Basic_Init();             //Init
  LCD_Backlight(_on);				//BacklightOn

	while(1)
	{
		Watchdog_Restart();
		PORT_Bootloader();
		PORT_RunTime();
    Modem_TurnOn();

		//------------------------------------------------GreatLinker
		switch(page)
		{
			case AutoPage:		page = LCD_AutoPage(page);		break;
			case ManualPage:  page = LCD_ManualPage(page);  break;
			case SetupPage:	  page = LCD_SetupPage(page);	  break;
			case DataPage:		page = LCD_DataPage(page);	  break;

			case PinManual:		page = LCD_PinPage(page);		break;
			case PinSetup: 		page = LCD_PinPage(page);		break;

			case AutoZone:	  page = LCD_AutoPage(page);  break;
			case AutoSetDown: page = LCD_AutoPage(page);  break;
			case AutoPumpOff: page = LCD_AutoPage(page);	break;
			case AutoMud:		  page = LCD_AutoPage(page);	break;
			case AutoCirc:   	page = LCD_AutoPage(page);	break;
      case AutoCircOff:	page = LCD_AutoPage(page);	break;
			case AutoAir:			page = LCD_AutoPage(page);	break;
			case AutoAirOff:  page = LCD_AutoPage(page);	break;

			case ManualMain:		    page = LCD_ManualPage(page);	break;
			case ManualCirc:	      page = LCD_ManualPage(page);	break;
      case ManualCircOff:	    page = LCD_ManualPage(page);	break;
			case ManualAir: 		    page = LCD_ManualPage(page);	break;
			case ManualSetDown:		  page = LCD_ManualPage(page);	break;
			case ManualPumpOff:		  page = LCD_ManualPage(page);	break;
			case ManualPumpOff_On:	page = LCD_ManualPage(page);	break;
			case ManualMud:			    page = LCD_ManualPage(page);	break;
			case ManualCompressor:	page = LCD_ManualPage(page);	break;
			case ManualPhosphor:	  page = LCD_ManualPage(page);	break;
			case ManualInflowPump:	page = LCD_ManualPage(page);	break;

			case SetupMain:			    page = LCD_SetupPage(page);	break;
			case SetupCirculate:	  page = LCD_SetupPage(page);	break;
			case SetupAir: 			    page = LCD_SetupPage(page);	break;
			case SetupSetDown:		  page = LCD_SetupPage(page);	break;
			case SetupPumpOff:		  page = LCD_SetupPage(page);	break;
			case SetupMud:			    page = LCD_SetupPage(page);	break;
			case SetupCompressor:	  page = LCD_SetupPage(page);	break;
			case SetupPhosphor:		  page = LCD_SetupPage(page);	break;
			case SetupInflowPump:	  page = LCD_SetupPage(page);	break;
			case SetupCal:			    page = LCD_SetupPage(page);	break;
			case SetupCalPressure:  page = LCD_SetupPage(page);	break;
			case SetupAlarm:		    page = LCD_SetupPage(page);	break;
			case SetupWatch:		    page = LCD_SetupPage(page);	break;
			case SetupZone:			    page = LCD_SetupPage(page);	break;

			case DataMain:			    page = LCD_DataPage(page);	break;
			case DataAuto:			    page = LCD_DataPage(page);	break;
			case DataManual:		    page = LCD_DataPage(page);	break;
			case DataSetup:			    page = LCD_DataPage(page);	break;
			case DataSonic:	        page = LCD_DataPage(page);	break;
			case DataSonicAuto:     page = LCD_DataPage(page);	break;
      case DataSonicBoot:    page = LCD_DataPage(page);	break;
      case DataSonicBootR:    page = LCD_DataPage(page);	break;
      case DataSonicBootW:    page = LCD_DataPage(page);	break;

			default:				        page = AutoPage;					  break;
    }
	}
}





/*********************************************************************\
 * End of file
\**********************************************************************/
