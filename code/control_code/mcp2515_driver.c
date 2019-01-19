/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    MCP2515-driver-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceCode of MCP2525-CAN-Controller-IC for ICT
* ------------------------------------------------------------------
*	Date:			05.01.2013
* lastChanges:
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"

#include "spi_func.h"
#include "tc_func.h"
#include "mcp2515_driver.h"



/* ==================================================================*
 * 						FUNCTIONS Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Init 100kps
 * ------------------------------------------------------------------*/

void MCP2515_Init(void)
{
	SPI_Init();
	TCC0_wait_ms(200);
	PORTE.DIRSET= PIN2_bm;				//Reset Output
	PORTE.OUTSET= PIN2_bm;				//Reset off
	TCC0_wait_ms(200);

	MCP2515_SWReset();
	TCC0_wait_ms(10);
										//-100kps
	MCP2515_WriteReg(CNF1, 0x04);		//BRP-16Mhz/(4+1)*2, SJW=1
	MCP2515_WriteReg(CNF2, 0xDE);		//PRSEG=6+1 PHSEG1=3+1 BTLMode 3xSample
	MCP2515_WriteReg(CNF3, 0x03);		//PHSEG2=3+1
}




/* ------------------------------------------------------------------*
 * 						Reset
 * ------------------------------------------------------------------*/

void MCP2515_SWReset(void)
{
	CS_CLR;
	SPI_WriteByte(0xA0);
	CS_SET;
}

void MCP2515_HWReset(void)
{
	TCC0_wait_ms(200);			//Wait
	PORTE.OUTCLR= PIN2_bm;		//Reset on
	TCC0_wait_ms(200);			//Wait
	PORTE.OUTSET= PIN2_bm;		//Reset off
}



/* ==================================================================*
 * 						FUNCTIONS Write Read Register
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Write Register
 * ------------------------------------------------------------------*/

void MCP2515_WriteReg(unsigned char addr, unsigned char data)
{
	CS_CLR;
	SPI_WriteByte(0x02);
	SPI_WriteByte(addr);
	SPI_WriteByte(data);
	CS_SET;
}



/* ------------------------------------------------------------------*
 * 						Read Register
 * ------------------------------------------------------------------*/

unsigned char MCP2515_ReadReg(unsigned char addr)
{
	unsigned char rReg=0;

	CS_CLR;
	SPI_WriteByte(0x03);
	SPI_WriteByte(addr);
	rReg= SPI_ReadByte();
	CS_SET;

	return rReg;
}


/* ------------------------------------------------------------------*
 * 						Bit Modify
 * ------------------------------------------------------------------*/

void MCP2515_BitModify(	unsigned char addr,
						unsigned char mask,
						unsigned char data)
{
	CS_CLR;
	SPI_WriteByte(addr);
	SPI_WriteByte(mask);
	SPI_WriteByte(data);
	CS_SET;
}



/* ==================================================================*
 * 						FUNCTIONS Write Read Buffer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Read RxB0 or RxB1 -> macros
 * ------------------------------------------------------------------*/

unsigned char MCP2515_ReadRxB(unsigned char cmd)
{
	unsigned char rReg=0;

	CS_CLR;
	SPI_WriteByte(cmd);
	rReg= SPI_ReadByte();
	CS_SET;

	return rReg;
}



/* ------------------------------------------------------------------*
 * 						Load TxBuffer -> macros
 * ------------------------------------------------------------------*/

void MCP2515_LoadTxBuffer(unsigned char cmd, unsigned char buffer)
{
	CS_CLR;
	SPI_WriteByte(cmd);
	SPI_WriteByte(buffer);
	CS_SET;
}



/* ==================================================================*
 * 						FUNCTIONS - Apps
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Start Transmission
 * ------------------------------------------------------------------*/

void MCP2515_StartTransmission(void)
{
	MCP2515_WriteReg(TXB0CTRL, TXREQ);
}

void MCP2515_WriteComand(unsigned char address, unsigned char cmd)
{
	MCP2515_LoadTxBuffer(CMD_TXB0SIDH, address);
	MCP2515_LoadTxBuffer(CMD_TXB0D0, cmd);
	MCP2515_StartTransmission();
}

void MCP2515_Test(void)
{
	MCP2515_LoadTxBuffer(CMD_TXB0SIDH, 0xF0);
	MCP2515_LoadTxBuffer(CMD_TXB0D0, 0xAA);
	MCP2515_StartTransmission();
}








/*********************************************************************\
 * End of mcp2515_diver.c
\**********************************************************************/
