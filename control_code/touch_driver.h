/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			Touchpanel-Driver-HeaderFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
	Header for xmA_Touchpanel_driver.c
* ------------------------------------------------------------------
*	Date:			01.06.2011
* lastChanges:
\**********************************************************************/


/* ==================================================================*
 * 						Defines
 * ==================================================================*/

#define TOP			(1<<PIN2)		      //ADC Read
#define BOTTOM	(1<<PIN4)		      //Resistor 220 - 470R
#define LEFT		(1<<PIN3)		      //ADC Read
#define RIGHT		(1<<PIN1)		      //Resistor 150 - 220R



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Touch Initialization
 * ------------------------------------------------------------------*/

void 			Touch_Cal			  (void);
int 			Touch_X_Cal_Init(void);
int 			Touch_Y_Cal_Init(void);



/* ------------------------------------------------------------------*
 * 						Touch Calibration
 * ------------------------------------------------------------------*/

int 			Touch_X_Cal			(int xBereich);
int			 	Touch_Y_Cal			(int yBereich);



/* ------------------------------------------------------------------*
 * 						Touch Measure
 * ------------------------------------------------------------------*/

void 			Touch_Clean		(void);

void 			Touch_Y_Measure	(void);					//Set up Measure
int 			Touch_Y_Read	(void);					//Read ADC LEFT (ADC0)
int 			Touch_Y_ReadData(void);					//All in One Y-Measure

void 			Touch_X_Measure	(void);					//Set up Measure
int 			Touch_X_Read	(void);					//Read ADC TOP (ADC1)
int 			Touch_X_ReadData(void);					//All in One X-Measure

unsigned char 	*Touch_Read		(void);					//TouchRead




/***********************************************************************
 * End of SL_Touch_driver.h
***********************************************************************/


