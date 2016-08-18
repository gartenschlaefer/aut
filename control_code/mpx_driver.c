/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    MPX-driver-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	MPX Drucksensor driver file
* ------------------------------------------------------------------
*	Date:			    09.06.2011
* lastChanges:  24.02.2015
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "lcd_sym.h"

#include "memory_app.h"
#include "output_app.h"
#include "tc_func.h"
#include "basic_func.h"
#include "mpx_driver.h"




/* ==================================================================*
 * 						FUNCTIONS - Basics
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MPX_Read
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 * ------------------------------------------------------------------*/

int MPX_Read(void)
{
	int data= 0;

	ADCB.CTRLA |= ADC_CH0START_bm;					      //Start Conversion
	while(!(ADCB.INTFLAGS & (1<<ADC_CH0IF_bp)));	//Wait
	ADCB.INTFLAGS |= (1<<ADC_CH0IF_bp);				    //Reset INT-Flag
	data= ADCB.CH0RES;								            //Data at LEFT
	data= data>>2;

	return data;
}


/* ------------------------------------------------------------------*
 * 						MPX - Read Calibrated
 * ------------------------------------------------------------------*/

int MPX_ReadCal(void)
{
	int cal=0;
	int data=0;

	data= MPX_Read();
	cal= ((MEM_EEPROM_ReadVar(CAL_H_druck)<<8) |
    (MEM_EEPROM_ReadVar(CAL_L_druck)));
	data= data - cal;
	if(data<0) 		data= 0;
	if(data>999)	data= 999;

	return data;
}




/* ==================================================================*
 * 						FUNCTIONS - Apps
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MPX_ReadAverage
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 * ------------------------------------------------------------------*/

int MPX_ReadAverage(t_textButtons page, t_FuncCmd cmd)
{
	static unsigned char 	mpxCount=0;
	static int			 	    mpx[10]={0x00};
	int						        add=0;
	unsigned char			    a= 0;

	//--------------------------------------------------------Clean
	if(cmd == _clean)
	{
		mpxCount= 0;
		for(a = 0; a < 10; a++) mpx[a] = 0x00;
	}

	//--------------------------------------------------------exe
	else if(cmd == _exe)
	{
		mpx[mpxCount] = MPX_ReadCal();						//Pressure
		mpxCount++;

		if(mpxCount > 9)										//Print
		{
			mpxCount = 0;
			for(a = 0; a < 10; a++)	add = add + mpx[a];
			add = add / 10;

			switch(page)
			{
				case Auto:		LCD_WriteValue3_MyFont(13,43, add);
				              return add;	break;

				case Manual:	LCD_WriteValue3(17,42, add);
				              return add;	break;

				case Setup:		return add;	break;
				case Data:	  return add;	break;
				default:									break;
			}
		}
	}
	return 0xFF00;
}


/* ------------------------------------------------------------------*
 * 						Average Value only
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_Value(void)
{
	unsigned char i=0;
	int avAge=0;

	MPX_ReadAverage(Data, _clean);					//MPX_Clean
	for(i=0; i<10; i++)
	{
		avAge= MPX_ReadAverage(Data, _exe);			//Read 1Value
		TCC0_wait_ms(100);							//Wait
		if(!(avAge == 0xFF00))	return avAge;
	}

	return avAge;
}


/* ------------------------------------------------------------------*
 * 						Average notCalibrated
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_UnCal(void)
{
	static unsigned char 	mpxCount=0;
	static int			 	mpx[10]={0x00};
	int						unCal=0;
	unsigned char			a= 0;

	mpx[mpxCount]= MPX_Read();				//Pressure
	mpxCount++;
	if(mpxCount>4)							//5Values
	{
		mpxCount=0;
		for(a=0; a<5; a++)	unCal= unCal+mpx[a];
		unCal= unCal/5;
		return unCal;
	}
	TCC0_wait_ms(100);						//Wait

	return 0xFF00;
}


/* ------------------------------------------------------------------*
 * 						Average UnCal
 * ------------------------------------------------------------------*/

int MPX_ReadAverage_UnCal_Value(void)
{
	unsigned char i=0;
	int avAge=0;

	for(i=0; i<6; i++)
	{
		avAge= MPX_ReadAverage_UnCal();				//Read 1Value
		TCC0_wait_ms(100);							      //Wait
		if(!(avAge == 0xFF00))	return avAge;
	}
	return avAge;
}


/* ------------------------------------------------------------------*
 * 						Average Waterlevel
 * ------------------------------------------------------------------*/

int MPX_LevelCal(t_FuncCmd cmd)
{
	static int 		level = 0;

	switch(cmd)
	{
		//------------------------------------------------ReadFromEEPROM
		case _init:
      level = ((MEM_EEPROM_ReadVar(TANK_H_MinP) << 8) |
               (MEM_EEPROM_ReadVar(TANK_L_MinP)));
      LCD_WriteValue3(17, 40, level);
      break;

		//------------------------------------------------Save2EEPROM
		case _save:
      MEM_EEPROM_WriteVar(TANK_L_MinP, level & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_MinP, ((level >> 8) & 0x00FF));
      break;

    //------------------------------------------------Meassure
		case _new:
      level = MPX_ReadAverage_Value();		    //ReadValue
      break;

    case _write:
      LCD_WriteValue3(17, 40, level); break;  //WriteInSetupCalPage
		default:									        break;
	}
	return level;
}


/*-------------------------------------------------------------------*
 * 	MPX_ReadTank
 * --------------------------------------------------------------
 *	Reads the position of Water, call only in air times
 *  gives back page to go next, or stay in same
 * ------------------------------------------------------------------*/

t_page MPX_ReadTank(t_page page, t_FuncCmd cmd)
{
	static int mpxAv = 0;
	static unsigned char error = 0;

	int	hO2 = 0;
	int hCirc = 0;
	int	minP = 0;
	int	perP = 0;

  //--------------------------------------------------DisabledReadTank
	if(!(MEM_EEPROM_ReadVar(SENSOR_inTank)) ||
  page == AutoPumpOff 					          ||
  page == AutoMud)
  {
    if(page == ManualCirc || page == ManualCircOff ||
    page == ManualAir)
    {
      LCD_Sym_MPX(_mmbar, perP);    //Manual
      return page;
    }
    else if(page == AutoZone)
    {
      LCD_Sym_MPX(_notav, 0);       //---
      return AutoCirc;              //NextStep
    }
    LCD_Sym_MPX(_notav, 0);         //---
    return page;
  }

  //------------------------------------------------ReadVars
	hO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2)<<8) |
        (MEM_EEPROM_ReadVar(TANK_L_O2)));

	hCirc = ((MEM_EEPROM_ReadVar(TANK_H_Circ)<<8) |
          (MEM_EEPROM_ReadVar(TANK_L_Circ)));

	minP = ((MEM_EEPROM_ReadVar(TANK_H_MinP)<<8) |
          (MEM_EEPROM_ReadVar(TANK_L_MinP)));

	//------------------------------------------------exe
	if(cmd == _exe)
	{
		mpxAv = MPX_LevelCal(_new);		        //ReadValue
		if(page == ManualCirc)	return page;	//Return
    LCD_Sym_MPX(_mbar, mpxAv);
		if(MEM_EEPROM_ReadVar(SONIC_on)) return page;	//IfSonicReturn

    //------------------------------------------------Circulate
		if(page == AutoCirc)
		{
			if(mpxAv >= (hO2 + minP))
			{
				error++;				        //Error-Counter
				return AutoSetDown;		  //2SetDown
			}
			error = 0;
			if(mpxAv >= (hCirc + minP))	return AutoAir;
		}

    //------------------------------------------------Air
		else if(page == AutoAir)			//--Air
		{
			if(mpxAv >= (hO2 + minP))	  return AutoSetDown;
		}

    //------------------------------------------------Zone
		else if(page == AutoZone)			//--Zone
		{
			if((mpxAv < (minP - 5)) || (mpxAv > (minP + 5)))  //ToMuchDiff
			  return AutoCirc;

			MPX_LevelCal(_save);
			if(mpxAv >= (hO2 + minP))	  return AutoSetDown;
			if(mpxAv >= (hCirc + minP))	return AutoAir;
			else					              return AutoCirc;
		}
	}

	//------------------------------------------------write
	else if(cmd == _write)
	{
		perP = mpxAv - minP;					    //calc
		if(perP <= 0)	perP = 0;					  //limit
		perP = ((perP * 100) / hO2);			//calc
		if(page == ManualCirc)
		{
			LCD_Sym_MPX(_mmbar, perP);  //ManualWrite
			return page;
		}
    LCD_Sym_MPX(_debug, perP);
	}

	//------------------------------------------------misc
	else if(cmd == _reset)	mpxAv = 0;
	else if(cmd == _error)	if(error >= 2) return ErrorMPX;
	return page;
}



/* ==================================================================*
 * 						FUNCTIONS - Test
 * ==================================================================*/

void MPX_Test(void)
{
	while(1)
	{
		Watchdog_Restart();
		LCD_WriteValue3(1,2, MPX_Read());
		LCD_WriteValue3(4,2, MPX_ReadCal());
		LCD_WriteValue3(1,40, MPX_ReadAverage_UnCal_Value());
		LCD_WriteValue3(4,40, MPX_ReadAverage_Value());
	}
}







/**********************************************************************\
 * End of file
\**********************************************************************/

