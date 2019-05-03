/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    SL_Error_func.h
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Error Detection and Treatment Header-File
* ------------------------------------------------------------------
*	Date:			    24.10.2011
* lastChanges:  24.07.2015
\**********************************************************************/

// Include guard
#ifndef ERROR_FUNC_H   
#define ERROR_FUNC_H

/* ==================================================================*
 * 						Defines - Error Positions
 * ==================================================================*/

#define 	E_T			(1<<0)			//Temperature
#define		E_OP		(1<<1)			//OverPressure
#define		E_UP		(1<<2)			//UnderPressure
#define		E_IT		(1<<3)			//MaxinTank
#define		E_OT		(1<<4)			//OutTank

/* ==================================================================*
 * 						Struct
 * ==================================================================*/

typedef struct _ErrTreat
{
  t_page page;
  unsigned char err_treated;
}ErrTreat;


/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void Error_ON(void);
void Error_OFF(void);

unsigned char Error_Read(t_page page);

t_page 		Error_Detection	(t_page page, int min, int sec);
ErrTreat  Error_Treatment	(t_page page, unsigned char error);
void      Error_Symbol    (unsigned char err);

unsigned char Error_Action_OP_Air     (t_page page);
unsigned char Error_Action_UP_Air	    (t_page page);

void Error_Action_Temp_SetError(void);
void Error_Action_OP_SetError(void);
void Error_Action_UP_SetError(void);
void Error_Action_IT_SetError(void);
void Error_Action_OT_SetError(void);

#endif

