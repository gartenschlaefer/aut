/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			MPX-driver-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	SourceCode of MPX Drucksensor from Freescale
* ------------------------------------------------------------------														
*	Date:			09.06.2011  	
* 	lastChanges:											
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_TC_func.h"
#include "SL_Watchdog_func.h"

#include "SL_Display_driver.h"
#include "SL_MPX_driver.h"

#include "SL_Memory_app.h"
#include "SL_Output_app.h"


/* ===================================================================*
 * 						FUNCTIONS - Basics
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	MPX_ADC_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set up ADC for Drucksensor MPX
 * 	MPX -> AD8555 = MPX * Gain -> Singleended PORTB PIN6
 * 	Sensitivity:	0.4mV/kPa
 * -------------------------------------------------------------------*/

void MPX_ADC_Init(void)
{
	ADCB.CTRLB=		ADC_RESOLUTION_12BIT_gc;		//Resolution 12BIT right
				
	ADCB.REFCTRL= 	ADC_REFSEL_AREFA_gc;			//ReferenceVoltage
	ADCB.PRESCALER= ADC_PRESCALER_DIV8_gc;			//Prescaler clk/8
	
	ADCB.CTRLA= ADC_ENABLE_bm;						//ADC ENABLE

	//ADC Channel 
	ADCB.CH0.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|	//MPX
						ADC_CH_GAIN_1X_gc;

	ADCB.CH0.MUXCTRL= 	ADC_CH_MUXPOS_PIN6_gc;

}



/*--------------------------------------------------------------------*
 * 	MPX_Read
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			unsigend char data	-	Conversion Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 * -------------------------------------------------------------------*/

int MPX_Read(void)
{
	int data= 0;
	
	ADCB.CTRLA |= ADC_CH0START_bm;					//Start Conversion
	while(!(ADCB.INTFLAGS & (1<<ADC_CH0IF_bp)));	//Wait
	ADCB.INTFLAGS |= (1<<ADC_CH0IF_bp);				//Reset INT-Flag
	data= ADCB.CH0RES;								//Data at LEFT
	data= data>>2;
	
	return data;
}

int MPX_ReadCal(void)
{
	int cal=0;
	int data=0;
	
	data= MPX_Read();
	cal= ((MEM_EEPROM_ReadVar(CAL_H_druck)<<8) | (MEM_EEPROM_ReadVar(CAL_L_druck)));
	data= data-cal;
	if(data<0) 		data= 0;
	if(data>999)	data= 999;

	return data;
}




/* ===================================================================*
 * 						FUNCTIONS - Apps
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	MPX_ReadAverage
 * --------------------------------------------------------------
 * 	parameter:		t_textSymbols page	-	Page to write pressure
 * --------------------------------------------------------------
 * 	return:			t
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 * -------------------------------------------------------------------*/

int MPX_ReadAverage(t_textSymbols page, t_FuncCmd cmd)
{
	static unsigned char 	mpxCount=0;
	static int			 	mpx[10]={0x00};
	int						add=0;
	unsigned char			a= 0;
	
	//--------------------------------------------------------Clean
	if(cmd == _clean)
	{
		mpxCount= 0;
		for(a=0; a<10; a++)
		{
			mpx[a]= 0x00;
		}
	}
	
	//--------------------------------------------------------exe
	else if(cmd == _exe)
	{
		mpx[mpxCount]= MPX_ReadCal();						//Pressure
		mpxCount++;

		if(mpxCount>9)
		{
			mpxCount=0;
			for(a=0; a<10; a++)	add= add+mpx[a];
			add= add/10;

			switch(page)
			{
				case Auto:		LCD_WriteValue_MyFont(13,43, add);	return add;	break;
				case Manual:	LCD_WriteValue(16,42, add);			return add;	break;
				case Setup:											return add;	break;
				case Data:											return add;	break;
				default:														break;
			}
		}
	}
	return 0xFF00;
}


/* -------------------------------------------------------------------*
 * 						Average Value only
 * -------------------------------------------------------------------*/

int MPX_ReadAverage_Value(void)
{
	unsigned char i=0;
	int avAge=0;
	
	MPX_ReadAverage(Data, _clean);					//MPX_Clean
	for(i=0; i<10; i++)
	{
		avAge= MPX_ReadAverage(Data, _exe);			//Read 1Value
		TC_DisplayData_Wait();						//Wait 100ms
		if(!(avAge == 0xFF00))	return avAge;
	}

	return avAge;
}


/* -------------------------------------------------------------------*
 * 						Average notCalibrated
 * -------------------------------------------------------------------*/

int MPX_ReadAverage_UnCal(void)
{
	static unsigned char 	mpxCount=0;
	static int			 	mpx[10]={0x00};
	int						unCal=0;
	unsigned char			a= 0;

	mpx[mpxCount]= MPX_Read();				//Pressure
	mpxCount++;
	if(mpxCount>4)								//5Values
	{
		mpxCount=0;
		for(a=0; a<5; a++)	unCal= unCal+mpx[a];
		unCal= unCal/5;
		return unCal;
	}
	TC_DisplayData_Wait();						//Wait 100ms
	
	return 0xFF00;
}

int MPX_ReadAverage_UnCal_Value(void)
{
	unsigned char i=0;
	int avAge=0;

	for(i=0; i<6; i++)
	{
		avAge= MPX_ReadAverage_UnCal();				//Read 1Value
		TC_DisplayData_Wait();						//Wait 100ms
		if(!(avAge == 0xFF00))	return avAge;
	}
	return avAge;
}



/* -------------------------------------------------------------------*
 * 						Average Waterlevel
 * -------------------------------------------------------------------*/

int MPX_ReadWaterLevel(t_FuncCmd cmd)
{
	static int 		level=0;
	int				sLevel=0;	
	unsigned char 	save=0;	

	switch(cmd)
	{
		case _init:	level= ((MEM_EEPROM_ReadVar(TANK_H_MinP)<<8) |
							(MEM_EEPROM_ReadVar(TANK_L_MinP)));		break;
		
		case _save:	sLevel= level;
					save= (sLevel & 0x00FF);
					MEM_EEPROM_WriteVar(TANK_L_MinP, save);
					save= ((sLevel>>8) & 0x00FF);
					MEM_EEPROM_WriteVar(TANK_H_MinP, save);		break;	//Save in EEPROM

		case _new:	level= MPX_ReadAverage_Value();						//ReadValue
					return level;								break;	//new level

		case _old:	return level;								break;
		default:												break;
	}

	return level;				
}





/*--------------------------------------------------------------------*
 * 	MPX_ReadTank
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			t_page	-	Page to go next
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Reads the position of Clear Water, call only in air times
 * -------------------------------------------------------------------*/

t_page MPX_ReadTank(t_page page, t_FuncCmd cmd)
{
	static int				mpxAv=0;
	static unsigned char 	error=0;

	int	hO2=0;
	int hCirc=0;
	int	minP=0;
	int	perP=0;
		
	if(!(MEM_EEPROM_ReadVar(SENSOR_inTank)) || 		//Disabled ReadTank	
		page==AutoPumpOff 					|| 
		page==AutoMud){								
			if(page==ManualCirculate)	LCD_WriteValue(16, 2, perP);
			else{	LCD_WriteMyFont(15, 5, 11);		//-
					LCD_WriteMyFont(15, 9, 11);		//-
					LCD_WriteMyFont(15, 13, 11);}	//-
			return page;}
	
	hO2=(	(MEM_EEPROM_ReadVar(TANK_H_O2)<<8) |
			(MEM_EEPROM_ReadVar(TANK_L_O2)));

	hCirc=(	(MEM_EEPROM_ReadVar(TANK_H_Circ)<<8) |
			(MEM_EEPROM_ReadVar(TANK_L_Circ)));
	
	minP=(	(MEM_EEPROM_ReadVar(TANK_H_MinP)<<8) |
			(MEM_EEPROM_ReadVar(TANK_L_MinP)));

	switch(cmd)
	{
		case _exe: 		mpxAv= MPX_ReadWaterLevel(_new);
						if(page == ManualCirculate)	return page;	//Return

						LCD_WriteValue_MyFont(17, 5, mpxAv);		//Write Value
						LCD_WriteMyFont(17, 18, 13);				//m	
						LCD_WriteMyFont(17, 22, 14);				//b	
						LCD_WriteMyFont(17, 26, 15);				//a
						LCD_WriteMyFont(17, 30, 16);				//r	

						if(page == AutoCirculate)							//--Circulate
						{
							if(mpxAv>=(hO2+minP)){	error++;				//Error-Counter
													return AutoSetDown;}	//SetDown
							error=0;
							if(mpxAv>=(hCirc+minP))	return AutoAir;		
						}
						if(page == AutoAir)									//--Air
						{
							if(mpxAv>=(hO2+minP))	return AutoSetDown;	
						}							
						if(page == AutoZone)								//--Zone
						{
							if(mpxAv>=(hO2+minP))	return AutoSetDown;
							if(mpxAv>=(hCirc+minP))	return AutoAir;
							else					return AutoCirculate;	
						}											break;

	
		case _write:	perP= mpxAv-minP;					//calc
						if(perP<=0)	perP=0;					//limit
						perP= ((perP*100)/hO2);				//calc
						if(page==ManualCirculate){			//ManualWrite
							LCD_WriteValue(16, 2, perP);
							return page;}
						LCD_WriteValue_MyFont(15, 5, perP);			break;	//Write

		case _reset:	mpxAv=0;									break;	//Reset
		case _error:	if(error>=2)	return ErrorMPX;			break;	//Error

		default:													break;
	}					

	return page;
}



/* ===================================================================*
 * 						FUNCTIONS - Test
 * ===================================================================*/

void MPX_Test(void)
{
	while(1)
	{
		Watchdog_Restart();
		LCD_WriteValue(1,2, MPX_Read());
		LCD_WriteValue(4,2, MPX_ReadCal());
		LCD_WriteValue(1,40, MPX_ReadAverage_UnCal_Value());
		LCD_WriteValue(4,40, MPX_ReadAverage_Value());
	}
}







/**********************************************************************\
 * End of SL_MPX_func.c
\**********************************************************************/

