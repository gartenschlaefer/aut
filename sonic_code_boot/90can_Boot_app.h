/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    90can_Boot_func.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Bootloader Functions Applications
* ------------------------------------------------------------------
*	Date:			    02.01.2016
* lastChanges:
\**********************************************************************/

/* ===================================================================*
 * 						Defines
 * ===================================================================*/

//*-*Configuration

#define SVERS         0x01    //SortwareVersion x.x
#define SONIC_TYPE    0x02    //Boot-Type



/* ===================================================================*
 * 						FUNSCTION-API
 * ===================================================================*/

t_UScmd Boot_Program_App(void);
t_UScmd Boot_Read_App(void);
void Boot_Write4Words2CAN(unsigned char page, unsigned char word);






/***********************************************************************
 *	End of file
 ***********************************************************************/

