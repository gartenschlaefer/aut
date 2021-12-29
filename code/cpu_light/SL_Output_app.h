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


/* ===================================================================*
 * 						FUNCTIONS API
 * ===================================================================*/

void OUT_SetDown(void);

void OUT_Set_PumpOff(void);
void OUT_Clr_PumpOff(void);

void OUT_Set_Mud(void);
void OUT_Clr_Mud(void);

void OUT_Set_Air(void);
void OUT_Clr_Air(void);

void OUT_Set_Compressor(void);
void OUT_Clr_Compressor(void);

void OUT_Set_Phosphor(void);
void OUT_Clr_Phosphor(void);

void OUT_Set_InflowPump(void);
void OUT_Clr_InflowPump(void);
void OUT_Clr_IPAir(void);

void OUT_CloseOff(void);
void OUT_TestVentil(void);
void OUT_TestVentil2(void);