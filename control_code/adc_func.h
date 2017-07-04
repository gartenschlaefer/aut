/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			  ADC-func-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	ADC Setup
* ------------------------------------------------------------------
*	Date:			    21.02.2015
* lastChanges:
\**********************************************************************/



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void ADC_Init			(void);
void ADC_Touch_Ch	(void);
void ADC_MPX_Ch		(void);
void ADC_USV_Ch   (void);
void ADC_USV_Check (void);

unsigned char ReadCalibrationByte(unsigned char index);





/**********************************************************************\
 * End of file
\**********************************************************************/
