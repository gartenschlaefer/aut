/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Touchpanel-Driver-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	This SourceFile contains the driver functions of the Touchpanel
*	for the EADOGXL160-7 Display
* ------------------------------------------------------------------
*	Date:			15.07.2011
\**********************************************************************/


#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "touch_driver.h"
#include "mpx_driver.h"

#include "adc_func.h"
#include "basic_func.h"
#include "tc_func.h"
#include "memory_app.h"


/* ==================================================================*
 * 						FUNCTIONS 	Cal
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_Cal
 * --------------------------------------------------------------
 *	Safes Calibration Data in EEPROM
 * ------------------------------------------------------------------*/

void Touch_Cal(void)
{
	int calX= 0;
	int calY= 0;

	//-----------------------------------------------NoTouchValue-----
	LCD_Clean();
	LCD_WriteStringFont(2, 0, "Do not Touch!");
	LCD_WriteStringFont(5, 0, "If you Touched, restart!");

	MEM_EEPROM_WriteVar(TOUCH_X_min, (Touch_X_ReadData()>>4));	//Save noTouch-Value X
	MEM_EEPROM_WriteVar(TOUCH_Y_min, (Touch_Y_ReadData()>>4));	//Save noTouch-Value Y

	TCD0_WaitSec_Init(2);
	while(!(TCD0_Wait_Query()));		//wait
	Watchdog_Restart();

	//-----------------------------------------------TouchValue-----
	LCD_Clean();
	LCD_WriteStringFont(2, 0, "Touch me!");
	while(!(TCD0_Wait_Query()));		//wait
	Watchdog_Restart();

	calX= Touch_X_Cal_Init();				//Read-X-Cal
	calY= Touch_Y_Cal_Init();				//Read-Y-Cal
	Watchdog_Restart();

	MEM_EEPROM_WriteVar(TOUCH_X_max, (calX>>4));	//Save Touch-Value X
	MEM_EEPROM_WriteVar(TOUCH_Y_max, (calY>>4));	//Save Touch-Value Y

	LCD_Clean();
	LCD_WriteStringFont(2,0,"Touchpanel Calibrated");
	LCD_WriteStringFont(5,0,"EEPROM written");
	TCD0_Stop();
}


/*-------------------------------------------------------------------*
 * 	Touch_X_Cal_Init
 * ------------------------------------------------------------------*/

int Touch_X_Cal_Init(void)
{
	int calData=0;
	int	xCal=0;

	Touch_Clean();
	LCD_Clean();
	LCD_WriteStringFont(2,0,"Touchpanel X-Calibrating");
	TCC0_Touch_Wait();

	for(int i=0; i<CAL_READS; i++)
	{
		Watchdog_Restart();
		calData= Touch_X_ReadData();
		if(calData>xCal) xCal= calData;
	}

	Touch_Clean();
	return xCal;
}


/*-------------------------------------------------------------------*
 * 	Touch_Y_Cal_Init
 * ------------------------------------------------------------------*/

int Touch_Y_Cal_Init(void)
{
	int calData=0;
	int	yCal=0;

	Touch_Clean();
	LCD_Clean();
	LCD_WriteStringFont(2,0,"Touchpanel Y-Calibrating");
	TCC0_Touch_Wait();

	for(int i=0; i<CAL_READS; i++)
	{
		Watchdog_Restart();
		calData= Touch_Y_ReadData();
		if(calData>yCal) yCal= calData;
	}

	Touch_Clean();
	return yCal;
}


/*-------------------------------------------------------------------*
 * 	Touch_X_Cal
 * --------------------------------------------------------------
 *	X-space will be calibrated
 * ------------------------------------------------------------------*/

int Touch_X_Cal(int xBereich)
{
	int xCal=0;
	int maxCal=0;
	int minCal=0;

	maxCal= MEM_EEPROM_ReadVar(TOUCH_X_max);		//maximum Value
	minCal= MEM_EEPROM_ReadVar(TOUCH_X_min);		//minimum Value

	xCal= 	xBereich - minCal;
	if(xCal<0)	xCal=0;
	xCal= ((xCal * 155) / maxCal);					//Berechnung

	return xCal;
}


/*-------------------------------------------------------------------*
 * 	Touch_Y_Cal
 * --------------------------------------------------------------
 *	Y-space will be calibrated
 * ------------------------------------------------------------------*/

int Touch_Y_Cal(int yBereich)
{
	int yCal=0;
	int maxCal=0;
	int minCal=0;

	maxCal= MEM_EEPROM_ReadVar(TOUCH_Y_max);		//maximum Value
	minCal= MEM_EEPROM_ReadVar(TOUCH_Y_min);		//minimum Value

	yCal= yBereich - minCal;
	if(yCal<0)	yCal=0;
	yCal= (((yCal) * 105) / maxCal);				//Berechnung

	return yCal;
}



/* ==================================================================*
 * 						FUNCTIONS 	Measure
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_Clean
 * --------------------------------------------------------------
 *	Set all Touch Pins Low,
 *	The Voltage of the capacitor will be decharged
 * ------------------------------------------------------------------*/

void Touch_Clean(void)
{
	PORTA.DIRSET= 	LEFT | RIGHT | TOP | BOTTOM;	//Output
	PORTA.OUTCLR=	LEFT | RIGHT | TOP | BOTTOM;	//Set LOW
}



/* ==================================================================*
 * 						FUNCTIONS 	Measure-Y
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_Y_Measure
 * --------------------------------------------------------------
 *	Set TOP 	and BOTTOM 	as Output, TOP=0, BOTTOM=1
 *	Set LEFT 	and RIGHT 	as Input, Read at LEFT (ADC0)
 * ------------------------------------------------------------------*/

void Touch_Y_Measure(void)
{
	PORTA.DIRSET= TOP 	| BOTTOM;		//Set Output
	PORTA.DIRCLR= LEFT 	| RIGHT;		//Set Input

	PORTA.OUTCLR=	TOP;				//TOP= L
	PORTA.OUTSET=	BOTTOM;				//BOTTOM= H
}


/*-------------------------------------------------------------------*
 * 	Touch_Y_Read
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 *	Read at LEFT (ADC3),	Convert CH0
 * ------------------------------------------------------------------*/

 int Touch_Y_Read(void)
 {
	int data= 0;

	ADCA.CTRLA |= ADC_CH0START_bm;					      //Start Conversion

	while(!(ADCA.INTFLAGS & (1<<ADC_CH0IF_bp)));	//Wait
	ADCA.INTFLAGS |= (1<<ADC_CH0IF_bp);				    //Reset INT-Flag
	data= ADCA.CH0RES;								            //Data at LEFT

	return data;
}


/*-------------------------------------------------------------------*
 * 	Touch_Y_ReadData
 * --------------------------------------------------------------
 *	Set UP Measure through Touch_Y_Measure
 *  Read Data on LEFT ADC0 through 	Touch_Y_Read
 *  Clean Up Measure through Touch_Clean
 * ------------------------------------------------------------------*/

int Touch_Y_ReadData(void)
{
	int yData= 0;

  // Setup Pins
	Touch_Clean();
	TCC0_Touch_Wait();
	Touch_Y_Measure();
	TCC0_Touch_Wait();

  // Read at ADC0
	yData= Touch_Y_Read();
	Touch_Clean();

	return yData;
}



/* ==================================================================*
 * 						FUNCTIONS 	Measure-X
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_X_Measure
 * --------------------------------------------------------------
 *	Set LEFT 	and RIGHT 	as Output, 	LEFT=1, RIGTH=0
 *	Set TOP 	and BOTTOM 	as Input, 	Read at TOP (ADC1)
 * ------------------------------------------------------------------*/

void Touch_X_Measure(void)
{
	PORTA.DIRSET= LEFT 	| RIGHT;		//Set Output
	PORTA.DIRCLR= TOP 	| BOTTOM;		//Set Input

	PORTA.OUTCLR=	LEFT;				//LEFT=		L
	PORTA.OUTSET=	RIGHT;				//RIGHT= 	H
}


/*-------------------------------------------------------------------*
 * 	Touch_X_Read
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 *	Read at TOP (ADC2),	Convert CH1
 * ------------------------------------------------------------------*/

 int Touch_X_Read(void)
 {
	int data= 0;

	ADCA.CTRLA |= ADC_CH1START_bm;					//Start Conversion

	while(!(ADCA.INTFLAGS & (1<<ADC_CH1IF_bp)));		//Wait
	ADCA.INTFLAGS |= (1<<ADC_CH1IF_bp);				      //Reset INT-Flag
	data= ADCA.CH1RES;								              //Data at TOP

	return data;
}


/*-------------------------------------------------------------------*
 * 	Touch_X_ReadData
 * --------------------------------------------------------------
 *	Set UP Measure through 			Touch_X_Measure
 *  Read Data at TOP ADC1 throuch 	Touch_X_Read
 *  Clean Up Measure through		Touch_Clean
 * ------------------------------------------------------------------*/

 int Touch_X_ReadData(void)
 {
	int xData= 0;

	// Setup Port
	Touch_Clean();
	TCC0_Touch_Wait();
	Touch_X_Measure();
	TCC0_Touch_Wait();

  // Read at ADC0
	xData= Touch_X_Read();
	Touch_Clean();

	return xData;
}




unsigned char *Touch_Read(void)
{
	static unsigned char touch[3] = {
	  _clean,	  //State
    0x00,	    //Y-Value
    0x00};	  //X-Value

	//--------------------------------------------------X-Measure
	if(touch[0] == _clean)
	{
		Touch_Clean();						        //clean
		TCD0_WaitMilliSec_Init(5);	  //weit
		touch[0]= _read1;					        //nextStep
	}

	else if((touch[0] == _read1) && TCD0_Wait_Query())
	{
		Touch_X_Measure();					      //X-Measure
		TCD0_WaitMilliSec_Init(5);	  //wait
		touch[0] = _write1;					      //nextStep
	}

	else if((touch[0] == _write1) && TCD0_Wait_Query())
	{
		touch[2] = (Touch_X_Read()>>4);		//X-Value
		Touch_Clean();						        //clean
		TCD0_WaitMilliSec_Init(5);	  //wait
		touch[0] = _read2;					      //nextStep
	}

	//--------------------------------------------------Y-Measure
	else if((touch[0] == _read2) && TCD0_Wait_Query())
	{
		Touch_Y_Measure();					      //X-Measure
		TCD0_WaitMilliSec_Init(5);	  //wait
		touch[0] = _write2;					      //nextStep
	}

	else if((touch[0] == _write2) && TCD0_Wait_Query())
	{
		touch[1] = (Touch_Y_Read()>>4);	//X-Value
		Touch_Clean();						      //Clean
		TCD0_Stop();					    //StopTimr
		touch[0] = _ready;					    //End
	}

	else if(touch[0] == _ready) touch[0] = _clean;
	return &touch[0];
}








/**********************************************************************\
 * End of file
\**********************************************************************/

