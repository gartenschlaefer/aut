/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Display-Symbol-SourceFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Symbol File, this Symbols are called in SL_Display_app.c
* ------------------------------------------------------------------														
*	Date:			26.07.2011	
* 	lastChanges:										
\**********************************************************************/


#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_Display_driver.h"

#include "SL_Output_app.h"
#include "SL_Display_app.h"
#include "SL_Eval_app.h"
#include "SL_Memory_app.h"

#include "SL_DS1302_driver.h"
#include "SL_MPX_driver.h" 
#include "SL_MCP9800_driver.h"

#include "SL_Error_func.h"
#include "SL_TC_func.h"
#include "SL_Display_sym.h"




/* ===================================================================*/
/* ===================================================================*
 * 						FUNCTIONS Write Symbols
 * ===================================================================*/
/* -------------------------------------------------------------------*
 * 	Here you can configurate the position of the Symbols
 * -------------------------------------------------------------------*/
/* ===================================================================*/



/* ===================================================================*
 * 						Auto Page
 * ===================================================================*/


/* -------------------------------------------------------------------*
 * 						Inflow Pump Sym
 * -------------------------------------------------------------------*/

void LCD_WriteAuto_IP_pSym(void)
{
	unsigned char pump=0;

	pump=  MEM_EEPROM_ReadVar(PUMP_inflowPump);
	switch (pump)
	{
		case 0:		LCD_Write_Symbol_1(5, 89, p_inflowPump);  		break;
		case 1:		LCD_Write_Symbol_3(5, 90, p_pump);  			break;
		case 2:		LCD_Write_Symbol_1(5, 90, p_pump2); 			break;
		default: 													break;
	}
}

void LCD_WriteAuto_IP_nSym(void)
{
	unsigned char pump=0;

	pump=  MEM_EEPROM_ReadVar(PUMP_inflowPump);
	switch (pump)
	{
		case 0:		LCD_Write_Symbol_1(5, 89, n_inflowPump);  		break;
		case 1:		LCD_Write_Symbol_3(5, 90, n_pump);  			break;
		case 2:		LCD_Write_Symbol_1(5, 90, n_pump2); 			break;
		default: 													break;
	}
}


/* -------------------------------------------------------------------*
 * 						Inflow Pump Sensor
 * -------------------------------------------------------------------*/

void LCD_WriteAuto_IP_Sensor(void)
{
	unsigned char sensor=0;

	sensor= MEM_EEPROM_ReadVar(SENSOR_outTank);
	if(sensor)	LCD_Write_Symbol_2(16, 90, p_sensor);	
}





/*--------------------------------------------------------------------*
 * 	LCD_WriteAutoVar
 * --------------------------------------------------------------
 * 	parameter:		unsigned char min	-	Minuten
 *					unsigned char sec	-	Seconds
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Variables to Display, _Comp should called, where comp is
 *	running, for updating OP Hours
 * -------------------------------------------------------------------*/

void LCD_WriteAutoVar(int min, int sec)
{
	static int oldSec=0;

	if(oldSec != sec)
	{
		oldSec=sec;
		DS1302_LCD_WriteTime(_exe);						//Time
		LCD_WriteValue_MyFont2(13,17, sec);				//CT sec
	}
	if(sec==59)	LCD_WriteValue_MyFont2(13,5, min);		//CT min

	LCD_Auto_Phosphor(sec, _exe);						//Phosphor
	MPX_ReadAverage(Auto, _exe);								//Average Preassure
}


void LCD_WriteAutoVar_Comp(int min, int sec)
{
	static int oldSecc=0;
	static int opMin=0;
	static unsigned char opCounter=0;
	
	int opHour=0;

	if(oldSecc != sec)
	{
		oldSecc=sec;
		DS1302_LCD_WriteTime(_exe);						//Time
		LCD_WriteValue_MyFont2(13,17, sec);				//CT sec
	}
	if(sec==59)	LCD_WriteValue_MyFont2(13,5, min);		//CT min

	if(opMin!= min)
	{
		opMin= min;
		opCounter++;
	}
	if(opCounter>=60)
	{
		opCounter=0;
		opHour= Eval_Comp_OpHours(_add);
		LCD_WriteValue_MyFont5(15,43, opHour);		//Write Compressor Op Hours 15
	}
	
	LCD_Auto_Phosphor(sec, _exe);					//Phosphor
	MPX_ReadAverage(Auto, _exe);					//Preassure
}





/* -------------------------------------------------------------------*
 * 						Auto Air
 * -------------------------------------------------------------------*/

t_page LCD_Write_AirVar(t_page page, int sec, t_FuncCmd cmd)
{
	static int			 cOld=0;		
	static t_FuncCmd	 airState= _on;
	static unsigned char airMin=0;
	static unsigned char airSec=0;

	t_page	cPage= AutoAir;

	//--------------------------------------------------------Init
	if(cmd== _init)
	{
		airState= _on;
		airMin= LCD_AutoRead_StartTime(page, 1);	//Read On
		airSec=0;
	}

	//--------------------------------------------------------Set	
	else if(cmd== _set)
	{
		LCD_Auto_InflowPump(0, _outSet);
		if(airState== _on)	OUT_Set_Circulate();
	}

	//--------------------------------------------------------OutSet	
	else if(cmd== _outSet)
	{
		if(airState== _on)	OUT_Set_Circulate();
		if(airState== _off)	OUT_Clr_Circulate();
	}

	//--------------------------------------------------------WriteSym
	else if(cmd== _write)
	{
		if(page== AutoCirculate)	LCD_Write_Symbol_2(6, 0, n_circulate);
		if(page== AutoAir)			LCD_Write_Symbol_2(6, 0, n_air);
		if(airState== _off)			LCD_Write_Symbol_2(6, 45, p_compressor);
		if(airState== _on)			LCD_Write_Symbol_2(6, 45, n_compressor);
		LCD_AutoCountDown(airMin, airSec);
	}

	//--------------------------------------------------------exe
	else if(cmd== _exe)
	{
		switch(page)
		{	
			case AutoAir:
			case AutoCirculate:
				if(airState==_on && !airMin && !airSec)			//--Change2Off
				{
					airState= _off;								//NewState_off
					LCD_Write_Symbol_2(6, 45, p_compressor);
					cPage= page;
					page= MPX_ReadTank(page, _exe);				//MPX
					MPX_ReadTank(page, _write);					//MPX
					OUT_Clr_Circulate();						//ClearAir
					
					if(cPage!= page){
						LCD_Auto_InflowPump(0, _reset);			//IP-Reset
						return page;}							//NextPage

					LCD_Auto_InflowPump(0, _set);				//IP-Set
					airMin= LCD_AutoRead_StartTime(page, 0);	//Read Off
					airSec= 0;
				}
				else if(airState==_off && !airMin && !airSec)	//--Change2On
				{
					airState= _on;								//NewState_on
					LCD_Write_Symbol_2(6, 45, n_compressor);
					if(LCD_Auto_InflowPump(0, _state)==_on){
						LCD_Auto_InflowPump(0, _reset);}		//IP-Reset
					OUT_Set_Circulate();						//SetAir
					airMin= LCD_AutoRead_StartTime(page, 1);	//Read On
					airSec= 0;					
				}										break; 	//Auto
					
			case ManualAir:	
			case ManualCirculate:	
				if(airState==_on && !airMin && !airSec)			//--Change2Off
				{
					airState= _off;								//NewState_off
					MPX_ReadTank(ManualCirculate, _exe);		//MPX
					MPX_ReadTank(ManualCirculate, _write);		//MPX
					OUT_Clr_Circulate();						//ClearAir
					airMin= LCD_AutoRead_StartTime(page, 0);	//Read Off
					airSec= 0;
				}
				else if(airState==_off && !airMin && !airSec)	//--Change2On
				{
					airState= _on;								//NewState_on
					OUT_Set_Circulate();
					airMin= LCD_AutoRead_StartTime(page, 1);	//Read On
					airSec= 0;
				}										break; 	//Manual
			default:									break;
		}

		//--------------------------------------------------------CountDown
		if(sec != cOld){	cOld= sec;	
							if(airSec<1){	airSec=60;
											airMin--;} airSec--;}

		//--------------------------------------------------------WriteAutoVar
		if((page == AutoCirculate)	||	(page == AutoAir))	
		{
			if(airState==_off)									//--OFF Air
			{		
				if(page== AutoCirculate){							
					page= Error_Detection(AutoCirculateOff, 0, 0);}	//ErrorDet
				if(page== AutoAir){						
					page=Error_Detection(AutoAirOff, 0, 0);}		//ErrorDet
							
				LCD_WriteAutoVar(airMin, airSec);				//Var
				LCD_Auto_InflowPump(airSec, _exe);				//InflowPump_exe
			}
			if(airState==_on)									//--ON Air
			{	
				page= Error_Detection(page, 0, 0);					//ErrorDet
				LCD_WriteAutoVar_Comp(airMin, airSec);			//Write Variables
				Eval_Oxygen(airMin, Oxygen_Count);				//Count Oxygen Hours
			}
		}
	}
	return page;
}


unsigned char LCD_AutoRead_StartTime(t_page page, unsigned char on)
{
	unsigned char sMin=5;

	switch(page)
	{
		case ManualCirculate:
		case AutoCirculate:		if(on)	sMin= MEM_EEPROM_ReadVar(ON_circulate);	
								else 	sMin= MEM_EEPROM_ReadVar(OFF_circulate);break;

		case ManualAir:
		case AutoAir:			if(on)	sMin= MEM_EEPROM_ReadVar(ON_air);
								else	sMin= MEM_EEPROM_ReadVar(OFF_air);		break;
		default:																break;	
	}

	return sMin;
}







/* ===================================================================*
 * 						Setup Page
 * ===================================================================*/

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



/* -------------------------------------------------------------------*
 * 						Date Time Page
 * -------------------------------------------------------------------*/

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










/* ===================================================================*
 * 						Data
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_WriteDataHeader
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Data Header
 * -------------------------------------------------------------------*/

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




/* -------------------------------------------------------------------*
 * 						Entries
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*		
 * 	LCD_WriteAutoEntry
 * --------------------------------------------------------------
 * 	parameter:		unsigned char pa		-	PageAddress
 *					unsigned char eePage	-	EEPROM Page
 *					unsigned char entry		-	Entry
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Reads Variable from EEPROM	PageSize: 32Bytes
 * -------------------------------------------------------------------*/

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
	
	//---------------------------------------------------------------Write-No-Data
	if(!varEnt[0] && !varEnt[1] && !varEnt[2])
	{
		for(i=1; i<130; i=i+4){	LCD_WriteMyFont(pa, i, 11);}		//--------
	}

	//-----------------------------------------------------------------Write-Data-
	else
	{
		LCD_WriteValue_MyFont2	(pa, 1,  varEnt[0]);	//day
		LCD_WriteMyFont			(pa, 9, 11);			//-
		LCD_WriteValue_MyFont2	(pa, 13, varEnt[1]);	//month

		LCD_WriteMyFont			(pa, 21, 11);			//-
		LCD_WriteValue_MyFont2	(pa, 25, varEnt[2]);	//year
		LCD_WriteMyFont			(pa, 38, 12);			//|

		LCD_WriteValue_MyFont2	(pa, 47, varEnt[3]);	//hour
		LCD_WriteMyFont			(pa, 55, 10);			//:
		LCD_WriteValue_MyFont2	(pa, 59, varEnt[4]);	//minutes

		LCD_WriteMyFont			(pa, 74, 12);			//|
		LCD_WriteValue_MyFont	(pa, 85, o2);			//O2/min
		LCD_WriteMyFont			(pa, 104, 12);			//|

		//-------------------------------------------------------------Write-AlarmCode
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


/* -------------------------------------------------------------------*
 * 						Manual Entry
 * -------------------------------------------------------------------*/

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
		LCD_WriteValue_MyFont2	(pa, 1,  varEnt[0]);	//day
		LCD_WriteMyFont			(pa, 9, 11);			//-
		LCD_WriteValue_MyFont2	(pa, 13, varEnt[1]);	//month

		LCD_WriteMyFont			(pa, 21, 11);			//-
		LCD_WriteValue_MyFont2	(pa, 25, varEnt[2]);	//year
		LCD_WriteMyFont			(pa, 38, 12);			//|

		LCD_WriteValue_MyFont2	(pa, 47, varEnt[3]);	//hour
		LCD_WriteMyFont			(pa, 55, 10);			//:
		LCD_WriteValue_MyFont2	(pa, 59, varEnt[4]);	//minutes

		LCD_WriteMyFont			(pa, 78, 12);			//|

		LCD_WriteValue_MyFont2	(pa, 88, varEnt[5]);	//hour
		LCD_WriteMyFont			(pa, 96, 10);			//:
		LCD_WriteValue_MyFont2	(pa, 100, varEnt[6]);	//minutes
	}
}


/* -------------------------------------------------------------------*
 * 						Setup Entry
 * -------------------------------------------------------------------*/

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
		LCD_WriteValue_MyFont2	(pa, 1,  varEnt[0]);	//day
		LCD_WriteMyFont			(pa, 9, 11);			//-
		LCD_WriteValue_MyFont2	(pa, 13, varEnt[1]);	//month

		LCD_WriteMyFont			(pa, 21, 11);			//-
		LCD_WriteValue_MyFont2	(pa, 25, varEnt[2]);	//year
		LCD_WriteMyFont			(pa, 38, 12);			//|

		LCD_WriteValue_MyFont2	(pa, 47, varEnt[3]);	//hour
		LCD_WriteMyFont			(pa, 55, 10);			//:
		LCD_WriteValue_MyFont2	(pa, 59, varEnt[4]);	//minutes
	}
}










/* ===================================================================*
 * 						PIN Page
 * ===================================================================*/

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
		default:										break;
	}
}












/* ===================================================================*/
/* ===================================================================*
 * 						FUNCTIONS Set Pages
 * ===================================================================*/
/* -------------------------------------------------------------------*
 * 	Here you can configurate the setting of the Pages
 * -------------------------------------------------------------------*/
/* ===================================================================*/



/* ===================================================================*
 * 						Set Auto Pages
 * ===================================================================*/

void LCD_AutoSet_Page(void)
{
	LCD_MarkTextButton(Auto);	
	LCD_Clean();
	LCD_MarkTextButton(Auto);	
	//LCD_Write_Purator(0,0);
	LCD_Write_HECS(0,0);	
	DS1302_LCD_WriteTime(_init); 
}


/* -------------------------------------------------------------------*
 * 						Auto Text
 * -------------------------------------------------------------------*/

void LCD_AutoCountDown(unsigned char min, unsigned char sec)
{
	LCD_WriteValue_MyFont2(13,5, min);
	LCD_WriteValue_MyFont2(13,17, sec);
}

void LCD_AutoText(void)
{
	LCD_ClrSpace(5, 0, 4, 35);
	
	LCD_WriteMyFont(13, 57, 13);		//m	
	LCD_WriteMyFont(13, 61, 14);		//b	
	LCD_WriteMyFont(13, 65, 15);		//a
	LCD_WriteMyFont(13, 69, 16);		//r		
	
	LCD_WriteMyFont(15, 69, 17);		//h
	LCD_WriteMyFont(13, 13, 10);		//:
	LCD_WriteMyFont(13, 103, 10);		//:
	LCD_WriteMyFont(13, 143, 10);		//:
	LCD_WriteMyFont(15, 18, 19);		//%

	MPX_ReadTank(AutoPage, _write);		//tank

	DS1302_LCD_WriteDate();
	DS1302_LCD_WriteTime(_init);
	LCD_WriteValue_MyFont5(15,43, Eval_Comp_OpHours(_init));		//Compressor Op Hours

	LCD_Auto_Phosphor(0, _set);			//Phophor writeSymbol
	LCD_Auto_InflowPump(0, _write);		//InflowPump write Symbol
}


/* -------------------------------------------------------------------*
 * 						Set Auto Zone
 * -------------------------------------------------------------------*/

void LCD_AutoSet_Zone(unsigned char min, unsigned char sec)
{
	LCD_AutoText();
	LCD_Write_Symbol_2(6, 0, n_circulate);
	LCD_Write_Symbol_2(6, 45, n_compressor);
	LCD_AutoCountDown(min, sec);
}


/* -------------------------------------------------------------------*
 * 						Set Auto SetDown
 * -------------------------------------------------------------------*/

void LCD_AutoSet_SetDown(unsigned char min, unsigned char sec)
{
	LCD_AutoText();
	LCD_Write_Symbol_2(6, 0, n_setDown);
	LCD_Write_Symbol_2(6, 45, p_compressor);
	LCD_AutoCountDown(min, sec);
}


/* -------------------------------------------------------------------*
 * 						Set Auto PumpOff
 * -------------------------------------------------------------------*/

void LCD_AutoSet_PumpOff(unsigned char min, unsigned char sec)
{
	LCD_AutoText();
	LCD_Write_Symbol_1(5, 0, n_pumpOff);
	if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))	LCD_Write_Symbol_2(6, 45, n_compressor);
	else									LCD_Write_Symbol_2(6, 45, p_compressor);
	LCD_AutoCountDown(min, sec);
	MPX_ReadTank(AutoPumpOff, _write);
}


/* -------------------------------------------------------------------*
 * 						Set Auto Mud
 * -------------------------------------------------------------------*/

void LCD_AutoSet_Mud(unsigned char min, unsigned char sec)
{
	LCD_AutoText();	
	LCD_Write_Symbol_1(5, 0, n_mud);
	LCD_Write_Symbol_2(6, 45, n_compressor);
	LCD_AutoCountDown(min, sec);
	MPX_ReadTank(AutoMud, _write);
	MPX_ReadTank(AutoAir, _reset);
}


/* -------------------------------------------------------------------*
 * 						Set Auto Circulate
 * -------------------------------------------------------------------*/

void LCD_AutoSet_Circulate(unsigned char min, unsigned char sec)
{
	LCD_AutoText();
	LCD_Write_AirVar(AutoCirculate, 0, _write);
}


/* -------------------------------------------------------------------*
 * 						Set Auto Air
 * -------------------------------------------------------------------*/

void LCD_AutoSet_Air(unsigned char min, unsigned char sec)
{
	LCD_AutoText();
	LCD_Write_AirVar(AutoAir, 0, _write);
}











/* ===================================================================*
 * 						Manual
 * ===================================================================*/

void LCD_ManualText(unsigned char min, unsigned char sec)
{
	LCD_WriteValue2(18,124, min);
	LCD_WriteValue2(18,142, sec);
	LCD_WriteStringFont(18,136,":"); 
	LCD_WriteStringFont(16,22,"%"); 
	LCD_WriteStringFont(16,61,"mbar");
	MPX_ReadTank(ManualCirculate, _write);
}

void LCD_WriteManualVar(unsigned char min, unsigned char sec)
{
	MPX_ReadAverage(Manual, _exe);
	LCD_WriteValue2(18,124, min);
	LCD_WriteValue2(18,142, sec);
	TC_DisplayManual_Wait();			//Wait 50ms
}


/* -------------------------------------------------------------------*
 * 						Set Manual Pages
 * -------------------------------------------------------------------*/

void LCD_ManualSet_Page(unsigned char min, unsigned char sec)
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

	//------------------------------------------------------Output----
	OUT_CloseOff();		
	MEM_EEPROM_WriteManualEntry(DS1302_ReadByte(R_HOUR), DS1302_ReadByte(R_MIN), _saveValue);
	//----------------------------------------------------------------
}








/* ===================================================================*
 * 						Set Setup Pages
 * ===================================================================*/

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


/* -------------------------------------------------------------------*
 * 						Set Setup Circulate
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Circulate(void)
{
	LCD_SetupSymbols(sn_circulate);
	LCD_Clean();

	LCD_WriteStringFont(16,40,"Time:"); 
	LCD_Write_Symbol_2(3, 0, n_circulate);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}
 

/* -------------------------------------------------------------------*
 * 						Set Setup Air
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Air(void)
{
	LCD_SetupSymbols(sn_air);
	LCD_Clean();

	LCD_Write_Symbol_2(3, 0, n_air);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* -------------------------------------------------------------------*
 * 						Set Setup SetDown
 * -------------------------------------------------------------------*/

void LCD_SetupSet_SetDown(void)
{
	LCD_SetupSymbols(sn_setDown);
	LCD_Clean();

	LCD_Write_Symbol_2(3, 0, n_setDown);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
	LCD_WriteStringFont(10,0,"Time:"); 
}


/* -------------------------------------------------------------------*
 * 						Set Setup PumpOff
 * -------------------------------------------------------------------*/

void LCD_SetupSet_PumpOff(void)
{
	LCD_SetupSymbols(sn_pumpOff);
	LCD_Clean();

	LCD_Write_Symbol_1(3, 0, n_pumpOff);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* -------------------------------------------------------------------*
 * 						Set Setup Mud
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Mud(void)
{
	LCD_SetupSymbols(sn_mud);
	LCD_Clean();

	LCD_Write_Symbol_1(2, 0, n_mud);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* -------------------------------------------------------------------*
 * 						Set Setup Compressor
 * -------------------------------------------------------------------*/

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


/* -------------------------------------------------------------------*
 * 						Set Setup Phosphor
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Phosphor(void)
{
	LCD_SetupSymbols(sn_phosphor);
	LCD_Clean();

	LCD_Write_Symbol_3(3, 0, n_phosphor);
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* -------------------------------------------------------------------*
 * 						Set Setup InflowPump
 * -------------------------------------------------------------------*/

void LCD_SetupSet_InflowPump(void)
{
	LCD_SetupSymbols(sn_inflowPump);
	LCD_Clean();

	LCD_Write_Symbol_1(3, 0, n_inflowPump);
	LCD_Write_Symbol_2(15, 0, p_sensor);		
	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}


/* -------------------------------------------------------------------*
 * 						Set Setup Cal
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Cal(void)
{
	LCD_SetupSymbols(sn_cal);
	LCD_Clean();

	LCD_Write_Symbol_2(2, 0, n_cal);
	LCD_Write_Symbol_2(15,1, p_level);

	LCD_ControlButtons(sp_esc);
	LCD_ControlButtons(sp_ok);
	LCD_Write_Symbol_2(9,125, p_cal);

	LCD_WriteStringFont(10,1, "mbar:");
	LCD_WriteStringFont(17,60, "mbar");

	MPX_ReadWaterLevel(_init);
	LCD_WriteValue(17, 40, MPX_ReadWaterLevel(_old));

	LCD_MarkTextButton(Setup);
}


/* -------------------------------------------------------------------*
 * 						Set Setup Alarm
 * -------------------------------------------------------------------*/

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


/* -------------------------------------------------------------------*
 * 						Set Setup Watch
 * -------------------------------------------------------------------*/

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


/* -------------------------------------------------------------------*
 * 						Set Setup Zone
 * -------------------------------------------------------------------*/

void LCD_SetupSet_Zone(void)
{
	LCD_SetupSymbols(sn_zone);
	LCD_Clean();

	LCD_Write_Symbol_2(2, 0, n_zone);

	LCD_WriteCtrlButton();
	LCD_MarkTextButton(Setup);
}




/* ===================================================================*
 * 						Set Data Pages
 * ===================================================================*/

void LCD_DataSet_Page(void)
{
	LCD_MarkTextButton(Data);
	LCD_Clean();

	LCD_WriteStringFont(0,40,"Data Page"); 
	LCD_MarkTextButton(Data);
	
	LCD_WriteStringFont(6,0,"Choose Data:"); 

	LCD_Write_TextButton(9, 0, Auto, 1); 		
	LCD_Write_TextButton(9, 40, Manual, 1);	
	LCD_Write_TextButton(9, 80, Setup, 1); 	

	LCD_Write_Symbol_3(	3, 	140, p_arrowUp);		
	LCD_Write_Symbol_3(	14,	140, p_arrowDown);
	LCD_Write_Symbol_3(	8, 	140, p_line);
}


/* -------------------------------------------------------------------*
 * 						Set Data Auto
 * -------------------------------------------------------------------*/

void LCD_DataSet_Auto(void)
{
	LCD_ClrSpace(4,0,9,135);

	LCD_WriteDataHeader_Auto(); 
	LCD_Write_TextButton(22, 0, Auto, 0);
	
	LCD_WriteStringMyFont(1,148,";8");
	LCD_WriteMyFont(1,144,1);
	LCD_WriteAutoEntryPage(Page1);					//Page
}


/* -------------------------------------------------------------------*
 * 						Set Data Manual
 * -------------------------------------------------------------------*/

void LCD_DataSet_Manual(void)
{
	LCD_ClrSpace(4,0,9,135);

	LCD_WriteDataHeader_Manual();
	LCD_Write_TextButton(22, 40, Manual, 0);	
	
	LCD_WriteStringMyFont(1,144,"1;2");
	LCD_WriteManualEntryPage(Page1);
}


/* -------------------------------------------------------------------*
 * 						Set Data Setup
 * -------------------------------------------------------------------*/

void LCD_DataSet_Setup(void)
{
	LCD_ClrSpace(4,0,9,135);

	LCD_WriteDataHeader_Setup(); 
	LCD_Write_TextButton(22, 80, Setup, 0);
	
	LCD_WriteStringMyFont(1,144,"1;2");
	LCD_WriteSetupEntryPage(Page1);
}







/* ===================================================================*
 * 						Set Pin Page
 * ===================================================================*/

void LCD_PinSet_Page(void)
{
	unsigned char i=0;

	LCD_Clean();
	LCD_WriteStringFont(0,0,"Pin-Code:"); 

	for(i=0; i<12; i++)
	{
		LCD_pPinButtons(i);
	}

	LCD_WriteStringFont(3,125,"xxxx"); 
}












/* ===================================================================*/
/* ===================================================================*
 * 						FUNCTIONS Buttons and mark
 * ===================================================================*/
/* -------------------------------------------------------------------*
 * 	Here you can configurate the settings of Buttons and mark
 * -------------------------------------------------------------------*/
/* ===================================================================*/



/* ===================================================================*
 * 						Buttons
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Main Buttons on Bottom
 * -------------------------------------------------------------------*/

void LCD_TextButton(t_textSymbols text, unsigned char pos)
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



/* -------------------------------------------------------------------*
 * 						Control Buttons
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	LCD_ControlButtons
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Loads Control Buttons: ESC, OK,//,Minus,Plus
 * -------------------------------------------------------------------*/

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


/* -------------------------------------------------------------------*
 * 						Write Control Buttons App
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	LCD_WriteCtrlButton
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes TextButtons and mark selected one
 * -------------------------------------------------------------------*/

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




/* -------------------------------------------------------------------*
 * 						On/Off Values
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	LCD_On/OffValue
 * --------------------------------------------------------------
 * 	parameter:		unsigned char 	value	-	Value to Display
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes TextButtons and mark selected one
 * -------------------------------------------------------------------*/

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







/* ===================================================================*
 * 						Mark
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Mark Button
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	LCD_MarkTextButton
 * --------------------------------------------------------------
 * 	parameter:		t_textSymbols 	text	-	Button to mark
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes TextButtons and mark selected one
 * -------------------------------------------------------------------*/

void LCD_MarkTextButton(t_textSymbols text)	
{
	unsigned char i=0;
	for(i=0; i<4; i++)
	{
		LCD_TextButton(i, 1);			//Write TextButtons
	}
	LCD_TextButton(text, 0);
}



/* -------------------------------------------------------------------*
 * 						Mark Symbol
 * -------------------------------------------------------------------*/

void LCD_Mark_ManualSymbol(t_SetupSym sym)	
{
	unsigned char i=0;
	for(i=12; i<20; i++)
	{
		if((12+sym==i))	LCD_SetupSymbols(sym);		//Write negative Manual Symbol
		else			LCD_SetupSymbols(i);		//Write positive Manual Symbol
	}
}







/**********************************************************************\
 * END of xmA_Display_app.h
\**********************************************************************/


