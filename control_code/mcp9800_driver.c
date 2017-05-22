/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    MCP9800-driver-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceCode of MCP9800 Temperature Sensor Driver File for ICT
* ------------------------------------------------------------------
*	Date:			    30.05.2011
* lastChanges:	06.06.2011
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "mcp9800_driver.h"
#include "twi_func.h"



/* ==================================================================*
 * 						FUNCTIONS
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MCP9800_Init
 * ------------------------------------------------------------------*/

void MCP9800_Init(void)
{
	unsigned char init =	(	MCP_ONESHOT_OFF |
                          MCP_RES				  |
                          MCP_FAULTQ			|
                          MCP_ALERTPOL		|
                          MCP_COMP_INT		|
                          MCP_SHUTDOWN_OFF		);

	MCP9800_SendByte(MCP_CONFREG, init);	//Write CONF-Reg
	MCP_PORT.DIRCLR =	MCP_ALERT;					//Alert Input
}


/*-------------------------------------------------------------------*
 * 	MCP9800_OneShot
 * --------------------------------------------------------------
 *	One Conversion in Shutdown Mode
 * 	Remain in Shutdown Mode, go back to Init
 * ------------------------------------------------------------------*/

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



/* ==================================================================*
 * 						FUNCTIONS Write and Read
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MCP9800_ReceiveByte
 * --------------------------------------------------------------
 *	Receives Byte from MCP9800 Temperatur Sensor
 * ------------------------------------------------------------------*/

unsigned char MCP9800_ReceiveByte(unsigned char pointer)
{
	unsigned char send[] = {pointer};
	unsigned char *rec;
	unsigned char rData;

  TWI2_Master_WriteString(MCP_ADDR, send, 1);
	rec = TWI2_Master_ReadString(MCP_ADDR, 1);	//Read Data
	rec++;
	rData = *rec;
	return rData;
}


/*-------------------------------------------------------------------*
 * 	MCP9800 SendByte
 * --------------------------------------------------------------
 *	Sends Byte to MCP9800 Temperatur Sensor
 * ------------------------------------------------------------------*/

void MCP9800_SendByte(unsigned char pointer, unsigned char sData)
{
	unsigned char send[]= {pointer, sData};
  TWI2_Master_WriteString(MCP_ADDR, send, 2);
}



/* ==================================================================*
 * 						FUNCTIONS - Apps
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MPC9800 - Read Temperature (only PlusTemp)
 * ------------------------------------------------------------------*/

unsigned char MCP9800_PlusTemp(void)
{
	unsigned char temp = 0;

	temp = MCP9800_ReceiveByte(0x00);   //ReadTempFromIC
	if(temp & 0x80)	temp = 0;           //Minus?
	temp = (temp & 0x7F);

	return temp;
}


/*-------------------------------------------------------------------*
 * 	MPC9800 - WriteTemp
 * ------------------------------------------------------------------*/

void MCP9800_WriteTemp(void)
{
	unsigned char temp = 0;
	static unsigned char run = 0;

	run++;
	if(run > 10)										//1s
	{
		run = 0;
    temp = MCP9800_ReceiveByte(0x00);   //ReadTempFromIC
		if(temp & 0x80)
		{
			LCD_WriteStringFont(17,84,"-");			//write Minus
			temp = -temp;
		}
		else LCD_ClrSpace(17, 84, 2, 6);			//Clear Minus
		temp = (temp & 0x7F);
		LCD_WriteValue2(17,90, temp);				//Write Temperature
	}
}





/*********************************************************************\
 * End of file
\**********************************************************************/


