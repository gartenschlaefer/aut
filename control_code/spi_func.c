/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    SPI-function-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceFile for SPI functions
* ------------------------------------------------------------------
*	Date:			31.12.2012
* lastChanges:
\**********************************************************************/


#include <avr/io.h>

#include "defines.h"
#include "spi_func.h"



/* ==================================================================*
 * 						FUNCTIONS Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Init
 * ------------------------------------------------------------------*/

void SPI_Init(void)
{
	PORTE.DIR=  	PIN4_bm	|					//CS Output
					PIN5_bm	|					//MOSI Output
					PIN7_bm;					//SCK Output

	PORTE.DIRCLR=	PIN6_bm;

	PORTE.OUTCLR= 	PIN5_bm	|
					PIN6_bm	|
					PIN7_bm;

	SPIE.CTRL= 		SPI_ENABLE_bm			|	//SPI Enable
					SPI_MASTER_bm			|	//MasterMode
					SPI_MODE_0_gc 			|	//RisingSampleFallingSetup
					SPI_PRESCALER_DIV64_gc;		//clk/64

	SPIE.INTCTRL= 	SPI_INTLVL_OFF_gc;			//Interrupt Off
	CS_SET;										//No CS
}




/* ==================================================================*
 * 						FUNCTIONS Write Read
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Write Byte
 * ------------------------------------------------------------------*/

unsigned char SPI_WriteByte(unsigned char wByte)
{
	unsigned char dummy=0;

	SPIE.DATA= wByte;						          //Start Transmission
	while(!(SPIE.STATUS & SPI_IF_bm));		//WaitUntilComplete
	dummy= SPIE.DATA;						          //Read Dummy Reset-SPIE
  dummy= dummy;                         //4CompilerWarning
	return 0;
}



/* ------------------------------------------------------------------*
 * 						Write String
 * ------------------------------------------------------------------*/

unsigned char SPI_WriteString(unsigned char	*sendData, unsigned char i)
{
	unsigned char sendPuffer=0;

	while(i)
	{
		sendPuffer= *sendData;				//update send Puffer
		SPI_WriteByte(sendPuffer);			//WriteByte
		sendData++;							//increase Pointer
		i--;								//decrease Counter
	}

	return F_SPI_SENT;
}



/* ------------------------------------------------------------------*
 * 						Read Byte
 * ------------------------------------------------------------------*/

unsigned char SPI_ReadByte(void)
{
	SPI_WriteByte(0xFF);					//Write DummyByte
	return SPIE.DATA;;
}






/**********************************************************************\
 * End of spi_func.c
\**********************************************************************/
