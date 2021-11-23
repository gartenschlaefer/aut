/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Memory-func-HeaderFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Header of xmA_Memory_func.c
* ------------------------------------------------------------------														
*	Date:			13.07.2011  	
* 	lastChanges:												
\**********************************************************************/



/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Basics
 * -------------------------------------------------------------------*/

void MEM_EEPROM_Init			(void);
void MEM_EEPROM_ExecuteCMD		(void);

void MEM_EEPROM_LoadPageBuffer	(unsigned char byte, unsigned char eeData);
void MEM_EEPROM_ErasePageBuffer	(void);

void MEM_EEPROM_PageErase		(unsigned char page);
void MEM_EEPROM_PageWrite		(unsigned char page);
void MEM_EEPROM_PageEraseWrite	(unsigned char page);

void MEM_EEPROM_Erase			(void);

unsigned char MEM_EEPROM_Read	(unsigned char page, unsigned char byte);

/* -------------------------------------------------------------------*
 * 						Write
 * -------------------------------------------------------------------*/

void MEM_EEPROM_WriteByte
(	
	unsigned char page,
	unsigned char byte, 
	unsigned char eeData	
);


/* -------------------------------------------------------------------*
 * 						Erase
 * -------------------------------------------------------------------*/

void MEM_EEPROM_EntPageErase	(unsigned char page);
void MEM_EEPROM_EntErase		(void);







/***********************************************************************
 *	END of xmA_Memory_func.h
 ***********************************************************************/

