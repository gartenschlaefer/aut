/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Eval-App-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	This ApplicationFile contains the Display Evaluation
*	and Built-In Functions
* ------------------------------------------------------------------
*	Date:			29.06.2011
\**********************************************************************/


#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "memory_app.h"
#include "touch_app.h"
#include "eval_app.h"
#include "output_app.h"

#include "touch_driver.h"
#include "mcp7941_driver.h"
#include "mcp9800_driver.h"
#include "mpx_driver.h"

#include "tc_func.h"
#include "port_func.h"

#include "lcd_sym.h"




/* ==================================================================*
 * 						O2 Evaluation
 * ==================================================================*/

void Eval_Oxygen(t_FuncCmd cmd, int min)
{
	static int o2 = 0;
	static int o2Min = 0;
	static int o2Counter = 0;

	switch(cmd)
	{
		case _entry:
      MEM_EEPROM_WriteAutoEntry(o2, 0, Write_o2);
      break;

		case _clear:
		  o2 = 0;
		  o2Counter = 0;
		  break;

		case _count:
      if(o2Min != min){	o2Counter++;	o2Min = min;}
      if(o2Counter >= 1){	o2Counter = 0;	o2++;}
      break;

    case _dec:
      o2--;
      break;

		default: break;
	}
}




/* ==================================================================*
 * 						PIN
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Eval_PinWrite
 * --------------------------------------------------------------
 *	Writes on Number to Display, corresponding to pressed NumButton
 * ------------------------------------------------------------------*/

void Eval_PinWrite(unsigned char pin, unsigned char codePos)
{
	LCD_nPinButtons(pin);							//Mark Pin
	LCD_WriteFont(3, (125+(6*codePos)), pin+15);	//Write Number
}



/*-------------------------------------------------------------------*
 * 	Eval_PinDel
 * --------------------------------------------------------------
 *	Delete written Numbers
 * ------------------------------------------------------------------*/

void Eval_PinDel(void)
{
	LCD_WriteStringFont(3,125,"xxxx");
}



/*-------------------------------------------------------------------*
 * 	Eval_PinDel
 * --------------------------------------------------------------
 *	unmark all Buttons
 * ------------------------------------------------------------------*/

void Eval_PinClr(unsigned char *pin)
{
	unsigned char i=0;

	for(i=0; i<11; i++)
	{
		if(pin[i]) LCD_pPinButtons(i);
	}
}



/* ==================================================================*
 * 						Auto
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Eval Comp_OpHours
 * --------------------------------------------------------------
 *	Returns operating hours of Compressor, if add hours++
 * ------------------------------------------------------------------*/

int Eval_Comp_OpHours(t_FuncCmd cmd)
{
	int hours = 0;

	hours =	MCP7941_Read_Comp_OpHours();
	if(cmd == _add)
	{
		hours++;
		MCP7941_Write_Comp_OpHours(hours);
		hours =	MCP7941_Read_Comp_OpHours();
	}

	return hours;
}



/* ==================================================================*
 * 						Countdown
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Eval_Countdown
 * --------------------------------------------------------------
 *	If *value==0, return 1	else return 0
 * ------------------------------------------------------------------*/

unsigned char Eval_CountDown(int *cMin, int *cSec)
{
	int min = 0;
	int sec = 0;
	unsigned char count = 0;
	static unsigned char ctOld = 0;
	unsigned char sTC = 0;

	min = *cMin;
	sec = *cSec;

	// savety for seconds
	if(sec < 0 || sec > 61) sec = 0;	
	
	count = MCP7941_ReadByte(TIC_SEC);
	sTC = TCD1_MainAuto_SafetyTC(_exe);			//Timer Safety

  // Countdown
	if(count != ctOld	|| sTC)
	{
		ctOld = count;

		// minute counter
		if(!sec && min)
		{
			sec = 60;
			min--;

			//*** entry debug every minute
			if (DEBUG && DEB_ENTRY)
			{
				MEM_EEPROM_WriteAutoEntry(10, 2, Write_Error);
				MEM_EEPROM_WriteAutoEntry(10, 2, Write_o2);
				MEM_EEPROM_WriteAutoEntry(10, 2, Write_Entry);
				MEM_EEPROM_WriteManualEntry(0, 0, _write);
				MEM_EEPROM_WriteSetupEntry();
			}
		}
		if(sec) sec--;
		TCD1_MainAuto_SafetyTC(_reset);
	}

  // End of Timer
  if(!sec && !min)
	{
	  TCD1_MainAuto_SafetyTC(_reset);
	  min = 0;
	  sec = 5;
    *cMin = min;
	  *cSec = sec;
	  return 1;
  }

  *cMin = min;
	*cSec = sec;

	return 0;
}



/* ==================================================================*
 * 						Setup
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Eval_SetupPlus/Minus
 * --------------------------------------------------------------
 *	Evaluation of Circulate Page, write variables
 * ------------------------------------------------------------------*/

int Eval_SetupPlus(int value, int max)
{
	if(value < max)	value++;
	else value = value;
	return value;
}

int Eval_SetupMinus(int	value, int min)
{
	if(value > min)	value--;
	else value = value;
	return value;
}


/*-------------------------------------------------------------------*
 * 	Eval_SetupCirculateMark
 * --------------------------------------------------------------
 *	First clear, then mark
 * ------------------------------------------------------------------*/

void Eval_SetupCircSensorMark(unsigned char sensor)
{
	switch (sensor)
	{
		case 0:		LCD_Write_Symbol_2(15, 0, p_sensor);
					LCD_FillSpace	(15, 39, 4, 31);
					LCD_WriteStringFontNeg(16,40,"Time:");		 	break;
		case 1:		LCD_Write_Symbol_2(15, 0, n_sensor);
					LCD_ClrSpace	(15, 39, 4, 31);
					LCD_WriteStringFont(16,40,"Time:");		 		break;
		default: 													break;
	}
}


/*-------------------------------------------------------------------*
 * 	Eval_SetupCirculateTextMark
 * --------------------------------------------------------------
 *	First clear, then mark
 * ------------------------------------------------------------------*/

void Eval_SetupCircTextMark(unsigned char on, unsigned char *p_var)
{
	unsigned char	var[4]={0};
	unsigned char	i=0;

	LCD_ClrSpace(15, 70, 4, 20);

	for(i=0; i<4; i++)
	{
		var[i]= *p_var;
		p_var++;
	}

	LCD_OnValue(var[0]);
	LCD_OffValue(var[1]);
	LCD_WriteValue3(16,72, ((var[3]<<8) | var[2]));

	switch (on)
	{
		case 0:		LCD_OnValueNeg(var[0]); 							break;
		case 1:		LCD_OffValueNeg(var[1]); 							break;
		case 2:		LCD_FillSpace	(15, 70, 4, 20);
					LCD_WriteValueNeg3(16,72, ((var[3]<<8) | var[2])); 	break;
		default: 														break;
	}
}

void Eval_SetupAirTextMark(unsigned char on, unsigned char *p_var)
{
	unsigned char	var[4]={0};
	unsigned char	i=0;

	LCD_ClrSpace(15, 39, 4, 51);
	LCD_WriteStringFont(16,40,"Time:");

	for(i=0; i<4; i++)
	{
		var[i]= *p_var;
		p_var++;
	}

	LCD_OnValue(var[0]);
	LCD_OffValue(var[1]);
	LCD_WriteValue3(16,72, ((var[3]<<8) | var[2]));

	switch (on)
	{
		case 0:		LCD_OnValueNeg(var[0]); 							break;
		case 1:		LCD_OffValueNeg(var[1]); 							break;
		case 2:		LCD_FillSpace	(15, 70, 4, 20);
					LCD_WriteValueNeg3(16,72, ((var[3]<<8) | var[2])); 	break;
		default: 														break;
	}
}


/*-------------------------------------------------------------------*
 * 	Eval_SetupPumpMark
 * --------------------------------------------------------------
 *	First clear, then mark
 * ------------------------------------------------------------------*/

void Eval_SetupPumpMark(unsigned char mark)
{
	LCD_Write_Symbol_2(15, 45, p_compressor);
	LCD_Write_Symbol_3(15, 90, p_pump);
	LCD_Write_Symbol_1(15, 120, p_pump2);

	switch (mark)
	{
		case 0:		LCD_Write_Symbol_2(15, 45, n_compressor);  		break;
		case 1:		LCD_Write_Symbol_3(15, 90, n_pump);  			break;
		case 2:		LCD_Write_Symbol_1(15, 120, n_pump2); 			break;
		default: 													break;
	}
}


/*-------------------------------------------------------------------*
 * 	Eval_SetupWatchMark
 * --------------------------------------------------------------
 *	First clear, then mark
 * ------------------------------------------------------------------*/

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
		case n_h:		  LCD_DateTime(n_h, var); 		break;
		case n_min:		LCD_DateTime(n_min, var); 		break;
		case n_day:		LCD_DateTime(n_day, var); 		break;
		case n_month:	LCD_DateTime(n_month, var);	 	break;
		case n_year:	LCD_DateTime(n_year, var);		break;
		default: 										break;
	}
}



/* ==================================================================*
 * 						Memory - Entries
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Eval_Memory_NoAutoEntry()
 * --------------------------------------------------------------
 *	Finds out the the placement of a no entry, return position
 * ------------------------------------------------------------------*/

unsigned char *Eval_Memory_NoEntry(t_textButtons data)
{
	static unsigned char memCount[3] = {AUTO_START_PAGE, 0, 0};

	unsigned char *p_count = memCount;
	unsigned char eep = 0;
	unsigned char i = 0;
	unsigned char stop = 0;

	unsigned char startPa = AUTO_START_PAGE;
	unsigned char endPa = AUTO_END_PAGE;

	// determine start and end page
	switch(data)
	{
		case Auto:		startPa = AUTO_START_PAGE; endPa = AUTO_END_PAGE; break;
		case Manual:	startPa = MANUAL_START_PAGE; endPa = MANUAL_END_PAGE; break;
		case Setup:		startPa = SETUP_START_PAGE; endPa = SETUP_END_PAGE; break;
		default: break;
	}

	// start page
	memCount[0] = startPa;

	// pages
	for(eep = startPa; eep <= endPa; eep++)
	{
		// update page
		memCount[0] = eep;

		// entries
		for(i = 0; i < 4; i++)
		{
			memCount[1] = i;
			if(!(MEM_EEPROM_ReadData(eep, i, DATA_day)))
			{
				stop = 1;
				break;
			}
		}

		// loop termination if null found
		if(stop)
		{
			// null indicator
			memCount[2] = 1;
			break;
		}

		//No null entries found
		else memCount[2] = 0;
	}

	return p_count;
}


/* ---------------------------------------------------------------*
 * 					Old Entry
 * ---------------------------------------------------------------*/

unsigned char *Eval_Memory_OldestEntry(t_textButtons data)
{
	static unsigned char old[2] = {AUTO_START_PAGE, 0};

	unsigned char *p_old= old;
	unsigned char eep=0;
	unsigned char i=0;

	unsigned char startPa = AUTO_START_PAGE;
	unsigned char endPa = AUTO_END_PAGE;

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

	// determine start and end page
	switch(data)
	{
		case Auto:		startPa = AUTO_START_PAGE; endPa = AUTO_END_PAGE; break;
		case Manual:	startPa = MANUAL_START_PAGE; endPa = MANUAL_END_PAGE; break;
		case Setup:		startPa = SETUP_START_PAGE; endPa = SETUP_END_PAGE; break;
		default: break;
	}

	// start page
	old[0] = startPa;

	// pages
	for(eep = startPa; eep <= endPa; eep++)
	{
		// entries
		for(i=0; i<4; i++)
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
				(rMin	<= min	&& 	rH <= h			&& 	rDay <= day		&&	rMonth <= month	&& 	rYear <= year)))
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

unsigned char *Eval_Memory_LatestEntry(t_textButtons data)
{
	static unsigned char latest[2] = {AUTO_START_PAGE, 0};

	unsigned char *p_latest= latest;
	unsigned char eep=0;
	unsigned char i=0;

	unsigned char startPa = AUTO_START_PAGE;
	unsigned char endPa = AUTO_END_PAGE;

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

	// determine start and end page
	switch(data)
	{
		case Auto:		startPa = AUTO_START_PAGE; endPa = AUTO_END_PAGE; break;
		case Manual:	startPa = MANUAL_START_PAGE; endPa = MANUAL_END_PAGE; break;
		case Setup:		startPa = SETUP_START_PAGE; endPa = SETUP_END_PAGE; break;
		default: break;
	}

	// start page
	latest[0] = startPa;

	// pages
	for(eep = startPa; eep <= endPa; eep++)
	{
		// entries
		for(i=0; i<4; i++)
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
				(rMin	>= min	&& 	rH >= h			&& 	rDay >= day		&&	rMonth >= month	&& 	rYear >= year)))
				{
          year = rYear;
          month = rMonth;
          day =	rDay;
          h = rH;
          min =	rMin;
          latest[0] =	eep;
          latest[1] = i;
        }
		}
	}

	return p_latest;
}

