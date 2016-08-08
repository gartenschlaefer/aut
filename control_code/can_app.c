/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    CAN-function-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceFile for CAN-Bus over MCP2515
* ------------------------------------------------------------------
*	Date:			    31.12.2012
* lastChanges:  08.02.2016
\**********************************************************************/


#include <avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "basic_func.h"
#include "tc_func.h"
#include "mcp2515_driver.h"
#include "at24c_driver.h"

#include "can_app.h"
#include "sonic_app.h"


/* ==================================================================*
 * 						FUNCTIONS Init
 * ==================================================================*/

void CAN_Init(void)
{
	MCP2515_Init();
	CAN_RxB0_Init();
	MCP2515_WriteReg(CANCTRL, 0x05);	//OutOfConfigMode2Normal
}


/* ------------------------------------------------------------------*
 * 						Receive Buffer Init
 * ------------------------------------------------------------------*/

void CAN_RxB0_Init(void)
{
	MCP2515_WriteReg(RXB0CTRL, 0x00);		//UseFilters

	MCP2515_WriteReg(RXF0SIDH, 0xFF);		//Write RXB0-FilterID-Reg
	MCP2515_WriteReg(RXF0SIDL, 0xFF);		//Write RXB0-FilterID-Reg
	MCP2515_WriteReg(RXM0SIDH, 0x00);		//Write RXB0-MaskID-Reg
	MCP2515_WriteReg(RXM0SIDL, 0x00);		//Write RXB0-MaskID-Reg

	MCP2515_WriteReg(RXB0D0, 0x00);			//Clear RXB0D0
}



/* ==================================================================*
 * 						FUNCTIONS Receive
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	CAN_RxB0_Read
 * --------------------------------------------------------------
 *	return[0]: dlc 	-	0: noMessage	-	1: Message
 *	return[1]: addr	-	Address High
 *	return[2]: cmd	-	Command
 *	return[3]: data	-	following DataBytes 	- return[3..10]
 * ------------------------------------------------------------------*/

unsigned char *CAN_RxB0_Read(void)
{
	static unsigned char rec[10];
	unsigned char i = 0;
	unsigned char dlc = 0;

  for(i = 0; i < 10; i++) rec[i] = 0x00;      //Init
	if(MCP2515_ReadReg(CANINTF) & RX0IF_bm)			//Is RxB0 filled?
	{
		dlc = (MCP2515_ReadReg(RXB0DLC) & 0x0F);	//ReadDLC
		rec[0] = dlc;								              //DLC
		rec[1] = MCP2515_ReadReg(RXB0SIDH);			  //Address
		rec[2] = MCP2515_ReadReg(RXB0D0);			    //CommandByte

		for(i = 3; i < (dlc + 2); i++)
		  rec[i] = MCP2515_ReadReg(0x64 + i);		//DataBytes
		MCP2515_WriteReg(CANINTF, 0x00);			  //Reset Flag
	}
	else rec[0] = 0x00;	//NoMessage
	return &rec[0];			//Return Buffer Address
}



/* ==================================================================*
 * 						FUNCTIONS Transmit
 * ==================================================================*/
/*-------------------------------------------------------------------*
 * 	CAN_TxB0_Write
 * --------------------------------------------------------------
 * 	return:			  - 1: Error      - 0: succeed
 *	txB0[0]: dlc 	-	0: noMessage	-	1: Message
 *	txB0[1]: addr	-	Address High
 *	txB0[2]: cmd	-	Command
 *	txB0[3]: data	-	following DataBytes 	- return[3..10]
 * ------------------------------------------------------------------*/

unsigned char CAN_TxB0_Write(unsigned char *txB0)
{
	unsigned char i = 0;
	unsigned char dlc = 0;
	unsigned char errMon = 0;
	static unsigned char err = 0;

	dlc = txB0[0];
	MCP2515_WriteReg(TXB0DLC, dlc);			  //DLC
	MCP2515_WriteReg(TXB0SIDH, txB0[1]);	//Address-H
	MCP2515_WriteReg(TXB0SIDL, 0x00);		  //Address-L

	while(dlc)								            //Datalength
	{
		dlc--;
		MCP2515_WriteReg(0x36 + i, txB0[2 + i]);	//Data
		i++;
	}

	MCP2515_WriteReg(TXB0CTRL, TXREQ);				    //StartTransmition
	while(!(MCP2515_ReadReg(CANINTF) & TX0IF_bm))	//waitUntilSent?
	{
		errMon = (MCP2515_ReadReg(TXB0CTRL) & 0xF0);
		if(errMon){
			err++;
			return errMon;}		//IfErrorReturn
	}
	return 0;
}



/* ==================================================================*
 * 						FUNCTIONS CAN Application
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TX - Command2UltraSonic
 * -------------------------------------------------------------------*
 *	txB0[0]: dlc 	-	0:noMessage	-	1:Message
 *	txB0[1]: addr	-	Address High
 *	txB0[2]: cmd	-	Command
 * ------------------------------------------------------------------*/

void CAN_TxCmd(t_UScmd cmd)
{
	unsigned char tx[3] = {1, 0x01, cmd};
	CAN_TxB0_Write(&tx[0]);
}


/* ------------------------------------------------------------------*
 * 						TX - UltraSonicStatusRegister command
 * ------------------------------------------------------------------*/

void CAN_TxUSSREG(unsigned char reg)
{
	unsigned char tx[4] = {1, 0x01, _writeUSSREG, reg};
	CAN_TxB0_Write(&tx[0]);
}


/* ------------------------------------------------------------------*
 * 						RX - ACK
 * ------------------------------------------------------------------*/

t_UScmd CAN_RxACK(void)
{
	unsigned char *rec;
	static unsigned char ack = 0;

	rec = CAN_RxB0_Read();

	if(rec[0])
	{
		switch(rec[2])
		{
			case _ack:				  ack++;	return _ack;
			case _oneShot:		  ack++;	return _oneShot;
			case _5Shots:			  ack++;	return _5Shots;
			case _startTemp:	  ack++;	return _startTemp;
			case _working:		  ack++;	return _working;
      case _readProgram:	ack++;  return _readProgram;
      case _program:      ack++;	return _program;
      case _boot:         ack++;	return _boot;
		}
	}
	return _wait;
}


/* ------------------------------------------------------------------*
 * 						LiveCheck
 * -------------------------------------------------------------------*
 *	Parameter	  -	CAN-Adresse
 *	return: 0 	-	No Ack Received
 *	return: 1 	-	Ack Received
 * ------------------------------------------------------------------*/

unsigned char CAN_LiveCheck(unsigned char addr)
{
	unsigned char tx[3] = {1, addr, _ack};
	CAN_TxB0_Write(&tx[0]);
	TCE1_WaitMilliSec_Init(10);
	while(!CAN_RxACK())	if(TCE1_Wait_Query()) return 0;
	return 1;
}


/* -------------------------------------------------------------------*
 * 						Measure Distance
 * -------------------------------------------------------------------*
 *	cmd: _init		  -	Restart Measurement
 *	cmd: _exe		    -	Measurement
 *	return:			    --------------------------------------------------
 *	sonic[0]: 3     -	State Received Distance -> OK
 *	sonic[0]: 10..  -	TimeOut Error - Restart requird
 * ------------------------------------------------------------------*/

unsigned char *CAN_SonicDistance(t_FuncCmd cmd, t_UScmd us)
{
	unsigned char *rec;						          //Pointer
	unsigned char ack = 0;
	static unsigned char err = 0;
	static unsigned char sonic[3] = {	0,		//state
									                  0,		//dataH
									                  0};		//dataL

	//--------------------------------------------------Init
	if(cmd == _init)
	{
		err = 0;												  //ErrorReset
		sonic[0] = 0;											//Start-Condition
		TCE1_WaitMilliSec_Init(25);	//SafetyTimer
		CAN_TxCmd(us);			              //CANTxCmd
	}
	//--------------------------------------------------Exe
	else if(cmd == _exe)
	{
		//--------------------------------------------Start+ACK
		if(sonic[0] == 0)
		{
			if(TCE1_Wait_Query())					  //ResendCmd
			{
				err++;
				if(err >= 5) sonic[0] = 10;			  //Error2Much
        CAN_TxCmd(us);			                //CANTxCmd
			}

			ack = CAN_RxACK();
			if(ack == _oneShot || ack == _5Shots)	//CANRxACK
			{
				err = 0;									          //ResetError
				sonic[0] = 1;							          //nextStep
				CAN_TxCmd(_readUSSREG);					    //RequestUSSREG2
				TCE1_WaitMilliSec_Init(25);	  //CAN-TimerInit
			}
			else if(ack == _working)				      //Working
			{
				err = 0;
				TCE1_WaitMilliSec_Init(25);
			}
		}
		//--------------------------------------------CheckDistanceData
		else if(sonic[0] == 1)
		{
			if(TCE1_Wait_Query())		        //ResendCmd
			{
				err++;
				if(err >= 5) sonic[0] = 11;	      //Error2Much
				CAN_TxCmd(_readUSSREG);			        //CANTxCmd
			}

			rec = CAN_RxB0_Read();			          //ReadCan
			if(rec[0])									          //checkDLC
			{
				if(rec[2] == _readUSSREG)		        //checkCMD
				{
          err = 0;								          //ResetError
					if(rec[3] & DISA)					        //CheckIfDistanceAv
					{
						sonic[0] = 2;						        //nextStep
						CAN_TxCmd(_readDistance);				//TxWriteCmd
						TCE1_WaitMilliSec_Init(25);	//CAN-TimerInit
					}
				}
			}
		}
		//--------------------------------------------ReadDistance
		else if(sonic[0] == 2)						//-ReadDistance
		{
			if(TCE1_Wait_Query())
			{
				err++;
				if(err >= 5) sonic[0] = 12;		//Error2Much
				CAN_TxCmd(_readDistance);			//TxWriteCmd
			}

			rec = CAN_RxB0_Read();					//ReadCan
			if(rec[0])
			{
				if(rec[2] == _readDistance)		//checkCMD
				{
					sonic[1] = rec[3]; 	//DataH
					sonic[2] = rec[4];	//DataL
					sonic[0] = 3;				//Received
				}
			}
		}
		else TCE1_Stop();		//StopTimer
	}
	return &sonic[0];
}


/* ------------------------------------------------------------------*
 * 						Measure Temperature
 * -------------------------------------------------------------------*
 *	Parameter:		  --------------------------------------------------
 *	cmd: _init		  -	Start Measurement
 *	cmd: _exe		    -	Request Temp
 *	return:			    --------------------------------------------------
 *	sonic[0]: 3 	  -	State Received Temperature -> OK
 *	sonic[0]: 10.. 	-	TimeOut Error - Restart requird
 * ------------------------------------------------------------------*/

unsigned char *CAN_SonicTemp(t_FuncCmd cmd)
{
	unsigned char *rec;						//Pointer
	unsigned char ack;
	static unsigned char err = 0;
	static unsigned char sonic[3] = { 0,		//state
                                    0,		//dataH
                                    0};		//dataL
	//--------------------------------------------Init
	if(cmd == _init)
	{
		err = 0;								          //ErrorReset
		sonic[0] = 0;						          //Start-Condition
		TCE1_WaitMilliSec_Init(25);	//SafetyTimer
		CAN_TxCmd(_startTemp);				    //CANTxCmd
	}
	//--------------------------------------------Exe
	else if(cmd == _exe)
	{
		//--------------------------------------------Start+ACK
		if(sonic[0] == 0)
		{
			if(TCE1_Wait_Query())				//ResendCmd
			{
				err++;
				if(err >= 5)	sonic[0] = 10;		//Error2Much
				CAN_TxCmd(_startTemp);					//CANTxCmd
			}

			ack = CAN_RxACK();
			if(ack == _startTemp)
			{
				err = 0;									        //ResetError
				sonic[0] = 1;							        //nextStep
				CAN_TxCmd(_readUSSREG);					  //RequestUSSREG2
				TCE1_WaitMilliSec_Init(25);	//CAN-TimerInit
			}
			else if(ack == _working)						    //Working
			{
				err = 0;
				TCE1_WaitMilliSec_Init(25);
			}
		}
		//--------------------------------------------CheckTempData
		else if(sonic[0] == 1)
		{
			if(TCE1_Wait_Query())		//ResendCmd
			{
				err++;
				if(err >= 5) sonic[0] = 11;	//Error2Much
				CAN_TxCmd(_readUSSREG);			//CANTxCmd
			}

			rec = CAN_RxB0_Read();				//ReadCan
			if(rec[0])									  //checkDLC
			{
				if(rec[2] == _readUSSREG)		//checkCMD
				{
					if(rec[3] & TEMPA)				//CheckIfDistanceAvailable
					{
						err = 0;								//ResetError
						sonic[0] = 2;						//nextStep
						CAN_TxCmd(_readTemp);				      //TxWriteCmd
						TCE1_WaitMilliSec_Init(25);	//CAN-TimerInit
					}
				}
			}
		}
		//--------------------------------------------ReadTemp
		else if(sonic[0] == 2)						//-ReadTemp
		{
			if(TCE1_Wait_Query())
			{
				err++;
				if(err >= 5) sonic[0] = 12;		//Error2Much
				CAN_TxCmd(_readTemp);					//TxWriteCmd
			}

			rec = CAN_RxB0_Read();			//ReadCan
			if(rec[0])
			{
				if(rec[2] == _readTemp)		//checkCMD
				{
					sonic[1] = rec[3]; 			//DataH
					sonic[2] = rec[4];			//DataL
					sonic[0] = 3;						//Received
				}
			}
		}
		else TCE1_Stop();				//StopTimerIfErrorOrReady
	}
	return &sonic[0];
}



/* ------------------------------------------------------------------*
 * 						Read Software Version
 * ------------------------------------------------------------------*/

unsigned char *CAN_SonicVersion(t_FuncCmd cmd)
{
	unsigned char *rec;						//Pointer
	static unsigned char err = 0;
	static unsigned char sonic[3] = { 0,		//state
                                    0,		//dataH
                                    0};		//dataL
	//--------------------------------------------Init
	if(cmd == _init)
	{
		err = 0;								          //ErrorReset
		sonic[0] = 0;						          //Start-Condition
		TCE1_WaitMilliSec_Init(25);	//SafetyTimer
		CAN_TxCmd(_sVersion);				      //CANTxCmd
	}
	//--------------------------------------------Exe
	else if(cmd == _exe)
	{
    if(TCE1_Wait_Query())		//ResendCmd
    {
      err++;
      if(err >= 5) sonic[0] = 11;	//Error2Much
      CAN_TxCmd(_sVersion);			  //CANTxCmd
    }

    rec = CAN_RxB0_Read();				//ReadCan
    if(rec[0])									  //checkDLC
    {
      if(rec[2] == _sVersion)		  //checkCMD
      {
        sonic[1] = rec[3]; 			//DataH
        sonic[2] = rec[4];			//DataL
        sonic[0] = 1;						//Received
        TCE1_Stop();			//StopTimerIfErrorOrReady
      }
    }
	}
	return &sonic[0];
}


/* ------------------------------------------------------------------*
 * 						Sonic Read Application Program
 * -------------------------------------------------------------------*
 *	Parameter:		  --------------------------------------------------
 *	cmd: _init		  -	Start Read App
 *	cmd: _exe		    -	Read Application Section from Sonic
 *	return:			    - state
 * ------------------------------------------------------------------*/

unsigned char CAN_SonicReadProgram(t_FuncCmd cmd)
{
	static unsigned char state = 0;
	unsigned char *rec;
  unsigned char data[128];
  unsigned char page = 0;
  unsigned char byte8 = 0;
  unsigned char byte = 0;
  int adr = 0;

	//--------------------------------------------Init
	if(cmd == _init)
	{
    state = 0;
		TCE1_WaitMilliSec_Init(TC_CAN_MS);	  //SafetyTimer
		CAN_TxCmd(_readProgram);		  //CANTxCmd
		state = 1;
	}
	//--------------------------------------------Exe
	else if(cmd == _exe)
	{
    //------------------------------------------------CheckBootloaderOK
    if(state == 1)
    {
      if(TCE1_Wait_Query()) state = 11;	    //Error NoBoot
		  if(CAN_RxACK() == _readProgram){
        TCE1_WaitMilliSec_Init(TC_CAN_MS);	//SafetyTimer
		    CAN_TxCmd(_readProgram);				    //CANTxCmd
		    state = 2;}
    }
    //------------------------------------------------ReadApplication
    else if(state == 2)
    {
      for(page = 0; page < 32; page++)  //32Pages = 4kB
      {
        LCD_WriteValue3_MyFont(17, 50, page);
        //--------------------------------------------FillBufferPage128
        for(byte8 = 0; byte8 < 128; byte8 += 8)
        {
          rec = CAN_RxB0_Read();
          while(!rec[0]){
            if(TCE1_Wait_Query()) return state = 12;
            rec = CAN_RxB0_Read();}	      //ReadCan

          for(byte = 0; byte < 8; byte++)
            data[byte8 + byte] = rec[byte + 2];

          //------------------------------------------checkFile
          if(!page && !byte8)
            if((rec[2] != 0x0C) && (rec[3] != 0x94)) return state = 13;

          TCE1_WaitMilliSec_Init(TC_CAN_MS);	    //SafetyTimer
          CAN_TxCmd(_readProgram);				        //CANTxCmd
        }
        WDT_RESET;
        adr = ((AT24C_BOOT_PAGE_OS + page) << 8); //SetPageAdr
        AT24C_WritePage(adr, &data[0]);             //WritePage
      }
      state = 4;
      CAN_TxCmd(_ack);				                    //CANTxCmd - END
    }
	}
	return state;
}


/* ------------------------------------------------------------------*
 * 						Sonic Write Application Program
 * -------------------------------------------------------------------*
 *	Parameter:		  --------------------------------------------------
 *	cmd: _init		  -	Start Write App
 *	cmd: _exe		    -	Write App from from AT24C to Sonic
 *	return:			    - state
 * ------------------------------------------------------------------*/

unsigned char CAN_SonicWriteProgram(t_FuncCmd cmd)
{
  unsigned char tx[10] = {8, 0x01, 0, 1, 2, 3, 4, 5, 6, 7};
  unsigned char *p_data;
	static unsigned char state = 0;
	unsigned char *rec;
  unsigned char page = 0;
  unsigned char byte8 = 0;
  unsigned char i = 0;
  int adr = 0;

	//--------------------------------------------Init
	if(cmd == _init)
	{
    state = 0;
		TCE1_WaitMilliSec_Init(TC_CAN_MS);	  //SafetyTimer
		CAN_TxCmd(_program);		  //CANTxCmd
		state = 1;
	}
	//--------------------------------------------Exe
	else if(cmd == _exe)
	{
    //------------------------------------------------CheckBootloaderOK
    if(state == 1)
    {
      if(TCE1_Wait_Query()) state = 11;	    //Error NoBoot
		  if(CAN_RxACK() == _program){
        TCE1_WaitMilliSec_Init(TC_CAN_MS);	//SafetyTimer
		    state = 2;}
    }
    //------------------------------------------------WriteApp
    else if(state == 2)
    {
      for(page = 0; page < 32; page++)  //32Pages = 4kB
      {
        WDT_RESET;
        LCD_WriteValue3_MyFont(17, 50, page);
        //--------------------------------------------Write1EEPage
        for(byte8 = 0; byte8 < 128; byte8 += 8)
        {
// TODO (chris#1#): check File
          adr = (((AT24C_BOOT_PAGE_OS + page) << 8) | byte8);
          p_data = AT24C_Read8Byte(adr);
          for(i = 0; i < 8; i++)
            tx[i + 2] = p_data[i];

          CAN_TxB0_Write(&tx[0]);
          TCE1_WaitMilliSec_Init(TC_CAN_MS);

          rec = CAN_RxB0_Read();
          while(!rec[0]){
            if(TCE1_Wait_Query()) return state = 12;
            rec = CAN_RxB0_Read();}
          if(rec[2] != _program)  return state = 12;
        }
      }
      state = 4;
      CAN_TxCmd(_ack);    //CANTxCmd - END
    }
	}
	return state;
}



/**********************************************************************\
 * End of file
\**********************************************************************/
