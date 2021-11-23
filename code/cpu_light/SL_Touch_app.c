/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Touchpanel-App-SourceFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Application-File for the Touchpanel of 
	the EADOGXL160-7 Display
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:											
\**********************************************************************/


#include<avr/io.h>

#include "SL_Define_sym.h"

#include "SL_Display_driver.h"
#include "SL_Touch_driver.h"

#include "SL_Display_app.h"
#include "SL_Touch_app.h"
#include "SL_Eval_app.h"
#include "SL_Output_app.h"
#include "SL_Memory_app.h"

#include "SL_DS1302_driver.h"
#include "SL_MPX_driver.h"
#include "SL_PORT_func.h"
#include "SL_Error_func.h"

#include "SL_Display_sym.h"



/* ===================================================================*
 * 						FUNCTIONS 	Matrix		
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	Touch_Matrix
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			unsigned char matrix	-	Touch Matrix
 * 	Descirption:	
 * --------------------------------------------------------------
 *	return 0xYX
 *	return Touch Matrix:	1.Row:	11, 12, 13 ,14
 *							2.Row:	21, 22, 23 ,24, usw.
 * -------------------------------------------------------------------*/

unsigned char Touch_Matrix(void)
{
	int xRead=0;
	int yRead=0;

	unsigned char x=0;
	unsigned char y=0;
	unsigned char lx=0;
	unsigned char hy=0;
	
	yRead= (Touch_Y_ReadData()>>4);
	xRead= (Touch_X_ReadData()>>4);
	
	y= Touch_Y_Cal(yRead);
	x= Touch_X_Cal(xRead);

	//xMatrix
	if		(x<16)				lx= 0;
	else if	(x>16 && x<35)		lx= 1;
	else if	(x>47 && x<70)		lx= 2;
	else if	(x>80 && x<105)		lx= 3;
	else if	(x>115 && x<145)	lx= 4;
	else						lx= 5;

	//yMatrix
	if		(y<16)				hy= 0;
	else if	(y>24 && y<37)		hy= 1;
	else if	(y>45 && y<58)		hy= 2;
	else if	(y>65 && y<77)		hy= 3;
	else if	(y>85 && y<105)		hy= 4;
	else						hy= 5;

	return ((hy<<4) | lx);
}



/* ===================================================================*
 * 						FUNCTIONS 	Linker
 * ===================================================================*/
/* ===================================================================*
 *	Linker request the touchmatrix and links to the selected page
 * ===================================================================*/



/* ===================================================================*
 * 						Auto Linker
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_AutoLinker
 * --------------------------------------------------------------
 * 	parameter:		unsigned char matrix	-	TouchMatrix()
 *					t_ventil ventil			-	Ventil to close
 *					t_page page				-	Saves Zyclus
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Links to Manual Pages
 * -------------------------------------------------------------------*/

t_page Touch_AutoLinker(unsigned char matrix, t_page page, int *p_min)
{	
	static unsigned char bug=0;
	static unsigned char bMark=0;

	switch(matrix)
	{
		//--------------------------------------------------------------BacklightON
		case 0x11: 	LCD_Backlight(Backlight_ON);	bug=0;	break;
		case 0x12: 	LCD_Backlight(Backlight_ON);	bug=0;	break;
		case 0x13:	LCD_Backlight(Backlight_ON);	bug=0;	break;
		case 0x14:	LCD_Backlight(Backlight_ON);	bug=0;	break;

		case 0x21:	if(!bMark){	bMark=1; 	
								if(bug== 0)			bug= 1;
								else if(bug== 1) 	bug= 2;
								else				bug= 0;
								LCD_Backlight(Backlight_ON);}	break;

		case 0x22:	LCD_Backlight(Backlight_ON);	bug=0;		break;
		case 0x23:	LCD_Backlight(Backlight_ON);	bug=0;		break;
		case 0x24:	if(!bMark){	bMark=1; 	
								if(bug== 2)			bug= 3;
								else if(bug== 3) 	bug= 4;
								else				bug= 0;
								LCD_Backlight(Backlight_ON);}	break;

		case 0x31:	LCD_Backlight(Backlight_ON);	bug=0;	break;
		case 0x32:	LCD_Backlight(Backlight_ON);	bug=0;	break;
		case 0x33:	LCD_Backlight(Backlight_ON);	bug=0;	break;
		case 0x34:	LCD_Backlight(Backlight_ON);	bug=0;	break;

		//--------------------------------------------------------------MainLinker
		case 0x41:	LCD_Backlight(Backlight_ON);	Error_OFF();
					if(bug== 4)	bug=5;
					else		bug=0;						break;	//Auto								

		case 0x42:	LCD_Backlight(Backlight_ON);	bug=0;
					LCD_MarkTextButton(Manual);
					PORT_Ventil_AutoClose(page);
					PORT_Relais_AllOff();
					Error_OFF();	
					return PinManual; 						break;	//Manual


		case 0x43:	LCD_Backlight(Backlight_ON);	bug=0;
					LCD_MarkTextButton(Setup);
					PORT_Ventil_AutoClose(page);
					PORT_Relais_AllOff();
					Error_OFF();	
					return PinSetup; 						break;	//Setup


		case 0x44:	LCD_Backlight(Backlight_ON);	bug=0;
					LCD_MarkTextButton(Data);
					PORT_Ventil_AutoClose(page);
					PORT_Relais_AllOff();
					Error_OFF();	
					return DataPage; 						break;	//Data
		
		case 0x00:	if(bMark)	bMark=0;					break;
		default:											break;
	}
	
	if(bug==5){	*p_min= 1;
				LCD_WriteValue_MyFont2(13,5, *p_min);}				//Bug

	return page;
}








/* ===================================================================*
 * 						Manual Linker
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_ManualLinker
 * --------------------------------------------------------------
 * 	parameter:		unsigned char matrix	-	TouchMatrix()
 *					t_ventil ventil			-	Ventil to close
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Links to Manual Pages
 * -------------------------------------------------------------------*/

t_page Touch_ManualLinker(unsigned char matrix, t_page page)
{
	static unsigned char mark1=0;

	switch(matrix)
	{
		case 0x11: 	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_circulate);	mark1=0;	
					return ManualCirculate;					break;		//Circulate

		case 0x12: 	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_air);			mark1=0;	
					return ManualAir;						break;		//Air

		case 0x13:	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_setDown);		mark1=0;	
					return ManualSetDown;					break;		//SetDown

		case 0x14:	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_pumpOff);		
					return ManualPumpOff;					break;		//PumpOff

		case 0x21:	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_mud);			mark1=0;
					return ManualMud;						break;		//Mud

		case 0x22:	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_compressor);	mark1=0;
					return ManualCompressor;				break;		//Comp

		case 0x23:	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_phosphor);		mark1=0;
					return ManualPhosphor;					break;		//Phosphor

		case 0x24:	LCD_Backlight(Backlight_ON);
					LCD_Mark_ManualSymbol(sn_inflowPump);	mark1=0;
					return ManualInflowPump;				break;		//IP
		

		case 0x33:	if(!mark1 && page== ManualPumpOff){		mark1=1;		
						LCD_Write_Symbol_3(15, 85, n_ok);		
						OUT_Set_PumpOff();
						LCD_ClrSpace(15, 2, 3, 120);
						LCD_WriteStringFont(18,136,":"); 
						return ManualPumpOff_On;}	break;	//PumpOff


		//--------------------------------------------------------------MainLinker
		case 0x41:	LCD_Backlight(Backlight_ON);
					LCD_MarkTextButton(Auto);		
					MEM_EEPROM_WriteManualEntry(0,0, _write);	mark1=0;
					return AutoPage; 							break;

		case 0x42:	LCD_Backlight(Backlight_ON);
					LCD_MarkTextButton(Manual);		
					MEM_EEPROM_WriteManualEntry(0,0, _write);	mark1=0;
					return ManualPage; 							break;

		case 0x43:	LCD_Backlight(Backlight_ON);
					LCD_MarkTextButton(Setup);		
					MEM_EEPROM_WriteManualEntry(0,0, _write);	mark1=0;
					return SetupPage; 							break;

		case 0x44:	LCD_Backlight(Backlight_ON);
					LCD_MarkTextButton(Data);		
					MEM_EEPROM_WriteManualEntry(0,0, _write);	mark1=0;
					return DataPage; 							break;

		default:												break;
	}
	return page;
}









/* ===================================================================*
 * 						Data Linker
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_DataLinker
 * --------------------------------------------------------------
 * 	parameter:		unsigned char matrix	-	TouchMatrix()
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Links to Setup Pages
 * -------------------------------------------------------------------*/

t_page Touch_DataLinker(unsigned char matrix, t_page page)
{
	switch(matrix)
	{
		case 0x14:												break;	//arrowUp
		case 0x34:												break;	//arrowDown

		case 0x21:	LCD_Write_TextButton(9, 0, Auto, 0); 
					return DataAuto;					break;	//Auto	

		case 0x22:	LCD_Write_TextButton(9, 40, Manual, 0);	
					return DataManual;					break;	//Manual

		case 0x23:	LCD_Write_TextButton(9, 80, Setup, 0);
					return DataSetup; 					break;	//Setup
		
		//-----------------------------------------------------------MainLinker
		case 0x41:	return AutoPage; 				break;
		case 0x42:	return PinManual; 				break;
		case 0x43:	return PinSetup; 				break;
		case 0x44:	return DataPage; 				break;

		default:									break;
	}
	return page;
}



/* -------------------------------------------------------------------*
 * 						Data Auto Linker
 * -------------------------------------------------------------------*/

t_page Touch_DataAutoLinker(unsigned char matrix, t_page pa)
{
	static unsigned char mark2=0;
	static unsigned char page=0;	

	switch(matrix)
	{
		case 0x14:	if(!mark2){	mark2= 1;	if(page)	page--;	
											else 		page=page;
								LCD_Write_Symbol_3(	3,	140, n_arrowUp);}	break;	//arrowUp

		case 0x34:	if(!mark2){	mark2= 2;	if(page<7) 	page++;
											else		page=page;
								LCD_Write_Symbol_3(	14,	140, n_arrowDown);}	break;	//arrowDown

		case 0x00:	if(mark2==1){mark2= 0;	
								LCD_Write_Symbol_3(	3,	140, p_arrowUp);
								LCD_WriteMyFont(1,144,page+1);
								LCD_WriteAutoEntryPage(page);}

					if(mark2==2){mark2= 0;	
								LCD_Write_Symbol_3(	14,	140, p_arrowDown);
								LCD_WriteMyFont(1,144,page+1);
								LCD_WriteAutoEntryPage(page);}				break;	//No Touch
													

		
		//-----------------------------------------------------------MainLinker
		case 0x41:	page=0;	return AutoPage; 		break;
		case 0x42:	page=0;	return PinManual; 		break;
		case 0x43:	page=0;	return PinSetup; 		break;
		case 0x44:	page=0;	return DataPage; 		break;

		default:									break;
	}
	return pa;
}



/* -------------------------------------------------------------------*
 * 						Data Manual Linker
 * -------------------------------------------------------------------*/

t_page Touch_DataManualLinker(unsigned char matrix, t_page pa)
{
	static unsigned char mark3=0;
	static unsigned char page2=0;	

	switch(matrix)
	{
		case 0x14:	if(!mark3){	mark3= 1;	if(page2)	page2--;	
											else 		page2=page2;
								LCD_Write_Symbol_3(	3,	140, n_arrowUp);}	break;	//arrowUp

		case 0x34:	if(!mark3){	mark3= 2;	if(page2<1) page2++;
											else		page2=page2;
								LCD_Write_Symbol_3(	14,	140, n_arrowDown);}	break;	//arrowDown

		case 0x00:	if(mark3==1){mark3= 0;	
								LCD_Write_Symbol_3(	3,	140, p_arrowUp);
								LCD_WriteMyFont(1,144,page2+1);
								LCD_WriteManualEntryPage(page2);}

					if(mark3==2){mark3= 0;	
								LCD_Write_Symbol_3(	14,	140, p_arrowDown);
								LCD_WriteMyFont(1,144,page2+1);
								LCD_WriteManualEntryPage(page2);}			break;	//No Touch
													

		
		//-----------------------------------------------------------MainLinker
		case 0x41:	page2=0;	return AutoPage; 		break;
		case 0x42:	page2=0;	return PinManual; 		break;
		case 0x43:	page2=0;	return PinSetup; 		break;
		case 0x44:	page2=0;	return DataPage; 		break;

		default:										break;
	}
	return pa;
}



/* -------------------------------------------------------------------*
 * 						Data Setup Linker
 * -------------------------------------------------------------------*/

t_page Touch_DataSetupLinker(unsigned char matrix, t_page pa)
{
	static unsigned char mark4=0;
	static unsigned char page3=0;	

	switch(matrix)
	{
		case 0x14:	if(!mark4){	mark4= 1;	if(page3)	page3--;	
											else 		page3=page3;
								LCD_Write_Symbol_3(	3,	140, n_arrowUp);}	break;	//arrowUp

		case 0x34:	if(!mark4){	mark4= 2;	if(page3<1) page3++;
											else		page3=page3;
								LCD_Write_Symbol_3(	14,	140, n_arrowDown);}	break;	//arrowDown

		case 0x00:	if(mark4==1){mark4= 0;	
								LCD_Write_Symbol_3(	3,	140, p_arrowUp);
								LCD_WriteMyFont(1,144,page3+1);
								LCD_WriteSetupEntryPage(page3);}

					if(mark4==2){mark4= 0;	
								LCD_Write_Symbol_3(	14,	140, p_arrowDown);
								LCD_WriteMyFont(1,144,page3+1);
								LCD_WriteSetupEntryPage(page3);}				break;	//No Touch
													

		
		//-----------------------------------------------------------MainLinker
		case 0x41:	page3=0;	return AutoPage; 		break;
		case 0x42:	page3=0;	return PinManual; 		break;
		case 0x43:	page3=0;	return PinSetup; 		break;
		case 0x44:	page3=0;	return DataPage; 		break;

		default:										break;
	}
	return pa;
}








/* ===================================================================*
 * 						Setup Linker
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_SetupLinker
 * --------------------------------------------------------------
 * 	parameter:		unsigned char matrix	-	TouchMatrix()
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Links to Setup Pages
 * -------------------------------------------------------------------*/

t_page Touch_SetupLinker(unsigned char matrix, t_page page)
{
	switch(matrix)
	{
		case 0x11: 	return SetupCirculate;			break;
		case 0x12: 	return SetupAir;				break;
		case 0x13:	return SetupSetDown;			break;
		case 0x14:	return SetupPumpOff;			break;

		case 0x21:	return SetupMud;				break;
		case 0x22:	return SetupCompressor;			break;
		case 0x23:	return SetupPhosphor;			break;
		case 0x24:	return SetupInflowPump;			break;

		case 0x31:	return SetupCal;				break;
		case 0x32:	return SetupAlarm;				break;
		case 0x33:	return SetupWatch;				break;
		case 0x34:	return SetupZone;				break;	//Zone
		
		//-----------------------------------------------------------MainLinker
		case 0x41:	return AutoPage; 				break;
		case 0x42:	return ManualPage; 				break;
		case 0x43:	return SetupPage; 				break;
		case 0x44:	return DataPage; 				break;

		default:									break;
	}
	return page;
}




/* -------------------------------------------------------------------*
 * 						Setup Circulate Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupCirculateLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	circ[4]= {0};
	static unsigned char 	sensor1=0;
	
	static int				time1=0;
	static unsigned char 	on1=0;
	static unsigned char 	decVar1=0;
	static unsigned char 	mark6=0;
	
	unsigned char 			*p_circ= circ;

	if(!decVar1)
	{
		decVar1=1;
		on1=0;
		circ[0]=	MEM_EEPROM_ReadVar(ON_circulate);
		circ[1]=	MEM_EEPROM_ReadVar(OFF_circulate);
		circ[2]=	MEM_EEPROM_ReadVar(TIME_L_circulate);
		circ[3]=	MEM_EEPROM_ReadVar(TIME_H_circulate);	
		sensor1=	MEM_EEPROM_ReadVar(SENSOR_inTank);
		
		time1= 		((circ[3]<<8) | circ[2]);

		if(sensor1)	Eval_SetupCircSensorMark(1);
		else		Eval_SetupCircSensorMark(0);

		Eval_SetupCircTextMark(on1, p_circ);
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark6){	mark6=6;		LCD_ControlButtons(sn_esc);}			
					decVar1=0;	return SetupPage;							break;	//Esc

		case 0x14:	if(!mark6){	mark6=7;		LCD_ControlButtons(sn_ok);}				
					decVar1=0;	MEM_EEPROM_WriteVar(ON_circulate, circ[0]);	
								MEM_EEPROM_WriteVar(OFF_circulate, circ[1]);
								MEM_EEPROM_WriteVar(TIME_L_circulate, circ[2]);
								MEM_EEPROM_WriteVar(TIME_H_circulate, circ[3]);
								MEM_EEPROM_WriteVar(SENSOR_inTank, sensor1);

								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;							break;	//Ok
								
		case 0x23:	if(!mark6){	mark6=5;		LCD_ControlButtons(sn_minus);}			
					switch(on1)
					{
						case 0:	circ[0]= 	Eval_SetupMinus(circ[0], 2);	break;	//Circ On Min
						case 1:	circ[1]= 	Eval_SetupMinus(circ[1], 5);	break;	//Circ On Max
						case 2:	time1=		Eval_SetupMinus2(time1, 0);		break;	
						default:											break;
					}														break;	//Minus

		case 0x24:	if(!mark6){	mark6=4;		LCD_ControlButtons(sn_plus);}
					switch(on1)
					{
						case 0:	circ[0]= 	Eval_SetupPlus(circ[0], 30);	break;	//Circ On Max
						case 1:	circ[1]= 	Eval_SetupPlus(circ[1], 60);	break;	//Circ Off Max
						case 2:	time1=		Eval_SetupPlus2(time1, 999);	break;	
						default:											break;
					}														break;	//Plus
					
		case 0x21:	if(!mark6){on1=0;	mark6=5;	Eval_SetupCircTextMark(on1, p_circ); }	break;	//On							
		case 0x22:	if(!mark6){on1=1;	mark6=5;	Eval_SetupCircTextMark(on1, p_circ); }	break;	//Off

		case 0x31:	if(!mark6){	mark6=5;		sensor1=1;
								Eval_SetupCircSensorMark(1);	}		break;	//Sensor
		
		case 0x32:	if(!mark6){	mark6=5;		sensor1=0;	on1=2;
								Eval_SetupCircSensorMark(0);
								Eval_SetupCircTextMark(on1, p_circ);}	break;	//Time

		case 0x00:	if(mark6){	circ[2]=	(time1 & 0x00FF);			
								circ[3]=	((time1>>8) & 0x00FF);
								LCD_ControlButtons(mark6);	mark6=0;}	break;	//No Touch
		
		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar1=0;	return AutoPage; 	break;
		case 0x42:	decVar1=0;	return ManualPage; 	break;
		case 0x43:	decVar1=0;	return SetupPage; 	break;
		case 0x44:	decVar1=0;	return DataPage; 	break;

		default:									break;
	}
	
	if(mark6)
	{
		switch (on1)
		{
			case 0:		LCD_OnValueNeg(circ[0]); 				break;
			case 1:		LCD_OffValueNeg(circ[1]); 				break;
			case 2:		LCD_WriteValueNeg(16,72, time1); 		break;
			default: 											break;
		}
	}
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Air Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupAirLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	air[4]= {0};
	
	static int				time2=0;
	static unsigned char 	on2=0;
	static unsigned char 	decVar2=0;
	static unsigned char 	mark7=0;

	unsigned char 			*p_air= air;
	
	if(!decVar2)
	{
		decVar2=1;
		on2=0;
		air[0]=		MEM_EEPROM_ReadVar(ON_air);
		air[1]=		MEM_EEPROM_ReadVar(OFF_air);
		air[2]=		MEM_EEPROM_ReadVar(TIME_L_air);
		air[3]=		MEM_EEPROM_ReadVar(TIME_H_air);	
		
		time2= 		((air[3]<<8) | air[2]);
		
		Eval_SetupAirTextMark(on2, p_air);
		LCD_ClrSpace	(15, 39, 3, 31); 
		LCD_WriteStringFont(16,40,"Time:");
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark7){	mark7=6;	LCD_ControlButtons(sn_esc);}			
					decVar2=0;	return SetupPage;							break;	//Esc

		case 0x14:	if(!mark7){	mark7=7;	LCD_ControlButtons(sn_ok);}				
					decVar2=0;	MEM_EEPROM_WriteVar(ON_air, air[0]);	
								MEM_EEPROM_WriteVar(OFF_air, air[1]);
								MEM_EEPROM_WriteVar(TIME_L_air, air[2]);
								MEM_EEPROM_WriteVar(TIME_H_air, air[3]);

								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;							break;	//Ok
								
		case 0x23:	if(!mark7){	mark7=5;		LCD_ControlButtons(sn_minus);}			
					switch(on2)
					{
						case 0:	air[0]= 	Eval_SetupMinus(air[0], 10);	break;	//Air On  Min
						case 1:	air[1]= 	Eval_SetupMinus(air[1], 5);		break;	//Air Off Min
						case 2:	time2=		Eval_SetupMinus2(time2, 0);		break;	//Time Min
						default:											break;
					}														break;	//Minus

		case 0x24:	if(!mark7){	mark7=4;		LCD_ControlButtons(sn_plus);}
					switch(on2)
					{
						case 0:	air[0]= 	Eval_SetupPlus(air[0], 60);		break;	//Air On  Max
						case 1:	air[1]= 	Eval_SetupPlus(air[1], 55);		break;	//Air Off Max
						case 2:	time2=		Eval_SetupPlus2(time2, 999);	break;	//Time Max
						default:											break;
					}														break;	//Plus


		case 0x21:	if(!mark7){on2=0;	mark7=5;	Eval_SetupAirTextMark(on2, p_air); }	break;	//On							
		case 0x22:	if(!mark7){on2=1;	mark7=5;	Eval_SetupAirTextMark(on2, p_air); }	break;	//Off
		
		case 0x32:	if(!mark7){	mark7=5;	on2=2;		
								Eval_SetupAirTextMark(on2, p_air);}		
								LCD_FillSpace	(15, 39, 4, 31);
								LCD_WriteStringFontNeg(16,40,"Time:");		break;	//Time

		case 0x00:	if(mark7){	air[2]=	(time2 & 0x00FF);			
								air[3]=	((time2>>8) & 0x00FF);
								LCD_ControlButtons(mark7);	mark7=0;}		break;	//No Touch

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar2=0;	return AutoPage; 	break;
		case 0x42:	decVar2=0;	return ManualPage; 	break;
		case 0x43:	decVar2=0;	return SetupPage; 	break;
		case 0x44:	decVar2=0;	return DataPage; 	break;
		default:									break;
	}

	if(mark7)
	{
		switch (on2)
		{
			case 0:		LCD_OnValueNeg(air[0]); 				break;
			case 1:		LCD_OffValueNeg(air[1]); 				break;
			case 2:		LCD_WriteValueNeg(16,72, time2); 		break;
			default: 											break;
		}
	}
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Set-Down Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupSetDownLinker(unsigned char matrix, t_page page)
{
	static unsigned char setDown=0;

	static unsigned char decVar3=0;
	static unsigned char mark8=0;
	
	if(!decVar3)
	{
		decVar3=1;
		setDown=		MEM_EEPROM_ReadVar(TIME_setDown);
		LCD_WriteValueNeg2(10,30, setDown);
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark8){	mark8=6;		LCD_ControlButtons(sn_esc);}			
					decVar3=0;	return SetupPage;							break;	//Esc

		case 0x14:	if(!mark8){	mark8=7;		LCD_ControlButtons(sn_ok);}				
					decVar3=0;	MEM_EEPROM_WriteVar(TIME_setDown, setDown);	
								
								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;							break;	//Ok

		case 0x23:	if(!mark8){	mark8=5;		LCD_ControlButtons(sn_minus);}		//Mark Minus
					setDown= 	Eval_SetupMinus(setDown, 50);				break;	//Minus

		case 0x24:	if(!mark8){	mark8=4;		LCD_ControlButtons(sn_plus);}		//Mark Plus
					setDown= 	Eval_SetupPlus(setDown, 90);				break;	//Plus
								

		case 0x00:	if(mark8){	LCD_ControlButtons(mark8);		mark8=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar3=0;	return AutoPage; 	break;
		case 0x42:	decVar3=0;	return ManualPage; 	break;
		case 0x43:	decVar3=0;	return SetupPage; 	break;
		case 0x44:	decVar3=0;	return DataPage; 	break;
		default:									break;		
	}

	if(mark8){	LCD_WriteValueNeg2(10,30, setDown); 	}	//TimeConfig
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Pump-Off Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupPumpOffLinker(unsigned char matrix, t_page page)
{
	static unsigned char pumpOn=0;
	static unsigned char pump=0;

	static unsigned char decVar4=0;
	static unsigned char mark9=0;
	
	if(!decVar4)
	{
		decVar4=1;
		pumpOn=		MEM_EEPROM_ReadVar(ON_pumpOff);
		pump=		MEM_EEPROM_ReadVar(PUMP_pumpOff);
					
		LCD_OnValueNeg(pumpOn);

		if(!pump){	LCD_Write_Symbol_2(15, 0, n_compressor);
					LCD_Write_Symbol_3(14, 50, p_pump);}

		else{		LCD_Write_Symbol_2(15, 0, p_compressor);
					LCD_Write_Symbol_3(14, 50, n_pump);}
	}
	
	switch(matrix)
	{
		
		case 0x13:	if(!mark9){	mark9=6;		LCD_ControlButtons(sn_esc);}			
					decVar4=0;	return SetupPage;							break;	//Esc

		case 0x14:	if(!mark9){	mark9=7;		LCD_ControlButtons(sn_ok);}				
					decVar4=0;	MEM_EEPROM_WriteVar(ON_pumpOff, pumpOn);
								MEM_EEPROM_WriteVar(PUMP_pumpOff, pump);	
								
								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;							break;	//Ok

		case 0x23:	if(!mark9){	mark9=5;	LCD_ControlButtons(sn_minus);}			
					if(mark9){	pumpOn= 	Eval_SetupMinus(pumpOn, 5);}	break;	//Minus
		
		case 0x24:	if(!mark9){	mark9=4;	LCD_ControlButtons(sn_plus);}			
					if(mark9){	pumpOn= 	Eval_SetupPlus(pumpOn, 60);}	break;	//Plus
		
		case 0x31:	if(!mark9){	mark9=5;		pump=0;	
								LCD_Write_Symbol_2(15, 0, n_compressor);		
								LCD_Write_Symbol_3(14, 50, p_pump);} 		break;	//Mamutpumpe
									
		
		case 0x32:	if(!mark9){	mark9=5;		pump=1;
								LCD_Write_Symbol_2(15, 0, p_compressor);		
								LCD_Write_Symbol_3(14, 50, n_pump);} 		break;	//elekt. Pumpe
								

		case 0x00:	if(mark9){	LCD_ControlButtons(mark9);		mark9=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar4=0;	return AutoPage; 	break;
		case 0x42:	decVar4=0;	return ManualPage; 	break;
		case 0x43:	decVar4=0;	return SetupPage; 	break;
		case 0x44:	decVar4=0;	return DataPage; 	break;
		default:									break;
	}

	if(mark9){	LCD_OnValueNeg(pumpOn); 	}										//Output
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Mud Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupMudLinker(unsigned char matrix, t_page page)
{
	static unsigned char mudMin=0;
	static unsigned char mudSec=0;
	static unsigned char onM=0;
	static unsigned char decVar5=0;
	static unsigned char mark10=0;
	
	if(!decVar5)
	{
		decVar5=1;
		mudMin=		MEM_EEPROM_ReadVar(ON_MIN_mud);
		mudSec=		MEM_EEPROM_ReadVar(ON_SEC_mud);
		LCD_WriteStringFont(11,7,"ON:"); 
		if(!onM){	LCD_WriteValue2(11,40, mudMin);
					LCD_WriteValueNeg2(16,40, mudSec);}
		else{		LCD_WriteValueNeg2(11,40, mudMin);
					LCD_WriteValue2(16,40, mudSec);}
		LCD_WriteStringFont(11,55,"min");
		LCD_WriteStringFont(16,55,"sec"); 
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark10){	mark10=6;		LCD_ControlButtons(sn_esc);}			
					decVar5=0;	return SetupPage;										break;	//Esc

		case 0x14:	if(!mark10){	mark10=7;		LCD_ControlButtons(sn_ok);}				
					decVar5=0;	MEM_EEPROM_WriteVar(ON_MIN_mud, mudMin);
								MEM_EEPROM_WriteVar(ON_SEC_mud, mudSec);
								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;										break;	//Ok

		case 0x23:	if(!mark10){	mark10=5;	LCD_ControlButtons(sn_minus);}			
					if(onM)			mudMin= Eval_SetupMinus(mudMin, 0);					
					else			mudSec= Eval_SetupMinus(mudSec, 0);					break;	//Minus

		case 0x24:	if(!mark10){	LCD_ControlButtons(sn_plus);	mark10=4;}			
					if(onM)			mudMin= Eval_SetupPlus(mudMin, 20);					
					else			mudSec= Eval_SetupPlus(mudSec, 59);					break;	//Plus
	
		case 0x22:	if(!mark10){onM=1;	LCD_WriteValue2(16,40, mudSec);	mark10=5;}		break;	//Min
		case 0x32:	if(!mark10){onM=0;	LCD_WriteValue2(11,40, mudMin);	mark10=5;}		break;	//Sec

		case 0x00:	if(mark10){	LCD_ControlButtons(mark10);		mark10=0;}	break;
		
		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar5=0;	return AutoPage; 	break;
		case 0x42:	decVar5=0;	return ManualPage; 	break;
		case 0x43:	decVar5=0;	return SetupPage; 	break;
		case 0x44:	decVar5=0;	return DataPage; 	break;
		default:									break;
	}

	if(onM  && mark10){	LCD_WriteValueNeg2(11,40, mudMin);	}	//Min
	if(!onM && mark10){	LCD_WriteValueNeg2(16,40, mudSec);	}	//Max
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Compressor Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupCompressorLinker(unsigned char matrix, t_page page)
{
	static int druckMin=0;
	static int druckMax=0;
	static unsigned char on3=0;
	static unsigned char decVar10=0;
	static unsigned char mark11=0;

	unsigned char h=0;
	unsigned char l=0;
	
	if(!decVar10)
	{
		decVar10=1;
		on3=0;
		h=			MEM_EEPROM_ReadVar(MAX_H_druck);
		l=			MEM_EEPROM_ReadVar(MAX_L_druck);
		druckMax=	((h<<8) | l);

		h=			MEM_EEPROM_ReadVar(MIN_H_druck);
		l=			MEM_EEPROM_ReadVar(MIN_L_druck);
		druckMin=	((h<<8) | l);

		if(on3)	{	LCD_WriteValue(16,7, druckMax);		LCD_WriteValueNeg(11,7, druckMin);	}	//Min
		if(!on3)	{	LCD_WriteValue(11,7, druckMin);	LCD_WriteValueNeg(16,7, druckMax);	}	//Max
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark11){	mark11=6;		LCD_ControlButtons(sn_esc);}			
					decVar10=0;		return SetupPage;								break;	//Esc

		case 0x14:	if(!mark11){	mark11=7;		LCD_ControlButtons(sn_ok);}				
					decVar10=0;	l=(druckMax & 0x00FF);
								h= ((druckMax>>8) & 0x00FF);	
								MEM_EEPROM_WriteVar(MAX_H_druck, h);	
								MEM_EEPROM_WriteVar(MAX_L_druck, l);

								l=(druckMin & 0x00FF);
								h= ((druckMin>>8) & 0x00FF);	
								MEM_EEPROM_WriteVar(MIN_H_druck, h);	
								MEM_EEPROM_WriteVar(MIN_L_druck, l);

								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;										break;	//Ok
								
		case 0x23:	if(!mark11){	mark11=5;			LCD_ControlButtons(sn_minus);}			
					if(on3)		druckMin= 	Eval_SetupMinus2(druckMin, 0);			
					else		druckMax= 	Eval_SetupMinus2(druckMax, 0);				break;	//Minus

		case 0x24:	if(!mark11){	mark11=4;		LCD_ControlButtons(sn_plus);}			
					if(on3)		druckMin= 	Eval_SetupPlus2(druckMin, 999);
					else		druckMax= 	Eval_SetupPlus2(druckMax, 999);				break;	//Plus
	

		case 0x21:	if(!mark11){on3=1;		LCD_WriteValue(16,7, druckMax);	mark11=5;}	break;	//Min
		case 0x31:	if(!mark11){on3=0;		LCD_WriteValue(11,7, druckMin);	mark11=5;}	break;	//Max

		case 0x00:	if(mark11){	LCD_ControlButtons(mark11);		mark11=0;}				break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar10=0;	return AutoPage; 	break;
		case 0x42:	decVar10=0;	return ManualPage; 	break;
		case 0x43:	decVar10=0;	return SetupPage; 	break;
		case 0x44:	decVar10=0;	return DataPage; 	break;
		default:									break;
	}

	if(on3  && mark11){	LCD_WriteValueNeg(11,7, druckMin);	}	//Min
	if(!on3 && mark11){	LCD_WriteValueNeg(16,7, druckMax);	}	//Max
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Phosphor Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupPhosphorLinker(unsigned char matrix, t_page page)
{
	static unsigned char pOn=10;
	static unsigned char pOff=10;
	static unsigned char on4=0;
	static unsigned char decVar6=0;
	static unsigned char mark12=0;
	
	if(!decVar6)
	{
		decVar6=1;
		pOff=		MEM_EEPROM_ReadVar(OFF_phosphor);
		pOn=		MEM_EEPROM_ReadVar(ON_phosphor);
		if(on4)	{	LCD_OnValueNeg(pOn);	LCD_OffValue(pOff);		}	//OnConfig
		if(!on4)	{	LCD_OnValue(pOn);	LCD_OffValueNeg(pOff);	}	//OffConfig
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark12){	mark12=6;		LCD_ControlButtons(sn_esc);}			
					decVar6=0;		return SetupPage;							break;	//Esc

		case 0x14:	if(!mark12){	mark12=7;		LCD_ControlButtons(sn_ok);}				
					decVar6=0;		MEM_EEPROM_WriteVar(ON_phosphor, pOn);	
									MEM_EEPROM_WriteVar(OFF_phosphor, pOff);
								
									MEM_EEPROM_WriteSetupEntry();
									LCD_Auto_Phosphor(0, _init);
									return SetupPage;							break;	//Ok
								
		case 0x23:	if(!mark12){	mark12=5;		LCD_ControlButtons(sn_minus);}	
					if(on4)			pOn= 	Eval_SetupMinus(pOn, 0);				
					else			pOff= 	Eval_SetupMinus(pOff, 0);			break;	//Minus

		case 0x24:	if(!mark12){	mark12=4;		LCD_ControlButtons(sn_plus);}			
					if(on4)			pOn= 	Eval_SetupPlus(pOn, 60);
					else			pOff= 	Eval_SetupPlus(pOff, 60);			break;	//Plus
		

		case 0x21:	if(!mark12){on4=1;		LCD_OffValue(pOff);	mark12=5;}		break;	//On							
		case 0x22:	if(!mark12){on4=0;		LCD_OnValue(pOn);	mark12=5;}		break;	//Off

		case 0x00:	if(mark12){	LCD_ControlButtons(mark12);		mark12=0;}		break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar6=0;	return AutoPage; 	break;
		case 0x42:	decVar6=0;	return ManualPage; 	break;
		case 0x43:	decVar6=0;	return SetupPage; 	break;
		case 0x44:	decVar6=0;	return DataPage; 	break;
		default:									break;
	}

	if(on4  && mark12){	LCD_OnValueNeg(pOn);	}	//OnConfig
	if(!on4 && mark12){	LCD_OffValueNeg(pOff);	}	//OffConfig
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Inflow-Pump Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupInflowPumpLinker(unsigned char matrix, t_page page)
{
	static unsigned char inflowPumpOn=0;
	static unsigned char inflowPumpOff=0;
	static unsigned char iPump=0;
	static unsigned char sensor3=0;

	static unsigned char on5=0;
	static unsigned char decVar7=0;
	static unsigned char mark13=0;
	
	if(!decVar7)
	{
		decVar7=1;
		inflowPumpOff=		MEM_EEPROM_ReadVar(OFF_inflowPump);
		inflowPumpOn=		MEM_EEPROM_ReadVar(ON_inflowPump);
		iPump=				MEM_EEPROM_ReadVar(PUMP_inflowPump);
		sensor3=			MEM_EEPROM_ReadVar(SENSOR_outTank);

		Eval_SetupPumpMark(iPump);
		if(sensor3)	LCD_Write_Symbol_2(15, 0, n_sensor);
		else		LCD_Write_Symbol_2(15, 0, p_sensor);
		if(on5)	{	LCD_OnValueNeg(inflowPumpOn);	LCD_OffValue(inflowPumpOff);	}	//OnConfig
		if(!on5){	LCD_OnValue(inflowPumpOn);		LCD_OffValueNeg(inflowPumpOff);	}	//OffConfig
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark13){	mark13=6;		LCD_ControlButtons(sn_esc);}			
					decVar7=0;		return SetupPage;								break;	//Esc

		case 0x14:	if(!mark13){	mark13=7;		LCD_ControlButtons(sn_ok);}				
					decVar7=0;	MEM_EEPROM_WriteVar(ON_inflowPump, inflowPumpOn);	
								MEM_EEPROM_WriteVar(OFF_inflowPump, inflowPumpOff);
								MEM_EEPROM_WriteVar(PUMP_inflowPump, iPump);
								MEM_EEPROM_WriteVar(SENSOR_outTank, sensor3);
								
								MEM_EEPROM_WriteSetupEntry();
								LCD_Auto_InflowPump(0, _init);
								return SetupPage;									break;	//Ok
								
		case 0x23:	if(!mark13){	mark13=5;	LCD_ControlButtons(sn_minus);}			
					if(on5)		inflowPumpOn= 	Eval_SetupMinus(inflowPumpOn, 0);			
					else		inflowPumpOff= 	Eval_SetupMinus(inflowPumpOff, 0);	break;	//Minus

		case 0x24:	if(!mark13){	mark13=4;		LCD_ControlButtons(sn_plus);}			
					if(on5)		inflowPumpOn= 	Eval_SetupPlus(inflowPumpOn, 60);
					else		inflowPumpOff= 	Eval_SetupPlus(inflowPumpOff, 60);	break;	//Plus
	

		case 0x21:	if(!mark13){on5=1;		LCD_OffValue(inflowPumpOff);	mark13=5;}	break;	//On							
		case 0x22:	if(!mark13){on5=0;		LCD_OnValue(inflowPumpOn);		mark13=5;}	break;	//Off

		case 0x31:	if(sensor3 && !mark13){	mark13=5;
					LCD_Write_Symbol_2(15, 0, p_sensor);	sensor3=0;}
					if(!sensor3 && !mark13){	mark13=5;	
					LCD_Write_Symbol_2(15, 0, n_sensor);	sensor3=1;}				break;	//Sensor
														
		case 0x32:	iPump=0;	Eval_SetupPumpMark(0);								break;	//Mamutpumpe
		case 0x33:	iPump=1;	Eval_SetupPumpMark(1);								break;	//elek. Pump
		case 0x34:	iPump=2;	Eval_SetupPumpMark(2);								break;	//2 elek. Pumps

		case 0x00:	if(mark13){	LCD_ControlButtons(mark13);		mark13=0;}			break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar7=0;	return AutoPage; 	break;
		case 0x42:	decVar7=0;	return ManualPage; 	break;
		case 0x43:	decVar7=0;	return SetupPage; 	break;
		case 0x44:	decVar7=0;	return DataPage; 	break;
		default:									break;
	}

	if(on5  && mark13){	LCD_OnValueNeg(inflowPumpOn);		}	//OnConfig
	if(!on5 && mark13){	LCD_OffValueNeg(inflowPumpOff);		}	//OffConfig
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Cal Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupCalLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	mark14=0;
	unsigned char 			write=0;
	int 					read=0;
	static int				cal=0;	
	static unsigned char	iniCal=0;
	
	if(!iniCal)
	{
		iniCal=1;
		cal=((MEM_EEPROM_ReadVar(CAL_H_druck)<<8)	|
			 (MEM_EEPROM_ReadVar(CAL_L_druck)));
	}	
	
	read= MPX_ReadAverage_UnCal();
	if(read!= 0xFF00)
	{	
		read= read-cal;
		if(read<0){	read= -read; 
					LCD_WriteStringFont(10,34, "-");}	//-Pressure
		else 		LCD_ClrSpace(10,34,1,5);			//+Pressure			
		LCD_WriteValue(10,40,read);						//Write Pressure
	}

	switch(matrix)
	{
		case 0x13:	if(!mark14){	mark14=6;	iniCal=0;	
						LCD_ControlButtons(sn_esc);
						if(page==SetupCalPressure)	OUT_Clr_Air();
						return SetupPage;}								break;	//Esc

		case 0x14:	if(!mark14 && page==SetupCal){	mark14=7;	iniCal=0;		
						LCD_ControlButtons(sn_ok);				
						MEM_EEPROM_WriteSetupEntry();
						write= (cal & 0x00FF);
						MEM_EEPROM_WriteVar(CAL_L_druck, write);
						write= ((cal>>8) & 0x00FF);
						MEM_EEPROM_WriteVar(CAL_H_druck, write);

						MPX_ReadWaterLevel(_save);						
						return SetupPage;}								break;	//Ok
								
		case 0x24:	if(!mark14 && page==SetupCal){	mark14=4;		
						LCD_Write_Symbol_2(9,125, n_cal);
						cal= MPX_ReadAverage_UnCal_Value();}			break;	//Cal
		
		case 0x31:	LCD_Write_Symbol_2(15,1, n_level);
					page= SetupCalPressure;								break;

		case 0x00:	if(mark14){	LCD_ControlButtons(mark14);	mark14=0;
								LCD_Write_Symbol_2(9,125, p_cal);}		break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	iniCal=0;			return AutoPage; 	break;
		case 0x42:	iniCal=0;			return ManualPage; 	break;
		case 0x43:	if(page==SetupCal){	iniCal=0;
										return SetupPage;} 	break;
		case 0x44:	if(page==SetupCal){	iniCal=0;	
										return DataPage;} 	break;
		default:											break;
	}
	return page;
}





/* -------------------------------------------------------------------*
 * 						Setup Alarm Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupAlarmLinker(unsigned char matrix, t_page page)
{
	static unsigned char sensor4=0;
	static unsigned char comp=0;
	static unsigned char temp=0;

	static unsigned char decVar8=0;
	static unsigned char mark15=0;
	
	if(!decVar8)
	{
		decVar8=1;
		sensor4=	MEM_EEPROM_ReadVar(ALARM_sensor);
		comp=		MEM_EEPROM_ReadVar(ALARM_comp);
		temp=		MEM_EEPROM_ReadVar(ALARM_temp);

		LCD_WriteValueNeg2(10,15, temp);

		if(comp)	LCD_Write_Symbol_2(15, 40, n_compressor);
		else		LCD_Write_Symbol_2(15, 40, p_compressor);
		if(sensor4)	LCD_Write_Symbol_2(15, 0, n_sensor);
		else		LCD_Write_Symbol_2(15, 0, p_sensor);	
	}
	
	switch(matrix)
	{
		
		case 0x13:	if(!mark15){	mark15=6;		LCD_ControlButtons(sn_esc);}			
					decVar8=0;		return SetupPage;								break;	//Esc

		case 0x14:	if(!mark15){	mark15=7;		LCD_ControlButtons(sn_ok);}				
							decVar8=0;	
							MEM_EEPROM_WriteVar(ALARM_comp, comp);
							MEM_EEPROM_WriteVar(ALARM_sensor, sensor4);
							MEM_EEPROM_WriteVar(ALARM_temp, temp);	
																
							MEM_EEPROM_WriteSetupEntry();
							return SetupPage;										break;	//Ok

		case 0x23:	if(!mark15){	mark15=5;	LCD_ControlButtons(sn_minus);}			
					if(mark15){		temp= 	Eval_SetupMinus(temp, 15);}				break;	//Minus
		
		case 0x24:	if(!mark15){	LCD_ControlButtons(sn_plus);	mark15=4;}			
					if(mark15){		temp= 	Eval_SetupPlus(temp, 99);}				break;	//Plus
		
		case 0x31:	if(!mark15)
					{	mark15=5;		
						if(sensor4){sensor4=0;	
									LCD_Write_Symbol_2(15, 0, p_sensor);}
						else{		sensor4=1;
									LCD_Write_Symbol_2(15, 0, n_sensor);}
					}																break;	//Sensor
									
		case 0x32:	if(!mark15)
					{	mark15=5;		
						if(comp){	comp=0;	
									LCD_Write_Symbol_2(15, 40, p_compressor);}
						else{		comp=1;
									LCD_Write_Symbol_2(15, 40, n_compressor);}
					}																break;	//Compressor
								

		case 0x00:	if(mark15){	LCD_ControlButtons(mark15);		mark15=0;}			break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar8=0;	return AutoPage; 	break;
		case 0x42:	decVar8=0;	return ManualPage; 	break;
		case 0x43:	decVar8=0;	return SetupPage; 	break;
		case 0x44:	decVar8=0;	return DataPage; 	break;
		default:									break;
	}

	if(mark15){	LCD_WriteValueNeg2(10,15, temp); 	}										//Output
	return page;
}
	




/* -------------------------------------------------------------------*
 * 						Setup Watch Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupWatchLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	dT[5]={0};
	unsigned char 			*p_dT;

	static unsigned char	on6=0;			
	static unsigned char 	decVar9=0;
	static unsigned char 	mark16=0;

	unsigned char 			time=0;
	
	p_dT= dT;

	if(!decVar9)
	{
		decVar9=1;
		
		time= DS1302_ReadByte(R_HOUR);
		dT[0]= (((time>>4) & 0x0F)*10 + (time & 0x0F));
		
		time= DS1302_ReadByte(R_MIN);
		dT[1]= (((time>>4) & 0x0F)*10 + (time & 0x0F));

		time= DS1302_ReadByte(R_DATE);
		dT[2]= (((time>>4) & 0x0F)*10 + (time & 0x0F));

		time= DS1302_ReadByte(R_MONTH);
		dT[3]= (((time>>4) & 0x0F)*10 + (time & 0x0F));

		time= DS1302_ReadByte(R_YEAR);
		dT[4]= (((time>>4) & 0x0F)*10 + (time & 0x0F));

		Eval_SetupWatchMark(n_h, p_dT);
		on6=0;
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark16){	mark16=6;		LCD_ControlButtons2(sn_esc);}			
					decVar9=0;		return SetupPage;						break;	//Esc

		case 0x14:	if(!mark16){	mark16=7;		LCD_ControlButtons2(sn_ok);}				
					decVar9=0;	time= (((dT[0]/10)<<4) | (dT[0]-(dT[0]/10)*10));
								DS1302_WriteByte(W_HOUR,	time);

								time= (((dT[1]/10)<<4) | (dT[1]-(dT[1]/10)*10));
								DS1302_WriteByte(W_MIN,  	time);

								time= (((dT[2]/10)<<4) | (dT[2]-(dT[2]/10)*10));
								DS1302_WriteByte(W_DATE, 	time);

								time= (((dT[3]/10)<<4) | (dT[3]-(dT[3]/10)*10));
								DS1302_WriteByte(W_MONTH, 	time);

								time= (((dT[4]/10)<<4) | (dT[4]-(dT[4]/10)*10));
								DS1302_WriteByte(W_YEAR, 	time);
													
								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;							break;	//Ok
		
		case 0x24:	if(!mark16){	mark16=4;		LCD_ControlButtons2(sn_plus);}
					switch(on6)
					{
						case 0:	dT[0]=	Eval_SetupPlus(dT[0], 23);			break;		
						case 1:	dT[1]= 	Eval_SetupPlus(dT[1], 59);			break;	
						case 2:	dT[2]= 	Eval_SetupPlus(dT[2], 31);			break;
						case 3:	dT[3]= 	Eval_SetupPlus(dT[3], 12);			break;
						case 4:	dT[4]= 	Eval_SetupPlus(dT[4], 99);			break;
						default:											break;
					}				
																			break;	//Plus
		
		case 0x34:	if(!mark16){	mark16=5;		LCD_ControlButtons2(sn_minus);}			
					switch(on6)
					{
						case 0:	dT[0]=	Eval_SetupMinus(dT[0], 0);			break;		
						case 1:	dT[1]= 	Eval_SetupMinus(dT[1], 0);			break;	
						case 2:	dT[2]= 	Eval_SetupMinus(dT[2], 1);			break;
						case 3:	dT[3]= 	Eval_SetupMinus(dT[3], 1);			break;
						case 4:	dT[4]= 	Eval_SetupMinus(dT[4], 0);			break;
						default:											break;
					}														break;		//Minus
		
		case 0x21:	if(!mark16){	Eval_SetupWatchMark(n_h, p_dT); 
									on6=0;	mark16=5;}						break;	//hours	
																	
		case 0x22:	if(!mark16){	Eval_SetupWatchMark(n_min, p_dT); 
									on6=1;	mark16=5;}						break;	//Minutes
		
		case 0x31:	if(!mark16){	Eval_SetupWatchMark(n_day, p_dT); 
									on6=2;	mark16=5;}						break;	//days

		case 0x32:	if(!mark16){	Eval_SetupWatchMark(n_month, p_dT); 
									on6=3;	mark16=5;}						break;	//Month

		case 0x33:	if(!mark16){	Eval_SetupWatchMark(n_year, p_dT); 
									on6=4;	mark16=5;}						break;	//year

		case 0x00:	if(mark16){	LCD_ControlButtons2(mark16);	mark16=0;}	break;

		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar9=0;	return AutoPage; 	break;
		case 0x42:	decVar9=0;	return ManualPage; 	break;
		case 0x43:	decVar9=0;	return SetupPage; 	break;
		case 0x44:	decVar9=0;	return DataPage; 	break;
		default:									break;
	}
	
	if(mark16)												//Write Date Time
	{
		switch(on6)												
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



/* -------------------------------------------------------------------*
 * 						Setup Zone Linker
 * -------------------------------------------------------------------*/

t_page Touch_SetupZoneLinker(unsigned char matrix, t_page page)
{
	static int lvO2=0;
	static int lvCirc=0;

	static unsigned char onM=0;
	static unsigned char decVar5=0;
	static unsigned char mark10=0;
	
	unsigned char h=0;

	if(!decVar5)
	{
		decVar5=1;
		lvO2=(	(MEM_EEPROM_ReadVar(TANK_H_O2)<<8)		|
				(MEM_EEPROM_ReadVar(TANK_L_O2)));	
		lvCirc=((MEM_EEPROM_ReadVar(TANK_H_Circ)<<8)	|
				(MEM_EEPROM_ReadVar(TANK_L_Circ)));

		if(!onM){	LCD_WriteValue(11,40, lvCirc);
					LCD_WriteValueNeg(16,40, lvO2);
					LCD_Write_Symbol_2(9, 0, p_air);
					LCD_Write_Symbol_2(14, 0, n_setDown);}
		else{		LCD_WriteValueNeg(11,40, lvCirc);
					LCD_WriteValue(16,40, lvO2);
					LCD_Write_Symbol_2(9, 0, n_air);
					LCD_Write_Symbol_2(14, 0, p_setDown);}

		LCD_WriteStringFont(11,60,"cm");
		LCD_WriteStringFont(16,60,"cm"); 
	}
	
	switch(matrix)
	{
		case 0x13:	if(!mark10){	mark10=6;		LCD_ControlButtons(sn_esc);}			
					decVar5=0;		return SetupPage;									break;	//Esc

		case 0x14:	if(!mark10){	mark10=7;		LCD_ControlButtons(sn_ok);}				
					decVar5=0;	h= (lvO2 & 0x00FF);
								MEM_EEPROM_WriteVar(TANK_L_O2, h);
								h= ((lvO2>>8)&0x00FF);
								MEM_EEPROM_WriteVar(TANK_H_O2, h);
								h= (lvCirc & 0x00FF);
								MEM_EEPROM_WriteVar(TANK_L_Circ, h);
								h= ((lvCirc>>8)&0x00FF);
								MEM_EEPROM_WriteVar(TANK_H_Circ, h);
								MEM_EEPROM_WriteSetupEntry();
								return SetupPage;								break;	//Ok

		case 0x23:	if(!mark10){	mark10=5;	LCD_ControlButtons(sn_minus);}			
					if(onM)			lvCirc= Eval_SetupMinus2(lvCirc, 0);						
					else			lvO2= 	Eval_SetupMinus2(lvO2, lvCirc);		break;	//Minus

		case 0x24:	if(!mark10){	LCD_ControlButtons(sn_plus);	mark10=4;}			
					if(onM)			lvCirc= Eval_SetupPlus2(lvCirc, lvO2);	
					else			lvO2= 	Eval_SetupPlus2(lvO2, 	999);		break;	//Plus
		
		case 0x21:
		case 0x22:	if(!mark10){onM=1;		mark10=5;
								LCD_WriteValue(16,40, lvO2);
								LCD_Write_Symbol_2(9, 0, n_air);
								LCD_Write_Symbol_2(14, 0, p_setDown);}		break;	//Circulate
		
		case 0x31:
		case 0x32:	if(!mark10){onM=0;		mark10=5;
								LCD_WriteValue(11,40, lvCirc);
								LCD_Write_Symbol_2(9, 0, p_air);
								LCD_Write_Symbol_2(14, 0, n_setDown);}		break;	//O2

		case 0x00:	if(mark10){	LCD_ControlButtons(mark10);		mark10=0;}		break;
		
		//-----------------------------------------------------------MainLinker
		case 0x41:	decVar5=0;	return AutoPage; 	break;
		case 0x42:	decVar5=0;	return ManualPage; 	break;
		case 0x43:	decVar5=0;	return SetupPage; 	break;
		case 0x44:	decVar5=0;	return DataPage; 	break;
		default:									break;
	}

	if(onM  && mark10){	LCD_WriteValueNeg(11,40, lvCirc);	}	//O2
	if(!onM && mark10){	LCD_WriteValueNeg(16,40, lvO2);}	//Circulate
	return page;
}


/* ===================================================================*
 * 						Pin Linker
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Touch_PinLinker
 * --------------------------------------------------------------
 * 	parameter:		unsigned char matrix	-	TouchMatrix()
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Links to Setup Pages, execute maintenance Features
 * -------------------------------------------------------------------*/

t_page Touch_PinLinker(unsigned char matrix, t_page page)
{
	static unsigned char 	touch[11]={0x00}; 		//Touched Buttons
	static unsigned char	in[4]={0x00};			//inputCode
	unsigned char			secret[4]={2,5,8,0};	//secret
	unsigned char			compH[4]={1,5,9,3};		//set Comp-Hours= 0

	unsigned char			*p_touch;
	unsigned char			i=0;
	static unsigned char	cp=0;				//CodePosition

	p_touch= touch;

	switch(matrix)
	{
		case 0x11: 	if(!touch[1]){
						Eval_PinWrite(1, cp);	touch[1]= 0x11;	in[cp]= 1;	cp++;}	break;

		case 0x12: 	if(!touch[2]){
						Eval_PinWrite(2, cp);	touch[2]= 0x12;	in[cp]= 2;	cp++;}	break;

		case 0x13:	if(!touch[3]){
						Eval_PinWrite(3, cp);	touch[3]= 0x13;	in[cp]= 3; 	cp++;}	break;

		case 0x21:	if(!touch[4]){
						Eval_PinWrite(4, cp);	touch[4]= 0x14;	in[cp]= 4; 	cp++;}	break;

		case 0x22:	if(!touch[5]){
						Eval_PinWrite(5, cp);	touch[5]= 0x15;	in[cp]= 5; 	cp++;}	break;

		case 0x23:	if(!touch[6]){
						Eval_PinWrite(6, cp);	touch[6]= 0x16;	in[cp]= 6; 	cp++;}	break;

		case 0x31:	if(!touch[7]){
						Eval_PinWrite(7, cp);	touch[7]= 0x17;	in[cp]= 7;	cp++;}	break;

		case 0x32:	if(!touch[8]){
						Eval_PinWrite(8, cp);	touch[8]= 0x18;	in[cp]= 8;	cp++;}	break;

		case 0x33:	if(!touch[9]){
						Eval_PinWrite(9, cp);	touch[9]= 0x19;	in[cp]= 9;	cp++;}	break;

		case 0x42:	if(!touch[0]){
						Eval_PinWrite(0, cp);	touch[0]= 0x10;	in[cp]= 0;	cp++;}	break;

		case 0x43:	Eval_PinDel();				touch[10]= 0x20; cp=0; 				break;	//Del				
		case 0x41:	LCD_nPinButtons(11);		cp=0;			 return AutoPage;	break;	//Esc

		case 0x00:	Eval_PinClr(p_touch);								
					for(i=0; i<10; i++){		touch[i]= 0;}	break;	//No Touch
		default:												break;
	}
		

	if(cp>3)
	{
		cp=0;
		if(	(in[0]== secret[0]) && (in[1]== secret[1]) &&		//Check Secret
			(in[2]== secret[2]) && (in[3]== secret[3])){		//Check Secret
				LCD_WriteStringFont(6,125,"Right!");  			//Right Code
				switch(page)
				{
					case PinManual:	return ManualPage;	break;
					case PinSetup:	return SetupPage;	break;
					default:							break;
				}}
	
		if(	(in[0]== compH[0]) && (in[1]== compH[1]) &&			//Check Secret
			(in[2]== compH[2]) && (in[3]== compH[3])){			//Check Secret
				DS1302_Write_Comp_OpHours(0);					//Reset CompH
				LCD_WriteStringFont(14,125,"worked"); }			//write changed
							
		Eval_PinDel();
		LCD_WriteStringFont(6,125,"wrong!");  
		LCD_WriteStringFont(9,125,"try"); 
		LCD_WriteStringFont(11,125,"again"); 
		return page;

	}
	return page;
}




/**********************************************************************\
 * END of xmA_Touch_app.c
\**********************************************************************/
