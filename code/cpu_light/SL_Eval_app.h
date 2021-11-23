/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Evaluation-App-HeaderFile	
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Application-File Header for xmA_Eval_app.c
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:											
\**********************************************************************/


/* ===================================================================*
 * 						Defines
 * ===================================================================*/

typedef enum 
{	Oxygen_Count,
	Oxygen_Write2Entry,	
	Oxygen_Clear
}t_Oxygen;



/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Oxygen
 * -------------------------------------------------------------------*/

void Eval_Oxygen	(unsigned char min, t_Oxygen oxCmd);


/* -------------------------------------------------------------------*
 * 						Pin
 * -------------------------------------------------------------------*/

void Eval_PinWrite	(unsigned char pin, unsigned char codePos);
void Eval_PinDel	(void);
void Eval_PinClr	(unsigned char *pin);


/* -------------------------------------------------------------------*
 * 						Auto
 * -------------------------------------------------------------------*/

int 			Eval_Comp_OpHours	(t_FuncCmd cmd);
unsigned char 	Eval_AutoCountDown	(int *cMin, int *cSec);

/* -------------------------------------------------------------------*
 * 						Manual
 * -------------------------------------------------------------------*/

unsigned char Eval_CountDown		(unsigned char *cMin, unsigned char *cSec);
unsigned char Eval_AutoAir_CountDown(unsigned char *cMin, unsigned char *cSec);

/* -------------------------------------------------------------------*
 * 						Data
 * -------------------------------------------------------------------*/

unsigned char 	*Eval_Memory_NoEntry(t_textSymbols data);
unsigned char 	*Eval_Memory_OldestEntry(t_textSymbols data);
unsigned char 	*Eval_Memory_LatestEntry(t_textSymbols data);



/* -------------------------------------------------------------------*
 * 						Setup
 * -------------------------------------------------------------------*/

unsigned char 	Eval_SetupPlus		(unsigned char value, unsigned char max);
unsigned char 	Eval_SetupMinus		(unsigned char value, unsigned char min);
int 			Eval_SetupPlus2		(int value, int max);
int 			Eval_SetupMinus2	(int value, int min);

void 			Eval_SetupCircSensorMark(unsigned char sensor);
void 			Eval_SetupCircTextMark	(unsigned char on, unsigned char *p_var);
void			Eval_SetupAirTextMark	(unsigned char on, unsigned char *p_var);

void 			Eval_SetupPumpMark		(unsigned char mark);
void 			Eval_SetupWatchMark		(t_DateTime time, unsigned char *p_dT);





/**********************************************************************\
 * END of xmA_Eval_app.h
\**********************************************************************/

