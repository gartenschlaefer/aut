/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Touchpanel-Driver-SourceFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	This SourceFile contains the driver functions of the Touchpanel
	for the EADOGXL160-7 Display
* ------------------------------------------------------------------														
*	Date:			15.07.2011  	
* 	lastChanges:											
\**********************************************************************/


#include<avr/io.h>

#include<stddef.h>
#include<avr\pgmspace.h>

#include "SL_Define_sym.h"
#include "SL_Touch_driver.h"
#include "SL_Display_driver.h"
#include "SL_MPX_driver.h"

#include "SL_Watchdog_func.h"
#include "SL_TC_func.h"
#include "SL_Memory_app.h"



/* ===================================================================*
 * 						ADC Init		
 * ===================================================================*/
 
 
/*--------------------------------------------------------------------*
 * 	ADC_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	ADC Init of Touchpanel and Drucksensor
 * -------------------------------------------------------------------*/

void ADC_Init(void)
{
	ADCA.CALL = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));	//Calibration Low
	ADCA.CALH = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));	//Calibration High
	ADCB.CALL = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0));	//Calibration Low
	ADCB.CALH = ReadCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1));	//Calibration High						

	ADCA.CTRLB=		ADC_RESOLUTION_12BIT_gc;	//Resolution 12BIT right
	ADCA.REFCTRL= 	ADC_REFSEL_AREFA_gc;		//ReferenceVoltage
	ADCA.PRESCALER= ADC_PRESCALER_DIV16_gc;		//Prescaler clk/16
	ADCA.CTRLA= 	ADC_ENABLE_bm;				//ADC ENABLE

	Touch_ADC_Init();							//ADC Channel Touchpanel
	MPX_ADC_Init();								//ADC Channel Drucksensor MPX
}

unsigned char ReadCalibrationByte(unsigned char index)
{
	unsigned char result;

	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(index);
	NVM_CMD = NVM_CMD_NO_OPERATION_gc; 			//Clean up NVM Command register

	return(result);
}



/* ===================================================================*
 * 						FUNCTIONS 	Init		
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	Touch_ADC_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set TOP 	and BOTTOM 	as Output, TOP=1, BOTTOM=0
 *	Set LEFT 	and RIGHT 	as Input, Read at LEFT (ADC0)
 *	ADC_Init
 * -------------------------------------------------------------------*/

void Touch_ADC_Init(void)
{
	//ADC Channel 
	ADCA.CH0.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|	//LEFT
						ADC_CH_GAIN_1X_gc;
	ADCA.CH0.MUXCTRL= 	ADC_CH_MUXPOS_PIN3_gc;
	

	ADCA.CH1.CTRL= 		ADC_CH_INPUTMODE_SINGLEENDED_gc	|	//TOP
						ADC_CH_GAIN_1X_gc;
	ADCA.CH1.MUXCTRL= 	ADC_CH_MUXPOS_PIN2_gc;
}





/* ===================================================================*
 * 						FUNCTIONS 	Cal
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_Cal
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Safes Calibration Data in EEPROM
 * -------------------------------------------------------------------*/

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
	
	TCD0_16MHzWaitSec_Init(2);				
	while(!(TCD0_16MHzWait_Query()));		//wait
	Watchdog_Restart();

	//-----------------------------------------------TouchValue-----
	LCD_Clean(); 
	LCD_WriteStringFont(2, 0, "Touch me!");
	while(!(TCD0_16MHzWait_Query()));		//wait
	Watchdog_Restart();

	calX= Touch_X_Cal_Init();				//Read-X-Cal
	calY= Touch_Y_Cal_Init();				//Read-Y-Cal
	Watchdog_Restart();

	MEM_EEPROM_WriteVar(TOUCH_X_max, (calX>>4));	//Save Touch-Value X
	MEM_EEPROM_WriteVar(TOUCH_Y_max, (calY>>4));	//Save Touch-Value Y

	LCD_Clean();
	LCD_WriteStringFont(2,0,"Touchpanel Calibrated"); 
	LCD_WriteStringFont(5,0,"EEPROM written"); 

	TCD0_16MHz_Stop();				//stop Timer
}



/*--------------------------------------------------------------------*
 * 	Touch_X/Y_Cal_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			int		x/yCal	-	X/Y-Calibrated-ADC
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Returns highest Value of Touchpanel in X/Y-Direction
 *	This Functions has to be used only once for each Touchpanel
 * --------------------------------------------------------------
 *	ToDo:	- Safe in EEPROM
 * -------------------------------------------------------------------*/

int Touch_X_Cal_Init(void)
{
	int calData=0;
	int	xCal=0;	
	
	Touch_Clean();							//Clean
	LCD_Clean();
	LCD_WriteStringFont(2,0,"Touchpanel X-Calibrating"); 

	TC_Touch_Wait();						//Wait 10ms

	for(int i=0; i<300; i++)				//6s / 20ms
	{
		Watchdog_Restart();
		calData= Touch_X_ReadData();
		if(calData>xCal) xCal= calData;	
	}

	Touch_Clean();							//Clean
	TCD0_16MHz_Stop();						//stop Timer

	return xCal;
}

int Touch_Y_Cal_Init(void)
{
	int calData=0;
	int	yCal=0;	

	Touch_Clean();							//Clean
	LCD_Clean();
	LCD_WriteStringFont(2,0,"Touchpanel Y-Calibrating"); 

	TC_Touch_Wait();						//Wait 10ms

	for(int i=0; i<300; i++)				//6s / 20ms
	{
		Watchdog_Restart();
		calData= Touch_Y_ReadData();
		if(calData>yCal) yCal= calData;	
	}
	
	Touch_Clean();							//Clean
	TCD0_16MHz_Stop();						//stop Timer

	return yCal;
}



/*--------------------------------------------------------------------*
 * 	Touch_X/Y_Cal
 * --------------------------------------------------------------
 * 	parameter:		int		xBereich	-	X/Y-Bereich H or L
 *					int		hADC_Cal	-	highest Value of X
 * --------------------------------------------------------------
 * 	return:			int		xCal		-	X/Y-Calibrated
 * 	Descirption:	
 * --------------------------------------------------------------
 *	X/Y-Bereich will be calibrated
 * -------------------------------------------------------------------*/

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




/* ===================================================================*
 * 						FUNCTIONS 	Measure
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_Clean
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set all Touch Pins Low, 
 *	The Voltage of the capacitor will be decharged
 * -------------------------------------------------------------------*/

void Touch_Clean(void)
{
	PORTA.DIRSET= 	LEFT | RIGHT | TOP | BOTTOM;	//Output			
	PORTA.OUTCLR=	LEFT | RIGHT | TOP | BOTTOM;	//Set LOW
}



/* ===================================================================*
 * 						FUNCTIONS 	Measure-Y
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	Touch_Y_Measure
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set TOP 	and BOTTOM 	as Output, TOP=0, BOTTOM=1
 *	Set LEFT 	and RIGHT 	as Input, Read at LEFT (ADC0)
 * -------------------------------------------------------------------*/

void Touch_Y_Measure(void)
{
	PORTA.DIRSET= TOP 	| BOTTOM;		//Set Output
	PORTA.DIRCLR= LEFT 	| RIGHT;		//Set Input

	PORTA.OUTCLR=	TOP;				//TOP= L
	PORTA.OUTSET=	BOTTOM;				//BOTTOM= H
}


/*--------------------------------------------------------------------*
 * 	Touch_Y_Read
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			int	data	-	Conversion Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 *	Read at LEFT (ADC3),	Convert CH0
 * -------------------------------------------------------------------*/

 int Touch_Y_Read(void)
 {
	int data= 0;
	
	ADCA.CTRLA |= ADC_CH0START_bm;					//Start Conversion

	while(!(ADCA.INTFLAGS & (1<<ADC_CH0IF_bp)));	//Wait
	ADCA.INTFLAGS |= (1<<ADC_CH0IF_bp);				//Reset INT-Flag
	data= ADCA.CH0RES;								//Data at LEFT

	return data;
}


/*--------------------------------------------------------------------*
 * 	Touch_Y_ReadData
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			int	yData	-	Conversion Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set UP Measure through 			Touch_Y_Measure
 *  Read Data on LEFT ADC0 throuch 	Touch_Y_Read
 *  Clean Up Measure through		Touch_Clean
 * -------------------------------------------------------------------*/

 int Touch_Y_ReadData(void)
 {
	int yData= 0;
	
	Touch_Clean();						//Clean
	TC_Touch_Wait();					//Wait 10ms

	Touch_Y_Measure();					//Measure Set up
	TC_Touch_Wait();					//Wait 10ms
	
	yData= Touch_Y_Read();				//Read at ADC0
	Touch_Clean();						//Clean
	TCD0_16MHz_Stop();					//stop Timer

	return yData;
}



/* ===================================================================*
 * 						FUNCTIONS 	Measure-X
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	Touch_X_Measure
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set LEFT 	and RIGHT 	as Output, 	LEFT=1, RIGTH=0
 *	Set TOP 	and BOTTOM 	as Input, 	Read at TOP (ADC1)
 * -------------------------------------------------------------------*/

void Touch_X_Measure(void)
{
	PORTA.DIRSET= LEFT 	| RIGHT;		//Set Output
	PORTA.DIRCLR= TOP 	| BOTTOM;		//Set Input

	PORTA.OUTCLR=	LEFT;				//LEFT=		L
	PORTA.OUTSET=	RIGHT;				//RIGHT= 	H
}


/*--------------------------------------------------------------------*
 * 	Touch_X_Read
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			int	data	-	Conversion Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Wait until Conversion Complete and return Data
 *	Read at TOP (ADC2),	Convert CH1
 * -------------------------------------------------------------------*/

 int Touch_X_Read(void)
 {
	int data= 0;
	
	ADCA.CTRLA |= ADC_CH1START_bm;					//Start Conversion
	
	while(!(ADCA.INTFLAGS & (1<<ADC_CH1IF_bp)));		//Wait	
	ADCA.INTFLAGS |= (1<<ADC_CH1IF_bp);				//Reset INT-Flag
	data= ADCA.CH1RES;								//Data at TOP

	return data;
}


/*--------------------------------------------------------------------*
 * 	Touch_X_ReadData
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			int	xData	-	Conversion Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set UP Measure through 			Touch_X_Measure
 *  Read Data at TOP ADC1 throuch 	Touch_X_Read
 *  Clean Up Measure through		Touch_Clean
 * -------------------------------------------------------------------*/

 int Touch_X_ReadData(void)
 {
	int xData= 0;
	
	Touch_Clean();						//Clean
	TC_Touch_Wait();					//Wait 10ms
	
	Touch_X_Measure();					//Measure Set up
	TC_Touch_Wait();					//Wait 10ms

	xData= Touch_X_Read();				//Read at ADC0
	Touch_Clean();						//Clean
	TCD0_16MHz_Stop();					//stop Timer

	return xData;
}









/**********************************************************************\
 * END of xmA_Touchpanel_driver.c
\**********************************************************************/

