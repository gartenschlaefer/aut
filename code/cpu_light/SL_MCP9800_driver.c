/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			MCP9800-driver-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	SourceCode of MCP9800 Temperature Sensor Driver File for SL
	ToDo: 			-	Error implementation
* ------------------------------------------------------------------														
*	Date:			30.05.2011  	
* 	lastChanges:	06.06.2011										
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_Display_driver.h"
#include "SL_MCP9800_driver.h"
#include "SL_TWI_func.h"



/* ===================================================================*
 * 						FUNCTIONS 
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	MCP9800_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	INIT:	-	Set Config Reg
 * 			-	Alert Input
 * -------------------------------------------------------------------*/

void MCP9800_Init(void)
{
	unsigned char init=		(	MCP_ONESHOT_OFF 	|
								MCP_RES				|
								MCP_FAULTQ			|
								MCP_ALERTPOL		|
								MCP_COMP_INT		|
								MCP_SHUTDOWN_OFF		);

	MCP9800_SendByte(MCP_CONFREG, init);	//Write CONF-Reg	
	MCP_PORT.DIRCLR=	MCP_ALERT;					//Alert Input
}



/*--------------------------------------------------------------------*
 * 	MCP9800_OneShot
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	One Conversion in Shutdown Mode
 * 	Remain in Shutdown Mode, go back to Init
 * -------------------------------------------------------------------*/

void MCP9800_OneShot(void)
{
	unsigned char shutdown=	(	MCP_ONESHOT_OFF	 	|
								MCP_RES				|
								MCP_FAULTQ			|
								MCP_ALERTPOL		|
								MCP_COMP_INT		|
								MCP_SHUTDOWN_ON			);

	MCP9800_SendByte(MCP_CONFREG, shutdown);						//Shutdown Mode
	MCP9800_SendByte(MCP_CONFREG, (shutdown | MCP_ONESHOT_ON));		//One Shot	
}



/*--------------------------------------------------------------------*
 * 	MCP9800_Write
 * --------------------------------------------------------------
 * 	parameter:		unsigned char send	-	Data to send
 * 					unsigned char i		-	Count of Data
 * --------------------------------------------------------------
 * 	return:			unsigned char error
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Sends Data to MCP9800 Temperatur Sensor
 * -------------------------------------------------------------------*/

void MCP9800_Write(unsigned char *send, unsigned char i)
{
	TWI2_Master_WriteString(MCP_ADDR, 
							send,
							i	);
}



/*--------------------------------------------------------------------*
 * 	MCP9800_Read
 * --------------------------------------------------------------
 * 	parameter:		unsigned char i		-	Count of Data
 * --------------------------------------------------------------
 * 	return:			unsigned char *rec	-	Received Data from MCP9800	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Receives Data from MCP9800 Temperatur Sensor
 * -------------------------------------------------------------------*/

unsigned char *MCP9800_Read(unsigned char i)
{
	static unsigned char *mcpRec;
	
	mcpRec= TWI2_Master_ReadString(MCP_ADDR, i);

	return mcpRec;
}




/* ===================================================================*
 * 						FUNCTIONS Write and Read
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	MCP9800_ReceiveByte
 * --------------------------------------------------------------
 * 	parameter:		unsinged char pointer	-	Reg-Pointer of MCP9800
 *						
 * --------------------------------------------------------------
 * 	return:			unsigned char *rec		-	Received Data from MCP9800	
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Receives Byte from MCP9800 Temperatur Sensor
 * -------------------------------------------------------------------*/

unsigned char MCP9800_ReceiveByte(unsigned char pointer)
{
	unsigned char send[]= {pointer};
	unsigned char *rec;
	unsigned char rData;

	MCP9800_Write(send, 1);		//Write Pointer
	rec= MCP9800_Read(1);		//Read Data
	rec++;
	rData= *rec;

	return rData;
}


/*--------------------------------------------------------------------*
 * 	MCP9800_SendByte
 * --------------------------------------------------------------
 * 	parameter:		unsinged char pointer	-	Reg-Pointer of MCP9800
 * 					unsigned char sData		-	Data to send
 * --------------------------------------------------------------
 * 	return:			
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Sends Byte to MCP9800 Temperatur Sensor
 * -------------------------------------------------------------------*/

void MCP9800_SendByte(unsigned char pointer, unsigned char sData)
{
	unsigned char send[]= {pointer, sData};		
	MCP9800_Write(send, 2);						//Write Pointer+Data
}



/* ===================================================================*
 * 						FUNCTIONS - Apps
 * ===================================================================*/

unsigned char MCP9800_ReadTemp(void)
{
	unsigned char temp=0;
	temp= MCP9800_ReceiveByte(0x00);
	return temp;
}

unsigned char MCP9800_PlusTemp(void)
{
	unsigned char temp=0;

	temp= MCP9800_ReadTemp();						//Read Temp
	if(temp & 0x80)	temp= 0;
	temp= (temp & 0x7F);

	return temp;
}

unsigned char MCP9800_Temp(void)
{
	unsigned char temp=0;

	temp= MCP9800_ReadTemp();						//Read Temp
	if(temp & 0x80)	temp= -temp;
	temp= (temp & 0x7F);

	return temp;
}

void MCP9800_WriteTemp(void)
{
	unsigned char temp=0;
	static unsigned char run=0;
	
	run++;
	if(run>10)										//1s
	{
		run=0;
		temp= MCP9800_ReadTemp();					//Read Temp
		if(temp & 0x80)
		{	
			LCD_WriteStringFont(17,84,"-");			//write Minus
			temp= -temp;
		}	
		else LCD_ClrSpace(17, 84, 1, 6);			//Clear Minus
		temp= (temp & 0x7F);
		LCD_WriteValue2(17,90, temp);				//Write Temperature
	}
}





/*********************************************************************\
 * End of SL_DS1302_diver.c
\**********************************************************************/


