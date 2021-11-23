/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Memory-func-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Functions to access Memory
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:									
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_Memory_func.h"
#include "SL_Memory_app.h"




/* ===================================================================*
 * 						FUNCTIONS Init
 * ===================================================================*/

void MEM_EEPROM_Init(void)
{
	MEM_EEPROM_WriteVarDefault();
	MEM_EEPROM_WriteDataDefault();
}




/* ===================================================================*
 * 						FUNCTIONS Basics
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_ExecuteCMD
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Execute Commands and wait until succeeded
 * -------------------------------------------------------------------*/

void MEM_EEPROM_ExecuteCMD(void)
{
	CCP= 		0xD8;						//Protection
	NVM.CTRLA= 	NVM_CMDEX_bm;				//execute CMD
	while(NVM.STATUS & NVM_NVMBUSY_bm);		//wait
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_LoadPageBuffer
 * --------------------------------------------------------------
 * 	parameter:		unsigned char byte		-	Byte Address in Page
 * 					unsigned char eeData	-	EEPROM Data
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Loads EEPROM Page Buffer with one Byte
 * -------------------------------------------------------------------*/

void MEM_EEPROM_LoadPageBuffer(unsigned char byte, unsigned char eeData)
{
	NVM.CMD= NVM_CMD_LOAD_EEPROM_BUFFER_gc;		//CMD Load
	NVM.ADDR0= byte;							//Address
	NVM.DATA0= eeData;							//load Data					
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_ErasePageBuffer
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Erase Page Buffer of EEPROM
 * -------------------------------------------------------------------*/

void MEM_EEPROM_ErasePageBuffer(void)
{
	NVM.CMD= 	NVM_CMD_ERASE_EEPROM_BUFFER_gc;		//CMD Erase PageBuffer
	MEM_EEPROM_ExecuteCMD();
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_PageErase
 * --------------------------------------------------------------
 * 	parameter:		unsigned char page		-	Page Address
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Erases Page where written in Buffer
 * -------------------------------------------------------------------*/

void MEM_EEPROM_PageErase(unsigned char page)
{
	NVM.CMD= NVM_CMD_ERASE_EEPROM_PAGE_gc;		//CMD Load
	
	NVM.ADDR0= ((page << 5) & 0xE0);			//Page0 
	NVM.ADDR1= (page >> 3);						//Page1
	
	MEM_EEPROM_ExecuteCMD();
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_PageWrite
 * --------------------------------------------------------------
 * 	parameter:		unsigned char page		-	Page Address
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Page
 * -------------------------------------------------------------------*/

void MEM_EEPROM_PageWrite(unsigned char page)
{
	NVM.CMD= NVM_CMD_WRITE_EEPROM_PAGE_gc;		//CMD Load
	
	NVM.ADDR0= ((page << 5) & 0xE0);			//Page0 + Byte
	NVM.ADDR1= (page >> 3);						//Page1
	
	MEM_EEPROM_ExecuteCMD();
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_PageEraseWrite
 * --------------------------------------------------------------
 * 	parameter:		unsigned char page		-	Page Address
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Erase and Write Page at the same time
 * -------------------------------------------------------------------*/

void MEM_EEPROM_PageEraseWrite(unsigned char page)
{
	NVM.CMD= NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;		//CMD Load
	
	NVM.ADDR0= ((page << 5) & 0xE0);					//Page0
	NVM.ADDR1= (page >> 3);								//Page1
	
	MEM_EEPROM_ExecuteCMD();
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_Erase
 * --------------------------------------------------------------
 * 	parameter:		unsigned char page		-	Page Address
 *					unsigned char byte		-	byte Address
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Erase whole EEPROM parts, which are dedicated in Page Buffer
 * -------------------------------------------------------------------*/

void MEM_EEPROM_Erase(void)
{
	NVM.CMD= NVM_CMD_ERASE_EEPROM_gc;		//CMD Load
	MEM_EEPROM_ExecuteCMD();
}



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_Read
 * --------------------------------------------------------------
 * 	parameter:		unsigned char page		-	Page Address
 *					unsigned char byte		-	Byte Address
 * --------------------------------------------------------------
 * 	return:			unsigned char eeData	-	EEPROM Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Reads one Byte from dedicated address in EEPROM
 * -------------------------------------------------------------------*/

unsigned char MEM_EEPROM_Read(unsigned char page, unsigned char byte)
{
	NVM.CMD= NVM_CMD_READ_EEPROM_gc;		//CMD Load
	
	NVM.ADDR0= ((page << 5) & 0xE0) | (byte & 0x1F);	//Page0 + Byte
	NVM.ADDR1= (page >> 3);								//Page1
	
	CCP= 		0xD8;						//Protection
	NVM.CTRLA= 	NVM_CMDEX_bm;				//execute CMD

	return NVM.DATA0;						//return EEPROM Data
}





/* ===================================================================*
 * 						EEPROM APPS
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_WriteByte
 * --------------------------------------------------------------
 * 	parameter:		unsigned char addr		-	Byte Address in Page
 * 					unsigned char eeData	-	EEPROM Data
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes on Byte to EEPROM
 * -------------------------------------------------------------------*/

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



/*--------------------------------------------------------------------*
 * 	MEM_EEPROM_EntPageErase			MEM_EEPROM_EntErase	
 * --------------------------------------------------------------
 * 	parameter:		unsigned char page	- 	Page Address 
 * --------------------------------------------------------------
 * 	return:			void	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Erases Entire Page (32Bytes)
 * -------------------------------------------------------------------*/

void MEM_EEPROM_EntPageErase(unsigned char page)
{
	unsigned char i=0;

	for(i=0; i<32; i++)
	{
		MEM_EEPROM_LoadPageBuffer(i, 0xFF);		//load with dummy Bytes
	}

	MEM_EEPROM_PageErase(page);
}

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
 *	END of xmA_Memory_func.c
 ***********************************************************************/

