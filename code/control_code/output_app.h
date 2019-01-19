/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			Output-App-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
	Output of Control, set Relais and ventils
* ------------------------------------------------------------------
*	Date:			18.07.2011
* lastChanges:
\**********************************************************************/


/* ==================================================================*
 * 						FUNCTIONS API
 * ==================================================================*/

void OUT_SetDown(void);

/* ------------------------------------------------------------------*
 * 						PumpOff
 * ------------------------------------------------------------------*/
void OUT_Set_PumpOff(void);
void OUT_Clr_PumpOff(void);
void OUT_Set_PumpOff_ww(void);
void OUT_Clr_PumpOff_ww(void);

/* ------------------------------------------------------------------*
 * 						Mud
 * ------------------------------------------------------------------*/
void OUT_Set_Mud(void);
void OUT_Clr_Mud(void);
void OUT_Set_Mud(void);
void OUT_Clr_Mud(void);

/* ------------------------------------------------------------------*
 * 						Air
 * ------------------------------------------------------------------*/
void OUT_Set_Circulate(void);
void OUT_Clr_Circulate(void);

void OUT_Set_Air(void);
void OUT_Clr_Air(void);

void OUT_Set_Compressor(void);
void OUT_Clr_Compressor(void);

void OUT_Set_Phosphor(void);
void OUT_Clr_Phosphor(void);

void OUT_Set_InflowPump(void);
void OUT_Clr_InflowPump(void);

void OUT_Set_Alarm(void);
void OUT_Clr_Alarm(void);

void OUT_Clr_IPAir(void);

void OUT_CloseOff(void);
void OUT_TestVentil(void);
void OUT_TestVentil2(void);




/**********************************************************************\
 * End of SL_Output_app.c
\**********************************************************************/
