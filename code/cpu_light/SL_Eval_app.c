/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Eval-App-SourceFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	This ApplicationFile contains the Display Evaluation
* ------------------------------------------------------------------														
*	Date:			29.06.2011  	
* 	lastChanges:										
\**********************************************************************/


#include "SL_Define_sym.h"

#include "SL_Display_driver.h"
#include "SL_Touch_driver.h"

#include "SL_Display_app.h"
#include "SL_Touch_app.h"
#include "SL_Eval_app.h"
#include "SL_Output_app.h"

#include "SL_DS1302_driver.h"
#include "SL_MCP9800_driver.h"
#include "SL_MPX_driver.h"

#include "SL_TC_func.h"
#include "SL_Memory_app.h"
#include "SL_PORT_func.h"

#include "SL_Display_sym.h"




/* ===================================================================*
 * 						O2 Evaluation
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Eval_Oxygen
 * --------------------------------------------------------------
 * 	parameter:		unsinged char min	-	Minutes
 *					t_Oxygen oxCmd		-	O2 Command
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Oxygen_Count:	Count Value for Entry
 *	Oxygen_Write:	Write to Entry
 *	Oxygen_Clear:	Clears all existing ErrorCode, 
 *					call bevore Write Entry!
 * -------------------------------------------------------------------*/

void Eval_Oxygen(unsigned char min, t_Oxygen oxCmd)
{
	static int	o2=0;
	static unsigned char o2Min=0;
	static unsigned char o2Counter=0;

	switch(oxCmd)
	{
		case Oxygen_Write2Entry:		
				MEM_EEPROM_WriteAutoEntry(o2, 0, Write_o2);	break;		//Write Entry

		case Oxygen_Clear:	o2= 0x00;	o2Counter=0x00;							break;
							
		case Oxygen_Count:	if(o2Min!= min){	o2Counter++;	o2Min= min;	}
							if(o2Counter>=1){	o2Counter=0;	o2++;}			break;		

		default:																break;
	}
}







/* ===================================================================*
 * 						PIN
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Eval_PinWrite
 * --------------------------------------------------------------
 * 	parameter:		unsigned char pin		-	pressed Pin Number Button
 *					unsigned char codePos	-	Code Position
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes on Number to Display, corresponding to pressed NumButton
 * -------------------------------------------------------------------*/

void Eval_PinWrite(unsigned char pin, unsigned char codePos)
{
	LCD_nPinButtons(pin);							//Mark Pin
	LCD_WriteFont(3, (125+(6*codePos)), pin+15);	//Write Number
}



/*--------------------------------------------------------------------*
 * 	Eval_PinDel
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Delete written Numbers
 * -------------------------------------------------------------------*/

void Eval_PinDel(void)
{
	LCD_nPinButtons(10);
	LCD_WriteStringFont(3,125,"xxxx"); 
}



/*--------------------------------------------------------------------*
 * 	Eval_PinDel
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	unmark all Buttons
 * -------------------------------------------------------------------*/

void Eval_PinClr(unsigned char *pin)
{
	unsigned char i=0;
	
	for(i=0; i<11; i++)
	{
		if(*pin) LCD_pPinButtons(i);
		pin++;
	}
}









/* ===================================================================*
 * 						Auto
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Eval_Comp_OpHours
 * --------------------------------------------------------------
 * 	parameter:		unsigned char add	-	add one hour
 * --------------------------------------------------------------
 * 	return:			int	hours			-	Operating Hours
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Returns operating hours of Compressor, if add hours++
 * -------------------------------------------------------------------*/

int Eval_Comp_OpHours(t_FuncCmd cmd)
{
	int hours=0;
	
	hours=	DS1302_Read_Comp_OpHours();
	if(cmd== _add)
	{
		hours++;
		DS1302_Write_Comp_OpHours(hours);
		hours=	DS1302_Read_Comp_OpHours();
	}
	
	return hours;
}








/* ===================================================================*
 * 						Countdown 
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Eval_Countdown
 * --------------------------------------------------------------
 * 	parameter:		unsigned char *value	-	Pointer on CountValue
 * --------------------------------------------------------------
 * 	return:			unsigned char 1/0		-	CountDown End reached?
 * 	Descirption:	
 * --------------------------------------------------------------
 *	If *value==0, return 1	else return 0
 * -------------------------------------------------------------------*/

unsigned char Eval_CountDown(unsigned char *cMin, unsigned char *cSec)
{
	unsigned char 			min=0;
	unsigned char			sec=0;
	static unsigned char 	count=0;
	static unsigned char	ctOld=0;
	unsigned char			sTC=0;

	min=	*cMin;
	sec=	*cSec;
	
	count=	DS1302_ReadByte(R_SEC);
	sTC=	TC_MainAuto_SafetyTC(_exe);			//Timer Safety

	if(count != ctOld	|| sTC)	
	{
		ctOld= count;
		if(sec<1)
		{
			sec=60;
			min--;
		}
		sec--;
		TC_MainAuto_SafetyTC(_reset);
	}
	
	*cMin= min;
	*cSec= sec;

	if(!min && !sec)	return 1;
	return 0;
}



/* -------------------------------------------------------------------*
 * 						Auto Countdown
 * -------------------------------------------------------------------*/

unsigned char Eval_AutoCountDown(int *cMin, int *cSec)
{
	int 					min=0;
	int						sec=0;
	static unsigned char 	count=0;
	static unsigned char	ctOld=0;
	unsigned char 			sTC=0;

	min=	*cMin;								//Read Min
	sec=	*cSec;								//Read Sec
	
	count=	DS1302_ReadByte(R_SEC);				//Read DS1302 Sek
	sTC=	TC_MainAuto_SafetyTC(_exe);			//Timer Safety

	if(count != ctOld 	|| sTC)									
	{
		ctOld= count;
		if(sec<1)
		{
			sec=60;
			min--;
			if(min<=0)	min= 0;					//limit
		}
		sec--;
		TC_MainAuto_SafetyTC(_reset);
	}
	
	*cMin= min;									//Write Min
	*cSec= sec;									//Write Sec

	if((min<=0) && !sec)	return 1;			//CountDown ends
	return 0;
}






/* ===================================================================*
 * 						Setup	
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Eval_SetupPlus/Minus
 * --------------------------------------------------------------
 * 	parameter:		unsigned char value		-	momentaner wert
 *					unsigned char max/min	-	maximal wert
 * --------------------------------------------------------------
 * 	return:			unsigned char new	-	neuer wert
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Evaluation of Circulate Page, write variables
 * -------------------------------------------------------------------*/

unsigned char Eval_SetupPlus(unsigned char value, unsigned char max)
{
	if(value<max)	value++;
	else value= value;
	return value;
}

unsigned char Eval_SetupMinus(unsigned char	value, unsigned char min)
{
	if(value>min)	value--;
	else value= value;
	return value;
}

int Eval_SetupPlus2(int value, int max)
{
	if(value<max)	value++;
	else value= value;
	return value;
}

int Eval_SetupMinus2(int	value, int min)
{
	if(value>min)	value--;
	else value= value;
	return value;
}



/*--------------------------------------------------------------------*
 * 	Eval_SetupCirculateMark
 * --------------------------------------------------------------
 * 	parameter:		unsigned char on	-	Value to mark 		
 * --------------------------------------------------------------
 * 	return:			unsigned char new	-	neuer wert
 * 	Descirption:	
 * --------------------------------------------------------------
 *	First clear, then mark
 * -------------------------------------------------------------------*/

void Eval_SetupCircSensorMark(unsigned char sensor)
{
	switch (sensor)
	{
		case 0:		LCD_Write_Symbol_2(15, 0, p_sensor);  
					LCD_FillSpace	(15, 39, 4, 31);
					LCD_WriteStringFontNeg(16,40,"Time:");		 	break;
		case 1:		LCD_Write_Symbol_2(15, 0, n_sensor); 
					LCD_ClrSpace	(15, 39, 3, 31); 
					LCD_WriteStringFont(16,40,"Time:");		 		break;
		default: 													break;
	}
}



/*--------------------------------------------------------------------*
 * 	Eval_SetupCirculateTextMark
 * --------------------------------------------------------------
 * 	parameter:		unsigned char on	-	Value to mark 		
 * --------------------------------------------------------------
 * 	return:			unsigned char new	-	neuer wert
 * 	Descirption:	
 * --------------------------------------------------------------
 *	First clear, then mark
 * -------------------------------------------------------------------*/

void Eval_SetupCircTextMark(unsigned char on, unsigned char *p_var)
{	
	unsigned char	var[4]={0};
	unsigned char	i=0;

	LCD_ClrSpace(15, 70, 3, 20);

	for(i=0; i<4; i++)
	{
		var[i]= *p_var;
		p_var++;
	}
	
	LCD_OnValue(var[0]);	
	LCD_OffValue(var[1]);
	LCD_WriteValue(16,72, ((var[3]<<8) | var[2]));
	
	switch (on)
	{
		case 0:		LCD_OnValueNeg(var[0]); 							break;
		case 1:		LCD_OffValueNeg(var[1]); 							break;
		case 2:		LCD_FillSpace	(15, 70, 4, 20);
					LCD_WriteValueNeg(16,72, ((var[3]<<8) | var[2])); 	break;
		default: 														break;
	}
}

void Eval_SetupAirTextMark(unsigned char on, unsigned char *p_var)
{	
	unsigned char	var[4]={0};
	unsigned char	i=0;

	LCD_ClrSpace(15, 39, 3, 51);
	LCD_WriteStringFont(16,40,"Time:");

	for(i=0; i<4; i++)
	{
		var[i]= *p_var;
		p_var++;
	}
	
	LCD_OnValue(var[0]);	
	LCD_OffValue(var[1]);
	LCD_WriteValue(16,72, ((var[3]<<8) | var[2]));
	
	switch (on)
	{
		case 0:		LCD_OnValueNeg(var[0]); 							break;
		case 1:		LCD_OffValueNeg(var[1]); 							break;
		case 2:		LCD_FillSpace	(15, 70, 4, 20);
					LCD_WriteValueNeg(16,72, ((var[3]<<8) | var[2])); 	break;
		default: 														break;
	}
}




/*--------------------------------------------------------------------*
 * 	Eval_SetupPumpMark
 * --------------------------------------------------------------
 * 	parameter:		unsigned char on	-	Value to mark 		
 * --------------------------------------------------------------
 * 	return:			unsigned char new	-	neuer wert
 * 	Descirption:	
 * --------------------------------------------------------------
 *	First clear, then mark
 * -------------------------------------------------------------------*/

void Eval_SetupPumpMark(unsigned char mark)
{
	LCD_Write_Symbol_2(15, 40, p_compressor); 
	LCD_Write_Symbol_3(15, 90, p_pump); 
	LCD_Write_Symbol_1(15, 120, p_pump2); 

	switch (mark)
	{
		case 0:		LCD_Write_Symbol_2(15, 40, n_compressor);  		break;
		case 1:		LCD_Write_Symbol_3(15, 90, n_pump);  			break;
		case 2:		LCD_Write_Symbol_1(15, 120, n_pump2); 			break;
		default: 													break;
	}
}



/*--------------------------------------------------------------------*
 * 	Eval_SetupWatchMark
 * --------------------------------------------------------------
 * 	parameter:		unsigned char on	-	Value to mark 		
 * --------------------------------------------------------------
 * 	return:			unsigned char new	-	neuer wert
 * 	Descirption:	
 * --------------------------------------------------------------
 *	First clear, then mark
 * -------------------------------------------------------------------*/

void Eval_SetupWatchMark(t_DateTime time, unsigned char *p_dT)
{
	unsigned char i=0; 
	unsigned char var;
	
	for(i=5; i<11; i++)
	{
		LCD_DateTime(i, *p_dT);
		p_dT++;
	}

	p_dT= p_dT-6+time;
	var= *p_dT;
	
	switch (time)
	{
		case n_h:		LCD_DateTime(n_h, var); 		break;
		case n_min:		LCD_DateTime(n_min, var); 		break;
		case n_day:		LCD_DateTime(n_day, var); 		break;
		case n_month:	LCD_DateTime(n_month, var);	 	break;
		case n_year:	LCD_DateTime(n_year, var);		break; 
		default: 										break;
	}
}






/* ===================================================================*
 * 						Memory
 * ===================================================================*/

/*--------------------------------------------------------------------*			
 * 	Eval_Memory_NoAutoEntry()
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			unsigned char *p_count	-	Pointer on noEntry
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Finds out the the placement of a no entry, return position
 * -------------------------------------------------------------------*/

unsigned char *Eval_Memory_NoEntry(t_textSymbols data)
{
	static unsigned char memCount[3]={2,0,0};

	unsigned char 	*p_count= memCount;				//Return Pointer
	unsigned char 	eep=0;
	unsigned char 	i=0;
	unsigned char 	stop=0;
	
	unsigned char startPa=0;
	unsigned char endPa=0;

	switch(data)
	{
		case Auto:		startPa= 2;		endPa= 17;	memCount[0]= 2;		break;	//8Pages
		case Manual:	startPa= 17; 	endPa= 21;	memCount[0]= 17;	break;	//2Pages
		case Setup:		startPa= 22;	endPa= 26;	memCount[0]= 22;	break;	//2Pages
		default:														break;
	}

	for(eep= startPa; eep< endPa; eep++)							//pages
	{
		for(i=0; i<4; i++)									//Entrys
		{
			memCount[1]= i;
			if(!(MEM_EEPROM_ReadData(eep, i, DATA_day)))	//Day
			{	
				stop=1; 
				break;	
			}
		}

		if(stop) 
		{	
			memCount[2]=1;				//Null found
			break;					//Loop termination
		}
		else memCount[2]=0;			//No null entries found
		memCount[0]++;
	}	

	return p_count;
}



/* ---------------------------------------------------------------*
 * 					Old Entry
 * ---------------------------------------------------------------*/

unsigned char *Eval_Memory_OldestEntry(t_textSymbols data)
{
	static unsigned char old[2]={2,0};

	unsigned char *p_old= old;	
	unsigned char eep=0;
	unsigned char i=0;
	
	unsigned char startPa=0;
	unsigned char endPa=0;

	unsigned char day=31;
	unsigned char month=12;
	unsigned char year=60;
	unsigned char h=23;
	unsigned char min= 59;

	unsigned char rDay=0;
	unsigned char rMonth=0;
	unsigned char rYear=0;
	unsigned char rH=0;
	unsigned char rMin=0;
	
	switch(data)
	{
		case Auto:		startPa= 2;		endPa= 17;	old[0]= 2;		break;	//8Pages
		case Manual:	startPa= 17; 	endPa= 21;	old[0]= 17;		break;	//2Pages
		case Setup:		startPa= 22;	endPa= 26;	old[0]= 22;		break;	//2Pages
		default:													break;
	}

	for(eep= startPa; eep< endPa; eep++)					//Pages
	{
		for(i=0; i<4; i++)									//Entrys
		{
			rDay= 	MEM_EEPROM_ReadData(eep, i, DATA_day);
			rMonth= MEM_EEPROM_ReadData(eep, i, DATA_month);
			rYear=	MEM_EEPROM_ReadData(eep, i, DATA_year);
			rH=		MEM_EEPROM_ReadData(eep, i, DATA_hour);
			rMin=	MEM_EEPROM_ReadData(eep, i, DATA_minute);

			if(	(rDay)			&&	(
				(rYear	< year) ||	
				(rMonth	< month	&&	rYear <= year)	||
				(rDay	< day	&&	rMonth <= month	&& 	rYear <= year)	||
				(rH		< h		&& 	rDay <= day		&&	rMonth <= month	&& 	rYear <= year)	||
				(rMin	< min	&& 	rH <= h			&& 	rDay <= day		&&	rMonth <= month	&& 	rYear <= year)))
				{	
						year= 	rYear;
						month=	rMonth;
						day=	rDay;
						h=		rH;
						min=	rMin;
						old[0]=	eep;
						old[1]= i;		}
		}
	}

	return p_old;
}



/* ---------------------------------------------------------------*
 * 					Latest Entry
 * ---------------------------------------------------------------*/

unsigned char *Eval_Memory_LatestEntry(t_textSymbols data)
{
	static unsigned char latest[2]={2,0};

	unsigned char *p_latest= latest;	
	unsigned char eep=0;
	unsigned char i=0;
	
	unsigned char startPa=0;
	unsigned char endPa=0;

	unsigned char day=0;
	unsigned char month=0;
	unsigned char year=0;
	unsigned char h=0;
	unsigned char min=0;
	
	unsigned char rDay=0;
	unsigned char rMonth=0;
	unsigned char rYear=0;
	unsigned char rH=0;
	unsigned char rMin=0;
	
	switch(data)
	{
		case Auto:		startPa= 2;		endPa= 17;	latest[0]= 2;		break;	//8Pages
		case Manual:	startPa= 17; 	endPa= 21;	latest[0]= 17;		break;	//2Pages
		case Setup:		startPa= 22;	endPa= 26;	latest[0]= 22;		break;	//2Pages
		default:													break;
	}

	for(eep= startPa; eep< endPa; eep++)					//Pages
	{
		for(i=0; i<4; i++)									//Entrys
		{
			rDay= 	MEM_EEPROM_ReadData(eep, i, DATA_day);
			rMonth= MEM_EEPROM_ReadData(eep, i, DATA_month);
			rYear=	MEM_EEPROM_ReadData(eep, i, DATA_year);
			rH=		MEM_EEPROM_ReadData(eep, i, DATA_hour);
			rMin=	MEM_EEPROM_ReadData(eep, i, DATA_minute);

			if(	(rDay)			&&	(
				(rYear	> year) ||	
				(rMonth	> month	&&	rYear >= year)	||
				(rDay	> day	&&	rMonth >= month	&& 	rYear >= year)	||
				(rH		> h		&& 	rDay >= day		&&	rMonth >= month	&& 	rYear >= year)	||
				(rMin	> min	&& 	rH >= h			&& 	rDay >= day		&&	rMonth >= month	&& 	rYear >= year)))
				{	
						year= 		rYear;
						month=		rMonth;
						day=		rDay;
						h=			rH;
						min=		rMin;
						latest[0]=	eep;
						latest[1]= 	i;		}
		}
	}

	return p_latest;
}








/**********************************************************************\
 * END of xmA_Display_app.c
\**********************************************************************/





