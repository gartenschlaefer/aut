/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			PORT+Interrupt-Initialisierung-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	PORT Funtkions for SL (Steuerung light)
* ------------------------------------------------------------------														
*	Date:			27.05.2011  	
* 	lastChanges:	07.11.2011										
\**********************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>

#include "SL_Define_sym.h"
#include "SL_Display_driver.h"
#include "SL_MCP9800_driver.h"
#include "SL_Memory_app.h"
#include "SL_Output_app.h"

#include "SL_TC_func.h"
#include "SL_PORT_func.h"
#include "SL_Watchdog_func.h"





/* ===================================================================*
 * 						FUNCTIONS Init
 * ===================================================================*/

void PORT_Init(void)
{
	//PORT Direction
	P_OPTO.DIR = 	0x00;			//PORTH Opto Input
	P_VENTIL.DIR = 	0xFF;			//PORTJ Ventil Output
	P_RELAIS.DIR = 	0xFF;			//PORTK Relais Output
			
	PORTCFG.MPCMASK=0xFF;						//set Mask
    P_OPTO.PIN0CTRL= PORT_OPC_WIREDANDPULL_gc;	//Pins PULL UP

	PORTD.PIN5CTRL= PORT_OPC_WIREDANDPULL_gc;	//FirmwareUpdate PullUp
}


void PORT_SoftwareRst(void)
{									
	CCP = 0xD8;					//Protection
	RST.CTRL= RST_SWRST_bm;
}

void PORT_Bootloader(void)
{
	if(!(PORTD.IN & PIN5_bm))
	{
		LCD_Clean();
		LCD_WriteStringFont(1, 1, "Bootloader-Modus");
		asm volatile("jmp 0x20000");		//Jump Instruction
	}
}



/* ===================================================================*
 * 						FUNCTIONS Buzzer        
 * ===================================================================*/

void PORT_Buzzer(t_FuncCmd cmd)
{
	static unsigned char 	count=0;
	static t_FuncCmd	 	state= _off;

	switch(cmd)
	{
		case _exe:	if(state== _on)
					{
						count++;
						if(count>20){	
							PORTD.DIRSET =	PIN6_bm;
							PORTD.OUTSET =	PIN6_bm;}	
						if(count>40){	count=0;
							PORTD.DIRCLR =	PIN6_bm;
							PORTD.OUTCLR =	PIN6_bm;}
					}									break;

		case _on:	state= _on;							break;

		case _off:	state= _off;
					PORTD.DIRCLR =	PIN6_bm;
					PORTD.OUTCLR =	PIN6_bm;			break;
		default:										break;
	}

}




/* ===================================================================*
 * 						FUNCTIONS Ventilator       
 * ===================================================================*/

void PORT_Ventilator(void)
{
	unsigned char temp=0;
	unsigned char hystOn=0;
	unsigned char hystOff=0;
	
	temp= MCP9800_ReadTemp();
	hystOn=		(MEM_EEPROM_ReadVar(ALARM_temp) - 15);
	hystOff=	(MEM_EEPROM_ReadVar(ALARM_temp) - 20);
	
	if(!(temp & 0x80))
	{
		if(temp>hystOn)		PORT_RelaisSet(R_VENTILATOR);
		if(temp<hystOff)	PORT_RelaisClr(R_VENTILATOR);
	}
	else PORT_RelaisClr(R_VENTILATOR);
}




/* ===================================================================*
 * 						FUNCTIONS Opto-Inputs Floats
 * ===================================================================*/

/*--------------------------------------------------------------------*		
 * 	PORT_FloatSwitch
 * --------------------------------------------------------------
 * 	parameter:		t_OptoInput floatSW		-	Request
 * --------------------------------------------------------------
 * 	return:			t_FloatPos position		-	Position of Float Switch
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Returns the position of the Float Switch
 * -------------------------------------------------------------------*/

t_FloatPos 	PORT_FloatSwitch(t_OptoInput f_sw)	
{
	static t_FloatPos 	s_res= 		Float_Max;
	static t_FloatPos 	s_outTank= 	Float_Max;
	//static t_FloatPos 	s_inTank= 	Float_Max;

	static unsigned char		sensor_out=0;
	//static unsigned char		sensor_in=0;

	switch(f_sw)
	{
		case Float_EnableQuery:	sensor_out=	MEM_EEPROM_ReadVar(SENSOR_outTank);		break;		
								//sensor_in=	MEM_EEPROM_ReadVar(SENSOR_inTank);		break;
										
		case Float_Reserve:		if(!(P_OPTO.IN & PIN6_bm))	s_res= Float_Min;		
								if(!(P_OPTO.IN & PIN7_bm)) 	s_res= Float_Max;
								return 	s_res;							break;		//Reserve Input

		case Float_outTank:		if(sensor_out)
								{
									if(!(P_OPTO.IN & PIN5_bm)) s_outTank= Float_Min;
									if(!(P_OPTO.IN & PIN4_bm)) s_outTank= Float_Max;							
								}	
								else	s_outTank=	Float_Disabled;		
								return	s_outTank;						break;		//outTank Float
		
		/*
		case Float_inTank:		if(sensor_in)
								{
									if(!(P_OPTO.IN & PIN3_bm)) s_inTank= Float_Min;
									if(!(P_OPTO.IN & PIN2_bm)) s_inTank= Float_Max;
									if(!(P_OPTO.IN & PIN1_bm)) s_inTank= Float_Alarm;
								}	
								else	s_inTank=	Float_Disabled;		
								return	s_inTank;						break;		//inTank Float
		*/

		default:														break;
	}

	return Float_Disabled;
}





/* ===================================================================*
 * 						FUNCTIONS Ventil
 * ===================================================================*/

void PORT_Ventil(t_ventil ventil)
{			
	Watchdog_Restart(); 

	switch(ventil)
	{
		case OPEN_Reserve:		P_VENTIL.OUTSET= C_RES;
								TC_Ventil_Wait_1s();
								P_VENTIL.OUTCLR= C_RES;
								TC_Ventil_Wait_500ms();
								P_VENTIL.OUTSET= O_RES;
								TC_Ventil_Wait_3s();				break;

		case CLOSE_Reserve:		P_VENTIL.OUTSET= C_RES;	
								TC_Ventil_Wait_3s5();				break;

		case OPEN_MudPump:		P_VENTIL.OUTSET= C_MUD;
								TC_Ventil_Wait_1s();
								P_VENTIL.OUTCLR= C_MUD;
								TC_Ventil_Wait_500ms();
								P_VENTIL.OUTSET= O_MUD;
								TC_Ventil_Wait_3s();				break;
								
		case CLOSE_MudPump:		P_VENTIL.OUTSET= C_MUD;
								TC_Ventil_Wait_3s5();				break;

		case OPEN_Air:			P_VENTIL.OUTSET= C_AIR;
								TC_Ventil_Wait_1s();
								P_VENTIL.OUTCLR= C_AIR;
								TC_Ventil_Wait_500ms();
								P_VENTIL.OUTSET= O_AIR;
								TC_Ventil_Wait_3s();				break;

		case CLOSE_Air:			P_VENTIL.OUTSET= C_AIR;	
								TC_Ventil_Wait_3s5();				break;

		case OPEN_ClearWater:	P_VENTIL.OUTSET= C_CLRW;
								TC_Ventil_Wait_1s();
								P_VENTIL.OUTCLR= C_CLRW;
								TC_Ventil_Wait_500ms();
								P_VENTIL.OUTSET= O_CLRW;
								TC_Ventil_Wait_3s();				break;

		case CLOSE_ClearWater:	P_VENTIL.OUTSET= C_CLRW;	
								TC_Ventil_Wait_3s5();				break;
		
		case CLOSE_IPAir:		P_VENTIL.OUTSET= C_AIR | C_RES;	
								TC_Ventil_Wait_3s5();				break;

		case OFF_Ventil:		P_VENTIL.OUTSET= 0x00;				break;
		default:													break;
	}

	switch(ventil)
	{
		case OPEN_Reserve:		P_VENTIL.OUTCLR= O_RES;				break;
		case CLOSE_Reserve:		P_VENTIL.OUTCLR= C_RES;				break;
		case OPEN_MudPump:		P_VENTIL.OUTCLR= O_MUD;				break;
		case CLOSE_MudPump:		P_VENTIL.OUTCLR= C_MUD;				break;
		case OPEN_Air:			P_VENTIL.OUTCLR= O_AIR;				break;
		case CLOSE_Air:			P_VENTIL.OUTCLR= C_AIR;				break;
		case OPEN_ClearWater:	P_VENTIL.OUTCLR= O_CLRW;			break;
		case CLOSE_ClearWater:	P_VENTIL.OUTCLR= C_CLRW;			break;
		case CLOSE_IPAir:		P_VENTIL.OUTCLR= C_AIR | C_RES;		break;
		case OFF_Ventil:		P_VENTIL.OUTCLR= 0x00;				break;		
		default:													break;
	}	

	Watchdog_Restart(); 
	TCE0_16MHz_Stop();
}



/* -------------------------------------------------------------------*
 * 						Ventil all open
 * -------------------------------------------------------------------*/

void PORT_Ventil_AllOpen(void)
{
	Watchdog_Restart();
	P_VENTIL.OUTSET= O_RES;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= O_MUD;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= O_AIR;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= O_CLRW;

	TC_Ventil_Wait_1s();
	TC_Ventil_Wait_500ms();

	P_VENTIL.OUTCLR= O_RES;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= O_MUD;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= O_AIR;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= O_CLRW;
	TC_Ventil_Wait_500ms();
	Watchdog_Restart(); 
	TCE0_16MHz_Stop();
}

void PORT_Ventil_AllOpen2(void)
{
	Watchdog_Restart();
	P_VENTIL.OUTSET= O_RES;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= O_MUD;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= O_AIR;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= O_CLRW;
	TC_Ventil_Wait_500ms();

	TC_Ventil_Wait_3s();

	P_VENTIL.OUTCLR= O_RES;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= O_MUD;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= O_AIR;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= O_CLRW;
	TC_Ventil_Wait_500ms();
	Watchdog_Restart(); 
	TCE0_16MHz_Stop();
}


/* -------------------------------------------------------------------*
 * 						Ventil all close
 * -------------------------------------------------------------------*/

void PORT_Ventil_AllClose(void)
{
	Watchdog_Restart();
	P_VENTIL.OUTSET= C_RES;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= C_MUD;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= C_AIR;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTSET= C_CLRW;
	TC_Ventil_Wait_500ms();

	TC_Ventil_Wait_2s();

	P_VENTIL.OUTCLR= C_RES;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= C_MUD;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= C_AIR;
	TC_Ventil_Wait_500ms();
	P_VENTIL.OUTCLR= C_CLRW;
	TC_Ventil_Wait_500ms();
	Watchdog_Restart(); 
	TCE0_16MHz_Stop();
}

void PORT_Ventil_AllOff(void)
{
	P_VENTIL.OUT= 0x00;
}




/* -------------------------------------------------------------------*
 * 						Ventil Auto Close
 * -------------------------------------------------------------------*/

void PORT_Ventil_AutoClose(t_page page)
{
	switch(page)
	{
		case AutoZone:			OUT_Clr_Circulate();	break;
		case AutoSetDown:								break;
		case AutoPumpOff:		OUT_Clr_PumpOff();		break;
		case AutoMud:			OUT_Clr_Mud();			break;
		case AutoCirculate:		OUT_Clr_IPAir();		break;
		case AutoAir:			OUT_Clr_IPAir();		break;

		case ManualCirculate:	OUT_Clr_Circulate();	break;
		case ManualAir:			OUT_Clr_Air();			break;
		case ManualSetDown:								break;
		case ManualPumpOff:		OUT_Clr_PumpOff();		break;
		case ManualMud:			OUT_Clr_Mud();			break;
		case ManualCompressor:	OUT_Clr_Compressor();	break;
		case ManualPhosphor:	OUT_Clr_Phosphor();		break;
		case ManualInflowPump:	OUT_Clr_InflowPump();	break;

		default:										break;
	}
}





/* ===================================================================*
 * 						FUNCTIONS Relais
 * ===================================================================*/
 
void PORT_RelaisSet(unsigned char relais)
{
	P_RELAIS.OUTSET= relais;
}

void PORT_RelaisClr(unsigned char relais)
{
	P_RELAIS.OUTCLR= relais;
}

void PORT_Relais_AllOff(void)
{
	P_RELAIS.OUT= 0x00;
}



/* ===================================================================*
 * 						FUNCTIONS RunTime
 * ===================================================================*/

void PORT_RunTime(void)
{
	static unsigned char runTime=0;

	runTime++;
	if(runTime>100)		//100 = 5s
	{
		runTime=0;
		PORT_Ventilator();
	}
}





/*********************************************************************\
 * End of SL_PORT_func.c
\**********************************************************************/
