/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			MPX-driver-HeaderFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Header of SL_MPX_func.c
* ------------------------------------------------------------------														
*	Date:			09.06.2011  	
* 	lastChanges:											
\**********************************************************************/


/* ===================================================================*
 * 						Enumeration
 * ===================================================================*/

typedef enum 
{	mpx_PosMin,		mpx_Pos1,	mpx_Pos2,	mpx_PosMax,		mpx_Calc,
	mpx_Disabled,	mpx_Alarm
}t_MpxPos;


/* ===================================================================*
 * 						FUNCTIONS API
 * ===================================================================*/
 
void 		MPX_ADC_Init			(void);
int 		MPX_Read				(void);
int 		MPX_ReadCal				(void);

int 		MPX_ReadAverage				(t_textSymbols page, t_FuncCmd cmd);
int 		MPX_ReadAverage_Value		(void);
int 		MPX_ReadAverage_UnCal		(void);
int 		MPX_ReadAverage_UnCal_Value	(void);

int 		MPX_ReadWaterLevel	(t_FuncCmd cmd);
t_page		MPX_ReadTank		(t_page page, t_FuncCmd cmd);
void 		MPX_Test			(void);







 
/**********************************************************************\
 * End of SL_MPX_func.h
\**********************************************************************/

