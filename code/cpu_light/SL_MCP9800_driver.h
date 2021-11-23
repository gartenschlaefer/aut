/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			MCP9800-driver-HeaderFile
* ------------------------------------------------------------------
*	�-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Header of MCP9800 Temperature Sensor Driver File for SL 
* ------------------------------------------------------------------														
*	Date:			30.05.2011  	
* 	lastChanges:	06.06.2011										
\**********************************************************************/

#include<avr/io.h>

/* ===================================================================*
 * 						DEFINES
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						PORT
 * -------------------------------------------------------------------*/

#define MCP_PORT	(PORTC)				//PORTC, TWI PC0 and PC1
#define MCP_ALERT	(1<<2)				//PC2


/* -------------------------------------------------------------------*
 * 						TWI Address
 * -------------------------------------------------------------------*/

#define MCP_ADDR	(0x48)


/* -------------------------------------------------------------------*
 * 						Register Pointer
 * -------------------------------------------------------------------*/

#define MCP_TAREG		(0x00)				//Temperature Ambient
#define MCP_CONFREG		(0x01)				//Configuration
#define MCP_HYSTREG		(0x02)				//Hysteresis
#define MCP_LIMREG		(0x00)				//Temp Limit


/* -------------------------------------------------------------------*
 * 						Alert
 * -------------------------------------------------------------------*/

#define MCP_LIMIT		(0x50)				//80�C
#define MCP_HYST		(0x4B)				//75�C


/* -------------------------------------------------------------------*
 * 						Config REG
 * -------------------------------------------------------------------*/

#define MCP_ONESHOT_ON		(1<<7)
#define MCP_ONESHOT_OFF		(0<<7)
#define MCP_SHUTDOWN_ON		(1<<0)	
#define MCP_SHUTDOWN_OFF	(0<<0)	

#define MCP_RES				(1<<5)			//10Bit Resolution
#define MCP_FAULTQ			(1<<3)			//2 Fault Queue
#define MCP_ALERTPOL		(0<<2)			//Alert Aktive Low
#define MCP_COMP_INT		(0<<1)			//Comparator Mode



/* ===================================================================*
 * 						FUCNTION API
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Basics
 * -------------------------------------------------------------------*/

void 			MCP9800_Init		(void);
void 			MCP9800_OneShot		(void);

void 			MCP9800_Write		(unsigned char *send, unsigned char i);
unsigned char 	*MCP9800_Read		(unsigned char i);

unsigned char 	MCP9800_ReceiveByte	(unsigned char pointer);

void 			MCP9800_SendByte	(unsigned char pointer, 
									 unsigned char sData);


/* -------------------------------------------------------------------*
 * 						Apps
 * -------------------------------------------------------------------*/

unsigned char 	MCP9800_Temp			(void);
unsigned char 	MCP9800_PlusTemp		(void);
unsigned char 	MCP9800_ReadTemp		(void);
void 			MCP9800_WriteTemp		(void);




/*********************************************************************\
 * End of SL_DS1302_diver.h
\**********************************************************************/


