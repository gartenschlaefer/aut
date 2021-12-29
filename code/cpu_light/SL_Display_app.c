/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Display-App-SourceFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Application-File for the EADOGXL160-7 Display
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:	10.11.2011									
\**********************************************************************/


#include<avr/io.h>

#include "SL_Define_sym.h"

#include "SL_Display_driver.h"
#include "SL_Touch_driver.h"
#include "SL_MPX_driver.h"
#include "SL_MCP9800_driver.h"

#include "SL_Output_app.h"
#include "SL_Display_app.h"
#include "SL_Touch_app.h"
#include "SL_Eval_app.h"
#include "SL_Memory_app.h"

#include "SL_Error_func.h"
#include "SL_TC_func.h"
#include "SL_Watchdog_func.h"
#include "SL_PORT_func.h"

#include "SL_Display_sym.h"



/* ===================================================================*/
/* ===================================================================*
 * 						FUNCTIONS Display Pages		
 * ===================================================================*/
/* ===================================================================*/

/* ===================================================================*
 * 						Auto Pages
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_AutoPage
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Loads Auto-Page in Display
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage(t_page autoPage)
{
	static t_page			sPage= AutoZone;		//Start-AutoPage
	unsigned char 			matrix=0;
	static int 				aMin=5;
	static int	 			aSec=0;
	static int				lcdReset=0;		
	static unsigned char	initVar=0;
		
	int sMin;
	int sSec;
	int *p_min;
	int *p_sec;

	p_min= &aMin;
	p_sec= &aSec;
	
	switch(autoPage)
	{
		case AutoPage:		
				LCD_AutoSet_Page();
				PORT_FloatSwitch(Float_EnableQuery);
				autoPage= sPage;
				if(!initVar || (aMin==0 && !(MEM_EEPROM_ReadVar(SENSOR_inTank)))){
						initVar=1;
						LCD_Write_AirVar(AutoCirculate, 0, _init);
						LCD_Auto_InflowPump(0, _init);	
						LCD_Auto_Phosphor(0, _init);							
						OUT_CloseOff();
						LCD_AutoSet(autoPage, p_min, p_sec);}	//Set Time

				else{	sMin= *p_min;							//Save Time
						sSec= *p_sec;
						LCD_AutoSet(autoPage, p_min, p_sec);
						*p_min= sMin;
						*p_sec= sSec;
						if(	(autoPage== AutoAir) ||
							(autoPage== AutoCirculate))		break;
						LCD_AutoCountDown(aMin, aSec);}		break;
		
		case AutoZone:
				sPage= AutoZone;	
				autoPage= LCD_AutoPage_Zone(p_min, p_sec);
				if(autoPage!= AutoZone)	LCD_AutoSet(autoPage, p_min, p_sec);	break;

		case AutoSetDown:
				sPage= AutoSetDown;	
				autoPage= LCD_AutoPage_SetDown(p_min, p_sec);
				if(autoPage!= AutoSetDown)	LCD_AutoSet(autoPage, p_min, p_sec);	break;		

		case AutoPumpOff:
				sPage= AutoPumpOff;	
				autoPage= LCD_AutoPage_PumpOff(p_min, p_sec);
				if(autoPage!= AutoPumpOff)	LCD_AutoSet(autoPage, p_min, p_sec);	break;

		case AutoMud:
				sPage= AutoMud;		
				autoPage= LCD_AutoPage_Mud(p_min, p_sec);
				if(autoPage!= AutoMud)	LCD_AutoSet(autoPage, p_min, p_sec);		break;

		case AutoCirculate:	
				sPage= AutoCirculate;
				autoPage= LCD_AutoPage_Circulate(p_min, p_sec);	
				if(autoPage!= AutoCirculate)	LCD_AutoSet(autoPage, p_min, p_sec);break;

		case AutoAir:
				sPage= AutoAir;
				autoPage= LCD_AutoPage_Air(p_min, p_sec);
				if(autoPage!= AutoAir)	LCD_AutoSet(autoPage, p_min, p_sec);		break;

		default:																	break;
	}	
	
	Watchdog_Restart();
	matrix= 	Touch_Matrix();
	autoPage= 	Touch_AutoLinker(matrix, autoPage, p_min);
	
	if(lcdReset>2000)								//100 = 5s
	{
		lcdReset= 0;			
		LCD_Init();                          		//Init LCD 
		TC_DisplayData_Wait();
		LCD_AutoSet_Symbol(autoPage, aMin, aSec);	//Clear Display
	}
	lcdReset++;
	
	return autoPage;
}



/* -------------------------------------------------------------------*
 * 						Auto ZoneCalc
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage_Zone(int *p_min, int *p_sec)
{
	unsigned char evalCD=0;
	int sec=0;
	int min=0;
	t_page page= AutoZone;	

	sec= *p_sec;
	min= *p_min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);
	Error_Detection(AutoCirculate, *p_min, *p_sec);
		
	LCD_WriteAutoVar(min, sec);							//Variables
	evalCD= Eval_AutoCountDown(p_min, p_sec);			//CountDown

	if(evalCD){
		page= MPX_ReadTank(AutoZone, _exe);
		MPX_ReadTank(AutoZone, _write);
		OUT_Clr_Air();
		if(page== AutoZone)	return AutoSetDown;
		return page;}
	return AutoZone;
}



/* -------------------------------------------------------------------*
 * 						Auto Set-Down
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage_SetDown(int *p_min, int *p_sec)
{
	unsigned char evalCD=0;
	int sec=0;
	int min=0;

	sec= *p_sec;
	min= *p_min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);
	Error_Detection(AutoSetDown, *p_min, *p_sec);
		
	LCD_WriteAutoVar(min, sec);					//Variables
	evalCD= Eval_AutoCountDown(p_min, p_sec);	//CountDown

	if(evalCD) 	return AutoPumpOff;											//Next Auto Page				
	return AutoSetDown;
}



/* -------------------------------------------------------------------*
 * 						Auto Pump-Off
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage_PumpOff(int *p_min, int *p_sec)
{
	unsigned char evalCD=0;

	int sec=0;
	int min=0;

	sec= *p_sec;
	min= *p_min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);
	Error_Detection(AutoPumpOff, *p_min, *p_sec);
		
	LCD_WriteAutoVar_Comp(min, sec);			//Variables
	evalCD= Eval_AutoCountDown(p_min, p_sec);	//CountDown

	if(evalCD)
	{		
		OUT_Clr_PumpOff();
		return AutoMud;				//Next Auto Page
	}			
	return AutoPumpOff;
}	



/* -------------------------------------------------------------------*
 * 						Auto Mud
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage_Mud(int *p_min, int *p_sec)
{
	unsigned char evalCD=0;
	int sec=0;
	int min=0;

	sec= *p_sec;
	min= *p_min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);
	Error_Detection(AutoMud, *p_min, *p_sec);
		
	LCD_WriteAutoVar_Comp(min, sec);			//Write Variables
	evalCD= Eval_AutoCountDown(p_min, p_sec);	//CountDown

	if(evalCD)
	{		
		OUT_Clr_Mud();
		//------------------------------------------------WriteEntry----
		Eval_Oxygen(0, Oxygen_Write2Entry);				//O2 Write2Entry
		MEM_EEPROM_WriteAutoEntry(0, 0, Write_Entry);	//Write Auto Entry
		Eval_Oxygen(0, Oxygen_Clear);					//02==0
		return AutoCirculate;							//Next Auto Page
	}			
	return AutoMud;
}	




/* -------------------------------------------------------------------*
 * 						Auto Circulate
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage_Circulate(int *p_min, int *p_sec)
{
	unsigned char 	evalCD=0;
	t_page 			read=0;

	int sec=0;
	int min=0;

	sec= *p_sec;
	min= *p_min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);
		
	read= LCD_Write_AirVar(AutoCirculate, sec,  _exe);	//WriteVar
	evalCD= Eval_AutoCountDown(p_min, p_sec);			//CountDown

	if(read == AutoSetDown){							//MPX-Sensor
		LCD_Auto_InflowPump(0, _reset);					//ResetIP
		OUT_Clr_Air();							//Clear Air	
		return AutoSetDown;}							//Next Auto Page
	
	if(read == AutoAir){
		LCD_Auto_InflowPump(0, _reset);					//ResetIP
		OUT_Clr_Air();							//Clear Air	
		LCD_Write_AirVar(AutoAir, 0, _init);			//reInit Air							
		return AutoAir;}								//Next Auto Page
	
	if(evalCD && (!MEM_EEPROM_ReadVar(SENSOR_inTank))){	//Time		
		LCD_Auto_InflowPump(0, _reset);					//ResetIP
		OUT_Clr_Air();							//Clear Air	
		LCD_Write_AirVar(AutoAir, 0, _init);			//reInit Air
		return AutoAir;}								//Next Auto Page
	
	return AutoCirculate;								//NewTurn
}	




/* -------------------------------------------------------------------*
 * 						Auto Air
 * -------------------------------------------------------------------*/

t_page LCD_AutoPage_Air(int *p_min, int *p_sec)
{
	unsigned char 	evalCD=0;
	t_page 			read=0;
	int sec=0;
	int min=0;

	sec= *p_sec;
	min= *p_min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);
		
	read= LCD_Write_AirVar(AutoAir, sec, _exe);		//WriteVar
	evalCD= Eval_AutoCountDown(p_min, p_sec);			//CountDown
	
	if(read == AutoSetDown){							//MPX-Sensor
		LCD_Auto_InflowPump(0, _reset);					//ResetIP
		OUT_Clr_Air();									//Clear Air	
		return AutoSetDown;}							//Next Auto Page

	if(evalCD){											//Time
		LCD_Auto_InflowPump(0, _reset);					//ResetIP
		OUT_Clr_Air();									//Clear Air
		LCD_Write_AirVar(AutoCirculate, 0, _init);		//reInit Air
		return AutoSetDown;}							//Next Auto Page
	
	return AutoAir;
}	






/* ===================================================================*
 * 						Auto Built-in-Page
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Auto Inflow Pump
 * -------------------------------------------------------------------*/

t_FuncCmd LCD_Auto_InflowPump(unsigned char rSec, t_FuncCmd cmd)
{
	static unsigned char ip_count=0;
	static unsigned char ip_min= 0;			//Init-Var
	static unsigned char ip_sec= 0;			//Init-Var
	
	static t_FuncCmd ip_state= _off;		//Init-Var
	
	//--------------------------------------------------------------Init
	if(cmd==_init)
	{	
		ip_min= MEM_EEPROM_ReadVar(ON_inflowPump);		//Read ON-Time
		if(!ip_min){	ip_sec=0;						//?-Disable
						ip_state= _disabled;			//InfolwPump=Disabled
						return ip_state;}				//Return

		ip_min= 1;										//?-IP-Enabled
		ip_sec= 2;
		ip_state= _off;
	}

	//--------------------------------------------------------------State
	else if(cmd==_state)
	{	
		return ip_state;
	}
	
	//--------------------------------------------------------------OutSet
	else if(cmd==_outSet)
	{	
		switch(ip_state)
		{
			case _disabled:
			case _off:		LCD_WriteAuto_IP_pSym();	break;
			case _on:		LCD_WriteAuto_IP_nSym();	
							OUT_Set_InflowPump();		break;
			default:									break;
		}
		LCD_WriteValue_MyFont2(13,95, ip_min);
		LCD_WriteValue_MyFont2(13,107, ip_sec);
	}
	
	//--------------------------------------------------------------Write-Symbols
	else if(cmd==_write)
	{	
		LCD_ClrSpace(5,90,4,40);
		LCD_WriteAuto_IP_Sensor();
		switch(ip_state)
		{
			case _disabled:
			case _off:		LCD_WriteAuto_IP_pSym();	break;
			case _on:		LCD_WriteAuto_IP_nSym();	break;
			default:									break;
		}	
		LCD_WriteValue_MyFont2(13,95, ip_min);
		LCD_WriteValue_MyFont2(13,107, ip_sec);
	}
	
	//==============================================================Termiantion
	else if(ip_state==_disabled)	return ip_state;	//Disabled?		
	//==============================================================

	//--------------------------------------------------------------Set
	else if(cmd==_set)
	{
		ip_state=_on;
		ip_min= MEM_EEPROM_ReadVar(ON_inflowPump);	ip_sec=2;
		LCD_WriteValue_MyFont2(13,95, ip_min);
		LCD_WriteValue_MyFont2(13,107, ip_sec);
		LCD_WriteAuto_IP_nSym();	
		OUT_Set_InflowPump();
	}
	
	//--------------------------------------------------------------Reset
	else if(cmd==_reset)
	{	
		LCD_WriteAuto_IP_pSym();
		if(ip_state==_on)	OUT_Clr_InflowPump();
		ip_state=_off;
	}
	
	//--------------------------------------------------------------exe
	else if(cmd==_exe)
	{
		//--------------------------------------------------------------AutoChange2Off
		if((ip_state==_on && !ip_min && !ip_sec)	||
		   (ip_state==_on && (PORT_FloatSwitch(Float_outTank)==Float_Min))){	
				ip_state= _off;
				ip_min= MEM_EEPROM_ReadVar(OFF_inflowPump);		ip_sec=0;
				LCD_ClrSpace(5,90,4,40);
				LCD_WriteAuto_IP_pSym();
				OUT_Clr_InflowPump();}
		
		//--------------------------------------------------------------AutoChange2On
		else if((ip_state==_off && !ip_min && !ip_sec)	|| 
				(ip_state==_off && (PORT_FloatSwitch(Float_outTank)==Float_Max))){	
					ip_state= _on;
					ip_min= MEM_EEPROM_ReadVar(ON_inflowPump);	ip_sec=0;
					LCD_ClrSpace(5,90,4,40);
					LCD_WriteAuto_IP_nSym();
					OUT_Set_InflowPump();}	

		//--------------------------------------------------------------CountDown
		if(ip_count!=rSec){	ip_count= rSec;									
							if(ip_sec<1){	ip_sec=60;	ip_min--;
											LCD_WriteValue_MyFont2(13,95, ip_min);}		
							ip_sec--;
							LCD_WriteValue_MyFont2(13,107, ip_sec);}
	}	
	
	return ip_state;
}



/* -------------------------------------------------------------------*
 * 						Auto Phosphor
 * -------------------------------------------------------------------*/

void LCD_Auto_Phosphor(unsigned char rSec, t_FuncCmd cmd)
{
	static unsigned char pCount=0;
	static unsigned char pMin= 0;
	static unsigned char pSec= 5;
	static t_FuncCmd	 p_state=0;
	
	//--------------------------------------------------------------Init
	if(cmd== _init)
	{	
		pMin= MEM_EEPROM_ReadVar(ON_phosphor);		//Read ON-Time
		if(!pMin){	pSec= 0;						//Phosphor=Disabled?
					p_state= _disabled;				//Disable
					return;}						//Return

		pMin= 1;
		pSec= 5;
		p_state= _off;
	}

	//--------------------------------------------------------------Set
	else if(cmd== _set)
	{
		switch(p_state)
		{
			case _on:		OUT_Set_Phosphor();	
							LCD_Write_Symbol_3(6, 134, n_phosphor);	break;
			case _disabled:
			case _off:		OUT_Clr_Phosphor();
							LCD_Write_Symbol_3(6, 134, p_phosphor);	break;
			default:													break;
		}
		LCD_WriteValue_MyFont2(13,135, pMin);
		LCD_WriteValue_MyFont2(13,147, pSec);
	}
	
	//==============================================================Termiantion
	else if(p_state==_disabled)		return;			//Disabled?	
	//==============================================================

	//--------------------------------------------------------------Exe
	else if(cmd==_exe)
	{
		if(pCount != rSec){	pCount= rSec;							//Counter
							if(pSec<1){	pSec=60;	pMin--;
										LCD_WriteValue_MyFont2(13,135, pMin);}	
							pSec--;
							LCD_WriteValue_MyFont2(13,147, pSec);}
	
		if(p_state==_on && !pMin && !pSec)								//Change to OFF
		{
			p_state=_off;
			pMin= MEM_EEPROM_ReadVar(OFF_phosphor);
			pSec=0;
			LCD_Write_Symbol_3(6, 134, p_phosphor);
			OUT_Clr_Phosphor();
		}
		else if(p_state==_off && !pMin && !pSec)						//Change to ON
		{
			p_state=_on;
			pMin= MEM_EEPROM_ReadVar(ON_phosphor);
			pSec=0;
			LCD_Write_Symbol_3(6, 134, n_phosphor);
			OUT_Set_Phosphor();
		}	
	}	
}	







/* ===================================================================*
 * 						Manual Pages
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_ManualPage
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Loads Manual-Page in Display
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage(t_page manualPage)
{
	static unsigned char mMin=5;
	static unsigned char mSec=0;
	
	unsigned char *p_min;
	unsigned char *p_sec;

	p_min= &mMin;
	p_sec= &mSec;
	
	switch(manualPage)
	{
		case ManualPage:
				manualPage= ManualMain;	
				mMin= 5;
				mSec= 0;
				LCD_ManualSet_Page(mMin, mSec);							break;	//Setup		
		
		case ManualMain:
				manualPage= LCD_ManualPage_Main(p_min, p_sec);			break;	//Main
					
		case ManualCirculate:	
				manualPage= LCD_ManualPage_Circulate(p_min, p_sec);		break;	//Circulate

		case ManualAir:			
				manualPage= LCD_ManualPage_Air(p_min, p_sec);			break;	//Air

		case ManualSetDown:		
				manualPage= LCD_ManualPage_SetDown(p_min, p_sec);		break;	//Set Down

		case ManualPumpOff:	
		case ManualPumpOff_On:	
				manualPage= LCD_ManualPage_PumpOff(p_min, p_sec);		break;	//Pump Off

		case ManualMud:			
				manualPage= LCD_ManualPage_Mud(p_min, p_sec);			break;	//Mud
		
		case ManualCompressor:		
				manualPage= LCD_ManualPage_Compressor(p_min, p_sec);	break;	//Compressor
			
		case ManualPhosphor:		
				manualPage= LCD_ManualPage_Phosphor(p_min, p_sec);		break;	//Phosphor
		
		case ManualInflowPump:	
				manualPage= LCD_ManualPage_InflowPump(p_min, p_sec);	break; 	//inflowPump
		
		default:														break;
	}	

	return manualPage;
}



/* -------------------------------------------------------------------*
 * 						Manual Main
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_Main(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualMain;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualMain);

	if(manPage!= ManualMain){	LCD_ManualSet(manPage, p_Min, p_Sec);	//nextPage
								return manPage;}

	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown
	
	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage
	
	return manPage;
}



/* -------------------------------------------------------------------*
 * 						Manual Circulate
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_Circulate(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualCirculate;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualCirculate);
	
	if(manPage!= ManualCirculate){	OUT_Clr_Air();						//Clr R+V
									LCD_ManualSet(manPage, p_Min, p_Sec);		//nextPage
									LCD_Write_AirVar(AutoCirculate, 0, _init);	//reInit Air
									return manPage;}

	LCD_WriteManualVar(min, sec);								//Variables
	LCD_Write_AirVar(ManualCirculate, sec, _exe);				//WriteVar
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage

	return manPage;
}	


/* -------------------------------------------------------------------*
 * 						Manual Air
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_Air(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualAir;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualAir);
	
	if(manPage!= ManualAir){	OUT_Clr_Air();							//Clr R+V
								LCD_ManualSet(manPage, p_Min, p_Sec);	//nextPage
								return manPage;}
	
	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage

	return manPage;
}	


/* -------------------------------------------------------------------*
 * 						Manual Set Down
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_SetDown(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualSetDown;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualSetDown);
	
	if(manPage!= ManualSetDown){	LCD_ManualSet(manPage, p_Min, p_Sec);	//nextPage
									return manPage;}

	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage
	
	return manPage;
}	


/* -------------------------------------------------------------------*
 * 						Manual Pump Off
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_PumpOff(unsigned char *p_Min, unsigned char *p_Sec)	
{
	t_page manPage= ManualPumpOff;
	static unsigned char on=0;
	static unsigned char count=0;

	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualPumpOff);
	
	if(manPage==ManualPumpOff_On){ 		on=1;		//--PumpOffOn
		LCD_ManualText(min, sec);}

	else if(manPage!= ManualPumpOff){				//--Else
		LCD_ClrSpace(15, 2, 3, 120);
		if(on)	OUT_Clr_PumpOff();					//Clr PumpOff
		LCD_ManualSet(manPage, p_Min, p_Sec);		//nextPage
		LCD_ManualText(min, sec);
		on=0;
		return manPage;}
	
	if(!on){	count++;							//--Blink				
		if(count>10){	LCD_WriteStringFontNeg(17,15,"PRESS OK!:");}	
		if(count>20){ 	LCD_WriteStringFont(17,15,"PRESS OK!:");
						count=0;}}	

	if(on)	LCD_WriteManualVar(min, sec);						//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown
	
	if(evalCD){	on=0;
				MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage
	
	return manPage;
}	


/* -------------------------------------------------------------------*
 * 						Manual Mud
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_Mud(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualMud;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualMud);
	
	if(manPage!= ManualMud){	OUT_Clr_Mud();							//Clr R+V
								LCD_ManualSet(manPage, p_Min, p_Sec);	//nextPage
								return manPage;}
	
	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage

	return manPage;
}		


/* -------------------------------------------------------------------*
 * 						Manual Compressor
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_Compressor(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualCompressor;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualCompressor);
	
	if(manPage!= ManualCompressor){	OUT_Clr_Compressor();						//Clr R+V
									LCD_ManualSet(manPage, p_Min, p_Sec);		//nextPage
									return manPage;}
	
	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage
	
	return manPage;
}	


/* -------------------------------------------------------------------*
 * 						Manual Phosphor
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_Phosphor(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualPhosphor;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualPhosphor);
	
	if(manPage!= ManualPhosphor){	OUT_Clr_Phosphor();					//Clr R+V
									LCD_ManualSet(manPage, p_Min, p_Sec);	//nextPage
									return manPage;}

	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage

	return manPage;
}		


/* -------------------------------------------------------------------*
 * 						Manual Inflow Pump
 * -------------------------------------------------------------------*/

t_page LCD_ManualPage_InflowPump(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page manPage= ManualInflowPump;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	unsigned char sec=0;
	unsigned char min=0;

	sec= *p_Sec;
	min= *p_Min;

	Watchdog_Restart();
	LCD_Backlight(Backlight_Count);

	matrix= 	Touch_Matrix();
	manPage=	Touch_ManualLinker(matrix, ManualInflowPump);
	
	if(manPage!= ManualInflowPump){	OUT_Clr_InflowPump();					//Clr R+V
									LCD_ManualSet(manPage, p_Min, p_Sec);	//nextPage
									return manPage;}
	
	LCD_WriteManualVar(min, sec);								//Variables
	evalCD= Eval_CountDown(p_Min, p_Sec);						//CountDown

	if(evalCD){	MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
				return AutoPage;}								//Back to AutoPage

	return manPage;
}	










/* ===================================================================*
 * 						Setup Pages
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_SetupPage
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Loads Setup-Page in Display
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage(t_page setupPage)
{
	static unsigned char sMin=5;
	static unsigned char sSec=0;
	
	unsigned char *p_min;
	unsigned char *p_sec;

	p_min= &sMin;
	p_sec= &sSec;
	
	switch(setupPage)
	{
		case SetupPage:
				//-------------------------------------------Main-Page----
				setupPage= SetupMain;	
				sMin= 5;
				sSec= 0;
				LCD_SetupSet_Page();								break;	//Setup	
		
		case SetupMain:
				setupPage= LCD_SetupPage_Main(p_min, p_sec);		break;	//Main
					
		case SetupCirculate:	
				setupPage= LCD_SetupPage_Circulate(p_min, p_sec);	break;	//Circulate

		case SetupAir:			
				setupPage= LCD_SetupPage_Air(p_min, p_sec);			break;	//Air

		case SetupSetDown:		
				setupPage= LCD_SetupPage_SetDown(p_min, p_sec);		break;	//Set Down

		case SetupPumpOff:		
				setupPage= LCD_SetupPage_PumpOff(p_min, p_sec);		break;	//Pump Off

		case SetupMud:			
				setupPage= LCD_SetupPage_Mud(p_min, p_sec);			break;	//Mud
		
		case SetupCompressor:		
				setupPage= LCD_SetupPage_Compressor(p_min, p_sec);	break;	//Compressor
			
		case SetupPhosphor:		
				setupPage= LCD_SetupPage_Phosphor(p_min, p_sec);	break;	//Phosphor
		
		case SetupInflowPump:	
				setupPage= LCD_SetupPage_InflowPump(p_min, p_sec);	break; 	//inflowPump

		case SetupCal:	
				setupPage= LCD_SetupPage_Cal(p_min, p_sec);			break; 	//Cal

		case SetupCalPressure:
				setupPage= LCD_SetupPage_Cal(p_min, p_sec);			break; 	//Cal

		case SetupAlarm:	
				setupPage= LCD_SetupPage_Alarm(p_min, p_sec);		break; 	//Alarm

		case SetupWatch:	
				setupPage= LCD_SetupPage_Watch(p_min, p_sec);		break; 	//Watch

		case SetupZone:	
				setupPage= LCD_SetupPage_Zone(p_min, p_sec);		break; 	//Zone
		
		default:													break;
	}	

	return setupPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Circulate
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Main(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupMain;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	
	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupLinker(matrix, setPage);

	if(setPage!= SetupMain){	LCD_SetupSet(setPage, p_Min, p_Sec);		//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Circulate
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Circulate(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupCirculate;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupCirculateLinker(matrix, setPage);	
	
	if(setPage!= SetupCirculate){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
									return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;						
}


/* -------------------------------------------------------------------*
 * 						Setup Air
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Air(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupAir;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupAirLinker(matrix, setPage);

	if(setPage!= SetupAir){		LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup SetDown
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_SetDown(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupSetDown;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupSetDownLinker(matrix, setPage);
	
	if(setPage!= SetupSetDown){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup PumpOff
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_PumpOff(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupPumpOff;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupPumpOffLinker(matrix, setPage);
	
	if(setPage!= SetupPumpOff){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Mud
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Mud(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupMud;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupMudLinker(matrix, setPage);

	if(setPage!= SetupMud){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Compressor
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Compressor(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupCompressor;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupCompressorLinker(matrix, setPage);

	if(setPage!= SetupCompressor){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Phosphor
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Phosphor(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupPhosphor;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupPhosphorLinker(matrix, setPage);

	if(setPage!= SetupPhosphor){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup InflowPump
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_InflowPump(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupInflowPump;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupInflowPumpLinker(matrix, setPage);

	if(setPage!= SetupInflowPump){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Cal
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Cal(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= 		SetupCal;
	unsigned char 			matrix=0;
	unsigned char 			evalCD=0;
	static unsigned char 	iniTime=0;
	int						read=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();								//Wait 100ms
	if(iniTime)	setPage= SetupCalPressure;

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupCalLinker(matrix, setPage);	//TouchApp
	
	if(setPage== SetupCalPressure)				//Cal minimum Pressure
	{
		if(!iniTime){	iniTime=1;				//Reset JumpAuto-Time
						*p_Min= 4;
						*p_Sec= 60;
						PORT_Ventil_AllClose();
						OUT_Set_Air();}						//Open Air

		LCD_WriteValue2(17, 135, *p_Sec);					//Write Time

		if(!*p_Sec){	iniTime=0;							//--ReadPressure
						read= MPX_ReadWaterLevel(_new);		//Read WaterLevel
						LCD_WriteValue(17,40, read);		//Write Niveau Pressure	

						OUT_Clr_Air();						//Close Air	
						LCD_ClrSpace(17, 135, 2, 15);
						LCD_Write_Symbol_2(15,1, p_level);
						setPage= SetupCal;}					//Back to Normal
	}
	else if(setPage!= SetupCal){	iniTime=0;
									PORT_Ventil_AllClose();
									LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
									return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);			//CountDown
	if(evalCD){		iniTime=0;	
					return AutoPage;}				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Alarm
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Alarm(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupAlarm;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();									//Wait 100ms

	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupAlarmLinker(matrix, setPage);	//Touch Apps
	MCP9800_WriteTemp();

	if(setPage!= SetupAlarm){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Watch
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Watch(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupWatch;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms
	
	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupWatchLinker(matrix, setPage);

	if(setPage!= SetupWatch){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}


/* -------------------------------------------------------------------*
 * 						Setup Zone
 * -------------------------------------------------------------------*/

t_page LCD_SetupPage_Zone(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page setPage= SetupZone;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	Watchdog_Restart();
	TC_DisplaySetup_Wait();						//Wait 100ms
	
	matrix= 	Touch_Matrix();
	setPage=	Touch_SetupZoneLinker(matrix, setPage);

	if(setPage!= SetupZone){	LCD_SetupSet(setPage, p_Min, p_Sec);	//nextPage
								return setPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return setPage;
}






/* ===================================================================*
 * 						Data Pages
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_DataPage
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Loads Data-Page in Display
 * -------------------------------------------------------------------*/

t_page LCD_DataPage(t_page dataPage)
{
	static unsigned char dMin=5;
	static unsigned char dSec=0;
	
	unsigned char *p_min;
	unsigned char *p_sec;

	p_min= &dMin;
	p_sec= &dSec;
	
	switch(dataPage)
	{
		case DataPage:
				dataPage= DataMain;	
				dMin= 5;
				dSec= 0;
				LCD_DataSet_Page();								break;	//Setup	
		
		case DataMain:
				dataPage= LCD_DataPage_Main(p_min, p_sec);		break;	//Main

		case DataAuto:
				dataPage= LCD_DataPage_Auto(p_min, p_sec);		break;	//Auto

		case DataManual:
				dataPage= LCD_DataPage_Manual(p_min, p_sec);	break;	//Manual

		case DataSetup:
				dataPage= LCD_DataPage_Setup(p_min, p_sec);		break;	//Setup
		
		default:												break;				
	}	

	return dataPage;
}


/* -------------------------------------------------------------------*
 * 						Data Main
 * -------------------------------------------------------------------*/

t_page LCD_DataPage_Main(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page datPage= DataMain;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	
	Watchdog_Restart();
	TC_DisplayData_Wait();						//Wait 100ms

	matrix= 	Touch_Matrix();
	datPage=	Touch_DataLinker(matrix, datPage);

	if(datPage!= DataMain){	LCD_DataSet(datPage, p_Min, p_Sec);		//nextPage
							return datPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return datPage;
}


/* -------------------------------------------------------------------*
 * 						Data Auto
 * -------------------------------------------------------------------*/

t_page LCD_DataPage_Auto(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page datPage= DataAuto;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	
	Watchdog_Restart();
	TC_DisplayData_Wait();						//Wait 100ms
	
	matrix= 	Touch_Matrix();
	datPage=	Touch_DataAutoLinker(matrix, datPage);

	if(datPage!= DataAuto){	LCD_DataSet(datPage, p_Min, p_Sec);		//nextPage
							return datPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return datPage;
}


/* -------------------------------------------------------------------*
 * 						Data Manual
 * -------------------------------------------------------------------*/

t_page LCD_DataPage_Manual(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page datPage= DataManual;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	
	Watchdog_Restart();
	TC_DisplayData_Wait();						//Wait 100ms
	
	matrix= 	Touch_Matrix();
	datPage=	Touch_DataManualLinker(matrix, datPage);

	if(datPage!= DataManual){	LCD_DataSet(datPage, p_Min, p_Sec);		//nextPage
								return datPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return datPage;
}


/* -------------------------------------------------------------------*
 * 						Data Setup
 * -------------------------------------------------------------------*/

t_page LCD_DataPage_Setup(unsigned char *p_Min, unsigned char *p_Sec)
{
	t_page datPage= DataSetup;
	unsigned char matrix=0;
	unsigned char evalCD=0;
	
	Watchdog_Restart();
	TC_DisplayData_Wait();						//Wait 100ms
	
	matrix= 	Touch_Matrix();
	datPage=	Touch_DataSetupLinker(matrix, datPage);

	if(datPage!= DataSetup){	LCD_DataSet(datPage, p_Min, p_Sec);		//nextPage
								return datPage;}

	evalCD= Eval_CountDown(p_Min, p_Sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return datPage;
}








/* ===================================================================*
 * 						Pin Page
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_PinPage
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Pin Page for Setup Security
 * -------------------------------------------------------------------*/

t_page LCD_PinPage(t_page pinPage)
{
	t_page pPage= pinPage;
	unsigned char matrix=0;
	unsigned char evalCD=0;

	static unsigned char pinMin=5;
	static unsigned char pinSec=0;
	static unsigned char dec=0;

	unsigned char *p_min;
	unsigned char *p_sec;

	p_min= &pinMin;
	p_sec= &pinSec;
	
	if(!dec)
	{
		dec=1;
		LCD_PinSet_Page();
		pinMin= 5;
		pinSec= 0;
	}
	
	Watchdog_Restart();
	TC_DisplayManual_Wait();						//Wait 50ms
	
	matrix= 	Touch_Matrix();
	pinPage=	Touch_PinLinker(matrix, pinPage);
	
	if(pPage!= pinPage){	dec=0;
							return pinPage;}

	evalCD= Eval_CountDown(p_min, p_sec);		//CountDown
	if(evalCD)	return AutoPage;				//Back to AutoPage
	
	return pinPage;
}








/* ===================================================================*/
/* ===================================================================*
 * 						Built in Functions
 * ===================================================================*/
/* ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Auto Set
 * -------------------------------------------------------------------*/

void LCD_AutoSet(t_page aPage, int *p_Min, int *p_Sec)
{
	switch(aPage)
	{
		case AutoPage:		LCD_AutoSet_Page();	
							OUT_CloseOff();										break;	//Set Page		
		
		case AutoZone:		*p_Min= 1;
							*p_Sec= 0;
							LCD_AutoSet_Zone(*p_Min, *p_Sec);	
							OUT_Set_Air();										break;	//Zone

		case AutoSetDown:	*p_Min= MEM_EEPROM_ReadVar(TIME_setDown);
							*p_Sec= 0;
							LCD_AutoSet_SetDown(*p_Min, *p_Sec);		
							OUT_SetDown();										break;	//Set Down
			
		case AutoPumpOff:	*p_Min= MEM_EEPROM_ReadVar(ON_pumpOff);
							*p_Sec= 0;
							LCD_AutoSet_PumpOff(*p_Min, *p_Sec);	
							OUT_Set_PumpOff();									break;	//Pump Off

		case AutoMud:		*p_Min= MEM_EEPROM_ReadVar(ON_MIN_mud);
							*p_Sec= MEM_EEPROM_ReadVar(ON_SEC_mud);
							if((*p_Min==0) && (*p_Sec==0)){	*p_Sec= 1;			break;}	//don´t open Mud
							LCD_AutoSet_Mud(*p_Min, *p_Sec);	
							OUT_Set_Mud();										break;	//Mud
				
		case AutoCirculate:	*p_Min=((	MEM_EEPROM_ReadVar(TIME_H_circulate)<<8) |
										MEM_EEPROM_ReadVar(TIME_L_circulate));
							*p_Sec= 0;
							LCD_AutoSet_Circulate(*p_Min, *p_Sec);
							LCD_Write_AirVar(AutoCirculate, 0, _set);			break;	//Circulate
				
		case AutoAir:		*p_Min=((	MEM_EEPROM_ReadVar(TIME_H_air)<<8) |
										MEM_EEPROM_ReadVar(TIME_L_air));
							*p_Sec= 0;
							LCD_AutoSet_Air(*p_Min, *p_Sec);
							LCD_Write_AirVar(AutoAir, 0, _set);					break;	//Air

		default:																break;
	}
}

void LCD_AutoSet_Symbol(t_page aPage, int aMin, int aSec)
{
	LCD_AutoSet_Page();

	switch(aPage)
	{
		case AutoPage:												break;	//Set Page
		case AutoZone:			LCD_AutoSet_Zone(aMin, aSec);		break;	//Zone	
		case AutoSetDown:		LCD_AutoSet_SetDown(aMin, aSec);	break;	//Set Down
		case AutoPumpOff:		LCD_AutoSet_PumpOff(aMin, aSec);	break;	//Pump Off
		case AutoMud:			LCD_AutoSet_Mud(aMin, aSec);		break;	//Mud

		case AutoCirculate:		LCD_AutoSet_Circulate(aMin, aSec);	break;	//Circulate
		case AutoAir:			LCD_AutoSet_Air(aMin, aSec);		break;	//Air

		case AutoAirOff:		LCD_AutoSet_Air(aMin, aSec);		break;	//Air
		case AutoCirculateOff:	LCD_AutoSet_Circulate(aMin, aSec);	break;	//Circulate
		default:													break;
	}

}








/* ===================================================================*
 * 						Manual 
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Manual Set
 * -------------------------------------------------------------------*/

void LCD_ManualSet(t_page manPage, unsigned char *p_Min, unsigned char *p_Sec)
{
	LCD_Backlight(Backlight_Reset);
	switch(manPage)
	{
		case ManualMain:		*p_Min=5;	
								*p_Sec=0;			break;

		case ManualCirculate:	LCD_Write_AirVar(AutoCirculate, 0, _init);	
								LCD_Write_AirVar(AutoCirculate, 0, _outSet);
								*p_Min=60;	
								*p_Sec=0;			break;
		
		case ManualAir: 		OUT_Set_Air();
								*p_Min=60;	
								*p_Sec=0;			break;
		
		case ManualSetDown:		*p_Min=60;	
								*p_Sec=0;			break;

		case ManualPumpOff:		LCD_ClrSpace(15, 2, 3, 120);
								LCD_WriteStringFont(17,15,"PRESS OK!:");
								LCD_Write_Symbol_3(15, 85, p_ok);
								*p_Min=20;	
								*p_Sec=0;	
								LCD_WriteValue2(18,124, *p_Min);
								LCD_WriteValue2(18,142, *p_Sec);		break;

		case ManualMud:			OUT_Set_Mud();
								*p_Min=5;	
								*p_Sec=0;			break;

		case ManualCompressor:	OUT_Set_Compressor();
								*p_Min=0;	
								*p_Sec=15;			break;

		case ManualPhosphor:	OUT_Set_Phosphor();
								*p_Min=30;	
								*p_Sec=0;			break;	

		case ManualInflowPump:	OUT_Set_InflowPump();
								*p_Min=5;	
								*p_Sec=0;			break;
		default:									break;
	}
}








/* ===================================================================*
 * 						Setup
 * ===================================================================*/

void LCD_SetupSet(t_page setPage, unsigned char *p_Min, unsigned char *p_Sec)
{
	*p_Min=5;	
	*p_Sec=0;

	switch(setPage)
	{
		case SetupMain:			LCD_SetupSet_Page();		break;
		case SetupCirculate:	LCD_SetupSet_Circulate();	break;
		case SetupAir: 			LCD_SetupSet_Air();			break;
		case SetupSetDown:		LCD_SetupSet_SetDown();		break;
		case SetupPumpOff:		LCD_SetupSet_PumpOff();		break;
		case SetupMud:			LCD_SetupSet_Mud();			break;
		case SetupCompressor:	LCD_SetupSet_Compressor();	break;
		case SetupPhosphor:		LCD_SetupSet_Phosphor();	break;	
		case SetupInflowPump:	LCD_SetupSet_InflowPump();	break;

		case SetupCal:			LCD_SetupSet_Cal();			
								PORT_Ventil_AllOpen();		break;

		case SetupAlarm:		LCD_SetupSet_Alarm();		break;
		case SetupWatch:		LCD_SetupSet_Watch();		break;
		case SetupZone:			LCD_SetupSet_Zone();		break;
		default:											break;
	}
}








/* ===================================================================*
 * 						Data
 * ===================================================================*/

void LCD_DataSet(t_page setPage, unsigned char *p_Min, unsigned char *p_Sec)
{
	*p_Min=5;	
	*p_Sec=0;

	switch(setPage)
	{
		case DataMain:		LCD_DataSet_Page();		break;
		case DataAuto:		LCD_DataSet_Auto();		break;
		case DataManual: 	LCD_DataSet_Manual();	break;
		case DataSetup:		LCD_DataSet_Setup();	break;
		default:									break;
	}
}



/* -------------------------------------------------------------------*
 * 						Data Setup Entry Page
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*			
 * 	LCD_WriteAutoEntryPage
 * --------------------------------------------------------------
 * 	parameter:		t_eeDataPage page	-	page to write
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Whole Page to EEPROM
 * -------------------------------------------------------------------*/

void LCD_WriteAutoEntryPage(t_eeDataPage page)
{
	unsigned char entry=0;
	unsigned char eep=0;
	unsigned char *p_ct= Eval_Memory_LatestEntry(Auto);
	unsigned char i=0;

	eep= *p_ct;
	p_ct++;
	entry= *p_ct;
	
	switch(page)
	{
		case Page1:		LCD_ClrSpace(4,1,9,135);
						LCD_wPage(Auto, eep, entry);	break;

		case Page2:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,2);
						LCD_wPage(Auto, eep, entry);	break;
		
		case Page3:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,4);
						LCD_wPage(Auto, eep, entry);	break;

		case Page4:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,6);
						LCD_wPage(Auto, eep, entry);	break;

		case Page5:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,8);
						LCD_wPage(Auto, eep, entry);	break;

		case Page6:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,10);
						LCD_wPage(Auto, eep, entry);	break;
		
		case Page7:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,12);
						LCD_wPage(Auto, eep, entry);	break;

		case Page8:		LCD_ClrSpace(4,1,9,135);	eep= LCD_eep_minus(eep,14);
						for(i=0; i<4; i++)
						{								
							LCD_WriteAutoEntry(5+(2*i), eep, entry);
							if(entry<1) 
							{	
								entry=4;		eep--;
								if(eep < 2)		eep= 16;
							}
							entry--;
						}	
						LCD_WriteStringFont(15,1,"End");	break;
	
		default:		break;
	}
}





/* -------------------------------------------------------------------*
 * 						Data Manual Entry Page
 * -------------------------------------------------------------------*/

void LCD_WriteManualEntryPage(t_eeDataPage page)
{
	unsigned char entry=0;
	unsigned char eep=0;
	unsigned char *p_ct= Eval_Memory_LatestEntry(Manual);
	unsigned char i=0;

	eep= *p_ct;
	p_ct++;
	entry= *p_ct;
	
	switch(page)
	{
		case Page1:		LCD_ClrSpace(4,1,9,135);
						LCD_wPage(Manual, eep, entry);	break;

		case Page2:		LCD_ClrSpace(4,1,9,135);		eep= LCD_eep_minus(eep,2);
						for(i=0; i<4; i++)
						{							
							LCD_WriteManualEntry(5+(2*i), eep, entry);
							if(entry<1) 
							{	
								entry=4;		eep--;
								if(eep < 17)	eep= 20;
							}
							entry--;
						}	
						LCD_WriteStringFont(15,1,"End");	break;

		default:											break;
	}		
}




/* -------------------------------------------------------------------*
 * 						Data Setup Entry Page
 * -------------------------------------------------------------------*/

void LCD_WriteSetupEntryPage(t_eeDataPage page)
{
	unsigned char entry=0;
	unsigned char eep=0;
	unsigned char *p_ct= Eval_Memory_LatestEntry(Setup);
	unsigned char i=0;

	eep= *p_ct;
	p_ct++;
	entry= *p_ct;
	
	switch(page)
	{
		case Page1:		LCD_ClrSpace(4,1,9,135);
						LCD_wPage(Setup, eep, entry);			break;

		case Page2:		LCD_ClrSpace(4,1,9,135);	eep= LCD_eep_minus(eep,2);
						for(i=0; i<4; i++)
						{								
							LCD_WriteSetupEntry(5+(2*i), eep, entry);
							if(entry<1) 
							{	
								entry=4;		eep--;
								if(eep < 22)	eep= 25;
							}
							entry--;
						}	
						LCD_WriteStringFont(15,1,"End");	break;

		default:											break;
	}					
}





/* -------------------------------------------------------------------*
 * 						Data Built-in
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*			
 * 	LCD_wPage
 * --------------------------------------------------------------
 * 	parameter:		t_textSymbols data	-	Display Page Selection
 					unsigned char eep	-	EEPROM-Page 
 *					unsigned char entry	-	entry
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes One Entry line, call in entryPages
 * -------------------------------------------------------------------*/

void LCD_wPage(t_textSymbols data, unsigned char eep, unsigned char entry)
{
	unsigned char i=0;
	unsigned char startPa=0;
	unsigned char endPa=0;	

	switch(data)
	{
		case Auto:		startPa= 2;		endPa= 16;		break;			//8Pages
		case Manual:	startPa= 17; 	endPa= 20;		break;			//2Pages
		case Setup:		startPa= 22;	endPa= 25;		break;			//2Pages
		default:										break;
	}
	
	//-----------------------------------------------------------------Half-Page
	for(i=0; i<4; i++)							
	{	
		//-------------------------------------------Write-Entry-------
		switch(data)
		{
			case Auto:		LCD_WriteAutoEntry(5+(2*i), eep, entry);	break;			
			case Manual:	LCD_WriteManualEntry(5+(2*i), eep, entry);	break;
			case Setup:		LCD_WriteSetupEntry(5+(2*i), eep, entry);	break;
			default:													break;
		}
		//-------------------------------------------Update-Varibles---
		if(entry<1) 
		{	
			entry=4;			eep--;
			if(eep < startPa)	eep= endPa;
		}
		entry--;
	}	

	//-----------------------------------------------------------------Complete-Page
	for(i=4; i<8; i++)							
	{	
		//-------------------------------------------Write-Entry-------
		switch(data)
		{
			case Auto:		LCD_WriteAutoEntry(5+(2*i), eep, entry);	break;			
			case Manual:	LCD_WriteManualEntry(5+(2*i), eep, entry);	break;
			case Setup:		LCD_WriteSetupEntry(5+(2*i), eep, entry);	break;
			default:													break;
		}
		//-------------------------------------------Update-Varibles---
		if(entry<1) 
		{	
			entry=4;			eep--;
			if(eep < startPa)	eep= endPa;
		}
		entry--;
	}	
}


/* -------------------------------------------------------------------*
 * 						Minus
 * -------------------------------------------------------------------*/

unsigned char LCD_eep_minus(unsigned char eep, unsigned char cnt)
{
	unsigned char i=0;
	for(i=0; i<cnt; i++)						
	{	
		eep--;
		if(eep<2) eep=16;
	}
	return eep;
}






/* ===================================================================*/
/* ===================================================================*
 * 						FUNCTIONS Misc
 * ===================================================================*/
/* ===================================================================*/

/*------------------------------------------------------------
void LCD_Calibration(void)
{
	int xRead=0;
	int yRead=0;
	int x=0;
	int y=0;

	Touch_Cal();
	LCD_Clean(); 

	LCD_WriteStringFont(2, 0, "X-Cal:");
	LCD_WriteStringFont(5, 0, "Y-Cal:");
	LCD_WriteStringFont(10, 0, "X-Value:");
	LCD_WriteStringFont(13, 0, "Y-Value:");
	LCD_WriteStringFont(18, 0, "X-Value-Cal:");
	LCD_WriteStringFont(21, 0, "Y-Value-Cal:");
	
	LCD_WriteValue(2, 60, 	MEM_EEPROM_ReadVar(TOUCH_X_min));
	LCD_WriteValue(5, 60, 	MEM_EEPROM_ReadVar(TOUCH_Y_min));
	LCD_WriteValue(2, 100, 	MEM_EEPROM_ReadVar(TOUCH_X_max));
	LCD_WriteValue(5, 100, 	MEM_EEPROM_ReadVar(TOUCH_Y_max));

	while(1)
	{
		Watchdog_Restart();

		xRead= (Touch_X_ReadData()>>4);
		yRead= (Touch_Y_ReadData()>>4);
		y= Touch_Y_Cal(yRead);
		x= Touch_X_Cal(xRead);

		LCD_WriteValue(10, 100, xRead);
		LCD_WriteValue(13, 100, yRead);
		LCD_WriteValue(18, 100, x);
		LCD_WriteValue(21, 100, y);
	}
}
//------------------------------------------------------------*/





/**********************************************************************\
 * END of xmA_Display_app.c
\**********************************************************************/



