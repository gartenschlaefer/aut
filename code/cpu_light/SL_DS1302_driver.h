/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			DS1302-driver-HeaderFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	Header of DS1303 Timer IC Driver File for SL (Steuerung light)
* ------------------------------------------------------------------														
*	Date:			30.05.2011  	
* 	lastChanges:											
\**********************************************************************/

#include<avr/io.h>

/* ===================================================================*
 * 						DEFINES
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						PORT
 * -------------------------------------------------------------------*/

#define DS_PORT			(PORTC)
#define DS_CE			(1<<7)
#define DS_IO			(1<<6)
#define DS_SCLK			(1<<5)

#define DS_CE_ON		(DS_PORT |= DS_CE)
#define DS_CE_OFF		(DS_PORT &= ~DS_CE)
#define DS_IO_ON		(DS_PORT |= DS_IO)
#define DS_IO_OFF		(DS_PORT &= ~DS_IO)	
#define DS_SCLK_ON		(DS_PORT |= DS_SCLK)
#define DS_SCLK_OFF		(DS_PORT &= ~DS_SCLK)


/* -------------------------------------------------------------------*
 * 						Time Read CMD
 * -------------------------------------------------------------------*/

#define	R_SEC		0x81
#define R_MIN		0x83
#define R_HOUR		0x85
#define R_DATE		0x87
#define R_MONTH		0x89
#define R_DAY		0x8B
#define R_YEAR		0x8D


/* -------------------------------------------------------------------*
 * 						Time Write CMD
 * -------------------------------------------------------------------*/

#define	W_SEC		0x80
#define W_MIN		0x82
#define W_HOUR		0x84
#define W_DATE		0x86
#define W_MONTH		0x88
#define W_DAY		0x8A
#define W_YEAR		0x8C


/* -------------------------------------------------------------------*
 * 						Write Protection
 * -------------------------------------------------------------------*/

#define R_WP		0x8F
#define W_WP		0x8E
#define WP_ON		0x80
#define WP_OFF		0x00


/* -------------------------------------------------------------------*
 * 						RAM
 * -------------------------------------------------------------------*/

#define RAM_ON				(3<<6)
#define RAM_R				(1<<0)
#define RAM_W				(0<<0)

#define RAM_READ_AT(addr)	(RAM_ON	| (addr<<1) | RAM_R)
#define RAM_WRITE_AT(addr)	(RAM_ON	| (addr<<1) | RAM_W)

#define RAM_OP_ADDR_H		(0x01)
#define RAM_OP_ADDR_L		(0x00)



/* ===================================================================*
 * 						FUCNTION API
 * ===================================================================*/
 
void 			DS1302_Init					(void);
void 			DS1302_WriteDefault			(void);

unsigned char 	DS1302_ReadByte				(unsigned char readCMD);

void 			DS1302_WriteByte			(unsigned char writeCMD, 
											 unsigned char wData);

unsigned char 	DS1302_ReadRAM				(unsigned char address);
void 			DS1302_WriteRAM				(unsigned char address, unsigned char wData);


void 			DS1302_Write_Comp_OpHours	(int hours);
int 			DS1302_Read_Comp_OpHours	(void);

void 			DS1302_LCD_WriteTime		(t_FuncCmd cmd);
void 			DS1302_LCD_WriteDate		(void);







/*********************************************************************\
 * End of SL_DS1302_diver.h
\**********************************************************************/

