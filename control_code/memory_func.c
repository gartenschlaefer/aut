/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Memory-func-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Functions to access Memory
* ------------------------------------------------------------------
*	Date:			    13.07.2011
* lastChanges:
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "memory_app.h"
#include "memory_func.h"



/* ==================================================================*
 * 						FUNCTIONS Basics
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_ExecuteCMD
 * --------------------------------------------------------------
 *	Execute Commands and wait until succeeded
 * ------------------------------------------------------------------*/

void MEM_EEPROM_ExecuteCMD(void)
{
	CCP = 0xD8;						              //Protection
	NVM.CTRLA = NVM_CMDEX_bm;				    //execute CMD
	while(NVM.STATUS & NVM_NVMBUSY_bm);	//wait
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_LoadPageBuffer
 * --------------------------------------------------------------
 *	Loads EEPROM Page Buffer with one Byte
 * ------------------------------------------------------------------*/

void MEM_EEPROM_LoadPageBuffer(unsigned char byte, unsigned char eeData)
{
	NVM.CMD= NVM_CMD_LOAD_EEPROM_BUFFER_gc;		//CMD Load
	NVM.ADDR0= byte;							//Address
	NVM.DATA0= eeData;							//load Data
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_ErasePageBuffer
 * --------------------------------------------------------------
 *	Erase Page Buffer of EEPROM
 * ------------------------------------------------------------------*/

void MEM_EEPROM_ErasePageBuffer(void)
{
	NVM.CMD= 	NVM_CMD_ERASE_EEPROM_BUFFER_gc;		//CMD Erase PageBuffer
	MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_PageErase
 * --------------------------------------------------------------
 *	Erases Page where written in Buffer
 * ------------------------------------------------------------------*/

void MEM_EEPROM_PageErase(unsigned char page)
{
	NVM.CMD= NVM_CMD_ERASE_EEPROM_PAGE_gc;		//CMD Load

	NVM.ADDR0= ((page << 5) & 0xE0);			//Page0
	NVM.ADDR1= (page >> 3);						//Page1

	MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_PageWrite
 * --------------------------------------------------------------
 *	Writes Page
 * ------------------------------------------------------------------*/

void MEM_EEPROM_PageWrite(unsigned char page)
{
	NVM.CMD = NVM_CMD_WRITE_EEPROM_PAGE_gc;	//CMD Load
	NVM.ADDR0= ((page << 5) & 0xE0);			  //Page0 + Byte
	NVM.ADDR1= (page >> 3);						      //Page1
	MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_PageEraseWrite
 * --------------------------------------------------------------
 *	Erase and Write Page at the same time
 * ------------------------------------------------------------------*/

void MEM_EEPROM_PageEraseWrite(unsigned char page)
{
	NVM.CMD= NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;	//CMD Load
	NVM.ADDR0= ((page << 5) & 0xE0);					    //Page0
	NVM.ADDR1= (page >> 3);								        //Page1
	MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_Erase
 * --------------------------------------------------------------
 *	Erase whole EEPROM parts, which are dedicated in Page Buffer
 * ------------------------------------------------------------------*/

void MEM_EEPROM_Erase(void)
{
	NVM.CMD = NVM_CMD_ERASE_EEPROM_gc;		//CMD Load
	MEM_EEPROM_ExecuteCMD();
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_Read
 * --------------------------------------------------------------
 *	Reads one Byte from dedicated address in EEPROM
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_Read(unsigned char page, unsigned char byte)
{
	NVM.CMD= NVM_CMD_READ_EEPROM_gc;		//CMD Load

	NVM.ADDR0= ((page << 5) & 0xE0) | (byte & 0x1F);	//Page0 + Byte
	NVM.ADDR1= (page >> 3);								//Page1

	CCP= 		0xD8;						        //Protection
	NVM.CTRLA= 	NVM_CMDEX_bm;				//execute CMD

	return NVM.DATA0;						    //return EEPROM Data
}



/* ==================================================================*
 * 						EEPROM APPS
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_WriteByte
 * --------------------------------------------------------------
 *	Writes on Byte to EEPROM
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteByte
(
	unsigned char page,
	unsigned char byte,
	unsigned char eeData
)
{
	if(NVM.STATUS & NVM_EELOAD_bm) 				//If Buffer Loaded
	{
		MEM_EEPROM_ErasePageBuffer();			//Erase
	}

	MEM_EEPROM_LoadPageBuffer(byte, eeData);	//Load new Data
	MEM_EEPROM_PageEraseWrite(page);			//Write new Data
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_Entire Page Erase
 * --------------------------------------------------------------
 *	Erases Entire Page (32Bytes)
 * ------------------------------------------------------------------*/

void MEM_EEPROM_EntPageErase(unsigned char page)
{
	unsigned char i=0;
	for(i=0; i<32; i++)
	{
		MEM_EEPROM_LoadPageBuffer(i, 0xFF);		//load with dummy Bytes
	}
	MEM_EEPROM_PageErase(page);
}


/*-------------------------------------------------------------------*
 * 	Memory EEPROM - Entire Erase
 * ------------------------------------------------------------------*/
void MEM_EEPROM_EntErase(void)
{
	unsigned char i=0;

	for(i=0; i<32; i++)
	{
		MEM_EEPROM_LoadPageBuffer(i, 0xFF);		//load with dummy Bytes
	}

	MEM_EEPROM_Erase();
}








/***********************************************************************
 *	End of file
 ***********************************************************************/

