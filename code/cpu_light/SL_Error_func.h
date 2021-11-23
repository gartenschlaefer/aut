/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			SL_Error_func.h
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Error Detection and Treatment Header-File
* ------------------------------------------------------------------														
*	Date:			24.10.2011  	
* 	lastChanges:											
\**********************************************************************/


/* ===================================================================*
 * 						Enumerations
 * ===================================================================*/


typedef enum 
{	E_Temp,					E_MaxInTank,			E_MaxOutTank,		
	E_UnderPressure,		E_UnderPressure_IP,		
	E_OverPressure_PO,		E_OverPressure_Air,		E_OverPressure_Mud,
	E_OverPressure_NO,		E_OverPressure_IP
}t_ErrorSym;



/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/

void 			Error_ON		(void);
void 			Error_OFF		(void);

t_page 			Error_Detection	(t_page errPage, int min, int sec);
unsigned char 	Error_ReadError	(void);
void 			Error_Treatment	(unsigned char err, t_page errPage);

unsigned char 	Error_Timing	(int 			*p_time, 
								 unsigned char 	errPos, 
								 unsigned char 	error, 
							     unsigned char 	errOut);

void 			Error_Symbol	(t_ErrorSym errSym);
void 			Error_Action	(t_ErrorSym errSym);
void 			Error_Action_OP	(unsigned char alComp, unsigned char eCount);





/**********************************************************************\
 * END of SL_Error_func.h
\**********************************************************************/

