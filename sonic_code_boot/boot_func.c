/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    boot_func.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Bootloader Functions
* ------------------------------------------------------------------
*	Date:			    02.01.2016
* lastChanges:  13.02.2016
\**********************************************************************/

#include<avr/io.h>

/* ===================================================================*
 * 						Header
 * ===================================================================*/

#include "defines.h"
#include "boot_func.h"


/* ===================================================================*
 * 						FUNCTIONS Jump
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Jump to Application
 * -------------------------------------------------------------------*/

void Boot_Jump2App(void)
{
	asm volatile("jmp 0x0000");			//Jump Instruction
}


/* -------------------------------------------------------------------*
 * 						Jump to Bootloader
 * -------------------------------------------------------------------*/

void Boot_Jump2Bootloader(void)
{
	asm volatile("jmp 0x3000");			//Jump Instruction
}



/* ===================================================================*
 * 						FUNCTIONS Inline Assembler
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Inline Read R0
 * -------------------------------------------------------------------*/

unsigned char Boot_ReadR0(void)
{
	unsigned char read = 0x00;
	asm volatile(	"mov %[var0], r0"		:		//instruction + Plazhalter
					      [var0]"=d"(read)		:		//Output
								              			:		//Input
					      "r0");							    //Clobber
	return read;
}


/* -------------------------------------------------------------------*
 * 						Inline Write R0R1
 * -------------------------------------------------------------------*/

void Boot_WriteR0R1(int vR)
{
	asm volatile(	"movw r0, %[LR0]" 		:	//instruction + Plazhalter
							                				:	//Output
					          [LR0]"d"(vR)			:	//Input
					          "r0", "r1");				//Clobber
}



/* ===================================================================*
 * 						FUNCTIONS Basics
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	Boot_ZPointer
 * --------------------------------------------------------------
 *	Page Z[14:8]
 *	Word Z[7:1]
 *	Byte Z[0]
 * -------------------------------------------------------------------*/

void Boot_ZPointer(unsigned char page, unsigned char word, unsigned char byte)
{
	int	work = 0;
	unsigned char h = 0;
	unsigned char l = 0;

  work = (page << 8) | (word << 1);
	h = ((work >> 8) & 0x00FF);
	l = ((work & 0xFF) | byte);

  asm volatile(	"mov r30, %[L]"	"\n\t"  //load R30(L), R31(H)
					      "mov r31, %[H]"			:	  //instruction + Plazhalter
										              	:	  //Output
					      [L]"d"(l),
					      [H]"d"(h)			      :	  //Input
					      "r31", "r30");				  //Clobber
}


/* -------------------------------------------------------------------*
 * 						Flash Read one Byte
 * -------------------------------------------------------------------*/

unsigned char Boot_Read(unsigned char page, unsigned char word,
unsigned char byte)
{
	unsigned char fData = 0;

	asm volatile(	"push r0"	  "\n\t"  "push r1"	"\n\t"
                "push r30"  "\n\t"  "push r31");

	Boot_ZPointer(page, word, byte);		//Load Z Pointer

  SPMCSR = (1 << SPMEN);          //Enale SPM
	asm("lpm");

	fData = Boot_ReadR0();					    //Read Data @ R0

  asm volatile(	"pop r31"	  "\n\t"  "pop r30"	"\n\t"
                "pop r1"	  "\n\t"  "pop r0");

	return fData;
}


/* -------------------------------------------------------------------*
 * 						Load Page Buffer Word
 * -------------------------------------------------------------------*/

void Boot_LoadPageBuffer_Word(unsigned char word, int fData)
{
  asm volatile(	"push r0"	  "\n\t"  "push r1"	"\n\t"
                "push r30"  "\n\t"  "push r31");

	Boot_ZPointer(0, word, 0);  //load Z with word-Address
	Boot_WriteR0R1(fData);			//Write R0R1

	SPMCSR = (1 << SPMEN);          //Enable SPM
	asm("spm");
  while(SPMCSR & (1 << SPMEN));		//ready

  asm volatile(	"pop r31"	  "\n\t"  "pop r30"	"\n\t"
                "pop r1"	  "\n\t"  "pop r0");
}



/* ===================================================================*
 * 						Flash Page
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	          Flash ErasePage
 * -------------------------------------------------------------------*/

void Boot_ErasePage(unsigned char page)
{
  asm volatile(	"push r0"	  "\n\t"  "push r1"	"\n\t"
                "push r30"  "\n\t"  "push r31");

	Boot_ZPointer(page, 0, 0);			//load Z with page-Address

  SPMCSR = (1 << PGERS) | (1 << SPMEN);   //Enale SPM
	asm("spm");
  while(SPMCSR & (1 << SPMEN));		//ready

  asm volatile(	"pop r31"	  "\n\t"  "pop r30"	"\n\t"
                "pop r1"	  "\n\t"  "pop r0");
}


/*--------------------------------------------------------------------*
 * 	          Memory WritePage
 * -------------------------------------------------------------------*/

void Boot_WritePage(unsigned char page)
{
	asm volatile(	"push r0"	  "\n\t"  "push r1"	"\n\t"
                "push r30"  "\n\t"  "push r31");

	Boot_ZPointer(page, 0, 0);		          //load Z with page
  SPMCSR = (1 << PGERS) | (1 << SPMEN);   //Enale SPM
	asm("spm");                             //SPM
  while(SPMCSR & (1 << SPMEN));		        //ready

  Boot_ZPointer(page, 0, 0);		          //load Z with page
	SPMCSR = (1 << PGWRT) | (1 << SPMEN);	  //CMD Write
	asm("spm");                             //SPM
  while(SPMCSR & (1 << SPMEN));		        //ready

  asm volatile(	"pop r31"	  "\n\t"  "pop r30"	"\n\t"
                "pop r1"	  "\n\t"  "pop r0");
}








/***********************************************************************
 *	End of File
 ***********************************************************************/
