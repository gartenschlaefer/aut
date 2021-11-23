/*********************************************************************\
* Author:       Christian Walter
* ------------------------------------------------------------------
* Project:      UltraSonic
* Name:         boot_func.h
* ------------------------------------------------------------------
* µ-Controler:  AT90CAN128/32
* Compiler:     avr-gcc (WINAVR 2010)
* Description:
* ------------------------------------------------------------------
* Bootloader Functions
* ------------------------------------------------------------------
* Date:         02.01.2016
* lastChanges:  13.02.2016
\**********************************************************************/



/* ===================================================================*
 *            FUNSCTION-API
 * ===================================================================*/

/* -------------------------------------------------------------------*
 *            BOOT Init
 * -------------------------------------------------------------------*/

void Boot_Init(void);
void Boot_Jump2App(void);


/* -------------------------------------------------------------------*
 *            Inline Assembler
 * -------------------------------------------------------------------*/

unsigned char Boot_ReadR0(void);
void Boot_RAMPZ_0(void);
void Boot_RAMPZ_1(void);
void Boot_WriteR0R1(int vR);


/* -------------------------------------------------------------------*
 *            Basics
 * -------------------------------------------------------------------*/

void Boot_ZPointer(unsigned char page, unsigned char word,
unsigned char byte);

unsigned char Boot_Read(unsigned char page, unsigned char word,
unsigned char byte);

void Boot_LoadPageBuffer_Word(unsigned char word, int fData);

void Boot_ErasePage(unsigned char page);
void Boot_WritePage(unsigned char page);







/***********************************************************************
 *  End of File
 ***********************************************************************/

