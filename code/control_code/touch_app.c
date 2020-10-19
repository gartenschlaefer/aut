/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Steuerung Light ICT
*	Name:			    Touchpanel-App-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Application-File for TouchEADOGXL160-7 Display
* ------------------------------------------------------------------
*	Date:			    13.07.2011
* Changes:      25.12.2015
*               19.10.2020 - touch matrix state safety
\**********************************************************************/


#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "lcd_sym.h"

#include "touch_app.h"
#include "eval_app.h"
#include "output_app.h"
#include "memory_app.h"

#include "touch_driver.h"
#include "mcp7941_driver.h"
#include "mpx_driver.h"
#include "modem_driver.h"

#include "tc_func.h"
#include "port_func.h"
#include "error_func.h"
#include "sonic_app.h"
#include "can_app.h"



/* ==================================================================*
 * 						FUNCTIONS 	Matrix
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_Matrix
 * --------------------------------------------------------------
 *	return 0xYX
 *	return Touch Matrix:
 *  1.Row:	11, 12, 13 ,14
 *	2.Row:	21, 22, 23 ,24, usw.
 * ------------------------------------------------------------------*/

unsigned char Touch_Matrix(void)
{
	static int x[2] = {0,0};
	static int y[2] = {0,0};
	int xAv = 0;
	int yAv = 0;
	unsigned char lx = 0;
	unsigned char hy = 0;
  static unsigned char state = 0;
  unsigned char *p_touch = Touch_Read();

  //*** remove this next time!!!
  /*
  static unsigned char death = 0;
  if (DEBUG) 
  {
    death++;
    if (death >= 100)
    {
      death = 0;
      LCD_DeathMan(0, 0);
    }
    LCD_WriteValue3(0, 35, p_touch[0]);
    LCD_WriteValue2(2, 35, state);
  }
  */

  if(p_touch[0] == _ready && (state == 0 || state == 1))
  {
    y[state] = p_touch[1];
    x[state] = p_touch[2];
    state++;
  }

	if(p_touch[0] == _ready && state == 2)
	{
    state = 0;

    // too much diffs, reject sample
		if(x[0] < x[1] - 10 || x[0] > x[1] + 10) return 0;
		if(y[0] < y[1] - 10 || y[0] > y[1] + 10) return 0;

    // calibrate
    yAv = Touch_Y_Cal(((y[0] + y[1]) / 2));
		xAv = Touch_X_Cal(((x[0] + x[1]) / 2));

		//------------------------------------------------xMatrix
		if		  (xAv < 16)				        lx = 0;
		else if	(xAv > 16   && xAv < 35)	lx = 1;
		else if	(xAv > 47   && xAv < 70)	lx = 2;
		else if	(xAv > 80   && xAv < 105)	lx = 3;
		else if	(xAv > 115  && xAv < 145) lx = 4;
		else						                  lx = 5;

		//------------------------------------------------yMatrix
		if		  (yAv < 16)				        hy = 0;
		else if	(yAv > 18 && yAv < 37)		hy = 1;
		else if	(yAv > 45 && yAv < 58)		hy = 2;
		else if	(yAv > 65 && yAv < 77)		hy = 3;
		else if	(yAv > 85 && yAv < 105)	  hy = 4;
		else						                  hy = 5;

    //*** debug touch matrix
    if (DEBUG)
    {
      LCD_WriteValue2_MyFont(22, 152, hy);
      LCD_WriteValue2_MyFont(24, 152, lx);
    }
	}

  // safety state
  if (state > 2)
  {
    state = 0;
  }

	return ((hy<<4) | lx);
}



/* ==================================================================*
 * 						Auto Linker
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_AutoLinker
 * ------------------------------------------------------------------*/

t_page Touch_AutoLinker(unsigned char matrix, t_page page,
                        int *p_min,           int *p_sec)
{
	static unsigned char bug=0;
	static unsigned char touch=0;

	switch(matrix)
	{
		//------------------------------------------------BacklightON
		case 0x11: 	LCD_Backlight(_on);	bug=0;	break;
		case 0x12: 	LCD_Backlight(_on);	bug=0;	break;
		case 0x13:	LCD_Backlight(_on);	bug=0;	break;
		case 0x14:	LCD_Backlight(_on);	bug=0;	break;

		case 0x21:
		  if(!touch){	touch = 1;
        if(bug == 0)			bug = 1;
        else if(bug == 1) bug = 2;
        else				      bug = 0;
        LCD_Backlight(_on);}
      break;

		case 0x22:	LCD_Backlight(_on);	bug = 0;		break;
		case 0x23:	LCD_Backlight(_on);	bug = 0;		break;

		case 0x24:
		  if(!touch){	touch=1;
        if(bug== 2)			  bug = 3;
        else if(bug== 3)  bug = 4;
        else				      bug = 0;
        LCD_Backlight(_on);}
      break;

		case 0x31:	LCD_Backlight(_on);	bug = 0;	break;
		case 0x32:	LCD_Backlight(_on);	bug = 0;	break;
		case 0x33:	LCD_Backlight(_on);	bug = 0;	break;
		case 0x34:	LCD_Backlight(_on);	bug = 0;	break;

		//--------------------------------------------------------------MainLinker
		case 0x41:	LCD_Backlight(_on);	Error_OFF();
					if(bug== 4)	bug = 5;
					else		    bug = 0;			break;	//Auto

		case 0x42:	LCD_Backlight(_on);	Error_OFF();	bug=0;
					LCD_MarkTextButton(Manual);
					PORT_Ventil_AutoClose(page);
					PORT_Relais_AllOff();
					return PinManual; 				break;	//Manual

		case 0x43:	LCD_Backlight(_on);	Error_OFF();	bug=0;
					LCD_MarkTextButton(Setup);
					PORT_Ventil_AutoClose(page);
					PORT_Relais_AllOff();
					return PinSetup; 					break;	//Setup

		case 0x44:	LCD_Backlight(_on);	Error_OFF();	bug=0;
					LCD_MarkTextButton(Data);
					PORT_Ventil_AutoClose(page);
					PORT_Relais_AllOff();
					return DataPage; 					break;	//Data

		case 0x00:	if(touch)	touch=0;	break;
		default:										    break;
	}

	if(bug == 5)        //Bug
	{
	  *p_min = 0;
	  *p_sec = 5;
	  LCD_AutoCountDown(*p_min, *p_min);
	  bug = 0;
  }

	return page;
}



/* ==================================================================*
 * 						Manual Linker
 * ==================================================================*/

t_page Touch_ManualLinker(unsigned char matrix, t_page page)
{
	switch(matrix)
	{
		case 0x11: 	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_circulate);
      return ManualCirc;					break;		//Circulate

		case 0x12: 	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_air);
      return ManualAir;						break;		//Air

		case 0x13:	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_setDown);
      return ManualSetDown;			  break;		//SetDown

		case 0x14:	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_pumpOff);
      return ManualPumpOff;			  break;		//PumpOff

		case 0x21:	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_mud);
      return ManualMud;					  break;		//Mud

		case 0x22:	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_compressor);
      return ManualCompressor;	  break;		//Comp

		case 0x23:	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_phosphor);
      return ManualPhosphor;			break;		//Phosphor

		case 0x24:	LCD_Backlight(_on);
      LCD_Mark_ManualSymbol(sn_inflowPump);
      return ManualInflowPump;		break;		//IP

		case 0x33:
		  if(page == ManualPumpOff){
        LCD_Write_Symbol_3(15, 85, n_ok);
        return ManualPumpOff_On;}
      break;

		//------------------------------------------------MainLinker
		case 0x41:	LCD_Backlight(_on);
      LCD_MarkTextButton(Auto);
      MEM_EEPROM_WriteManualEntry(0,0, _write);
      return AutoPage; 							break;

		case 0x42:	LCD_Backlight(_on);
      LCD_MarkTextButton(Manual);
      MEM_EEPROM_WriteManualEntry(0,0, _write);
      return ManualPage; 							break;

		case 0x43:	LCD_Backlight(_on);
      LCD_MarkTextButton(Setup);
      MEM_EEPROM_WriteManualEntry(0,0, _write);
      return SetupPage; 							break;

		case 0x44:	LCD_Backlight(_on);
      LCD_MarkTextButton(Data);
      MEM_EEPROM_WriteManualEntry(0,0, _write);
      return DataPage; 							break;

		default:												break;
	}
	return page;
}



/* ==================================================================*
 * 						Setup Linker
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_SetupLinker
 * ------------------------------------------------------------------*/

t_page Touch_SetupLinker(unsigned char matrix, t_page page)
{
	switch(matrix)
	{
		case 0x11: 	return SetupCirculate;		break;
		case 0x12: 	return SetupAir;				  break;
		case 0x13:	return SetupSetDown;			break;
		case 0x14:	return SetupPumpOff;			break;

		case 0x21:	return SetupMud;				  break;
		case 0x22:	return SetupCompressor;		break;
		case 0x23:	return SetupPhosphor;			break;
		case 0x24:	return SetupInflowPump;		break;

		case 0x31:	return SetupCal;				  break;
		case 0x32:	return SetupAlarm;				break;
		case 0x33:	return SetupWatch;				break;
		case 0x34:	return SetupZone;				  break;	//Zone

		//------------------------------------------------MainLinker
		case 0x41:	return AutoPage; 				  break;
		case 0x42:	return ManualPage; 				break;
		case 0x43:	return SetupPage; 				break;
		case 0x44:	return DataPage; 				  break;
		default:									            break;
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Circulate Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupCirculateLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	circ[4]= {0};
	static unsigned char 	sensor=0;

	static int				time1=0;
	static unsigned char 	on=0;
	static unsigned char 	init=0;
	static unsigned char 	touch=0;

	unsigned char 			*p_circ= circ;

	if(!init)
	{
		init = 1;
		on = 0;
		circ[0] =	MEM_EEPROM_ReadVar(ON_circ);
		circ[1] =	MEM_EEPROM_ReadVar(OFF_circ);
		circ[2] =	MEM_EEPROM_ReadVar(TIME_L_circ);
		circ[3] =	MEM_EEPROM_ReadVar(TIME_H_circ);
		sensor =	MEM_EEPROM_ReadVar(SENSOR_inTank);

		time1= 		((circ[3]<<8) | circ[2]);

		if(sensor)	Eval_SetupCircSensorMark(1);
		else		Eval_SetupCircSensorMark(0);

		Eval_SetupCircTextMark(on, p_circ);
	}

	switch(matrix)
	{
		case 0x13:
		  if(!touch){	touch=6;		LCD_ControlButtons(sn_esc);}
      init=0;	return SetupPage;				break;	//Esc

		case 0x14:
		  if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init=0;
      MEM_EEPROM_WriteVar(ON_circ, circ[0]);
      MEM_EEPROM_WriteVar(OFF_circ, circ[1]);
      MEM_EEPROM_WriteVar(TIME_L_circ, circ[2]);
      MEM_EEPROM_WriteVar(TIME_H_circ, circ[3]);
      MEM_EEPROM_WriteVar(SENSOR_inTank, sensor);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;							  break;	//Ok

		case 0x23:
		  if(!touch){	touch=5;  LCD_ControlButtons(sn_minus);}
      switch(on)
      {
        case 0:	circ[0] = Eval_SetupMinus(circ[0], 2);	break;	//CircOnMin
        case 1:	circ[1] = Eval_SetupMinus(circ[1], 5);	break;	//CircOnMax
        case 2:	time1 =		Eval_SetupMinus(time1, 0);		break;
        default:											                  break;
      }														                      break;	//Minus

		case 0x24:	if(!touch){	touch=4;		LCD_ControlButtons(sn_plus);}
      switch(on)
      {
        case 0:	circ[0] = Eval_SetupPlus(circ[0], 30);	break;	//CircOnMax
        case 1:	circ[1] = Eval_SetupPlus(circ[1], 60);	break;	//CircOffMax
        case 2:	time1 =		Eval_SetupPlus(time1, 999);	  break;
        default:											                  break;
      }														                      break;	//Plus

		case 0x21:
		  if(!touch){
		    on=0;	touch=5;	Eval_SetupCircTextMark(on, p_circ); }	break;	//On

		case 0x22:
		  if(!touch){
		    on=1;	touch=5;	Eval_SetupCircTextMark(on, p_circ); }	break;	//Off

		case 0x31:	if(!touch){	touch=5;		sensor=1;
								Eval_SetupCircSensorMark(1);}		      break;	//Sensor

		case 0x32:	if(!touch){	touch=5;		sensor=0;	on=2;
								Eval_SetupCircSensorMark(0);
								Eval_SetupCircTextMark(on, p_circ);}	break;	//Time

		case 0x00:	if(touch){	circ[2]=	(time1 & 0x00FF);
								circ[3]=	((time1>>8) & 0x00FF);
								LCD_ControlButtons(touch);	touch=0;}	break;	//NoTouch

		//------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	  break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	  break;
		default:									              break;
	}

	if(touch)
	{
		switch (on)
		{
			case 0:		LCD_OnValueNeg(circ[0]); 				  break;
			case 1:		LCD_OffValueNeg(circ[1]); 				break;
			case 2:		LCD_WriteValueNeg3(16,72, time1); break;
			default: 											              break;
		}
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Air Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupAirLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	air[4]= {0};
	static int				    time2=0;
	static unsigned char  on=0;
	static unsigned char 	init=0;
	static unsigned char 	touch=0;
	unsigned char 			  *p_air= air;

	if(!init)
	{
		init = 1;
		on = 0;
		air[0] = MEM_EEPROM_ReadVar(ON_air);
		air[1] = MEM_EEPROM_ReadVar(OFF_air);
		air[2] = MEM_EEPROM_ReadVar(TIME_L_air);
		air[3] = MEM_EEPROM_ReadVar(TIME_H_air);

		time2= 		((air[3]<<8) | air[2]);

		Eval_SetupAirTextMark(on, p_air);
		LCD_ClrSpace	(15, 39, 3, 31);
		LCD_WriteStringFont(16,40,"Time:");
	}

	switch(matrix)
	{
		case 0x13:
		  if(!touch){	touch=6;	LCD_ControlButtons(sn_esc);}
      init=0;	return SetupPage;						break;	//Esc

		case 0x14:
		  if(!touch){	touch=7;	LCD_ControlButtons(sn_ok);}
      init=0;
      MEM_EEPROM_WriteVar(ON_air, air[0]);
      MEM_EEPROM_WriteVar(OFF_air, air[1]);
      MEM_EEPROM_WriteVar(TIME_L_air, air[2]);
      MEM_EEPROM_WriteVar(TIME_H_air, air[3]);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;							      break;	//Ok

		case 0x23:
		  if(!touch){	touch=5; LCD_ControlButtons(sn_minus);}
      switch(on)
      {
        case 0:	air[0]= 	Eval_SetupMinus(air[0], 10);	break;	//AirOnMin
        case 1:	air[1]= 	Eval_SetupMinus(air[1], 5);		break;	//AirOffMin
        case 2:	time2=		Eval_SetupMinus(time2, 0);		break;	//TimeMin
        default:											break;
      }														    break;	//Minus

		case 0x24:
		  if(!touch){	touch=4;	LCD_ControlButtons(sn_plus);}
      switch(on)
      {
        case 0:	air[0]= 	Eval_SetupPlus(air[0], 60);	break;	//AirOnMax
        case 1:	air[1]= 	Eval_SetupPlus(air[1], 55);	break;	//AirOffMax
        case 2:	time2=		Eval_SetupPlus(time2, 999);	break;	//TimeMax
        default:											break;
      }														    break;	//Plus


		case 0x21:	if(!touch){on=0;	touch=5;
		              Eval_SetupAirTextMark(on, p_air); }	break;	//On

		case 0x22:	if(!touch){on=1;	touch=5;
		              Eval_SetupAirTextMark(on, p_air); }	break;	//Off

		case 0x32:	if(!touch){	touch=5;	on=2;
								Eval_SetupAirTextMark(on, p_air);}
								LCD_FillSpace	(15, 39, 4, 31);
								LCD_WriteStringFontNeg(16,40,"Time:");		break;	//Time

		case 0x00:	if(touch){	air[2]=	(time2 & 0x00FF);
								air[3]=	((time2>>8) & 0x00FF);
								LCD_ControlButtons(touch);	touch=0;}		break;	//No Touch

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	  break;
		case 0x42:	init=0;	return ManualPage;  break;
		case 0x43:	init=0;	return SetupPage;   break;
		case 0x44:	init=0;	return DataPage; 	  break;
		default:									              break;
	}

	if(touch)
	{
		switch (on)
		{
			case 0:		LCD_OnValueNeg(air[0]); 				  break;
			case 1:		LCD_OffValueNeg(air[1]); 				  break;
			case 2:		LCD_WriteValueNeg3(16,72, time2); break;
			default: 											              break;
		}
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Set-Down Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupSetDownLinker(unsigned char matrix, t_page page)
{
	static unsigned char setDown=0;

	static unsigned char init=0;
	static unsigned char touch=0;

	if(!init)
	{
		init=1;
		setDown=		MEM_EEPROM_ReadVar(TIME_setDown);
		LCD_WriteValueNeg3(10,30, setDown);
	}

	switch(matrix)
	{
		case 0x13:	if(!touch){	touch=6;		LCD_ControlButtons(sn_esc);}
      init=0;	return SetupPage;			break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init=0;
      MEM_EEPROM_WriteVar(TIME_setDown, setDown);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;							break;	//Ok

		case 0x23:	if(!touch){	touch=5;  LCD_ControlButtons(sn_minus);}	//MarkMinus
      setDown= 	Eval_SetupMinus(setDown, 50);				break;	//Minus

		case 0x24:	if(!touch){	touch=4;  LCD_ControlButtons(sn_plus);}		//MarkPlus
      setDown= 	Eval_SetupPlus(setDown, 90);				break;	//Plus

		case 0x00:	if(touch){	LCD_ControlButtons(touch);		touch=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	break;
		default:								break;
	}

	if(touch) LCD_WriteValueNeg3(10,30, setDown);	//TimeConfig
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Pump-Off Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupPumpOffLinker(unsigned char matrix, t_page page)
{
	static unsigned char pumpOn=0;
	static unsigned char pump=0;

	static unsigned char init=0;
	static unsigned char touch=0;

	if(!init)
	{
		init=1;
		pumpOn=	MEM_EEPROM_ReadVar(ON_pumpOff);
		pump=		MEM_EEPROM_ReadVar(PUMP_pumpOff);

		LCD_OnValueNeg(pumpOn);

		if(!pump){	LCD_Write_Symbol_2(15, 0, n_compressor);
					LCD_Write_Symbol_3(14, 50, p_pump);}

		else{		LCD_Write_Symbol_2(15, 0, p_compressor);
					LCD_Write_Symbol_3(14, 50, n_pump);}
	}

	switch(matrix)
	{

		case 0x13:	if(!touch){	touch=6;		LCD_ControlButtons(sn_esc);}
					init=0;	return SetupPage;							break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init=0;
      MEM_EEPROM_WriteVar(ON_pumpOff, pumpOn);
      MEM_EEPROM_WriteVar(PUMP_pumpOff, pump);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;							break;	//Ok

		case 0x23:	if(!touch){	touch=5;	LCD_ControlButtons(sn_minus);}
      if(touch){	pumpOn= 	Eval_SetupMinus(pumpOn, 5);}	break;	//Minus

		case 0x24:	if(!touch){	touch=4;	LCD_ControlButtons(sn_plus);}
      if(touch){	pumpOn= 	Eval_SetupPlus(pumpOn, 60);}	break;	//Plus

		case 0x31:	if(!touch){	touch=5;		pump=0;
      LCD_Write_Symbol_2(15, 0, n_compressor);
      LCD_Write_Symbol_3(14, 50, p_pump);} 		break;	//Mamutpumpe


		case 0x32:	if(!touch){	touch=5;		pump=1;
      LCD_Write_Symbol_2(15, 0, p_compressor);
      LCD_Write_Symbol_3(14, 50, n_pump);} 		break;	//elekt. Pumpe


		case 0x00:	if(touch){	LCD_ControlButtons(touch);		touch=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	break;
		default:									break;
	}

	if(touch){	LCD_OnValueNeg(pumpOn); 	}										//Output
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Mud Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupMudLinker(unsigned char matrix, t_page page)
{
	static unsigned char mudMin=0;
	static unsigned char mudSec=0;
	static unsigned char onM=0;
	static unsigned char init=0;
	static unsigned char touch=0;

	if(!init)
	{
		init=1;
		mudMin=		MEM_EEPROM_ReadVar(ON_MIN_mud);
		mudSec=		MEM_EEPROM_ReadVar(ON_SEC_mud);
		LCD_WriteStringFont(11,7,"ON:");
		if(!onM){	LCD_WriteValue2(11,40, mudMin);
      LCD_WriteValueNeg2(16,40, mudSec);}
		else{LCD_WriteValueNeg2(11,40, mudMin);
      LCD_WriteValue2(16,40, mudSec);}
		LCD_WriteStringFont(11,55,"min");
		LCD_WriteStringFont(16,55,"sec");
	}

	switch(matrix)
	{
		case 0x13:	if(!touch){	touch=6; LCD_ControlButtons(sn_esc);}
      init=0;	return SetupPage;										break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init=0;
      MEM_EEPROM_WriteVar(ON_MIN_mud, mudMin);
      MEM_EEPROM_WriteVar(ON_SEC_mud, mudSec);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;										break;	//Ok

		case 0x23:	if(!touch){	touch=5;	LCD_ControlButtons(sn_minus);}
      if(onM)	mudMin= Eval_SetupMinus(mudMin, 0);
      else		mudSec= Eval_SetupMinus(mudSec, 0);					break;	//Minus

		case 0x24:	if(!touch){	LCD_ControlButtons(sn_plus);	touch=4;}
      if(onM)	mudMin= Eval_SetupPlus(mudMin, 20);
      else		mudSec= Eval_SetupPlus(mudSec, 59);					break;	//Plus

		case 0x22:	if(!touch){onM=1;	LCD_WriteValue2(16,40, mudSec);	touch=5;}		break;	//Min
		case 0x32:	if(!touch){onM=0;	LCD_WriteValue2(11,40, mudMin);	touch=5;}		break;	//Sec

		case 0x00:	if(touch){	LCD_ControlButtons(touch);		touch=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	break;
		default:									break;
	}

	if(onM  && touch){	LCD_WriteValueNeg2(11,40, mudMin);	}	//Min
	if(!onM && touch){	LCD_WriteValueNeg2(16,40, mudSec);	}	//Max
	return page;
}





/* ------------------------------------------------------------------*
 * 						Setup Compressor Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupCompressorLinker(unsigned char matrix, t_page page)
{
	static int druckMin=0;
	static int druckMax=0;
	static unsigned char on=0;
	static unsigned char init0=0;
	static unsigned char touch=0;

	unsigned char h=0;
	unsigned char l=0;

	if(!init0)
	{
		init0 = 1;
		on = 0;
		h = MEM_EEPROM_ReadVar(MAX_H_druck);
		l =	MEM_EEPROM_ReadVar(MAX_L_druck);
		druckMax=	((h<<8) | l);

		h =	MEM_EEPROM_ReadVar(MIN_H_druck);
		l =	MEM_EEPROM_ReadVar(MIN_L_druck);
		druckMin=	((h<<8) | l);

		if(on)	{	LCD_WriteValue3(16,7, druckMax);		LCD_WriteValueNeg3(11,7, druckMin);	}	//Min
		if(!on)	{	LCD_WriteValue3(11,7, druckMin);	LCD_WriteValueNeg3(16,7, druckMax);	}	//Max
	}

	switch(matrix)
	{
		case 0x13:	if(!touch){	touch=6;		LCD_ControlButtons(sn_esc);}
					init0=0;		return SetupPage;								break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init0=0;
      l=(druckMax & 0x00FF);
      h= ((druckMax>>8) & 0x00FF);
      MEM_EEPROM_WriteVar(MAX_H_druck, h);
      MEM_EEPROM_WriteVar(MAX_L_druck, l);

      l=(druckMin & 0x00FF);
      h= ((druckMin>>8) & 0x00FF);
      MEM_EEPROM_WriteVar(MIN_H_druck, h);
      MEM_EEPROM_WriteVar(MIN_L_druck, l);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;										break;	//Ok

		case 0x23:	if(!touch){	touch=5;			LCD_ControlButtons(sn_minus);}
      if(on)  druckMin = Eval_SetupMinus(druckMin, 0);
      else	  druckMax = Eval_SetupMinus(druckMax, 0);	break;	//Minus

		case 0x24:	if(!touch){	touch=4;		LCD_ControlButtons(sn_plus);}
      if(on)  druckMin = Eval_SetupPlus(druckMin, 999);
      else		druckMax = Eval_SetupPlus(druckMax, 999);	break;	//Plus


		case 0x21:	if(!touch){on=1;
      LCD_WriteValue3(16,7, druckMax);	touch=5;}	break;	//Min

		case 0x31:	if(!touch){on=0;
      LCD_WriteValue3(11,7, druckMin);	touch=5;}	break;	//Max

		case 0x00:	if(touch){LCD_ControlButtons(touch); touch=0;}	break;

		//------------------------------------------------MainLinker
		case 0x41:	init0=0;	return AutoPage; 	  break;
		case 0x42:	init0=0;	return ManualPage; 	break;
		case 0x43:	init0=0;	return SetupPage; 	break;
		case 0x44:	init0=0;	return DataPage; 	  break;
		default:									                break;
	}

	if(on  && touch){	LCD_WriteValueNeg3(11,7, druckMin);	}	//Min
	if(!on && touch){	LCD_WriteValueNeg3(16,7, druckMax);	}	//Max
	return page;
}





/* ------------------------------------------------------------------*
 * 						Setup Phosphor Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupPhosphorLinker(unsigned char matrix, t_page page)
{
	static unsigned char pOn=10;
	static unsigned char pOff=10;
	static unsigned char on=0;
	static unsigned char init=0;
	static unsigned char touch=0;

	if(!init)
	{
		init=1;
		pOff=		MEM_EEPROM_ReadVar(OFF_phosphor);
		pOn=		MEM_EEPROM_ReadVar(ON_phosphor);
		if(on)	LCD_OnValueNeg(pOn);	LCD_OffValue(pOff);		//OnConfig
		if(!on)	LCD_OnValue(pOn);	LCD_OffValueNeg(pOff);		//OffConfig
	}

	switch(matrix)
	{
		case 0x13:
		  if(!touch){	touch=6;		LCD_ControlButtons(sn_esc);}
      init=0;		return SetupPage;							break;	//Esc

		case 0x14:
      if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init=0;		MEM_EEPROM_WriteVar(ON_phosphor, pOn);
      MEM_EEPROM_WriteVar(OFF_phosphor, pOff);
      MEM_EEPROM_WriteSetupEntry();
      LCD_Auto_Phosphor(0, _init);
      return SetupPage;							break;	//Ok

		case 0x23:
		  if(!touch){	touch=5;		LCD_ControlButtons(sn_minus);}
      if(on)  pOn= 	Eval_SetupMinus(pOn, 0);
      else		pOff= Eval_SetupMinus(pOff, 0);			break;	//Minus

		case 0x24:
		  if(!touch){	touch=4;		LCD_ControlButtons(sn_plus);}
      if(on)	pOn= 	Eval_SetupPlus(pOn, 60);
      else		pOff= Eval_SetupPlus(pOff, 60);			break;	//Plus


		case 0x21:
		  if(!touch){on=1; LCD_OffValue(pOff);  touch=5;}		break;	//On
		case 0x22:
		  if(!touch){on=0; LCD_OnValue(pOn);	  touch=5;}		break;	//Off

		case 0x00:
		  if(touch){LCD_ControlButtons(touch);	touch=0;}		break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	  break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage;  	break;
		default:									              break;
	}

	if(on  && touch){	LCD_OnValueNeg(pOn);	}	//OnConfig
	if(!on && touch){	LCD_OffValueNeg(pOff);	}	//OffConfig
	return page;
}





/* ------------------------------------------------------------------*
 * 						Setup Inflow-Pump Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupInflowPumpLinker(unsigned char matrix, t_page page)
{
	static unsigned char t_ipVal[3]={0,0,0};
	static unsigned char iPump=0;
	static unsigned char sensor=0;

	static unsigned char cho=4;
	static unsigned char init=0;
	static unsigned char touch=0;

	if(!init)
	{
		init=1;
		t_ipVal[0]=	MEM_EEPROM_ReadVar(T_IP_off_h);			//Off-Hour
		t_ipVal[1]=	MEM_EEPROM_ReadVar(OFF_inflowPump);		//Off-Min
		t_ipVal[2]=	MEM_EEPROM_ReadVar(ON_inflowPump);		//On-Min
		iPump=		MEM_EEPROM_ReadVar(PUMP_inflowPump);
		sensor=		MEM_EEPROM_ReadVar(SENSOR_outTank);

		Eval_SetupPumpMark(iPump);
		if(sensor)	LCD_Write_Symbol_2(15, 5, n_sensor);
		else		LCD_Write_Symbol_2(15, 5, p_sensor);

		LCD_Setup_IPText(0b10110100);					//Init Text
		LCD_Setup_IPValues(0b00110100, &t_ipVal[0]);	//Init Symbols
	}

	switch(matrix)
	{
		case 0x12:	if(!touch){	cho=1;	touch=5;
      LCD_Setup_IPText(0b01101001);
      LCD_Setup_IPValues(0b01100001, &t_ipVal[0]);} 	break; //Off h

		case 0x13:	if(!touch){	touch=6; LCD_ControlButtons(sn_esc);}
      init=0;		return SetupPage;								break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
      init=0;	MEM_EEPROM_WriteVar(ON_inflowPump, 	t_ipVal[2]);
      MEM_EEPROM_WriteVar(OFF_inflowPump, t_ipVal[1]);
      MEM_EEPROM_WriteVar(T_IP_off_h, 	t_ipVal[0]);
      MEM_EEPROM_WriteVar(PUMP_inflowPump, iPump);
      MEM_EEPROM_WriteVar(SENSOR_outTank, sensor);

      MEM_EEPROM_WriteSetupEntry();
      LCD_Auto_InflowPump(page, 0, _init);
      return SetupPage;									break;	//Ok

		case 0x23:	if(!touch){	touch=5;	LCD_ControlButtons(sn_minus);}
      if(cho==1)		  t_ipVal[0]= Eval_SetupMinus(t_ipVal[0], 0);
      else if(cho==2)	t_ipVal[1]= Eval_SetupMinus(t_ipVal[1], 0);
      else if(cho==4)	t_ipVal[2]= Eval_SetupMinus(t_ipVal[2], 0);	break;	//Minus

		case 0x24:	if(!touch){	touch=4;	LCD_ControlButtons(sn_plus);}
      if(cho==1)		  t_ipVal[0]= Eval_SetupPlus(t_ipVal[0], 99);
      else if(cho==2)	t_ipVal[1]= Eval_SetupPlus(t_ipVal[1], 59);
      else if(cho==4)	t_ipVal[2]= Eval_SetupPlus(t_ipVal[2], 60);	break;	//Plus

		case 0x21:	if(!touch){	cho=4;	touch=5;
      LCD_Setup_IPText(0b10110100);
      LCD_Setup_IPValues(0b00110100, &t_ipVal[0]);}break;	//On Min

		case 0x22:	if(!touch){cho=2;	touch=5;
      LCD_Setup_IPText(0b01011010);
      LCD_Setup_IPValues(0b01010010, &t_ipVal[0]);}break;	//Off Min

		case 0x31:	if(sensor && !touch){	touch=5; sensor=0;
      LCD_Write_Symbol_2(15, 5, p_sensor);}
      if(!sensor && !touch){	touch=5; sensor=1;
        LCD_Write_Symbol_2(15, 5, n_sensor);}		break;	//Sensor

		case 0x32:	iPump=0;	Eval_SetupPumpMark(0);				break;	//Mamutpumpe
		case 0x33:	iPump=1;	Eval_SetupPumpMark(1);				break;	//elek. Pump
		case 0x34:	iPump=2;	Eval_SetupPumpMark(2);				break;	//2 elek. Pumps

		case 0x00:	if(touch){	LCD_ControlButtons(touch);		touch=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 		break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 		break;
		default:									              break;
	}
	switch(cho)
	{
		case 1:	LCD_Setup_IPValues(cho, &t_ipVal[0]);	break;	//Off-Hour
		case 2:	LCD_Setup_IPValues(cho, &t_ipVal[0]);	break;	//Off-Min
		case 4:	LCD_Setup_IPValues(cho, &t_ipVal[0]);	break;	//On-Min
		default: 										break;
	}

	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Cal Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupCalLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	touch = 0;
	int 					        read = 0;
	static int				    cal = 0;
	static unsigned char	iniCal = 0;
	static unsigned char	calRedo = 0;
	static unsigned char	i = 0;
  static unsigned char  openV = 0;

	if(!iniCal)
	{
    i = 0;
		iniCal = 1;
    cal = ((MEM_EEPROM_ReadVar(CAL_H_druck) << 8) |
           (MEM_EEPROM_ReadVar(CAL_L_druck)));
    calRedo = MEM_EEPROM_ReadVar(CAL_Redo_on);
	}

	read = MPX_Read();
	if(read!= 0xFF00)
	{
		i++;
		if(i > 10)			//Print every 10th Time
		{
			i = 0;
			read = read - cal;
			if(read < 0){	read = -read;
        LCD_WriteStringFont(10,34, "-");}	//-Pressure
			else 	LCD_ClrSpace(10,34,2,5);			//+Pressure
			LCD_WriteValue3(10,40,read);			  //Write Pressure
		}
	}

	switch(matrix)
	{
		case 0x13:	if(!touch){	touch = 6;
      LCD_ControlButtons(sn_esc);
      if(page == SetupCalPressure)	OUT_Clr_Air();
      page = SetupPage;}								      break;	//Esc

		case 0x14:	if(!touch && page == SetupCal){	touch = 7;
      LCD_ControlButtons(sn_ok);
      MEM_EEPROM_WriteSetupEntry();
      MEM_EEPROM_WriteVar(CAL_Redo_on, calRedo);
      if(MEM_EEPROM_ReadVar(SONIC_on)) Sonic_LevelCal(_save);
      else MPX_LevelCal(_save);
      page = SetupPage;}								      break;	//Ok

    case 0x23:  if(!touch && page == SetupCal){	touch = 8;
      if(!openV){ openV = 1;
      	LCD_Write_TextButton(9, 80, OpenV, 0);
        PORT_Ventil_AllOpen();}}  break;

    // CAL for setting pressure to zero level
		case 0x24:	if(!touch && page == SetupCal){	touch = 4;
      LCD_Write_Symbol_2(9,125, n_cal);
      cal = MPX_ReadAverage_UnCal_Value();
      MEM_EEPROM_WriteVar(CAL_L_druck, (cal & 0x00FF));
      MEM_EEPROM_WriteVar(CAL_H_druck, ((cal >> 8) & 0x00FF));
      } break;

		case 0x31:	if(!touch){	touch = 5;
		  LCD_Write_Symbol_2(15,1, n_level);
		  if(openV){
		    openV = 0;
        LCD_Write_TextButton(9, 80, OpenV, 1);
		    PORT_Ventil_AllClose();}
      page = SetupCalPressure;}								break;	//Level-Messure

    // Cal redo with pressure -> Auto Zone page
		case 0x34:	if(!touch){	touch = 3;
      if(!MEM_EEPROM_ReadVar(SONIC_on)){
        if(calRedo){ calRedo = 0;
          LCD_Write_Symbol_3(15,130, p_arrowRedo);}
        else{	calRedo = 1;
          LCD_Write_Symbol_3(15,130, n_arrowRedo);}}}
      break;	//Cal-Redo

		case 0x00:	if(touch){
		  if(touch == 4) LCD_Write_Symbol_2(9,125, p_cal);
      touch = 0;}								                break;	//NoTouch

		//------------------------------------------------MainLinker
		case 0x41:	page = AutoPage;    break;
		case 0x42:	page = ManualPage; 	break;
		case 0x43:  page = SetupPage; 	break;
		case 0x44:	page = DataPage;	  break;
		default:											  break;
	}
  if(page != SetupCal && page != SetupCalPressure){
    iniCal = 0;
    if(openV){
      openV = 0;
      PORT_Ventil_AllClose();}}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Alarm Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupAlarmLinker(unsigned char matrix, t_page page)
{
	static unsigned char sensor=0;
	static unsigned char comp=0;
	static unsigned char temp=0;

	static unsigned char init=0;
	static unsigned char touch=0;

	if(!init)
	{
		init=1;
		sensor=	MEM_EEPROM_ReadVar(ALARM_sensor);
		comp=		MEM_EEPROM_ReadVar(ALARM_comp);
		temp=		MEM_EEPROM_ReadVar(ALARM_temp);

		LCD_WriteValueNeg3(10,15, temp);

		if(comp)	LCD_Write_Symbol_2(15, 40, n_compressor);
		else		LCD_Write_Symbol_2(15, 40, p_compressor);
		if(sensor)	LCD_Write_Symbol_2(15, 0, n_sensor);
		else		LCD_Write_Symbol_2(15, 0, p_sensor);
	}

	switch(matrix)
	{

		case 0x13:	if(!touch){	touch=6;		LCD_ControlButtons(sn_esc);}
					init=0;		return SetupPage;								break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons(sn_ok);}
							init=0;
							MEM_EEPROM_WriteVar(ALARM_comp, comp);
							MEM_EEPROM_WriteVar(ALARM_sensor, sensor);
							MEM_EEPROM_WriteVar(ALARM_temp, temp);

							MEM_EEPROM_WriteSetupEntry();
							return SetupPage;										break;	//Ok

		case 0x23:	if(!touch){	touch=5;	LCD_ControlButtons(sn_minus);}
      if(touch){temp= 	Eval_SetupMinus(temp, 15);}				break;	//Minus

		case 0x24:	if(!touch){	LCD_ControlButtons(sn_plus);	touch=4;}
      if(touch){temp= 	Eval_SetupPlus(temp, 99);}				break;	//Plus

		case 0x31:	if(!touch){
		  touch=5;
      if(sensor){sensor=0;
            LCD_Write_Symbol_2(15, 0, p_sensor);}
      else{		sensor=1;
            LCD_Write_Symbol_2(15, 0, n_sensor);}}	break;	//Sensor

		case 0x32:	if(!touch)
					{	touch=5;
						if(comp){	comp=0;
									LCD_Write_Symbol_2(15, 40, p_compressor);}
						else{		comp=1;
									LCD_Write_Symbol_2(15, 40, n_compressor);}
					}																break;	//Compressor


		case 0x00:	if(touch){	LCD_ControlButtons(touch);		touch=0;}			break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	  break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	  break;
		default:									              break;
	}

	if(touch){	LCD_WriteValueNeg3(10,15, temp); 	}						//Output
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Watch Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupWatchLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	dT[5]={0};
	static unsigned char	on=0;
	static unsigned char 	init=0;
	static unsigned char 	touch=0;
	unsigned char 			  time=0;

	if(!init)
	{
		init=1;
		dT[0]= MCP7941_ReadTime(TIC_HOUR);  TCC0_wait_us(25);
    dT[1]= MCP7941_ReadTime(TIC_MIN);   TCC0_wait_us(25);
		dT[2]= MCP7941_ReadTime(TIC_DATE);  TCC0_wait_us(25);
		dT[3]= MCP7941_ReadTime(TIC_MONTH); TCC0_wait_us(25);
		dT[4]= MCP7941_ReadTime(TIC_YEAR);
		Eval_SetupWatchMark(n_h, &dT[0]);
		on=0;
	}

	switch(matrix)
	{
		case 0x13:	if(!touch){	touch=6;		LCD_ControlButtons2(sn_esc);}
					init=0;		return SetupPage;						break;	//Esc

		case 0x14:	if(!touch){	touch=7;		LCD_ControlButtons2(sn_ok);}
					      init=0;
					      time= (((dT[0]/10)<<4) | (dT[0]%10));
								MCP7941_WriteByte(TIC_HOUR,	time);
								time= (((dT[1]/10)<<4) | (dT[1]%10));
								MCP7941_WriteByte(TIC_MIN,  	time);
								time= (((dT[2]/10)<<4) | (dT[2]%10));
								MCP7941_WriteByte(TIC_DATE, 	time);
								time= (((dT[3]/10)<<4) | (dT[3]%10));
								MCP7941_WriteByte(TIC_MONTH, 	time);
								time= (((dT[4]/10)<<4) | (dT[4]%10));
								MCP7941_WriteByte(TIC_YEAR, 	time);

								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;							break;	//Ok

		case 0x24:	if(!touch){	touch=4;		LCD_ControlButtons2(sn_plus);}
					switch(on)
					{
						case 0:	dT[0]=	Eval_SetupPlus(dT[0], 23);			break;
						case 1:	dT[1]= 	Eval_SetupPlus(dT[1], 59);			break;
						case 2:	dT[2]= 	Eval_SetupPlus(dT[2], 31);			break;
						case 3:	dT[3]= 	Eval_SetupPlus(dT[3], 12);			break;
						case 4:	dT[4]= 	Eval_SetupPlus(dT[4], 99);			break;
						default:											break;
					}
																			break;	//Plus

		case 0x34:	if(!touch){	touch=5;		LCD_ControlButtons2(sn_minus);}
					switch(on)
					{
						case 0:	dT[0]=	Eval_SetupMinus(dT[0], 0);	break;
						case 1:	dT[1]= 	Eval_SetupMinus(dT[1], 0);	break;
						case 2:	dT[2]= 	Eval_SetupMinus(dT[2], 1);  break;
						case 3:	dT[3]= 	Eval_SetupMinus(dT[3], 1);	break;
						case 4:	dT[4]= 	Eval_SetupMinus(dT[4], 0);  break;
						default:											              break;
					}														break;		//Minus

		case 0x21:	if(!touch){	Eval_SetupWatchMark(n_h, &dT[0]);
									on=0;	touch=5;}						break;	//hours

		case 0x22:	if(!touch){	Eval_SetupWatchMark(n_min, &dT[0]);
									on=1;	touch=5;}						break;	//Minutes

		case 0x31:	if(!touch){	Eval_SetupWatchMark(n_day, &dT[0]);
									on=2;	touch=5;}						break;	//days

		case 0x32:	if(!touch){	Eval_SetupWatchMark(n_month, &dT[0]);
									on=3;	touch=5;}						break;	//Month

		case 0x33:	if(!touch){	Eval_SetupWatchMark(n_year, &dT[0]);
									on=4;	touch=5;}						break;	//year

		case 0x00:	if(touch){	LCD_ControlButtons2(touch);	touch=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	break;
		default:									break;
	}

	if(touch)												//Write Date Time
	{
		switch(on)
		{
			case 0:	LCD_DateTime(n_h, dT[0]);			break;
			case 1:	LCD_DateTime(n_min, dT[1]);			break;
			case 2:	LCD_DateTime(n_day, dT[2]);			break;
			case 3:	LCD_DateTime(n_month, dT[3]);		break;
			case 4:	LCD_DateTime(n_year, dT[4]);		break;
			default: 									break;
		}
	}
	return page;
}



/* ------------------------------------------------------------------*
 * 						Setup Zone Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupZoneLinker(unsigned char matrix, t_page page)
{
	static int lvO2 = 0;
	static int lvCirc = 0;
	static int sonic = 0;
	static unsigned char onM=0;
	static unsigned char init=0;
	static unsigned char touch=0;
	unsigned char h=0;

	if(!init)
	{
		init=1;
		lvO2=(	(MEM_EEPROM_ReadVar(TANK_H_O2)<<8)		|
				(MEM_EEPROM_ReadVar(TANK_L_O2)));
		lvCirc=((MEM_EEPROM_ReadVar(TANK_H_Circ)<<8)	|
				(MEM_EEPROM_ReadVar(TANK_L_Circ)));

		sonic = MEM_EEPROM_ReadVar(SONIC_on);
		if(sonic)	LCD_Write_Symbol_3(3, 47, n_sonic);
		else		  LCD_Write_Symbol_3(3, 47, p_sonic);

		if(!onM){	LCD_WriteValue3(11,40, lvCirc);
					LCD_WriteValueNeg3(16,40, lvO2);
					LCD_Write_Symbol_2(9, 0, p_air);
					LCD_Write_Symbol_2(14, 0, n_setDown);}
		else{		LCD_WriteValueNeg3(11,40, lvCirc);
					LCD_WriteValue3(16,40, lvO2);
					LCD_Write_Symbol_2(9, 0, n_air);
					LCD_Write_Symbol_2(14, 0, p_setDown);}

		LCD_WriteStringFont(11,60,"cm");
		LCD_WriteStringFont(16,60,"cm");
	}

	switch(matrix)
	{
		case 0x12:	if(!touch){	touch=8;
      if(sonic){
        sonic=0;
        LCD_Write_Symbol_3(3, 47, p_sonic);}
      else{
        sonic=1;
        LCD_Write_Symbol_3(3, 47, n_sonic);}}	break;

		case 0x13:	if(!touch){	touch=6;	LCD_ControlButtons(sn_esc);}
        init=0;		return SetupPage;						break;	//Esc

		case 0x14:	if(!touch){	touch=7;	LCD_ControlButtons(sn_ok);}
      init=0;	h= (lvO2 & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_L_O2, h);
      h= ((lvO2>>8)&0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_O2, h);
      h= (lvCirc & 0x00FF);
      MEM_EEPROM_WriteVar(TANK_L_Circ, h);
      h= ((lvCirc>>8)&0x00FF);
      MEM_EEPROM_WriteVar(TANK_H_Circ, h);
      MEM_EEPROM_WriteVar(SONIC_on, sonic);
      MEM_EEPROM_WriteSetupEntry();
      return SetupPage;								break;	//Ok

		case 0x23:	if(!touch){	touch=5;	LCD_ControlButtons(sn_minus);}
					if(onM)			lvCirc= Eval_SetupMinus(lvCirc, 0);
					else			lvO2= 	Eval_SetupMinus(lvO2, lvCirc);		break;	//Minus

		case 0x24:	if(!touch){	LCD_ControlButtons(sn_plus);	touch=4;}
					if(onM)			lvCirc= Eval_SetupPlus(lvCirc, lvO2);
					else			lvO2= 	Eval_SetupPlus(lvO2, 	999);		break;	//Plus

		case 0x21:
		case 0x22:	if(!touch){onM=1;		touch=5;
								LCD_WriteValue3(16,40, lvO2);
								LCD_Write_Symbol_2(9, 0, n_air);
								LCD_Write_Symbol_2(14, 0, p_setDown);}		break;	//Circulate

		case 0x31:
		case 0x32:	if(!touch){onM=0;		touch=5;
								LCD_WriteValue3(11,40, lvCirc);
								LCD_Write_Symbol_2(9, 0, p_air);
								LCD_Write_Symbol_2(14, 0, n_setDown);}		break;	//O2

		case 0x00:	if(touch){	LCD_ControlButtons(touch);		touch=0;}		break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	init=0;	return AutoPage; 	break;
		case 0x42:	init=0;	return ManualPage; 	break;
		case 0x43:	init=0;	return SetupPage; 	break;
		case 0x44:	init=0;	return DataPage; 	break;
		default:									break;
	}

	if(onM  && touch){	LCD_WriteValueNeg3(11,40, lvCirc);	}	//O2
	if(!onM && touch){	LCD_WriteValueNeg3(16,40, lvO2);}	//Circulate
	return page;
}



/* ==================================================================*
 * 						Data Linker
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_DataLinker
 * ------------------------------------------------------------------*/

t_page Touch_DataLinker(unsigned char matrix, t_page page)
{
	switch(matrix)
	{
		case 0x21:	LCD_Write_TextButton(9, 0, Auto, 0);
					return DataAuto;					break;	//Auto

		case 0x22:	LCD_Write_TextButton(9, 40, Manual, 0);
					return DataManual;					break;	//Manual

		case 0x23:	LCD_Write_TextButton(9, 80, Setup, 0);
					return DataSetup; 					break;	//Setup

		case 0x24:	LCD_Write_TextButton(9, 120, Sonic, 0);
					return DataSonic; 				break;	//Setup

		//-----------------------------------------------------------MainLinker
		case 0x41:	return AutoPage; 				break;
		case 0x42:	return PinManual; 				break;
		case 0x43:	return PinSetup; 				break;
		case 0x44:	return DataPage; 				break;

		default:									break;
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data Auto Linker
 * ------------------------------------------------------------------*/

t_page Touch_DataAutoLinker(unsigned char matrix, t_page pa)
{
	static unsigned char mark=0;
	static unsigned char iData=0;

	switch(matrix)
	{
		case 0x14:	if(!mark){	mark= 1;	if(iData)	iData--;
											else 		iData=iData;
								LCD_Write_Symbol_3(	3,	140, n_arrowUp);}	break;	//arrowUp

		case 0x34:	if(!mark){	mark= 2;	if(iData < DATA_PAGE_NUM_AUTO) iData++;
											else		iData=iData;
								LCD_Write_Symbol_3(	14,	140, n_arrowDown);}	break;	//arrowDown

		case 0x00:	if(mark==1){mark= 0;
								LCD_Write_Symbol_3(	3,	140, p_arrowUp);
								LCD_WriteAutoEntryPage(iData);}

					if(mark==2){mark= 0;
								LCD_Write_Symbol_3(	14,	140, p_arrowDown);
								LCD_WriteAutoEntryPage(iData);}				break;	//No Touch



		//-----------------------------------------------------------MainLinker
		case 0x41:	iData=0;	return AutoPage; 		break;
		case 0x42:	iData=0;	return PinManual; 		break;
		case 0x43:	iData=0;	return PinSetup; 		break;
		case 0x44:	iData=0;	return DataPage; 		break;

		default:									break;
	}
	return pa;
}


/* ------------------------------------------------------------------*
 * 						Data Manual Linker
 * ------------------------------------------------------------------*/

t_page Touch_DataManualLinker(unsigned char matrix, t_page page)
{
	static unsigned char mark=0;
	static unsigned char iData=0;

	switch(matrix)
	{
		case 0x14:	if(!mark){	mark= 1;	if(iData)	iData--;
											else 		iData=iData;
								LCD_Write_Symbol_3(	3,	140, n_arrowUp);}	break;	//arrowUp

		case 0x34:	if(!mark){	mark= 2;	if(iData < DATA_PAGE_NUM_MANUAL) iData++;
											else		iData=iData;
								LCD_Write_Symbol_3(	14,	140, n_arrowDown);}	break;	//arrowDown

		case 0x00:	if(mark==1){mark= 0;
								LCD_Write_Symbol_3(	3,	140, p_arrowUp);
								LCD_WriteManualEntryPage(iData);}

					if(mark==2){mark= 0;
								LCD_Write_Symbol_3(	14,	140, p_arrowDown);
								LCD_WriteManualEntryPage(iData);}			break;	//No Touch



		//-----------------------------------------------------------MainLinker
		case 0x41:	iData=0;	return AutoPage; 		break;
		case 0x42:	iData=0;	return PinManual; 		break;
		case 0x43:	iData=0;	return PinSetup; 		break;
		case 0x44:	iData=0;	return DataPage; 		break;

		default:										break;
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data Setup Linker
 * ------------------------------------------------------------------*/

t_page Touch_DataSetupLinker(unsigned char matrix, t_page page)
{
	static unsigned char mark=0;
	static unsigned char iData=0;

	switch(matrix)
	{
		case 0x14:	if(!mark){	mark= 1;	if(iData)	iData--;
											else 		iData=iData;
								LCD_Write_Symbol_3(	3,	140, n_arrowUp);}	break;	//arrowUp

		case 0x34:	if(!mark){	mark= 2;	if(iData < DATA_PAGE_NUM_SETUP) iData++;
											else		iData=iData;
								LCD_Write_Symbol_3(	14,	140, n_arrowDown);}	break;	//arrowDown

		case 0x00:	if(mark==1){mark= 0;
								LCD_Write_Symbol_3(	3, 140, p_arrowUp);
								LCD_WriteSetupEntryPage(iData);}

					if(mark==2){mark= 0;
								LCD_Write_Symbol_3(	14,	140, p_arrowDown);
								LCD_WriteSetupEntryPage(iData);}				break;	//No Touch



		//-----------------------------------------------------------MainLinker
		case 0x41:	iData=0;	return AutoPage; 		break;
		case 0x42:	iData=0;	return PinManual; 		break;
		case 0x43:	iData=0;	return PinSetup; 		break;
		case 0x44:	iData=0;	return DataPage; 		break;

		default:										break;
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data UltraSonic Linker
 * ------------------------------------------------------------------*/

t_page Touch_DataSonicLinker(unsigned char matrix, t_page page)
{
	static unsigned char touch = 0;

	switch(matrix)
	{
		case 0x11:
		  if(!touch && page != DataSonicBoot){ touch = 1;
		    if(page != DataSonic) LCD_Data_SonicWrite(_clear, 0);
        LCD_Write_TextButton(4, 0, Shot, 0);
        Sonic_Data_Shot();
        page = DataSonic;}
      break;	//Shot

		case 0x21:
		  if(!touch && page != DataSonicBoot){	touch = 2;
		    LCD_Data_SonicWrite(_clear, 0);
		    LCD_Data_SonicWrite(_autotext, 0);
        CAN_SonicQuery(_init, _startTemp);
        page = DataSonicAuto;}
      break;	//AutoShot

    case 0x31:
		  if(!touch){	touch = 3;
        LCD_Data_SonicWrite(_clear, 0);
        LCD_Write_TextButton(16, 0, Boot, 0);
        LCD_Write_TextButton(4, 120, Read, 1);
        LCD_Write_TextButton(10, 120, Write, 1);

        if(page == DataSonicBoot){
          LCD_Write_TextButton(16, 0, Boot, 1);
          LCD_Data_SonicWrite(_clear, 0);
          Sonic_Data_Boot(_off);
          page = DataSonic; break;}

        Sonic_Data_Boot(_on);
        page = DataSonicBoot;
        }
      break;	//Bootloader

    case 0x14:
      if(!touch && page == DataSonicBoot){ touch = 4;
        LCD_Write_TextButton(4, 120, Read, 0);
        page = DataSonicBootR;
      }
      break;

    case 0x24:
      if(!touch && page == DataSonicBoot){ touch = 5;
        LCD_Write_TextButton(10, 120, Write, 0);
        page = DataSonicBootW;
      }
      break;

		case 0x00:
		  if(touch == 1){touch = 0;
        LCD_Write_TextButton(4, 0, Shot, 1);}
      if(touch == 2){touch = 0;
        LCD_Write_TextButton(10, 0, Auto, 1);}
      if(touch == 3) touch = 0;
      if(touch == 4) touch = 0;
      if(touch == 5) touch = 0;
      break;	//ResetButtons

		//------------------------------------------------MainLinker
		case 0x41:	touch = 0; LCD_Data_SonicWrite(_clear, 0);
                return AutoPage; 	break;
		case 0x42:	touch = 0; LCD_Data_SonicWrite(_clear, 0);
                return PinManual; break;
		case 0x43:	touch = 0; LCD_Data_SonicWrite(_clear, 0);
                return PinSetup; 	break;
		case 0x44:	touch = 0; LCD_Data_SonicWrite(_clear, 0);
                return DataPage; 	break;
		default:									                break;
	}
	return page;
}



/* ==================================================================*
 * 						Pin Linker
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	Touch_PinLinker
 * ------------------------------------------------------------------*/

t_page Touch_PinLinker(unsigned char matrix, t_page page)
{
	static unsigned char touch[12]={0x00};     //Touched Buttons
	static unsigned char in[4]={0x00};			   //inputCode

	unsigned char			   secret[4]={2,5,8,0};	//secret
	unsigned char			   compH[4]={1,5,9,3};	//set Comp-Hours= 0
  unsigned char        tel1[4]={0,0,0,0};		//Telephone1
  unsigned char        tel2[4]={0,0,0,1};		//Telephone2

	unsigned char			    i=0;
	static unsigned char	cp=0;				//CodePosition
  static TelNr          nr;         //Nr+Pos

	switch(matrix)
	{
    //------------------------------------------------1
		case 0x11: 	if(!touch[1]){touch[1]= 0x11;
		  if(nr.id){
        nr.tel= 1;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(1, cp); in[cp]= 1;  cp++;}}	break;

    //------------------------------------------------2
		case 0x12: 	if(!touch[2]){touch[2]= 0x12;
		  if(nr.id){
        nr.tel= 2;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(2, cp); in[cp]= 2;  cp++;}}	break;

    //------------------------------------------------3
		case 0x13:	if(!touch[3]){touch[3]= 0x13;
		  if(nr.id){
        nr.tel= 3;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(3, cp); in[cp]= 3;  cp++;}}	break;

    //------------------------------------------------4
		case 0x21:	if(!touch[4]){touch[4]= 0x14;
		  if(nr.id){
        nr.tel= 4;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(4, cp); in[cp]= 4;  cp++;}}	break;

    //------------------------------------------------5
		case 0x22:	if(!touch[5]){touch[5]= 0x15;
		  if(nr.id){
        nr.tel= 5;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(5, cp); in[cp]= 5;  cp++;}}	break;

    //------------------------------------------------6
		case 0x23:	if(!touch[6]){touch[6]= 0x16;
		  if(nr.id){
        nr.tel= 6;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(6, cp); in[cp]= 6;  cp++;}}	break;

    //------------------------------------------------7
		case 0x31:	if(!touch[7]){touch[7]= 0x17;
		  if(nr.id){
        nr.tel= 7;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(7, cp); in[cp]= 7;  cp++;}}	break;

    //------------------------------------------------8
		case 0x32:	if(!touch[8]){touch[8]= 0x18;
		  if(nr.id){
        nr.tel= 8;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(8, cp); in[cp]= 8;  cp++;}}	break;

    //------------------------------------------------9
		case 0x33:	if(!touch[9]){touch[9]= 0x19;
		  if(nr.id){
        nr.tel= 9;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(9, cp); in[cp]= 9;  cp++;}}	break;

    //------------------------------------------------0
		case 0x42:	if(!touch[0]){touch[0]= 0x10;
		  if(nr.id){
        nr.tel= 0;  Modem_TelNr(_write, nr);
        LCD_Pin_Write(_telnr, nr);  nr.pos++;}
      else{
        Eval_PinWrite(0, cp); in[cp]= 0;  cp++;}}	break;

    //------------------------------------------------Del
		case 0x43:	LCD_nPinButtons(10);
		            Eval_PinDel();
		            touch[10]= 0x20;
		            cp=0;
                if(nr.id)
                {
                  nr.pos= 0;
                  Modem_TelNr(_reset, nr);
                  LCD_Pin_Write(_write, nr);
                }
                break;

    //------------------------------------------------Esc
		case 0x41:	LCD_nPinButtons(11);	cp=0;
                nr.id= 0; nr.pos= 0;
		            return DataPage;
		            break;

    //------------------------------------------------Okay4Tel
    case 0x44:
      if(!touch[11] && nr.id){
        touch[11]= 1;
        LCD_Pin_WriteOK(1);
        Modem_TelNr(_save, nr);

        // test new number
        if(SMS_ON)
        {
          Modem_SMS(nr, "Hello from your wastewater treatment plant, you are added.");
        }
        else
        {
          Modem_Call(nr);
        }
        nr.id= 0;
        nr.pos= 0;}
      break;

    //------------------------------------------------NoTouch
		case 0x00:
      Eval_PinClr(&touch[0]);
      for(i=0; i<10; i++) touch[i]= 0;
	    if(touch[11]){
	      touch[11]= 0;
        LCD_Pin_Write(_clear, nr);}
      break;

		default:												              break;
	}

	if(cp>3 && !nr.id)
	{
		cp=0;
		LCD_Pin_Write(_clear, nr);
		//------------------------------------------------GoToManualorSetup
		if((in[0]== secret[0]) && (in[1]== secret[1]) &&		//Check Secret
    (in[2]== secret[2]) && (in[3]== secret[3]))         //Check Secret
    {
      LCD_Pin_Write(_right, nr);
      switch(page)
      {
        case PinManual:	return ManualPage;  break;
        case PinSetup:	return SetupPage;	  break;
        default:							              break;
      }
    }

    //------------------------------------------------OpHourReset
		else if((in[0]== compH[0]) && (in[1]== compH[1]) &&
    (in[2]== compH[2]) && (in[3]== compH[3]))
    {
      MCP7941_Write_Comp_OpHours(0);			//Reset CompH
      LCD_Pin_Write(_op, nr);
    }

    //------------------------------------------------EnterTel1
    else if((in[0]== tel1[0]) && (in[1]== tel1[1]) &&
    (in[2]== tel1[2]) && (in[3]== tel1[3]))
    {
      nr.id= 1;  nr.pos= 0;
      Modem_TelNr(_init, nr);
      LCD_Pin_Write(_write, nr);
      LCD_Pin_WriteOK(0);
    }

    //------------------------------------------------EnterTel2
    else if((in[0]== tel2[0]) && (in[1]== tel2[1]) &&
    (in[2]== tel2[2]) && (in[3]== tel2[3]))
    {
      nr.id= 2;  nr.pos= 0;
      Modem_TelNr(_init, nr);
      LCD_Pin_Write(_write, nr);
      LCD_Pin_WriteOK(0);
    }

    //------------------------------------------------Wrong
    else  LCD_Pin_Write(_wrong, nr);

		Eval_PinDel();
		return page;
	}
	if(nr.pos>14)    //Stop
	{
    nr.id= 0;        //Tel-written
    nr.pos= 0;
    cp=0;
    LCD_Pin_Write(_clear, nr);
	}
	if(nr.id) Modem_ReadSLED(PinModem);

	return page;
}





/**********************************************************************\
 * End of file
\**********************************************************************/
