/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
* Name:			AD8555-driver-HeaderFile
* ------------------------------------------------------------------
* µC:        	  ATxmega128A1
* Compiler:		  avr-gcc (WINAVR 2010)
* Description:
* ------------------------------------------------------------------
	Amplifier for Pressure Sensor MPX
* ------------------------------------------------------------------
* Date:			25.07.2011
* lastChanges:
\**********************************************************************/


#include<avr/io.h>

/* ==================================================================*
 * 						Defines
 * ==================================================================*/

#define AD8555_OUTPUT		(PORTA.DIRSET= PIN5_bm)
#define AD8555_INPUT		(PORTA.DIRCLR= PIN5_bm)
#define AD8555_HIGH			(PORTA.OUTSET= PIN5_bm)
#define AD8555_LOW			(PORTA.OUTCLR= PIN5_bm)

#define AD8555_FC_SIM		  (0x01)
#define AD8555_FC_2GAIN		(0x00)
#define AD8555_FC_1GAIN		(0x02)
#define AD8555_FC_DUMMY 	(0x02)

#define AD8555_DATA_2GAIN	(1)
#define AD8555_DATA_1GAIN	(55)



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void AD8555_Init(void);
void AD8555_Write_Bit(unsigned char bit);

void AD8555_Write_FC_SimFirstGain(void);
void AD8555_Write_FC_SimSecondGain(void);

void AD8555_Write_Data_SimFirstGain(void);
void AD8555_Write_Data_SimSecondGain(void);

void AD8555_Write_StartPacket(void);
void AD8555_Write_EndPacket(void);

void AD8555_SetSim_FirstGain(void);
void AD8555_SetSim_SecondGain(void);





/**********************************************************************\
 * End of ad8555_driver.h
\**********************************************************************/
