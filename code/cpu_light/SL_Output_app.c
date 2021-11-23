/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Output-App-SourceFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Output of Control, set Relais and ventils
* ------------------------------------------------------------------														
*	Date:			18.07.2011  	
* 	lastChanges:										
\**********************************************************************/


#include "SL_Define_sym.h"
#include "SL_TC_func.h"
#include "SL_PORT_func.h"
#include "SL_Memory_app.h"



/* -------------------------------------------------------------------*
 * 						SetDown
 * -------------------------------------------------------------------*/

void OUT_SetDown(void)
{
	PORT_RelaisClr(R_COMP);			PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
	PORT_RelaisClr(R_INFLOW1);		PORT_RelaisClr(R_INFLOW2);
	PORT_RelaisClr(R_CLEARWATER);
}



/* -------------------------------------------------------------------*
 * 						PumpOff
 * -------------------------------------------------------------------*/

void OUT_Set_PumpOff(void)
{
	unsigned char pump=0;

	pump= MEM_EEPROM_ReadVar(PUMP_pumpOff);
	if(!pump)														//Mammutpumpe?
	{	
		PORT_Ventil(OPEN_ClearWater);
		PORT_RelaisSet(R_COMP);		PORT_RelaisSet(R_EXT_COMP);		//Compressor ON									
	}		
	else	PORT_RelaisSet(R_CLEARWATER);							//ext. Pump ON
}

void OUT_Clr_PumpOff(void)
{
	unsigned char pump=0;

	PORT_RelaisClr(R_COMP);			PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
	PORT_RelaisClr(R_CLEARWATER);									//ext. Pump  OFF
	pump= MEM_EEPROM_ReadVar(PUMP_pumpOff);
	if(!pump)														//Mammutpumpe?
	{	
		PORT_Ventil(CLOSE_ClearWater);
	}		
}


/* -------------------------------------------------------------------*
 * 						Mud
 * -------------------------------------------------------------------*/

void OUT_Set_Mud(void)
{
	PORT_Ventil(OPEN_MudPump);
	PORT_RelaisSet(R_COMP);		PORT_RelaisSet(R_EXT_COMP);		//Compressor ON								
}

void OUT_Clr_Mud(void)
{
	PORT_RelaisClr(R_COMP);		PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
	PORT_Ventil(CLOSE_MudPump);
}




/* -------------------------------------------------------------------*
 * 						Circulate
 * -------------------------------------------------------------------*/

void OUT_Set_Circulate(void)
{
	PORT_Ventil(OPEN_Air);
	PORT_RelaisSet(R_COMP);		PORT_RelaisSet(R_EXT_COMP);		//Compressor ON
}

void OUT_Clr_Circulate(void)
{
	PORT_RelaisClr(R_COMP);		PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
	PORT_Ventil(CLOSE_Air);
}




/* -------------------------------------------------------------------*
 * 						Air
 * -------------------------------------------------------------------*/

void OUT_Set_Air(void)
{
	PORT_Ventil(OPEN_Air);
	PORT_RelaisSet(R_COMP);		PORT_RelaisSet(R_EXT_COMP);		//Compressor ON
}

void OUT_Clr_Air(void)
{
	PORT_RelaisClr(R_COMP);		PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
	PORT_Ventil(CLOSE_Air);
}


/* -------------------------------------------------------------------*
 * 						Compressor
 * -------------------------------------------------------------------*/

void OUT_Set_Compressor(void)
{
	PORT_RelaisSet(R_COMP);										//Compressor ON
}

void OUT_Clr_Compressor(void)
{
	PORT_RelaisClr(R_COMP);		PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
}


/* -------------------------------------------------------------------*
 * 						Phosphor
 * -------------------------------------------------------------------*/

void OUT_Set_Phosphor(void)
{
	PORT_RelaisSet(R_PHOSPHOR);
}

void OUT_Clr_Phosphor(void)
{
	PORT_RelaisClr(R_PHOSPHOR);	
}




/* -------------------------------------------------------------------*
 * 						InflowPump
 * -------------------------------------------------------------------*/

void OUT_Set_InflowPump(void)
{
	static unsigned char pumpChange=0;
	unsigned char pump=0;
	
	pump= MEM_EEPROM_ReadVar(PUMP_inflowPump);
	switch(pump)
	{
		case 0:	PORT_Ventil(OPEN_Reserve);	
				PORT_RelaisSet(R_COMP);	
				PORT_RelaisSet(R_EXT_COMP);							break;	//Mammutpumpe

		case 1:	PORT_RelaisSet(R_INFLOW1);							break;	//ext. Pump1

		case 2:	if(!pumpChange){	pumpChange=1;									
									PORT_RelaisSet(R_INFLOW1);}
				else{				pumpChange=0;
									PORT_RelaisSet(R_INFLOW2);}		break;	//2ext. Pump
		default:													break;			
	}	
}

void OUT_Clr_InflowPump(void)
{
	unsigned char pump=0;

	PORT_RelaisClr(R_COMP);		PORT_RelaisClr(R_EXT_COMP);									//Compressor OFF
	PORT_RelaisClr(R_INFLOW1);	PORT_RelaisClr(R_INFLOW2);		//ext. Pumps OFF
	pump= MEM_EEPROM_ReadVar(PUMP_inflowPump);
	if(!pump)	PORT_Ventil(CLOSE_Reserve);
}



/* -------------------------------------------------------------------*
 * 						InflowPump and Air
 * -------------------------------------------------------------------*/

void OUT_Clr_IPAir(void)
{
	PORT_RelaisClr(R_COMP);		PORT_RelaisClr(R_EXT_COMP);		//Compressor OFF
	PORT_RelaisClr(R_INFLOW1);	PORT_RelaisClr(R_INFLOW2);		//ext. Pumps OFF
	PORT_Ventil(CLOSE_IPAir);									//Close AIR and IP
}




/* -------------------------------------------------------------------*
 * 						Alarm
 * -------------------------------------------------------------------*/

void OUT_Set_Alarm(void)
{
	PORT_RelaisSet(R_ALARM);
}

void OUT_Clr_Alarm(void)
{
	PORT_RelaisClr(R_ALARM);
}




/* -------------------------------------------------------------------*
 * 						All Off
 * -------------------------------------------------------------------*/

void OUT_CloseOff(void)
{
	PORT_Relais_AllOff();		
	PORT_Ventil_AllClose();	
}



/* -------------------------------------------------------------------*
 * 						Test
 * -------------------------------------------------------------------*/

void OUT_TestVentil(void)
{	
	PORT_Ventil(OPEN_Reserve);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_Reserve);
	TC_Ventil_Wait_1s();

	PORT_Ventil(OPEN_MudPump);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_MudPump);
	TC_Ventil_Wait_1s();
	
	PORT_Ventil(OPEN_Air);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_Air);
	TC_Ventil_Wait_1s();
	
	PORT_Ventil(OPEN_ClearWater);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_ClearWater);
	TC_Ventil_Wait_1s();

	PORT_Ventil_AllOpen();
}

void OUT_TestVentil2(void)
{
	while(1)
	{
		PORT_Ventil_AllClose();
		PORT_Ventil_AllOpen();
	}
}




 
/**********************************************************************\
 * End of SL_Output_app.c
\**********************************************************************/
