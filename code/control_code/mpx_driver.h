/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    MPX-driver-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Header of SL_MPX_func.c
* ------------------------------------------------------------------
*	Date:			    09.06.2011
* lastChanges:  17.05.2015
\**********************************************************************/


/* ==================================================================*
 * 						Enumeration
 * ==================================================================*/

typedef enum
{
  mpx_PosMin,	mpx_Pos1,	    mpx_Pos2,	  mpx_PosMax,
  mpx_Calc,   mpx_Disabled,	mpx_Alarm
}t_MpxPos;


/* ==================================================================*
 * 						FUNCTIONS API
 * ==================================================================*/

int MPX_Read(void);
int MPX_ReadCal(void);

int MPX_ReadAverage(t_textButtons page, t_FuncCmd cmd);
int MPX_ReadAverage_Value(void);
int MPX_ReadAverage_UnCal(void);
int MPX_ReadAverage_UnCal_Value(void);

int 		MPX_LevelCal(t_FuncCmd cmd);
t_page	MPX_ReadTank(t_page page, t_FuncCmd cmd);








/**********************************************************************\
 * End of file
\**********************************************************************/

