/**********************************************************************\
*	Author:			Red_Calcifer
* * ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			TWI-Functions-SourceFile
* ------------------------------------------------------------------										
*	Âµ-Controller:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	SourceCode for TWI Master Functions
* ------------------------------------------------------------------														
*	Date:			30.05.2011	
* 	lastChanges:	25.08.2011								
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"

#include "SL_TC_func.h"
#include "SL_TWI_func.h"
#include "SL_Display_driver.h"


/* ===================================================================*
 * 								TWIC
 * ===================================================================*/
/* ===================================================================*
 * 				TWI1 only for Display EA-DOGXL160-7
 * ===================================================================*/

/* ===================================================================*
 * 						FUNCTIONS Init + Error
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	TWI_Master_INIT
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	TWI Master Initialisierung:
 * 		-Enable
 * 		-Timeout
 * 		-Baudrate + Busstatus idle setzen	
 * -------------------------------------------------------------------*/
 
void TWI_Master_Init(void)
{
	TWIC.MASTER.CTRLA &=	~TWI_MASTER_ENABLE_bm;				//Master Disable
	PORTC.DIRSET = 			PIN0_bm | PIN1_bm;					//SDA, SCL Output
	PORTC.OUTCLR =			PIN0_bm | PIN1_bm;					//SDA, SCL GND

	TCC1_16MHzWaitMilliSec_Init(10);	
	while(TCC1_16MHzWait_Query());								//wait
			
	TWIC.MASTER.CTRLB= 		TWI_MASTER_TIMEOUT_DISABLED_gc;		//TimeoutDisabled

	//-----------------------------------------------------Boud=(fsys/2*ftwi)-5--
	TWIC.MASTER.BAUD = 35;								//200kHz/16MHz
	//TWIC.MASTER.BAUD = 21;							//300kHz/16MHz
	//TWIC.MASTER.BAUD = 15;							//400kHz/16MHz
	
	TWIC.MASTER.CTRLA=	TWI_MASTER_ENABLE_bm;			//enable Master
	TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
	TWIC.MASTER.CTRLC=	TWI_MASTER_CMD_STOP_gc;			//stopCondition 	
}




/*--------------------------------------------------------------------*
 * 	TWI_Master_Error-Conditions
 *	abgeschlossen:	13.05.2011
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			unsigned char Error
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Wenn Errors auftreten werden diese durch, definierte 
 *  Error-Makros im HeaderFile, ausgegeben ansonsten return 0
 *  Errors:
 *  	-Timer-Abbruch
 * 		-Arbitration Lost
 * 		-Bus-Error (z.B.: 2StartConditions, etc)
 * 		-Nack (Es wird eine StopCondition gesendet ohne return)
 * -------------------------------------------------------------------*/
 
unsigned char TWI_Master_Error(void)
{	
	if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_ARBLOST_bp)) //Arbitration
	{
		//sendNACK
		TWIC.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
							TWI_MASTER_CMD_STOP_gc;		//stopCondition 
										
		TWI_Master_Reset();								//TWI Reset	
		return E_TWI_ARBLOST;
	}
	
	if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_BUSERR_bp))	//Bus-Error
	{
		//sendNACK
		TWIC.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
							TWI_MASTER_CMD_STOP_gc;		//stopCondition 
							
		TWI_Master_Reset();								//TWI Reset
		return E_TWI_BUSERR;
	}
	
	if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_RXACK_bp))	//Nack
	{
		//sendNACK
		TWIC.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
							TWI_MASTER_CMD_STOP_gc;		//stopCondition 
		
		TWI_Master_Reset();								//TWI Reset	
		return E_TWI_NACK;
	}
	return 0;		//Keine Errors
}




/* ===================================================================*
 * 						Funktionen WRITE
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	TWI_Master_WriteString			MasterWriteMode
 *	abgeschlossen:	
 * --------------------------------------------------------------
 * 	parameter:		unsigned char address
 * 					unsigned char *sendByte[]	Pointer
 *					unsigned char i
 * --------------------------------------------------------------
 * 	return:			unsigned char (ERROR, Flags)
 * 	Descirption:	
 * --------------------------------------------------------------
 *	TWI Master Write Mode: 
 * 	Ein Byte wird vom Master zum Slave Ã¼bertragen	
 * -------------------------------------------------------------------*/
 
unsigned char TWI_Master_WriteString( 	unsigned char 	address, 
										unsigned char	*sendData,
										unsigned char 	i	)
{
	unsigned char errorMaster=0;
	unsigned char sendPuffer=0;
	
	TCC1_16MHzWaitMilliSec_Init(300);		//maximale Dauer der Ã?bertragung
	
	if(	(TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_IDLE_gc))
	{
		//--------------------------------------------------Send-Address----------
		errorMaster= TWI_Master_AddressWriteMode(address);	//Send Address
		if(errorMaster)	return errorMaster;					//return Error if occured
		//------------------------------------------------------------------------
		//ACK Received
		//--------------------------------------------------Beginn-Transaction-----
		while(i)
		{						
			sendPuffer= *sendData;						//update send Puffer
			errorMaster= TWI_Master_Send(sendPuffer);	//send Databyte
			if(errorMaster)	return errorMaster;			//return Error if occured
			sendData++;									//increase Pointer
			i--;										//decrease Counter
		}

		//------------------------------------------------Send Stop Condition-------
		TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;			//stopCondition 
		TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle

		return 		F_TWI_DATA_SENT;					//DataSent Flag
	}
	
	TWI_Master_Reset();									//TWI Reset
	TCC1_16MHz_Stop();									//Stop Timer

	return E_TWI_WAIT;
}



/* ===================================================================*
 * 						Funktionen READ
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	TWI_Master_ReadString		MasterReadMode
 *	abgeschlossen:				
 * --------------------------------------------------------------
 * 	parameter:		unsigned char address		
 * 					unsigned char* sendByte		Return Buffer
 *					unsigned char i				Bytes to Read
 * --------------------------------------------------------------
 * 	return:			Pointer auf Buffer
 * 	Descirption:	
 * --------------------------------------------------------------
 *	TWI Master Read Mode: 
 * 	Auf Anfrage einer Slave Addresse wird ein Datenbyte vom Slave angefordert
 * 	Im Hoeheren Byte des return stehen die Errors
 * -------------------------------------------------------------------*/
 
unsigned char *TWI_Master_ReadString(unsigned char address, unsigned char i)
{
	static unsigned char 	receiveData[10]= {0x00,0xFF};		//max length[], init
	unsigned char 			errorMaster1= 0;
	unsigned char 			*p_data;
	
	TCC1_16MHzWaitMilliSec_Init(300);		//maximale Dauer der Ã?bertragung

	if(	(TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_IDLE_gc) ||
	(	(TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_OWNER_gc)))
	{
		//--------------------------------------------------------Send-Address----------
		TWIC.MASTER.ADDR= (C_TWI_ADDRESS(address) | C_TWI_READ);
		//--------------------------------------------------------ACK Received

		//--------------------------------------------------------Read-Data-------------	
		while(i)
		{
			while(!(TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm))	//wait
			{
				//Timer
				if(TCC1_16MHzWait_Query())							//Timer
				{
					TWIC.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
										TWI_MASTER_CMD_STOP_gc;		//stopCondition 

					receiveData[0]= E_TWI_NO_SENT;		//Write ErrorCode
					p_data= &receiveData[0];			//update Pointer
					return p_data;						//Abbruch
				}
			}
			//.........................................................ERROR
			errorMaster1= TWI_Master_Error();			//Error Detection
			if(errorMaster1)	
			{
				receiveData[0]= errorMaster1;		//Write ErrorCode
				p_data= &receiveData[0];			//update Pointer
				return p_data;						//return Error if occured
			}
			//..............................................................

			receiveData[i]= TWIC.MASTER.DATA;	//Receive Byte

			//ACK or NACK?
			if(i==1) {
			//----------------------------------------SendNACK------------
			TWIC.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |	//NACK
								TWI_MASTER_CMD_STOP_gc;	//stopCondition 
			//-----------------------------------------------------------
			}
			else {
			//----------------------------------------SendACK------------
			TWIC.MASTER.CTRLC=	(TWIC.MASTER.CTRLC & ~(TWI_MASTER_ACKACT_bm)) |
								TWI_MASTER_CMD_RECVTRANS_gc;
			//-----------------------------------------------------------
			}
			i--;							//decrease Counter
		}
		//--------------------------------------------------------------
		TCC1_16MHz_Stop();				//Stop Timer

		receiveData[0]= 0;				//NO Errors occured
		p_data= &receiveData[0];		//update Pointer
		return p_data;					//return Pointer
	}

	if((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_UNKNOWN_gc))	
	{
		TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
	} 
	
	TCC1_16MHz_Stop();				//Stop Timer
	
	//Wait
	receiveData[0]= E_TWI_WAIT;		//Write ErrorCode
	p_data= &receiveData[0];		//update Pointer
	return p_data;					//return Error if occured
}






/* ===================================================================*
 * 						Funktionen MISC
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	TWI_Master_Send				MasterWriteMode
 *	abgeschlossen:				16.05.2011	
 * --------------------------------------------------------------
 * 	parameter:		unsigned char send
 * --------------------------------------------------------------
 * 	return:			unsigned char ERROR
 * 	Descirption:	
 * --------------------------------------------------------------
 *	write DataReg from master and wait until sent, with error
 *	detection,	if no error occured return 0
 * -------------------------------------------------------------------*/
 
unsigned char TWI_Master_Send(unsigned char send)
{
	unsigned char errorMaster=0;

	TWIC.MASTER.DATA= send;							//Send Data							
	while(!(TWIC.MASTER.STATUS & (1<<TWI_MASTER_WIF_bp)))	//wait
	{
		//Timer
		if(TCC1_16MHzWait_Query())							//Timer
		{
			TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;			//send Stop
			TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
			TWI_Master_Init();									//Master Init
			LCD_Init();                          				//Init LCD
			TCC1_16MHz_Stop();									//Stop Timer

			return E_TWI_NO_SENT;							//Abbruch
		}
	}
	//........................................ERROR.................
	errorMaster= TWI_Master_Error();		//Error Detection
	if(errorMaster) return errorMaster;		//return Error
	//..............................................................
	return 0;
}


/*--------------------------------------------------------------------*
 * 	TWI_Master_Address			MasterWriteMode
 *	abgeschlossen:				16.05.2011
 * --------------------------------------------------------------
 * 	parameter:		unsigned char f_address
 * --------------------------------------------------------------
 * 	return:			unsigned char ERROR
 * 	Descirption:	
 * --------------------------------------------------------------
 *	write AddresReg and wait until sent, return errors or 0
 * -------------------------------------------------------------------*/
 
unsigned char TWI_Master_AddressWriteMode(unsigned char f_address)
{
	unsigned char errorMaster=0;

	TWIC.MASTER.ADDR= (C_TWI_ADDRESS(f_address) | C_TWI_WRITE);	//writeAddress
	while(!(TWIC.MASTER.STATUS & (1<<TWI_MASTER_WIF_bp)))		//wait
	{
		//Timer
		if(TCC1_16MHzWait_Query())								//Timer
		{
			TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;			//send Stop
			TWI_Master_Reset();									//TWI Reset
			TCC1_16MHz_Stop();									//Stop Timer

			return E_TWI_NO_SENT;								//Abbruch
		}
	}
	//........................................ERROR.................
	errorMaster= TWI_Master_Error();		//Error Detection
	if(errorMaster) return errorMaster;		//return Error
	//..............................................................
	return 0;
}


/* -------------------------------------------------------------------*
 * 						TWI Reset
 * -------------------------------------------------------------------*/

void TWI_Master_Reset(void)
{
	TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
	TWI_Master_Init();									//Master Init
}











/* ===================================================================*
 * 								TWID
 * ===================================================================*/
/* ===================================================================*
 * 			TWI2 for Temp Sensor and external Components
 * ===================================================================*/

/* ===================================================================*
 * 						FUNCTIONS Init + Error
 * ===================================================================*/

void TWI2_Master_Init(void)
{
	TWID.MASTER.CTRLA &=	~TWI_MASTER_ENABLE_bm;				//Master Disable
	PORTD.DIRSET = 			PIN0_bm | PIN1_bm;					//SDA, SCL Output
	PORTD.OUTCLR =			PIN0_bm | PIN1_bm;					//SDA, SCL GND

	TCC1_16MHzWaitMilliSec_Init(10);	
	while(TCC1_16MHzWait_Query());								//wait
			
	TWID.MASTER.CTRLB= 		TWI_MASTER_TIMEOUT_DISABLED_gc;		//TimeoutDisabled

	//----------------------------------------------------Boud=(fsys/2*ftwi)-5)--
	//TWID.MASTER.BAUD = 155;							//50kHz/16MHz
	//TWID.MASTER.BAUD = 75;							//100kHz/16MHz
	TWID.MASTER.BAUD = 35;								//200kHz/16MHz
	
	TWID.MASTER.CTRLA=	TWI_MASTER_ENABLE_bm;			//enable Master
	TWID.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
	TWID.MASTER.CTRLC=	TWI_MASTER_CMD_STOP_gc;			//stopCondition 	
}



/* -------------------------------------------------------------------*
 * 						Error
 * -------------------------------------------------------------------*/
 
unsigned char TWI2_Master_Error(void)
{	
	if(TWID.MASTER.STATUS & (1<<TWI_MASTER_ARBLOST_bp)) //Arbitration
	{
		//sendNACK
		TWID.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
							TWI_MASTER_CMD_STOP_gc;		//stopCondition 
										
		TWI2_Master_Reset();								//TWI Reset	
		return E_TWI_ARBLOST;
	}
	
	if(TWID.MASTER.STATUS & (1<<TWI_MASTER_BUSERR_bp))	//Bus-Error
	{
		//sendNACK
		TWID.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
							TWI_MASTER_CMD_STOP_gc;		//stopCondition 
							
		TWI2_Master_Reset();								//TWI Reset
		return E_TWI_BUSERR;
	}
	
	if(TWID.MASTER.STATUS & (1<<TWI_MASTER_RXACK_bp))	//Nack
	{
		//sendNACK
		TWID.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
							TWI_MASTER_CMD_STOP_gc;		//stopCondition 
		
		TWI2_Master_Reset();								//TWI Reset	
		return E_TWI_NACK;
	}
	return 0;		//Keine Errors
}




/* -------------------------------------------------------------------*
 * 						Write String
 * -------------------------------------------------------------------*/
 
unsigned char TWI2_Master_WriteString( 	unsigned char 	address, 
										unsigned char	*sendData,
										unsigned char 	i	)
{
	unsigned char errorMaster=0;
	unsigned char sendPuffer=0;
	
	TCC1_16MHzWaitMilliSec_Init(300);		//maximale Dauer der Ã?bertragung
	
	if(	(TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_IDLE_gc))
	{
		//--------------------------------------------------Send-Address----------
		errorMaster= TWI2_Master_AddressWriteMode(address);	//Send Address
		if(errorMaster)	return errorMaster;					//return Error if occured
		//------------------------------------------------------------------------
		//ACK Received
		//--------------------------------------------------Beginn-Transaction-----
		while(i)
		{						
			sendPuffer= *sendData;						//update send Puffer
			errorMaster= TWI2_Master_Send(sendPuffer);	//send Databyte
			if(errorMaster)	return errorMaster;			//return Error if occured
			sendData++;									//increase Pointer
			i--;										//decrease Counter
		}

		//------------------------------------------------Send Stop Condition-------
		TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;			//stopCondition 
		TWID.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle

		return 		F_TWI_DATA_SENT;					//DataSent Flag
	}
	
	TWI2_Master_Reset();									//TWI Reset
	TCC1_16MHz_Stop();									//Stop Timer

	return E_TWI_WAIT;
}



/* -------------------------------------------------------------------*
 * 						Read String
 * -------------------------------------------------------------------*/
 
unsigned char *TWI2_Master_ReadString(unsigned char address, unsigned char i)
{
	static unsigned char 	receiveData[10]= {0x00,0xFF};		//max length[], init
	unsigned char 			errorMaster1= 0;
	unsigned char 			*p_data;
	
	TCC1_16MHzWaitMilliSec_Init(300);		//maximale Dauer der Ã?bertragung

	if(	(TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_IDLE_gc) ||
	(	(TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_OWNER_gc)))
	{
		//--------------------------------------------------------Send-Address----------
		TWID.MASTER.ADDR= (C_TWI_ADDRESS(address) | C_TWI_READ);
		//--------------------------------------------------------ACK Received

		//--------------------------------------------------------Read-Data-------------	
		while(i)
		{
			while(!(TWID.MASTER.STATUS & TWI_MASTER_RIF_bm))	//wait
			{
				//Timer
				if(TCC1_16MHzWait_Query())							//Timer
				{
					TWID.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |		//NACK
										TWI_MASTER_CMD_STOP_gc;		//stopCondition 

					receiveData[0]= E_TWI_NO_SENT;		//Write ErrorCode
					p_data= &receiveData[0];			//update Pointer
					return p_data;						//Abbruch
				}
			}
			//.........................................................ERROR
			errorMaster1= TWI2_Master_Error();			//Error Detection
			if(errorMaster1)	
			{
				receiveData[0]= errorMaster1;		//Write ErrorCode
				p_data= &receiveData[0];			//update Pointer
				return p_data;						//return Error if occured
			}
			//..............................................................

			receiveData[i]= TWID.MASTER.DATA;	//Receive Byte

			//ACK or NACK?
			if(i==1) {
			//----------------------------------------SendNACK------------
			TWID.MASTER.CTRLC=	TWI_MASTER_ACKACT_bm |	//NACK
								TWI_MASTER_CMD_STOP_gc;	//stopCondition 
			//-----------------------------------------------------------
			}
			else {
			//----------------------------------------SendACK------------
			TWID.MASTER.CTRLC=	(TWID.MASTER.CTRLC & ~(TWI_MASTER_ACKACT_bm)) |
								TWI_MASTER_CMD_RECVTRANS_gc;
			//-----------------------------------------------------------
			}
			i--;							//decrease Counter
		}
		//--------------------------------------------------------------
		TCC1_16MHz_Stop();				//Stop Timer

		receiveData[0]= 0;				//NO Errors occured
		p_data= &receiveData[0];		//update Pointer
		return p_data;					//return Pointer
	}

	if((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_UNKNOWN_gc))	
	{
		TWID.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
	} 
	
	TCC1_16MHz_Stop();				//Stop Timer
	
	//Wait
	receiveData[0]= E_TWI_WAIT;		//Write ErrorCode
	p_data= &receiveData[0];		//update Pointer
	return p_data;					//return Error if occured
}






/* -------------------------------------------------------------------*
 * 						Built in Functions
 * -------------------------------------------------------------------*/
 
unsigned char TWI2_Master_Send(unsigned char send)
{
	unsigned char errorMaster=0;

	TWID.MASTER.DATA= send;							//Send Data							
	while(!(TWID.MASTER.STATUS & (1<<TWI_MASTER_WIF_bp)))	//wait
	{
		//Timer
		if(TCC1_16MHzWait_Query())							//Timer
		{
			TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;			//send Stop
			TWID.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
			TWI2_Master_Init();									//Master Init
			LCD_Init();                          				//Init LCD
			TCC1_16MHz_Stop();									//Stop Timer

			return E_TWI_NO_SENT;							//Abbruch
		}
	}
	//........................................ERROR.................
	errorMaster= TWI2_Master_Error();		//Error Detection
	if(errorMaster) return errorMaster;		//return Error
	//..............................................................
	return 0;
}
 
unsigned char TWI2_Master_AddressWriteMode(unsigned char f_address)
{
	unsigned char errorMaster=0;

	TWID.MASTER.ADDR= (C_TWI_ADDRESS(f_address) | C_TWI_WRITE);	//writeAddress
	while(!(TWID.MASTER.STATUS & (1<<TWI_MASTER_WIF_bp)))		//wait
	{
		//Timer
		if(TCC1_16MHzWait_Query())								//Timer
		{
			TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;			//send Stop
			TWI2_Master_Reset();									//TWI Reset
			TCC1_16MHz_Stop();									//Stop Timer

			return E_TWI_NO_SENT;								//Abbruch
		}
	}
	//........................................ERROR.................
	errorMaster= TWI2_Master_Error();		//Error Detection
	if(errorMaster) return errorMaster;		//return Error
	//..............................................................
	return 0;
}


/* -------------------------------------------------------------------*
 * 						TWI Reset
 * -------------------------------------------------------------------*/

void TWI2_Master_Reset(void)
{
	TWID.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;	//set Idle
	TWI2_Master_Init();									//Master Init
}




/*********************************************************************\
 * End of xmA_TWI_Master_func.c
\**********************************************************************/

