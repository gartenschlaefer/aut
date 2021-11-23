/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			SL_Error_func.c
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Error Detection and Treatment Source-File
* ------------------------------------------------------------------														
*	Date:			24.10.2011  	
* 	lastChanges:										
\**********************************************************************/


#include "SL_Define_sym.h"

#include "SL_MCP9800_driver.h"
#include "SL_MPX_driver.h"
#include "SL_Display_driver.h"
#include "SL_Display_sym.h"

#include "SL_Display_app.h"
#include "SL_Output_app.h"
#include "SL_Memory_app.h"

#include "SL_Error_func.h"
#include "SL_TC_func.h"
#include "SL_PORT_func.h"



/* ===================================================================*
 * 						Error ON/OFF
 * ===================================================================*/

/*--------------------------------------------------------------------*		
 * 	PORT_ErrorOn
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Sets Error Signals
 * -------------------------------------------------------------------*/

void Error_ON(void)
{
	PORT_Buzzer(_on);
	PORT_RelaisSet(R_ALARM);
	LCD_Backlight(Backlight_ErrorOn);
}

void Error_OFF(void)
{
	PORT_Buzzer(_off);
	PORT_RelaisClr(R_ALARM);
	LCD_Backlight(Backlight_ErrorOff);
}



/* ===================================================================*
 * 						Error Detection
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Error
 * --------------------------------------------------------------
 * 	parameter:		t_Error errCmd	-	Error Command
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Error_Detect:	Detects error and writes to entry
 *	Error_Clear:	Clears all existing ErrorCode, 
 *					call after Write Entry!
 * -------------------------------------------------------------------*/

t_page Error_Detection(t_page errPage, int min, int sec)
{
	static unsigned char	errOut=0;
	static unsigned char	occur=0;
	unsigned char 			error=0;
	
	static int time0=0;
	static int time1=0;
	static int time2=0;
	static int time3=0;
	static int time4=0;

	int *p_time;
	
	error=	Error_ReadError();

	p_time= &time0;
	errOut= Error_Timing(p_time, 0, error, errOut);			//Temp
	p_time= &time1;
	errOut= Error_Timing(p_time, 1, error, errOut);			//Over-Pressure
	p_time= &time2;
	if(errPage!= AutoSetDown){	
		errOut= Error_Timing(p_time, 2, error, errOut);}	//Under-Pressure
	p_time= &time3;
	errOut= Error_Timing(p_time, 3, error, errOut);			//Max-in-Tank
	p_time= &time4;
	errOut= Error_Timing(p_time, 4, error, errOut);			//Max-out-Tank	
	
	if(errOut)												//--Error Ocur
	{ 
		occur=1;
		Error_Treatment(errOut, errPage);					//Error Treatment
		if(((errPage==AutoAir) 		|| 
			(errPage==AutoAirOff))	&& (errOut & 0x01)){	//Temp Error
				return AutoSetDown;}						//-ToSetDown
	}

	if(!error && occur)										//--Error Clean
	{
		occur= 0;	 
		Error_OFF();										//Error Clean
		Error_Action_OP(0, _clear);							//Clear OP-Counter				
		LCD_AutoSet_Symbol(errPage, min, sec);				//Clear Display	
	}
	
	PORT_Buzzer(_exe);										//ErrorBuzzer
	errOut=0;		
	return errPage;											
}



/* ---------------------------------------------------------------*
 * 					Read Error
 * ---------------------------------------------------------------*/

unsigned char Error_ReadError(void)
{
	unsigned char error=0;
	unsigned char read=0;
	int druck=0;
	
	read= MEM_EEPROM_ReadVar(ALARM_temp);
	if(MCP9800_PlusTemp() > read)			error |= (1<<0);			//Temp
	else									error &= ~(1<<0);
	
	druck=((MEM_EEPROM_ReadVar(MAX_H_druck)<<8) | 
		   (MEM_EEPROM_ReadVar(MAX_L_druck)));

	if(MPX_ReadCal() > druck) 	error |= (1<<1);						//over-pressure
	else						error &= ~(1<<1);

	druck= ((MEM_EEPROM_ReadVar(MIN_H_druck)<<8) | 
			(MEM_EEPROM_ReadVar(MIN_L_druck)));

	if(MPX_ReadCal() < druck)	error |= (1<<2);						//under-pressure
	else						error &= ~(1<<2);
	
	if((MPX_ReadTank(AutoAir, _error)==ErrorMPX))	error |= (1<<3);	//max in Tank
	else											error &= ~(1<<3);

	if(	PORT_FloatSwitch(Float_outTank)== Float_Max)error |= (1<<4);	//max out Tank
	else											error &= ~(1<<4);

	return error;
}



/* ---------------------------------------------------------------*
 * 					Error Timing
 * ---------------------------------------------------------------*/

unsigned char Error_Timing(	int 			*p_time, 
							unsigned char 	errPos, 
							unsigned char 	error, 
							unsigned char 	errOut		)
{
	int	time=0;
	time= *p_time;							//Read at Pointer

	if(error & (1<<errPos)) time++;			//Error detected
	else {	time=0;
			errOut &= ~(1<<errPos);}		

	if(time>180){	errOut |= (1<<errPos);	//Error Time and write 13s
					time=0;}
	
	*p_time= time;							//Write at Pointer
	return errOut;
}



/* ---------------------------------------------------------------*
 * 					Error Treatment
 * ---------------------------------------------------------------*/

void Error_Treatment(unsigned char err, t_page errPage)
{
	switch(errPage)
	{
		case AutoSetDown:				
			if(err & (1<<1)){	Error_Action(E_OverPressure_NO);}		//OverPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank
			
		case AutoPumpOff:	
			if(err & (1<<1)){	Error_Action(E_OverPressure_PO);}		//OverPressure		
			if(err & (1<<2)){	Error_Action(E_UnderPressure);}			//UnderPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank

		case AutoMud:
			if(err & (1<<1)){	Error_Action(E_OverPressure_Mud);}		//OverPressure	
			if(err & (1<<2)){	Error_Action(E_UnderPressure);}			//UnderPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank

		case AutoAirOff:
			if(err & (1<<1)){	Error_Action(E_OverPressure_IP);}		//OverPressure
			if(err & (1<<2)){	Error_Action(E_UnderPressure_IP);}		//UnderPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank

		case AutoCirculateOff:
			if(err & (1<<1)){	Error_Action(E_OverPressure_IP);}		//OverPressure
			if(err & (1<<2)){	Error_Action(E_UnderPressure_IP);}		//UnderPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank

		case AutoCirculate:
			if(err & (1<<1)){	Error_Action(E_OverPressure_Air);}		//OverPressure
			if(err & (1<<2)){	Error_Action(E_UnderPressure);}			//UnderPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank

		case AutoAir:
			if(err & (1<<0)){	Error_Action(E_Temp);}					//Temp
			if(err & (1<<1)){	Error_Action(E_OverPressure_Air);}		//OverPressure
			if(err & (1<<2)){	Error_Action(E_UnderPressure);}			//UnderPressure
			if(err & (1<<3)){	Error_Action(E_MaxInTank);}				//Max in Tank
			if(err & (1<<4)){	Error_Action(E_MaxOutTank);} 	break;	//Max out Tank

		default:												break;
	}	
}




/* ===================================================================*
 * 						Error Symbols
 * ===================================================================*/

void Error_Symbol(t_ErrorSym errSym)
{
	switch(errSym)
	{
		case E_Temp:			LCD_Write_Symbol_3(16, 134, n_grad);break;	//Temp
		
		case E_OverPressure_IP:
		case E_OverPressure_PO:
		case E_OverPressure_Air:
		case E_OverPressure_Mud:
		case E_OverPressure_NO:	LCD_ClrSpace(6, 44, 3, 35);						
								LCD_Write_Symbol_2(6, 45, n_alarm);	break;	//OverPressure
		
		case E_UnderPressure_IP:
		case E_UnderPressure:	LCD_ClrSpace(6, 44, 3, 35);					
								LCD_Write_Symbol_2(6, 45, n_alarm);	break;	//UnderPressure

		case E_MaxInTank:		LCD_Write_Symbol_2(17, 1, n_alarm);
								LCD_TextButton(Auto, 0); 			break;	//MaxInTank

		case E_MaxOutTank:		LCD_Write_Symbol_2(16, 90, n_alarm);break;	//MaxOutTank
		default:													break;
	}
}
	



/* ===================================================================*
 * 						Error Action
 * ===================================================================*/

void Error_Action(t_ErrorSym errSym)
{
	unsigned char 			alComp=0;

	alComp= 	MEM_EEPROM_ReadVar(ALARM_comp);			//Read AlarmComp ON?

	switch(errSym)
	{
		case E_Temp:			Error_ON();	
								Error_Symbol(E_Temp);
								MEM_EEPROM_WriteAutoEntry(0, 1, Write_Error);	//Write Error	
								break;											//Temp
		
		case E_OverPressure_NO: Error_Symbol(E_OverPressure_NO);
								OUT_Clr_Compressor();
								PORT_Ventil(OPEN_Air);
								PORT_Ventil(CLOSE_Air);
								Error_Action_OP(alComp, _exe);			//AlarmAction
								break;									//NO-OverPressure

		case E_OverPressure_IP:	Error_Symbol(E_OverPressure_IP);
								if(LCD_Auto_InflowPump(0, _state)==_on){
									PORT_Ventil(CLOSE_Reserve);
									PORT_Ventil(OPEN_Reserve);}
								if(LCD_Auto_InflowPump(0, _state)==_off){
									PORT_Ventil(OPEN_Reserve);
									PORT_Ventil(CLOSE_Reserve);}	
								Error_Action_OP(alComp, _exe);			//AlarmAction
								break;									//IP-OverPressure

		case E_OverPressure_PO:	Error_Symbol(E_OverPressure_PO);			
								OUT_Clr_PumpOff();	
								OUT_Set_PumpOff();					
								Error_Action_OP(alComp, _exe);			//AlarmAction
								break;									//PO-OverPressure
		
		case E_OverPressure_Air:Error_Symbol(E_OverPressure_Air);
								OUT_Clr_Air();	
								OUT_Set_Air();						
								Error_Action_OP(alComp, _exe);			//AlarmAction
								break;									//Air-OverPressure	
									
		case E_OverPressure_Mud:Error_Symbol(E_OverPressure_Mud);
								OUT_Clr_Mud();	
								OUT_Set_Mud();						
								Error_Action_OP(alComp, _exe);			//AlarmAction
								break;									//Mud-OverPressure

		case E_UnderPressure:	Error_Symbol(E_UnderPressure);	
								if(alComp)	Error_ON();	
								MEM_EEPROM_WriteAutoEntry(0, 4, Write_Error);	//Write Error
								break;											//Under Pressure
		
		case E_UnderPressure_IP:if((LCD_Auto_InflowPump(0, _state)==_on) &&
								(!MEM_EEPROM_ReadVar(PUMP_inflowPump))){
									Error_Symbol(E_UnderPressure);
									if(alComp)	Error_ON();
									MEM_EEPROM_WriteAutoEntry(0, 4, Write_Error);}		
								break;											//Under Pressure

		case E_MaxInTank:		Error_Symbol(E_MaxInTank);
								if(MEM_EEPROM_ReadVar(ALARM_sensor)) Error_ON();	
								MEM_EEPROM_WriteAutoEntry(0, 8, Write_Error);	//WriteError
								break;											//MaxInTank

		case E_MaxOutTank:		Error_Symbol(E_MaxOutTank);
								if(MEM_EEPROM_ReadVar(ALARM_sensor)) Error_ON();
								MEM_EEPROM_WriteAutoEntry(0, 16, Write_Error);	//WriteError
								break;											//MaxOutTank
		default:				break;
	}
}


/* -------------------------------------------------------------------*
 * 						Error Action OverPressure
 * -------------------------------------------------------------------*/

void Error_Action_OP(unsigned char alComp, t_FuncCmd cmd)
{
	static unsigned char	eCount=0;

	if(cmd==_exe)
	{
		eCount++;
		if(alComp && (eCount>2))	Error_ON();				//ErrorSignal-ON
		if(eCount>2){				
			MEM_EEPROM_WriteAutoEntry(0, 2, Write_Error);}	//Write Error
	}
	else if(cmd==_clear)
	{
		eCount=0;
	}
}






/**********************************************************************\
 * END of SL_Error_func.c
\**********************************************************************/





