/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			DS1302-driver-SourceFile
* ------------------------------------------------------------------
*	Âµ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	SourceCode of DS1303 Timer IC Driver File for SL (Steuerung light)
* ------------------------------------------------------------------														
*	Date:			30.05.2011  	
* 	lastChanges:											
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_TC_func.h"
#include "SL_Display_driver.h"
#include "SL_DS1302_driver.h"


/* ===================================================================*
 * 						FUNCTIONS 
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	DS1302_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	INIT:	-	Set all Input
 * 			-	Set all LOW
 * 			-	Start Clock + Rücksetzen
 * -------------------------------------------------------------------*/

void DS1302_Init(void)
{
	DS_PORT.DIRCLR = DS_CE | DS_IO | DS_SCLK;		//all inputs
	DS_PORT.OUTCLR = DS_CE | DS_IO | DS_SCLK;		//all LOW
}


void DS1302_WriteDefault(void)
{
	DS1302_Write_Comp_OpHours(0);

	DS1302_WriteByte(W_SEC,		0x00);
	DS1302_WriteByte(W_MIN, 	0x23);
	DS1302_WriteByte(W_HOUR, 	0x14);

	DS1302_WriteByte(W_DAY, 	0x05);
	DS1302_WriteByte(W_DATE, 	0x16);
	DS1302_WriteByte(W_MONTH, 	0x07);
	DS1302_WriteByte(W_YEAR, 	0x12);
}



/*--------------------------------------------------------------------*
 * 	DS1302_ReadByte
 * --------------------------------------------------------------
 * 	parameter:		unsigend char readCMD	-	What should be read?
 * --------------------------------------------------------------
 * 	return:			unsigned char rData	-	Data from DS1302
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Reads Data from DS1302
 * -------------------------------------------------------------------*/

unsigned char DS1302_ReadByte(unsigned char readCMD)
{
	unsigned char rData=0;
	unsigned char out=0;
	unsigned char i=0;
	
	DS_PORT.DIRSET = DS_CE | DS_IO | DS_SCLK;		//all Output
	DS_PORT.OUTCLR = DS_CE | DS_IO | DS_SCLK;		//all LOW
	DS_PORT.OUTSET = DS_CE; 						//CE HIGH
	
	TC_DS1302_Wait();								//Wait 20us
	
	//-------------------------------------Write-readCMD----------------
	for(i=0; i<8; i++)
	{
		out= (readCMD>>i) & 0x01;
		if(out)	DS_PORT.OUTSET = DS_IO;				//IO HIGH
		else 	DS_PORT.OUTCLR = DS_IO;				//IO LOW
		
		DS_PORT.OUTSET = DS_SCLK;					//SCLK HIGH
		TC_DS1302_Wait();							//Wait 20us
		
		if(i<7)
		{
			DS_PORT.OUTCLR = DS_SCLK;				//SCLK LOW
			TC_DS1302_Wait();						//Wait 20us
		}
	}
	//------------------------------------------------------------------
	
	DS_PORT.DIRCLR = DS_IO;			//IO Input
	
	//-------------------------------------Read-Data--------------------
	for(i=0; i<8; i++)
	{
		DS_PORT.OUTCLR = DS_SCLK;					//SCLK LOW
		TC_DS1302_Wait();							//Wait 20us
		
		if(DS_PORT.IN & DS_IO)	rData|= (1<<i);		//IF DS HIGH
		else 					rData|= (0<<i);		//DS LOW
		
		DS_PORT.OUTSET = DS_SCLK;					//SCLK HIGH
		TC_DS1302_Wait();							//Wait 20us
	}
	//------------------------------------------------------------------
	
	DS_PORT.OUTCLR = DS_CE | DS_IO | DS_SCLK;		//all LOW
	DS_PORT.DIRCLR = DS_CE | DS_IO | DS_SCLK;		//all Input
	TCD1_16MHz_Stop();								//stop Timer
	
	return rData;
}




/*--------------------------------------------------------------------*
 * 	DS1302_Write
 * --------------------------------------------------------------
 * 	parameter:		unsigend char writeCMD	-	What should be written?
 * 					unsigned char wData	-	Data to write
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Data from DS1302
 * -------------------------------------------------------------------*/

void DS1302_WriteByte(unsigned char writeCMD, unsigned char wData)
{
	unsigned char out=0;
	unsigned char i=0;
	
	DS_PORT.DIRSET = DS_CE | DS_IO | DS_SCLK;		//all Output
	DS_PORT.OUTCLR = DS_CE | DS_IO | DS_SCLK;		//all LOW
	DS_PORT.OUTSET = DS_CE; 						//CE HIGH
	
	TC_DS1302_Wait();							//Wait 20us
	
	//-------------------------------------Write-writeCMD----------------
	for(i=0; i<8; i++)
	{
		out= (writeCMD>>i) & 0x01;
		if(out)	DS_PORT.OUTSET = DS_IO;			//IO HIGH
		else 	DS_PORT.OUTCLR = DS_IO;			//IO LOW
		
		DS_PORT.OUTSET = DS_SCLK;				//SCLK HIGH
		TC_DS1302_Wait();						//Wait 20us
		
		DS_PORT.OUTCLR = DS_SCLK;				//SCLK LOW
		TC_DS1302_Wait();						//Wait 20us
		
	}
	//------------------------------------------------------------------
	
	//-------------------------------------Read-Data--------------------
	for(i=0; i<8; i++)
	{
		out= (wData>>i) & 0x01;
		if(out)	DS_PORT.OUTSET = DS_IO;			//IO HIGH
		else 	DS_PORT.OUTCLR = DS_IO;			//IO LOW
		
		DS_PORT.OUTSET = DS_SCLK;				//SCLK HIGH
		TC_DS1302_Wait();						//Wait 20us

		DS_PORT.OUTCLR = DS_SCLK;				//SCLK LOW
		TC_DS1302_Wait();						//Wait 20us
	}
	//------------------------------------------------------------------
	
	DS_PORT.OUTCLR = DS_CE | DS_IO | DS_SCLK;		//all LOW
	DS_PORT.DIRCLR = DS_CE | DS_IO | DS_SCLK;		//all Input
	TCD1_16MHz_Stop();								//stop Timer
}



/*--------------------------------------------------------------------*
 * 	DS1302_ReadRAM
 * --------------------------------------------------------------
 * 	parameter:		unsigned char address	-	RAM Address
 * --------------------------------------------------------------
 * 	return:			unsigned char rData	-	RAM Data
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Returns Data at address from DS1302 internal RAM
 * -------------------------------------------------------------------*/

unsigned char DS1302_ReadRAM(unsigned char address)
{
	unsigned char readRAM= 0;
	readRAM= DS1302_ReadByte(RAM_READ_AT(address));
	
	return readRAM;
}



/*--------------------------------------------------------------------*
 * 	DS1302_WriteRAM
 * --------------------------------------------------------------
 * 	parameter:		unsigned char address	-	RAM Address
 * 					unsigned char wData		-	RAM Data to write
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Data at address in DS1302 internal RAM
 * -------------------------------------------------------------------*/
 
void DS1302_WriteRAM(unsigned char address, unsigned char wData)
{
	DS1302_WriteByte(	RAM_WRITE_AT(address), wData);
}







/* ===================================================================*
 * 						FUNCTIONS Apps
 * ===================================================================*/


/* -------------------------------------------------------------------*
 * 						Compressor Op Hours
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	DS1302_Write_Comp_OpHours()
 * --------------------------------------------------------------
 * 	parameter:		int hours	-	Operating Hours to write
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Operating Hours to RAM
 * -------------------------------------------------------------------*/
 
void DS1302_Write_Comp_OpHours(int hours)
{
	unsigned char h=0;
	unsigned char l=0;

	l= (hours 		& 0x00FF);
	h= ((hours>>8) 	& 0x00FF);

	DS1302_WriteRAM(RAM_OP_ADDR_H, h);
	DS1302_WriteRAM(RAM_OP_ADDR_L, l);
}



/*--------------------------------------------------------------------*
 * 	DS1302_Write_Comp_OpHours()
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			int hours	-	Read Operating Hours
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Reads Operating Hours from RAM
 * -------------------------------------------------------------------*/
 
int DS1302_Read_Comp_OpHours(void)
{
	unsigned char h=0;
	unsigned char l=0;

	h= DS1302_ReadRAM(RAM_OP_ADDR_H);
	l= DS1302_ReadRAM(RAM_OP_ADDR_L);

	return ((h<<8) | l);
}




/* -------------------------------------------------------------------*
 * 						Time and Date
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	DS1302_LCD_WriteTime
 * --------------------------------------------------------------
 * 	parameter:		unsigned char row	-	Placment Row
 *					unsigned char col	-	Placment Collumn
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Writes Time on LCD
 * -------------------------------------------------------------------*/

void DS1302_LCD_WriteTime(t_FuncCmd cmd)
{
	unsigned char time=0;
	unsigned char wTime=0;	
	
	switch(cmd)
	{
		case _init:		
			
			LCD_WriteMyFont(2, 128, 10);				//:  Time
			LCD_WriteMyFont(2, 140, 10); 				//:	 Time
			//----------------------------------------------------Hour-----
			time= DS1302_ReadByte(R_HOUR);
			wTime= (((time>>4) & 0x0F)*10 + (time & 0x0F));
			LCD_WriteValue_MyFont2	(2,120, wTime);	
					
			//----------------------------------------------------Min------
			time= DS1302_ReadByte(R_MIN);
			wTime= (((time>>4) & 0x0F)*10 + (time & 0x0F));
			LCD_WriteValue_MyFont2	(2,132, wTime);	

			//----------------------------------------------------Sec------
			time= DS1302_ReadByte(R_SEC);
			wTime= (((time>>4) & 0x0F)*10 + (time & 0x0F));
			LCD_WriteValue_MyFont2	(2,144, wTime);		
			break;

		case _exe:

			//----------------------------------------------------Sec------
			time= DS1302_ReadByte(R_SEC);
			wTime= (((time>>4) & 0x0F)*10 + (time & 0x0F));
			LCD_WriteValue_MyFont2	(2,144, wTime);		
			
			//----------------------------------------------------Min------
			if(!wTime){	time= DS1302_ReadByte(R_MIN);
						wTime= (((time>>4) & 0x0F)*10 + (time & 0x0F));
						LCD_WriteValue_MyFont2	(2,132, wTime);}	
			else break;

			//----------------------------------------------------Hour-----
			if(!wTime){	time= DS1302_ReadByte(R_HOUR);
						wTime= (((time>>4) & 0x0F)*10 + (time & 0x0F));
						LCD_WriteValue_MyFont2	(2,120, wTime);}	
			else break;
			

			break;

		default:	break;
	}
}



/* -------------------------------------------------------------------*
 * 						Date
 * -------------------------------------------------------------------*/

void DS1302_LCD_WriteDate(void)
{
	unsigned char date=0;
	unsigned char wDate=0;

	LCD_WriteMyFont			(0, 128, 11);		//-  
	LCD_WriteMyFont			(0, 140, 11); 		//-	 
	LCD_WriteStringMyFont	(0, 144,"20"); 		//2011	
			
	date= DS1302_ReadByte(R_DATE);
	wDate= (((date>>4) & 0x0F)*10 + (date & 0x0F));
	LCD_WriteValue_MyFont2	(0,120, wDate);				//Date
	
	date= DS1302_ReadByte(R_MONTH);
	wDate= (((date>>4) & 0x0F)*10 + (date & 0x0F));
	LCD_WriteValue_MyFont2	(0,132, wDate);				//Month
	
	date= DS1302_ReadByte(R_YEAR);
	wDate= (((date>>4) & 0x0F)*10 + (date & 0x0F));
	LCD_WriteValue_MyFont2	(0,152, wDate);				//Year
}






 



/*********************************************************************\
 * End of SL_DS1302_diver.c
\**********************************************************************/

