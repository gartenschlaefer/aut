/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Steuerung Light ICT
*	Name:			    Display-Symbol-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Symbol File, this Symbols are called in lcd_app.c
* ------------------------------------------------------------------
*	Date:			    26.07.2011
* lastChanges:	27.12.2015
\**********************************************************************/


#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "output_app.h"
#include "eval_app.h"
#include "memory_app.h"

#include "mcp7941_driver.h"
#include "mpx_driver.h"
#include "mcp9800_driver.h"
#include "modem_driver.h"

#include "error_func.h"
#include "tc_func.h"
#include "lcd_sym.h"
#include "sonic_app.h"




/* ==================================================================*
 * 						Auto Page
 * --------------------------------------------------------------
 *	Auto Pages layout
 * --------------------------------------------------------------
 * ==================================================================*/

/* ==================================================================*
 * 						Set Auto Pages
 * ==================================================================*/

void LCD_AutoSet_Page(void)
{
	LCD_MarkTextButton(Auto);
	LCD_Clean();
	LCD_MarkTextButton(Auto);

	if(!COMPANY)   LCD_Write_Purator(0,0);
	else LCD_Write_HECS(0,0);
	MCP7941_LCD_WriteTime(_init);
}


/* ------------------------------------------------------------------*
 * 						Auto Countdown
 * ------------------------------------------------------------------*/

void LCD_AutoCountDown(unsigned char min, unsigned char sec)
{
	LCD_WriteValue2_MyFont(13,5, min);
	LCD_WriteValue2_MyFont(13,17, sec);
}


/* ------------------------------------------------------------------*
 * 						Auto Text
 * ------------------------------------------------------------------*/

void LCD_AutoText(void)
{
	LCD_WriteMyFont(13, 57, 13);		//m
	LCD_WriteMyFont(13, 61, 14);		//b
	LCD_WriteMyFont(13, 65, 15);		//a
	LCD_WriteMyFont(13, 69, 16);		//r

	LCD_WriteMyFont(15, 69, 17);		//h
	LCD_WriteMyFont(13, 13, 10);		//:

	LCD_WriteMyFont(13, 105, 10);		//:-Ip
	LCD_WriteMyFont(13, 93, 10);		//:-Ip
	LCD_WriteMyFont(13, 143, 10);		//:-Ph

  MCP7941_LCD_WriteTime(_init);
  MCP7941_LCD_WriteDate();
	MPX_ReadTank(AutoPage, _write);	//tank
	LCD_WriteValue5_MyFont(15,43, Eval_Comp_OpHours(_init));

	LCD_WriteAuto_IP_Sensor();
	Sonic_ReadTank(AutoPage, _write); //Sonic
}


/* ------------------------------------------------------------------*
 * 						Auto UltraSonic Value
 * ------------------------------------------------------------------*/

void LCD_Auto_SonicVal(int sonic)
{
  int zero = 0;
  int dif = 0;
  int per = 0;
  int cal = 0;
  int lvO2 = 0;

  LCD_WriteValue4_MyFont(17, 5, sonic);	//value
  LCD_WriteMyFont(17, 22, 13);		      //m
  LCD_WriteMyFont(17, 26, 13);		      //m

  //--------------------------------------------------Percentage
  zero = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) |
          (MEM_EEPROM_ReadVar(SONIC_L_LV)));
  lvO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2)<<8)		|
				  (MEM_EEPROM_ReadVar(TANK_L_O2)));

  dif = lvO2 * 10;             //Waterlevel-difference
  cal = sonic - (zero - (lvO2 * 10));
  if(sonic > zero) per = 0;
  else per = 100 - ((cal * 10) / dif) * 10;

  if(!sonic) per = 0;
  LCD_WriteValue3_MyFont(15, 5, per);	  //value
}


/* ------------------------------------------------------------------*
 * 						Set Auto Zone
 * ------------------------------------------------------------------*/

void LCD_AutoSet_Zone(unsigned char min, unsigned char sec)
{
	LCD_Write_Symbol_2(6, 0, n_circulate);
	LCD_Write_Symbol_2(6, 45, n_compressor);
	LCD_WriteStringFontNeg(8, 1, "z");
	LCD_AutoCountDown(min, sec);
}


/* ------------------------------------------------------------------*
 * 						Set Auto SetDown
 * ------------------------------------------------------------------*/

void LCD_AutoSet_SetDown(unsigned char min, unsigned char sec)
{
	LCD_Write_Symbol_2(6, 0, n_setDown);
	LCD_Write_Symbol_2(6, 45, p_compressor);
	LCD_AutoCountDown(min, sec);
}


/* ------------------------------------------------------------------*
 * 						Set Auto PumpOff
 * ------------------------------------------------------------------*/

void LCD_AutoSet_PumpOff(unsigned char min, unsigned char sec)
{
	LCD_Write_Symbol_1(5, 0, n_pumpOff);
	if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
  {
    LCD_Write_Symbol_2(6, 45, n_compressor);
  }
	else
  {
    LCD_Write_Symbol_2(6, 45, p_compressor);
  }
	LCD_AutoCountDown(min, sec);
}


/* ------------------------------------------------------------------*
 * 						Set Auto Mud
 * ------------------------------------------------------------------*/

void LCD_AutoSet_Mud(unsigned char min, unsigned char sec)
{
	LCD_Write_Symbol_1(5, 0, n_mud);
	LCD_Write_Symbol_2(6, 45, n_compressor);
	LCD_AutoCountDown(min, sec);
	MPX_ReadTank(AutoAir, _reset);
}



/* ==================================================================*
 * 						Auto Symbols
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Inflow Pump Sym
 * ------------------------------------------------------------------*/

void LCD_SymbolAuto_IP(t_page page, t_FuncCmd cmd)
{
	unsigned char pump = 0;
	pump =  MEM_EEPROM_ReadVar(PUMP_inflowPump);

	if(cmd == _off || cmd == _disabled)
	{
		switch (pump)
		{
			case 0:
			  LCD_Write_Symbol_1(5, 89, p_inflowPump);
        if(page == AutoAirOff || page == AutoCircOff)
        {
          LCD_Write_Symbol_2(6, 45, p_compressor);
        }
        break;

			case 1:		LCD_Write_Symbol_3(5, 90, p_pump);  		break;
			case 2:		LCD_Write_Symbol_1(5, 90, p_pump2); 		break;
			default: 													                break;
		}
	}
	else if(cmd == _on)
	{
		switch (pump)
		{
			case 0:
			  LCD_Write_Symbol_1(5, 89, n_inflowPump);
        if(page == AutoAirOff || page == AutoCircOff)
        {
          LCD_Write_Symbol_2(6, 45, n_compressor);
        }
        break;

			case 1:		LCD_Write_Symbol_3(5, 90, n_pump);  		break;
			case 2:		LCD_Write_Symbol_1(5, 90, n_pump2); 		break;
			default: 													                break;
		}
	}
}


/* ------------------------------------------------------------------*
 * 						Inflow Pump Var
 * ------------------------------------------------------------------*/

void LCD_WriteAutoVar_IP(unsigned char cho, unsigned char *t_ip)
{
	if(cho & 0x01)	LCD_WriteValue2_MyFont(13,109, t_ip[0]);	//sec
	if(cho & 0x02)	LCD_WriteValue2_MyFont(13,97, t_ip[1]);		//min
	if(cho & 0x04)	LCD_WriteValue2_MyFont(13,85, t_ip[2]);		//h
}


/* ------------------------------------------------------------------*
 * 						Inflow Pump Sensor
 * ------------------------------------------------------------------*/

void LCD_WriteAuto_IP_Sensor(void)
{
	unsigned char sensor = 0;
	sensor = MEM_EEPROM_ReadVar(SENSOR_outTank);
	if(sensor)	LCD_Write_Symbol_2(16, 90, p_sensor);
}


/* ------------------------------------------------------------------*
 * 						Phosphor Symbols
 * ------------------------------------------------------------------*/

void LCD_SymbolAuto_Ph(t_FuncCmd state)
{
  switch(state)
  {
    case _on:
      LCD_Write_Symbol_3(6, 134, n_phosphor);	break;

    case _disabled:
    case _off:
      LCD_Write_Symbol_3(6, 134, p_phosphor);	break;

    default:													break;
  }
}


/* ------------------------------------------------------------------*
 * 						Phosphor Var
 * ------------------------------------------------------------------*/

void LCD_WriteAutoVar_Ph(unsigned char min, unsigned char sec)
{
  LCD_WriteValue2_MyFont(13,135, min);
  LCD_WriteValue2_MyFont(13,147, sec);
}


/* ------------------------------------------------------------------*
 * 						AutoVar
 * ------------------------------------------------------------------*/

void LCD_WriteAutoVar(int min, int sec)
{
	static int oldSec=0;

	if(oldSec != sec)
	{
		oldSec=sec;
		MCP7941_LCD_WriteTime(_exe);					//Time
		LCD_WriteValue2_MyFont(13,17, sec);		//CT sec
	}
	if(sec==59)	LCD_WriteValue2_MyFont(13,5, min);		//CT min
}


/* ------------------------------------------------------------------*
 * 						AutoVar - Comp
 * ------------------------------------------------------------------*/

void LCD_WriteAutoVar_Comp(int min, int sec)
{
	static int oldSecc = 0;
	static int opMin = 0;
	static unsigned char opCounter = 0;

	int opHour = 0;

	if(oldSecc != sec)
	{
		oldSecc = sec;
		MCP7941_LCD_WriteTime(_exe);					//Time
		LCD_WriteValue2_MyFont(13,17, sec);		//CT sec
	}
	if(sec == 59)	LCD_WriteValue2_MyFont(13,5, min);		//CT min

	if(opMin != min)
	{
		opMin = min;
		opCounter++;
	}
	if(opCounter >= 60)
	{
		opCounter = 0;
		opHour = Eval_Comp_OpHours(_add);
		LCD_WriteValue5_MyFont(15,43, opHour);
	}
}


/* ------------------------------------------------------------------*
 * 						StartTime
 * ------------------------------------------------------------------*/

int LCD_AutoRead_StartTime(t_page page)
{
	int sMin = 5;

	switch(page)
	{
		case AutoCirc:
		case ManualCirc:
      sMin = MEM_EEPROM_ReadVar(ON_circ);   break;

    case AutoCircOff:
		case ManualCircOff:
      sMin = MEM_EEPROM_ReadVar(OFF_circ);  break;

		case AutoAir:
		case ManualAir:
      sMin = MEM_EEPROM_ReadVar(ON_air);         break;

    case AutoAirOff:
      sMin = MEM_EEPROM_ReadVar(OFF_air);		    break;

		default:																    break;
	}
	return sMin;
}


/* ------------------------------------------------------------------*
 * 						LCD_AutoSym - Auto Air Symbols
 * ------------------------------------------------------------------*/

void LCD_AutoAirSym(t_page page)
{
  switch(page)
  {
    case AutoCirc:
      LCD_Write_Symbol_2(6, 0, n_circulate);
      LCD_Write_Symbol_2(6, 45, n_compressor);
      break;

    case AutoAir:
      LCD_Write_Symbol_2(6, 0, n_air);
      LCD_Write_Symbol_2(6, 45, n_compressor);
      break;

    case AutoCircOff:
      LCD_Write_Symbol_2(6, 0, p_circulate);
      LCD_Write_Symbol_2(6, 45, p_compressor);
      break;

    case AutoAirOff:
      LCD_Write_Symbol_2(6, 0, p_air);
      LCD_Write_Symbol_2(6, 45, p_compressor);
      break;

    default:  break;
  }
}



/* ==================================================================*
 * 						Manual Page
 * --------------------------------------------------------------
 *	Manual Pages layout
 * --------------------------------------------------------------
 * ==================================================================*/

void LCD_ManualText(int min, unsigned char sec)
{
	LCD_WriteValue2(17,124, min);
	LCD_WriteValue2(17,142, sec);
	LCD_WriteStringFont(17,136,":");
	LCD_WriteStringFont(17,61,"mbar");
	MPX_ReadTank(ManualCirc, _write);
}


/* ------------------------------------------------------------------*
 * 						Set Manual Pages
 * ------------------------------------------------------------------*/

void LCD_WriteManualVar(int min, unsigned char sec)
{
	static unsigned char oldSec = 0;

	if(oldSec != sec)
	{
		oldSec = sec;
    LCD_WriteValue2(17,142, sec);		//CT sec
	}
	if(sec == 59)
	{
	  if(min >= 100) LCD_WriteValue3(17,118, min);
	  else LCD_WriteValue2(17,124, min);		//CT min
  }
  MPX_ReadAverage(Manual, _exe);
}


/* ------------------------------------------------------------------*
 * 						Set Manual Pages
 * ------------------------------------------------------------------*/

void LCD_ManualSet_Page(int min, unsigned char sec)
{
	unsigned char i=0;

	LCD_MarkTextButton(Manual);
	LCD_Clean();

	for(i=12; i<20; i++)
	{
		LCD_SetupSymbols(i);			//Write positive Setup Symbols
	}
	LCD_MarkTextButton(Manual);
	LCD_ManualText(min, sec);

	OUT_CloseOff();
	MEM_EEPROM_WriteManualEntry(
	  MCP7941_ReadByte(TIC_HOUR),
    MCP7941_ReadByte(TIC_MIN),
    _saveValue);
}



/* ==================================================================*
 * 						Setup Page
 * --------------------------------------------------------------
 *	Setup Pages layout
 * --------------------------------------------------------------
 * ==================================================================*/

/* ==================================================================*
 * 						Set Setup Pages
 * ==================================================================*/

void LCD_SetupSet_Page(void)
{
	unsigned char i=0;

	LCD_MarkTextButton(Setup);
	LCD_Clean();

	for(i=12; i<24; i++)
	{
		LCD_SetupSymbols(i);			//Write positive Setup Symbols
	}
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Circulate
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Circulate(void)
{
	LCD_SetupSymbols(sn_circulate);
	LCD_Clean();

	LCD_WriteStringFont(16,40,"Time:");
	LCD_Write_Symbol_2(3, 0, n_circulate);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Air
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Air(void)
{
	LCD_SetupSymbols(sn_air);
	LCD_Clean();

	LCD_Write_Symbol_2(3, 0, n_air);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup SetDown
 * ------------------------------------------------------------------*/

void LCD_SetupSet_SetDown(void)
{
	LCD_SetupSymbols(sn_setDown);
	LCD_Clean();

	LCD_Write_Symbol_2(3, 0, n_setDown);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
	LCD_WriteStringFont(10,0,"Time:");
}


/* ------------------------------------------------------------------*
 * 						Set Setup PumpOff
 * ------------------------------------------------------------------*/

void LCD_SetupSet_PumpOff(void)
{
	LCD_SetupSymbols(sn_pumpOff);
	LCD_Clean();

	LCD_Write_Symbol_1(3, 0, n_pumpOff);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Mud
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Mud(void)
{
	LCD_SetupSymbols(sn_mud);
	LCD_Clean();

	LCD_Write_Symbol_1(2, 0, n_mud);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Compressor
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Compressor(void)
{
	LCD_SetupSymbols(sn_compressor);
	LCD_Clean();

	LCD_WriteStringFont(11,28, "mbar MIN.");
	LCD_WriteStringFont(16,28, "mbar MAX.");
	LCD_Write_Symbol_2(3, 0, n_compressor);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Phosphor
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Phosphor(void)
{
	LCD_SetupSymbols(sn_phosphor);
	LCD_Clean();

	LCD_Write_Symbol_3(3, 0, n_phosphor);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup InflowPump
 * ------------------------------------------------------------------*/

void LCD_SetupSet_InflowPump(void)
{
	LCD_SetupSymbols(sn_inflowPump);
	LCD_Clean();

	LCD_Write_Symbol_1(1, 0, n_inflowPump);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}

/* ------------------------------------------------------------------*
 * 						InflowPump - Values
 * -------------------------------------------------------------------
 *		0000 0000 -> 0 on min h 0 Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Setup_IPValues(unsigned char select, unsigned char *val)
{
	if(select & (1<<0))	LCD_WriteValueNeg2(5,47, *val);		//h
	if(select & (1<<4))	LCD_WriteValue2(5,47, *val);
	val++;

	if(select & (1<<1))	LCD_WriteValueNeg2(10,47, *val);	//min
	if(select & (1<<5))	LCD_WriteValue2(10,47, *val);
	val++;

	if(select & (1<<2))	LCD_WriteValueNeg2(10,19, *val);	//on
	if(select & (1<<6))	LCD_WriteValue2(10,19, *val);
}


/* ------------------------------------------------------------------*
 * 						InflowPump - Values - TextSelect
 * -------------------------------------------------------------------
 *		0000 0000 -> off on min h Noff Non Nmin Nh
 * ------------------------------------------------------------------*/

void LCD_Setup_IPText(unsigned char select)
{
	if(select & (1<<0))	LCD_WriteStringFontNeg(5,59,"h");
	if(select & (1<<4))	LCD_WriteStringFont(5,59,"h");

	if(select & (1<<1))	LCD_WriteStringFontNeg(10,59,"min");
	if(select & (1<<5))	LCD_WriteStringFont(10,59,"min");

	if(select & (1<<2))	LCD_WriteStringFontNeg(10,1,"ON:");
	if(select & (1<<6))	LCD_WriteStringFont(10,1,"ON:");

	if(select & (1<<3))	LCD_WriteStringFontNeg(1,47,"OFF");
	if(select & (1<<7))	LCD_WriteStringFont(1,47,"OFF");
}



/* ------------------------------------------------------------------*
 * 						Set Setup Cal
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Cal(void)
{
  unsigned char sonic = 0;
  unsigned char calRedo = 0;
  LCD_SetupSymbols(sn_cal);
	LCD_Clean();

	LCD_Write_Symbol_2(2, 0, n_cal);
	LCD_Write_Symbol_2(15,1, p_level);

	LCD_ControlButtons(sp_esc);
	LCD_ControlButtons(sp_ok);
	LCD_Write_TextButton(9, 80, OpenV, 1);
	LCD_Write_Symbol_2(9,125, p_cal);

	LCD_WriteStringFont(10,1, "mbar:");

  sonic = MEM_EEPROM_ReadVar(SONIC_on);
  if(sonic){ LCD_Write_Symbol_3(2, 40, n_sonic);   //SonicON
    LCD_WriteStringFont(17,66, "mm");
    Sonic_LevelCal(_init);}

  else{ LCD_Write_Symbol_3(2, 40, p_sonic);        //SonicOff
    LCD_WriteStringFont(17,60, "mbar");
    MPX_LevelCal(_init);}

  calRedo = MEM_EEPROM_ReadVar(CAL_Redo_on);
  if(calRedo) LCD_Write_Symbol_3(15,130, n_arrowRedo);
  else        LCD_Write_Symbol_3(15,130, p_arrowRedo);


	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Alarm
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Alarm(void)
{
	LCD_SetupSymbols(sn_alarm);
	LCD_Clean();

	LCD_Write_Symbol_2(3, 0, n_alarm);
	LCD_WriteStringFontNeg(10,3, "T:");

	LCD_WriteFont(10,32, 94);					//Grad
	LCD_WriteStringFont(10,38, 	"C");			//Celsius

	MCP9800_WriteTemp();
	LCD_WriteFont(17,104, 94);					//Grad
	LCD_WriteStringFont(17,110, "C");			//Celsius

	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* ------------------------------------------------------------------*
 * 						Set Setup Watch
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Watch(void)
{
	LCD_SetupSymbols(sn_watch);
	LCD_Clean();

	LCD_Write_Symbol_2(3, 0, n_watch);
	LCD_MarkTextButton(Setup);
	LCD_WriteCtrlButton2();

	LCD_WriteStringFont(11,10, "hh");
	LCD_WriteStringFont(11,50, "mm");
	LCD_WriteStringFont(17,10, "dd");
	LCD_WriteStringFont(17,50, "mm");
	LCD_WriteStringFont(17,96, "yy");

	LCD_WriteStringFont(9,33, ":");
	LCD_WriteStringFont(15,33, "/");
	LCD_WriteStringFont(15,70, "/");
	LCD_WriteStringFont(15,84, "20");
}


/* ------------------------------------------------------------------*
 * 						Set Setup Zone
 * ------------------------------------------------------------------*/

void LCD_SetupSet_Zone(void)
{
	LCD_SetupSymbols(sn_zone);
	LCD_Clean();

	LCD_Write_Symbol_2(2, 0, n_zone);

	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}



/* ==================================================================*
 * 						Set Setup Symbols
 * ==================================================================*/

void LCD_SetupSymbols(t_SetupSym sym)
{
	switch(sym)
	{
		case sp_circulate:	LCD_Write_Symbol_2(3, 0, p_circulate); 		break;
		case sp_air:		LCD_Write_Symbol_2(3, 40, p_air);			break;
		case sp_setDown:	LCD_Write_Symbol_2(3, 80, p_setDown); 		break;
		case sp_pumpOff:	LCD_Write_Symbol_1(2, 120, p_pumpOff); 		break;
		case sp_mud:		LCD_Write_Symbol_1(8, 0, p_mud); 			break;
		case sp_compressor:	LCD_Write_Symbol_2(9, 40, p_compressor);	break;
		case sp_phosphor:	LCD_Write_Symbol_3(9, 85, p_phosphor); 		break;
		case sp_inflowPump:	LCD_Write_Symbol_1(8, 120, p_inflowPump);	break;
		case sp_cal:		LCD_Write_Symbol_2(15, 0, p_cal);			break;
		case sp_alarm:		LCD_Write_Symbol_2(15, 40, p_alarm); 		break;
		case sp_watch:		LCD_Write_Symbol_2(15, 80, p_watch);		break;
		case sp_zone:		LCD_Write_Symbol_2(15, 120, p_zone);		break;

		case sn_circulate:	LCD_Write_Symbol_2(3, 0, n_circulate); 		break;
		case sn_air:		LCD_Write_Symbol_2(3, 40, n_air);			break;
		case sn_setDown:	LCD_Write_Symbol_2(3, 80, n_setDown); 		break;
		case sn_pumpOff:	LCD_Write_Symbol_1(2, 120, n_pumpOff); 		break;
		case sn_mud:		LCD_Write_Symbol_1(8, 0, n_mud); 			break;
		case sn_compressor:	LCD_Write_Symbol_2(9, 40, n_compressor);	break;
		case sn_phosphor:	LCD_Write_Symbol_3(9, 85, n_phosphor); 		break;
		case sn_inflowPump:	LCD_Write_Symbol_1(8, 120, n_inflowPump);	break;
		case sn_cal:		LCD_Write_Symbol_2(15, 0, n_cal);			break;
		case sn_alarm:		LCD_Write_Symbol_2(15, 40, n_alarm); 		break;
		case sn_watch:		LCD_Write_Symbol_2(15, 80, n_watch);		break;
		case sn_zone:		LCD_Write_Symbol_2(15, 120, n_zone);		break;

		default:														break;
	}
}



/* ------------------------------------------------------------------*
 * 						Date Time Page
 * ------------------------------------------------------------------*/

void LCD_DateTime(t_DateTime time, unsigned char value)
{
	switch(time)
	{
		case p_h:			LCD_WriteValue2(9,10, value); 		break;
		case p_min:			LCD_WriteValue2(9,50, value); 		break;
		case p_day:			LCD_WriteValue2(15,10, value); 		break;
		case p_month:		LCD_WriteValue2(15,50, value); 		break;
		case p_year:		LCD_WriteValue2(15,96, value); 		break;

		case n_h:			LCD_WriteValueNeg2(9,10, value); 	break;
		case n_min:			LCD_WriteValueNeg2(9,50, value); 	break;
		case n_day:			LCD_WriteValueNeg2(15,10, value); 	break;
		case n_month:		LCD_WriteValueNeg2(15,50, value); 	break;
		case n_year:		LCD_WriteValueNeg2(15,96, value);	break;
		default:												break;
	}
}



/* ==================================================================*
 * 						Data Page
 * --------------------------------------------------------------
 * 	Description:
 * --------------------------------------------------------------
 *	Data Pages layout
 * --------------------------------------------------------------
 * ==================================================================*/

/* ==================================================================*
 * 						Set Data Pages
 * ==================================================================*/

void LCD_DataSet_Page(void)
{
	LCD_MarkTextButton(Data);
	LCD_Clean();
	LCD_MarkTextButton(Data);

  LCD_WriteMyFont(1,0, 21);     //S
  LCD_WriteMyFont(1,8, SV1);    //x.-.-
  LCD_WriteMyFont(1,16, SV2);   //-.x.-
  LCD_WriteMyFont(1,24, SV3);   //-.-.x

  LCD_WriteMyFont(1,4, 22);     //.
  LCD_WriteMyFont(1,12, 22);    //.
  LCD_WriteMyFont(1,20, 22);    //.

	LCD_WriteStringFont(6,0,"Choose Data:");

	LCD_Write_TextButton(9, 0, Auto, 1);
	LCD_Write_TextButton(9, 40, Manual, 1);
	LCD_Write_TextButton(9, 80, Setup, 1);
	LCD_Write_TextButton(9, 120, Sonic, 1);
}


/* ------------------------------------------------------------------*
 * 						Set Data Auto
 * ------------------------------------------------------------------*/

void LCD_DataSet_Auto(void)
{
	LCD_ClrSpace(4,0,9,160);
	LCD_WriteDataHeader_Auto();
	LCD_Write_TextButton(22, 0, Auto, 0);
	LCD_WriteDataArrows();

	LCD_WriteStringMyFont(1,148,";8");
	LCD_WriteMyFont(1,144,1);
	LCD_WriteAutoEntryPage(Page1);					//Page
}


/* ------------------------------------------------------------------*
 * 						Set Data Manual
 * ------------------------------------------------------------------*/

void LCD_DataSet_Manual(void)
{
	LCD_ClrSpace(4,0,9,160);
	LCD_WriteDataHeader_Manual();
	LCD_Write_TextButton(22, 40, Manual, 0);
	LCD_WriteDataArrows();

	LCD_WriteStringMyFont(1,144,"1;2");
	LCD_WriteManualEntryPage(Page1);
}


/* ------------------------------------------------------------------*
 * 						Set Data Setup
 * ------------------------------------------------------------------*/

void LCD_DataSet_Setup(void)
{
	LCD_ClrSpace(4,0,9,160);
	LCD_WriteDataHeader_Setup();
	LCD_Write_TextButton(22, 80, Setup, 0);
	LCD_WriteDataArrows();

	LCD_WriteStringMyFont(1,144,"1;2");
	LCD_WriteSetupEntryPage(Page1);
}



/* ------------------------------------------------------------------*
 * 						Set Data Sonic
 * ------------------------------------------------------------------*/

void LCD_DataSet_Sonic(void)
{
	LCD_ClrSpace(1, 0, 12, 160);
  Sonic_Data_Boot(_off);
	LCD_WriteStringFont(1, 91, "[ C]");
	LCD_WriteFont(1, 97, 94);					    //Grad
	LCD_WriteStringFont(1, 124, "[mm]");
	LCD_Write_TextButton(4, 0, Shot, 1);
	LCD_Write_TextButton(10, 0, Auto, 1);
  LCD_Write_TextButton(16, 0, Boot, 1);

	LCD_Write_Symbol_3(3, 50, p_sonic);
}




/* ==================================================================*
 * 						Data Symbols
 * ==================================================================*/

void LCD_WriteDataHeader_Auto(void)
{
	LCD_FillSpace(0,0,4,137);
	LCD_WriteStringFontNeg(1,1,"Date");
	LCD_WriteStringFontNeg(1,37,"|");
	LCD_WriteStringFontNeg(1,45,"Time");
	LCD_WriteStringFontNeg(1,73,"|");
	LCD_WriteStringFontNeg(1,85,"O2");
	LCD_WriteStringFontNeg(1,103,"|");
	LCD_WriteStringFontNeg(1,113,"Err");
}

void LCD_WriteDataHeader_Manual(void)
{
	LCD_FillSpace(0,0,4,137);
	LCD_WriteStringFontNeg(1,1,"Date");
	LCD_WriteStringFontNeg(1,37,"|");
	LCD_WriteStringFontNeg(1,45,"Begin");
	LCD_WriteStringFontNeg(1,77,"|");
	LCD_WriteStringFontNeg(1,88,"End");
}

void LCD_WriteDataHeader_Setup(void)
{
	LCD_FillSpace(0,0,4,137);
	LCD_WriteStringFontNeg(1,1,"Date");
	LCD_WriteStringFontNeg(1,37,"|");
	LCD_WriteStringFontNeg(1,45,"Time");
}

void LCD_WriteDataArrows(void)
{
	LCD_Write_Symbol_3(	3, 	140, p_arrowUp);
	LCD_Write_Symbol_3(	14,	140, p_arrowDown);
	LCD_Write_Symbol_3(	8, 	140, p_line);
}


/* ------------------------------------------------------------------*
 * 						Data SonicShot
 * ------------------------------------------------------------------*/

void LCD_Data_SonicWrite(t_FuncCmd cmd, int shot)
{
	static unsigned char i = 5;
  static unsigned int max = 0;
  static unsigned int min = 10000;
  static unsigned int max_temp = 0;
  static unsigned int min_temp = 10000;

  //--------------------------------------------------ClearDisplay
	if(cmd == _clear)
	{
		i = 5;
		max = 0;
		min = 10000;
		max_temp = 0;
		min_temp = 10000;
		LCD_ClrSpace(4, 91, 15, 69);
		LCD_ClrSpace(17, 50, 4, 60);  //MessageLine
	}
  //--------------------------------------------------OneShot
	else if(cmd == _shot)
	{
		LCD_WriteValue5_MyFont(i, 126, shot); //WriteOneShot
		i = i + 3;
	}
  //--------------------------------------------------AutoShot
	else if(cmd == _shot1)
	{
    LCD_WriteValue5_MyFont(5, 126, shot);
	  if(shot > max)
    {
	    max = shot;
	    LCD_WriteValue5_MyFont(11, 126, max);
    }
    if(shot < min)
    {
      min = shot;
	    LCD_WriteValue5_MyFont(17, 126, min);
    }
	}
  //--------------------------------------------------Temperature
	else if(cmd == _temp)
	{
    if(i > 17)
    {
      i = 5;
      LCD_ClrSpace(4, 95, 15, 60);
    }
    if(shot & 0x8000)
    {
      shot &= 0x7FFF;
      LCD_WriteMyFont(i, 91, 11);  //-
    }
    LCD_WriteMyFont(i, 102, 22);        //.
    LCD_WriteValue2_MyFont(i, 105, (shot & 0x00FF));
    LCD_WriteValue2_MyFont(i, 95, ((shot >> 8) & 0x00FF));
	}
  //--------------------------------------------------AutoTemp
  else if(cmd == _temp1)
	{
    unsigned char minus_sign = 0;
    if(shot & 0x8000)
    {
      minus_sign = 1;
      shot &= 0x7FFF;
      LCD_WriteMyFont(5, 91, 11); //-
    }
    LCD_WriteValue2_MyFont(5, 105, (shot & 0x00FF));
    LCD_WriteValue2_MyFont(5, 95, ((shot >> 8) & 0x00FF));
    if(shot > max_temp)
    {
	    max_temp = shot;
	    if(minus_sign)  LCD_WriteMyFont(11, 91, 11); //-
	    LCD_WriteValue2_MyFont(11, 105, (max_temp & 0x00FF));
      LCD_WriteValue2_MyFont(11, 95, ((max_temp >> 8) & 0x00FF));
    }
    if(shot < min_temp)
    {
      min_temp = shot;
      if(minus_sign)  LCD_WriteMyFont(17, 91, 11); //-
      LCD_WriteValue2_MyFont(17, 105, (min_temp & 0x00FF));
      LCD_WriteValue2_MyFont(17, 95, ((min_temp >> 8) & 0x00FF));
    }
	}
  //--------------------------------------------------AutoTemp
	else if(cmd == _autotext)
  {
    LCD_Write_TextButton(10, 0, Auto, 0);
    LCD_WriteMyFont(9, 113, 13); //m
    LCD_WriteMyFont(9, 117, 15); //a
    LCD_WriteMyFont(9, 121, 25); //x
    LCD_WriteMyFont(15, 113, 13); //m
    LCD_WriteMyFont(15, 117, 23); //i
    LCD_WriteMyFont(15, 121, 24); //n
    LCD_WriteMyFont(17, 102, 22); //.
    LCD_WriteMyFont(11, 102, 22); //.
    LCD_WriteMyFont(5, 102, 22);  //.
  }
  //--------------------------------------------------Messages
	else if(cmd == _noUS)   LCD_WriteStringFont(17, 50, "NoUS");
  else if(cmd == _noBoot) LCD_WriteStringFont(17, 50, "NoBoot");
  else if(cmd == _error)  LCD_WriteStringFont(17, 50, "Error");
  else if(cmd == _ok) LCD_WriteStringFont(17, 50, "OK...Cycle Power!");
  else if(cmd == _noData) LCD_WriteStringFont(17, 50, "NoData");
  else if(cmd == _success) LCD_WriteStringFont(17, 50, "Success");
  else if(cmd == _mcp_fail) LCD_WriteStringFont(17, 100, "MCP-fail");
}



/* ==================================================================*
 * 						Data Page - Entries
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	LCD_WriteAutoEntry
 * --------------------------------------------------------------
 *	Reads Variable from EEPROM	PageSize: 32Bytes
 * ------------------------------------------------------------------*/

void LCD_WriteAutoEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
	unsigned char varEnt[8]={0x00};
	unsigned char i=0;
	int o2=0;

	for(i=0; i<8; i++)
	{
		varEnt[i]= MEM_EEPROM_ReadData(eePage, entry, i);
	}
	o2= ((varEnt[5]<<8) | (varEnt[6]));

	//--------------------------------------------------Write-No-Data
	if(!varEnt[0] && !varEnt[1] && !varEnt[2])
	{
		for(i=1; i<130; i=i+4){	LCD_WriteMyFont(pa, i, 11);}		//--------
	}

	//--------------------------------------------------Write-Data-
	else
	{
		LCD_WriteValue2_MyFont	(pa, 1,  varEnt[0]);	//day
		LCD_WriteMyFont			(pa, 9, 11);			//-
		LCD_WriteValue2_MyFont	(pa, 13, varEnt[1]);	//month

		LCD_WriteMyFont			(pa, 21, 11);			//-
		LCD_WriteValue2_MyFont	(pa, 25, varEnt[2]);	//year
		LCD_WriteMyFont			(pa, 38, 12);			//|

		LCD_WriteValue2_MyFont	(pa, 47, varEnt[3]);	//hour
		LCD_WriteMyFont			(pa, 55, 10);			//:
		LCD_WriteValue2_MyFont	(pa, 59, varEnt[4]);	//minutes

		LCD_WriteMyFont			(pa, 74, 12);			//|
		LCD_WriteValue3_MyFont	(pa, 85, o2);			//O2/min
		LCD_WriteMyFont			(pa, 104, 12);			//|

		//------------------------------------------------Write-AlarmCode
		if(varEnt[7] & 0x01)	LCD_WriteMyFont(pa, 113, 1);
		else					LCD_WriteMyFont(pa, 113, 0);	//Alarm

		if(varEnt[7] & 0x02)	LCD_WriteMyFont(pa, 117, 2);
		else					LCD_WriteMyFont(pa, 117, 0);	//Alarm

		if(varEnt[7] & 0x04)	LCD_WriteMyFont(pa, 121, 3);
		else					LCD_WriteMyFont(pa, 121, 0);	//Alarm

		if(varEnt[7] & 0x08)	LCD_WriteMyFont(pa, 125, 4);
		else					LCD_WriteMyFont(pa, 125, 0);	//Alarm

		if(varEnt[7] & 0x10)	LCD_WriteMyFont(pa, 129, 5);
		else					LCD_WriteMyFont(pa, 129, 0);	//Alarm
	}
}


/* ------------------------------------------------------------------*
 * 						Manual Entry
 * ------------------------------------------------------------------*/

void LCD_WriteManualEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
	unsigned char varEnt[7]={0x00};
	unsigned char i=0;

	for(i=0; i<7; i++)
	{
		varEnt[i]= MEM_EEPROM_ReadData(eePage, entry, i);
	}

	//---------------------------------------------------------------Write-No-Data
	if(!varEnt[0] && !varEnt[1] && !varEnt[2])
	{
		for(i=1; i<108; i=i+4){	LCD_WriteMyFont(pa, i, 11);}		//--------
	}

	//-----------------------------------------------------------------Write-Data-
	else
	{
		LCD_WriteValue2_MyFont	(pa, 1,  varEnt[0]);	//day
		LCD_WriteMyFont			(pa, 9, 11);			//-
		LCD_WriteValue2_MyFont	(pa, 13, varEnt[1]);	//month

		LCD_WriteMyFont			(pa, 21, 11);			//-
		LCD_WriteValue2_MyFont	(pa, 25, varEnt[2]);	//year
		LCD_WriteMyFont			(pa, 38, 12);			//|

		LCD_WriteValue2_MyFont	(pa, 47, varEnt[3]);	//hour
		LCD_WriteMyFont			(pa, 55, 10);			//:
		LCD_WriteValue2_MyFont	(pa, 59, varEnt[4]);	//minutes

		LCD_WriteMyFont			(pa, 78, 12);			//|

		LCD_WriteValue2_MyFont	(pa, 88, varEnt[5]);	//hour
		LCD_WriteMyFont			(pa, 96, 10);			//:
		LCD_WriteValue2_MyFont	(pa, 100, varEnt[6]);	//minutes
	}
}


/* ------------------------------------------------------------------*
 * 						Setup Entry
 * ------------------------------------------------------------------*/

void LCD_WriteSetupEntry(unsigned char pa, unsigned char eePage, unsigned char entry)
{
	unsigned char varEnt[7]={0x00};
	unsigned char i=0;

	for(i=0; i<5; i++)
	{
		varEnt[i]= MEM_EEPROM_ReadData(eePage, entry, i);
	}

	//---------------------------------------------------------------Write-No-Data
	if(!varEnt[0] && !varEnt[1] && !varEnt[2])
	{
		for(i=1; i<68; i=i+4){	LCD_WriteMyFont(pa, i, 11);}		//--------
	}

	//-----------------------------------------------------------------Write-Data-
	else
	{
		LCD_WriteValue2_MyFont	(pa, 1,  varEnt[0]);	//day
		LCD_WriteMyFont			(pa, 9, 11);			//-
		LCD_WriteValue2_MyFont	(pa, 13, varEnt[1]);	//month

		LCD_WriteMyFont			(pa, 21, 11);			//-
		LCD_WriteValue2_MyFont	(pa, 25, varEnt[2]);	//year
		LCD_WriteMyFont			(pa, 38, 12);			//|

		LCD_WriteValue2_MyFont	(pa, 47, varEnt[3]);	//hour
		LCD_WriteMyFont			(pa, 55, 10);			//:
		LCD_WriteValue2_MyFont	(pa, 59, varEnt[4]);	//minutes
	}
}



/* ==================================================================*
 * 						MPX - Layout
 * --------------------------------------------------------------
 * 	Description:
 * --------------------------------------------------------------
 *	MPX Pressure Sensor - Set text and vars
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						MPX-Text
 * ------------------------------------------------------------------*/

void LCD_Sym_MPX(t_FuncCmd cmd, int value)
{
  //--------------------------------------------------NoMbarInAuto
  if(cmd == _notav)
  {
    LCD_WriteMyFont(15, 5, 11);		//-
    LCD_WriteMyFont(15, 9, 11);		//-
    LCD_WriteMyFont(15, 13, 11);  //-
    LCD_WriteMyFont(15, 18, 19);  //%
  }

  //--------------------------------------------------mbarInAuto
  else if(cmd == _mbar)
  {
		LCD_WriteValue3_MyFont(17, 43, value);		//Write Value
    LCD_WriteMyFont(17, 57, 13);		//m
	  LCD_WriteMyFont(17, 61, 14);		//b
	  LCD_WriteMyFont(17, 65, 15);		//a
	  LCD_WriteMyFont(17, 69, 16);		//r
  }

  //--------------------------------------------------mbarInManual
  else if(cmd == _mmbar)
  {
    LCD_WriteValue3(17, 2, value);
    LCD_WriteStringFont(17,22,"%");
  }

  //--------------------------------------------------2ControlValue
  else if(cmd == _debug)
  {
    LCD_WriteValue3_MyFont(15, 5, value);		//Write
    LCD_WriteMyFont(15, 18, 19);		        //%
  }
}



/* ==================================================================*
 * 						Pin Page
 * --------------------------------------------------------------
 *	Pin Pages layout
 * --------------------------------------------------------------
 * ==================================================================*/

void LCD_pPinButtons(unsigned char pPin)
{
	switch(pPin)
	{
		case 1:	LCD_Write_Pin(2, 0, frame, 0x11); 		break;
		case 4: LCD_Write_Pin(8, 0, frame, 0x14); 		break;
		case 7:	LCD_Write_Pin(14, 0, frame, 0x17); 		break;
		case 11: LCD_Write_Pin(20, 0, p_escape, 0x20); 	break;

		case 2: LCD_Write_Pin(2, 40, frame, 0x12); 		break;
		case 5: LCD_Write_Pin(8, 40, frame, 0x15); 		break;
		case 8: LCD_Write_Pin(14, 40, frame, 0x18); 	break;
		case 0: LCD_Write_Pin(20, 40, frame, 0x10); 	break;

		case 3: LCD_Write_Pin(2, 80, frame, 0x13); 		break;
		case 6: LCD_Write_Pin(8, 80, frame, 0x16);	 	break;
		case 9: LCD_Write_Pin(14, 80, frame, 0x19); 	break;
		case 10:LCD_Write_Pin(20, 80, p_del, 0x20); 	break;
		default:										break;
	}
}

void LCD_nPinButtons(unsigned char nPin)
{
	switch(nPin)
	{
		case 1:	LCD_Write_Pin(2, 0, black, 0x01); 		break;
		case 4: LCD_Write_Pin(8, 0, black, 0x04); 		break;
		case 7:	LCD_Write_Pin(14, 0, black, 0x07); 		break;
		case 11: LCD_Write_Pin(20, 0, n_escape, 0x20);	break;

		case 2: LCD_Write_Pin(2, 40, black, 0x02); 		break;
		case 5: LCD_Write_Pin(8, 40, black, 0x05); 		break;
		case 8: LCD_Write_Pin(14, 40, black, 0x08); 	break;
		case 0: LCD_Write_Pin(20, 40, black, 0x00); 	break;

		case 3: LCD_Write_Pin(2, 80, black, 0x03); 		break;
		case 6: LCD_Write_Pin(8, 80, black, 0x06); 		break;
		case 9: LCD_Write_Pin(14, 80, black, 0x09); 	break;
		case 10:LCD_Write_Pin(20, 80, n_del, 0x20); 	break;
		default:										                  break;
	}
}



/* ==================================================================*
 * 						Set Pin Page
 * ==================================================================*/

void LCD_PinSet_Page(void)
{
	unsigned char i=0;

	LCD_Clean();
	for(i=0; i<12; i++) LCD_pPinButtons(i);
	Eval_PinDel();
}


/* ------------------------------------------------------------------*
 * 						Pin-Write
 * ------------------------------------------------------------------*/

void LCD_Pin_Write(t_FuncCmd cmd, TelNr nr)
{
  unsigned char i=0;

  switch(cmd)
  {
    case _right:
      LCD_WriteStringFont(6,125,"right!");  break;

    case _op:
      LCD_WriteStringFont(6,119,"OP");      break;

    //------------------------------------------------WriteWholeNumber
    case _write:
      LCD_WriteStringFont(6,119,"Tel");
      LCD_WriteFont(6,137, nr.id+14);       //Number
      LCD_WriteStringFont(6,143,":");
      LCD_WriteMyFont(9, 119, 20);			  	//+

      for(i=0; i<2; i++){                   //43
        nr.pos= i;
        LCD_WriteMyFont(9, 123+4*i, Modem_TelNr(_read2, nr));}

      for(i=2; i<5; i++){                   //680
        nr.pos= i;
        LCD_WriteMyFont(9, 135+4*(i-2), Modem_TelNr(_read2, nr));}

      for(i=5; i<14; i++){                   //2104 231
        nr.pos= i;
        LCD_WriteMyFont(11, 119+4*(i-5), Modem_TelNr(_read2, nr));}
      break;

    //------------------------------------------------PrintOneNumber
    case _telnr:
      LCD_nPinButtons(nr.tel);
      if(nr.pos < 2)
      {
        LCD_WriteMyFont(9, 123+4*nr.pos, nr.tel);
        nr.pos++;
        if(nr.pos < 2)  LCD_WriteMyFontNeg(9, 127, 14);
        else            LCD_WriteMyFontNeg(9, 135, 14);
      }

      else if((nr.pos > 1) && (nr.pos < 5))
      {
        LCD_WriteMyFont(9, 135+4*(nr.pos-2), nr.tel);
        nr.pos++;
        if(nr.pos < 5)
          LCD_WriteMyFontNeg(9, 135+4*(nr.pos-2), 14);
        else
          LCD_WriteMyFontNeg(11, 119, 14);
      }
      else
      {
        LCD_WriteMyFont(11,119+4*(nr.pos-5), nr.tel);
        nr.pos++;
        if(nr.pos < 14)
          LCD_WriteMyFontNeg(11, 119+4*(nr.pos-5), 14);
      }
      break;

    case _wrong:
      LCD_WriteStringFont(6,119,"wrong!");
      LCD_WriteStringFont(9,119,"try");
      LCD_WriteStringFont(11,119,"again");    break;

    case _clear:  LCD_ClrSpace(6,119,25,41);   break;

    default:                                  break;
  }
}



/* ==================================================================*
 * 						Control Utilities
 * --------------------------------------------------------------
 *	Buttons and highlight
 * --------------------------------------------------------------
 * ==================================================================*/

/* ==================================================================*
 * 						Buttons
 * ==================================================================*/

void LCD_TextButton(t_textButtons text, unsigned char pos)
{
	if(pos)
	{
		switch(text)
		{
			case Auto:		LCD_Write_TextButton(22, 0, Auto, 1); 		break;
			case Manual:	LCD_Write_TextButton(22, 40, Manual, 1);	break;
			case Setup:		LCD_Write_TextButton(22, 80, Setup, 1); 	break;
			case Data:		LCD_Write_TextButton(22, 120, Data, 1); 	break;
			default:													break;
		}
	}
	else
	{
		switch(text)
		{
			case Auto:		LCD_Write_TextButton(22, 0, Auto, 0); 		break;
			case Manual:	LCD_Write_TextButton(22, 40, Manual, 0);	break;
			case Setup:		LCD_Write_TextButton(22, 80, Setup, 0); 	break;
			case Data:		LCD_Write_TextButton(22, 120, Data, 0); 	break;
			default:													break;
		}
	}
}



/* ------------------------------------------------------------------*
 * 						Control Buttons
 * ------------------------------------------------------------------*/

void LCD_ControlButtons(t_CtrlButtons ctrl)
{
	switch(ctrl)
	{
		case sp_esc:	LCD_Write_Symbol_3(3, 90, p_esc);	break;
		case sp_ok:		LCD_Write_Symbol_3(3, 130, p_ok);	break;
		case sp_minus:	LCD_Write_Symbol_3(9, 90, p_minus);	break;
		case sp_plus:	LCD_Write_Symbol_3(9, 130, p_plus);	break;

		case sn_esc:	LCD_Write_Symbol_3(3, 90, n_esc);	break;
		case sn_ok:		LCD_Write_Symbol_3(3, 130, n_ok);	break;
		case sn_minus:	LCD_Write_Symbol_3(9, 90, n_minus);	break;
		case sn_plus:	LCD_Write_Symbol_3(9, 130, n_plus);	break;

		default:											break;
	}
}

void LCD_ControlButtons2(t_CtrlButtons ctrl)
{
	switch(ctrl)
	{
		case sp_esc:	LCD_Write_Symbol_3(	3, 	90, p_esc);		break;
		case sp_ok:		LCD_Write_Symbol_3(	3, 	130, p_ok);		break;
		case sp_minus:	LCD_Write_Symbol_3(	15,	130, p_minus);	break;
		case sp_plus:	LCD_Write_Symbol_3(	9, 	130, p_plus);	break;

		case sn_esc:	LCD_Write_Symbol_3(	3, 	90, n_esc);		break;
		case sn_ok:		LCD_Write_Symbol_3(	3, 	130, n_ok);		break;
		case sn_minus:	LCD_Write_Symbol_3(	15,	130, n_minus);	break;
		case sn_plus:	LCD_Write_Symbol_3(	9, 	130, n_plus);	break;

		default: 												break;
	}
}


/* ------------------------------------------------------------------*
 * 						Pin Okay
 * ------------------------------------------------------------------*/

void LCD_Pin_WriteOK(unsigned char on)
{
	switch(on)
	{
		case 0:		LCD_Write_Symbol_3(20, 130, p_ok);	break;
		case 1:		LCD_Write_Symbol_3(20, 130, n_ok);	break;
		default:	LCD_Write_Symbol_3(20, 130, n_ok);	break;
	}
}


/* ------------------------------------------------------------------*
 * 						Write Control Buttons App
 * ------------------------------------------------------------------*/

void LCD_WriteCtrlButton(void)
{
	unsigned char i=0;
	for(i=4; i<9; i++)
	{
		LCD_ControlButtons(i);			//Write TextButtons
	}
}

void LCD_WriteCtrlButton2(void)
{
	unsigned char i=0;
	for(i=4; i<9; i++)
	{
		LCD_ControlButtons2(i);			//Write TextButtons
	}
}


/* ------------------------------------------------------------------*
 * 						On/Off Values
 * ------------------------------------------------------------------*/

void LCD_OnValue(unsigned char value)
{
	LCD_WriteStringFont(10,1,"ON:");
	LCD_WriteValue2(10,19, value);
}

void LCD_OffValue(unsigned char value)
{
	LCD_WriteStringFont(10,40,"OFF:");
	LCD_WriteValue2(10,64, value);
}

void LCD_OnValueNeg(unsigned char value)
{
	LCD_WriteStringFontNeg(10,1,"ON:");
	LCD_WriteValueNeg2(10,19, value);
}

void LCD_OffValueNeg(unsigned char value)
{
	LCD_WriteStringFontNeg(10,40,"OFF:");
	LCD_WriteValueNeg2(10,64, value);
}



/* ==================================================================*
 * 						Mark
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Mark Text Button
 * ------------------------------------------------------------------*/

void LCD_MarkTextButton(t_textButtons text)
{
	unsigned char i = 0;
	for(i = 0; i < 4; i++)
	{
		if(i != text)
    {
      LCD_TextButton(i, 1);			//Write TextButtons
    }
	}
	LCD_TextButton(text, 0);
}


/* ------------------------------------------------------------------*
 * 						Mark Manual Symbol
 * ------------------------------------------------------------------*/

void LCD_Mark_ManualSymbol(t_SetupSym sym)
{
	unsigned char i=0;
	for(i=12; i<20; i++)
	{
		if((12+sym==i))	LCD_SetupSymbols(sym);		//Write negative Manual Symbol
		else			LCD_SetupSymbols(i);		//Write positive Manual Symbol
	}
}



/* ==================================================================*
 * 						Sonic - General
 * ==================================================================*/

unsigned char LCD_Sym_NoUS(t_page page, t_FuncCmd cmd)
{
  static unsigned char errC = 0;
  unsigned char row = 0;
  unsigned char col = 0;

  switch(page)
  {
    case AutoSetDown:	case AutoMud:	    case AutoPumpOff:
    case AutoCirc:    case AutoCircOff: case AutoPage:
    case AutoAir:			case AutoAirOff:
      row = 17; col = 5;  break;

    case SetupCal: row = 0; col = 38; break;
    default:                          break;
  }

  if(cmd == _clear)
  {
    if(errC){
      errC = 0;
      LCD_ClrSpace(row, col, 2, 24);}
  }
  //--------------------------------------------------Write
  else if(cmd == _write)
  {
    errC++;
    if(errC == 10) LCD_WriteStringFont(row, col, "noUS");
  }
  //--------------------------------------------------Check
  else if(cmd == _check)
  {
    if(errC > 20) return 1;
  }
  return 0;
}





/**********************************************************************\
 * End of file
\**********************************************************************/
