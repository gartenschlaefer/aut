/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    CAN-function-SourceFile
* ------------------------------------------------------------------
*	�C:        	  ATxmega128A1
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
#include "lcd_sym.h"

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
	static int err = 0;

	dlc = txB0[0];
	MCP2515_WriteReg(TXB0DLC, dlc);			  //DLC
	MCP2515_WriteReg(TXB0SIDH, txB0[1]);	//Address-H
	MCP2515_WriteReg(TXB0SIDL, 0x00);		  //Address-L

	while(dlc)
	{
		dlc--;
		MCP2515_WriteReg(0x36 + i, txB0[2 + i]);	//Data
		i++;
	}

	MCP2515_WriteReg(TXB0CTRL, TXREQ);				    //StartTransmission
	while(!(MCP2515_ReadReg(CANINTF) & TX0IF_bm))	//waitUntilSent?
	{
		// CAN Error
		if((MCP2515_ReadReg(TXB0CTRL) & 0xF0))
    {
			err = 0;
			return 1;
    }

    // MCP2515 failed
    if(err > 1000)
    {
      err = 0;
      LCD_Data_SonicWrite(_mcp_fail, 0);
      return 2;
    }
    err++;
	}
	err = 0;
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
 * 						Measure Temperature or Distance
 * -------------------------------------------------------------------*
 *	Parameter:		  --------------------------------------------------
 *	cmd: _init		  -	Start Measurement
 *	cmd: _exe		    -	Request Temp
 *	return:			    --------------------------------------------------
 *	sonic[0]: 3 	  -	State Received Temperature -> OK
 *	sonic[0]: 10.. 	-	TimeOut Error - Restart requird
 * ------------------------------------------------------------------*/

unsigned char *CAN_SonicQuery(t_FuncCmd cmd, t_UScmd us)
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
		err = 0;
		switch(us)
		{
      case _startTemp:  sonic[0] = _usTempReq; break;
      case _oneShot:
      case _5Shots:     sonic[0] = _usDistReq; break;
      default:          sonic[0] = _usWait; break;
		}
		TCE1_WaitMilliSec_Init(25);
		CAN_TxCmd(us);
	}
	//--------------------------------------------------Exe
	else if(cmd == _exe)
	{
		//--------------------------------------------Start+ACK
		if(sonic[0] == _usTempReq || sonic[0] == _usDistReq)
		{
			// Error check
			if(TCE1_Wait_Query())
			{
				err++;
				if(err >= 5)
        {
          sonic[0] = _usErrTimeout1;
          return sonic;
        }
				else if(sonic[0] == _usTempReq) CAN_TxCmd(_startTemp);
        else if(sonic[0] == _usDistReq) CAN_TxCmd(_5Shots);
			}
      // Check ACK
			ack = CAN_RxACK();
			switch(ack)
			{
        case _startTemp:
          if(sonic[0] == _usTempReq)
          {
            sonic[0] = _usTempAckOK;
            CAN_TxCmd(_readUSSREG);
            TCE1_WaitMilliSec_Init(25);
          }
          else
          {
            sonic[0] = _usErrWrongReq;
            return sonic;
          } break;

        case _5Shots:
          if(sonic[0] == _usDistReq)
          {
            sonic[0] = _usDistAckOK;
            CAN_TxCmd(_readUSSREG);
            TCE1_WaitMilliSec_Init(25);
          }
          else
          {
            sonic[0] = _usErrWrongReq;
            return sonic;
          } break;

        case _working:
          err = 0;
          TCE1_WaitMilliSec_Init(25);
          break;

        default: break;
			}
		}
		//--------------------------------------------CheckData
		else if(sonic[0] == _usDistAckOK || sonic[0] == _usTempAckOK)
		{
			// Error check
			if(TCE1_Wait_Query())
			{
				err++;
        if(err >= 5)
        {
          sonic[0] = _usErrTimeout2;
          return sonic;
        }
				CAN_TxCmd(_readUSSREG);
			}
      // Check if Data available
			rec = CAN_RxB0_Read();
			if(rec[0])
			{
				if(rec[2] == _readUSSREG)
				{
          err = 0;
          // Distance
					if(rec[3] & DISA && sonic[0] == _usDistAckOK)
					{
						sonic[0] = _usDistAv;
						CAN_TxCmd(_readDistance);
						TCE1_WaitMilliSec_Init(25);
					}
					// Temperature
					else if(rec[3] & TEMPA && sonic[0] == _usTempAckOK)
          {
            sonic[0] = _usTempAv;
						CAN_TxCmd(_readTemp);
						TCE1_WaitMilliSec_Init(25);
          }
				}
			}
		}
		//--------------------------------------------ReadData
		else if(sonic[0] == _usDistAv || sonic[0] == _usTempAv)
		{
			// Error Check
			if(TCE1_Wait_Query())
			{
				err++;
        if(err >= 5)
        {
          sonic[0] = _usErrTimeout3;
          return sonic;
        }
				else if(sonic[0] == _usDistAv) CAN_TxCmd(_readDistance);
				else if(sonic[0] == _usTempAv) CAN_TxCmd(_readTemp);
			}
      // Check data
			rec = CAN_RxB0_Read();
			if(rec[0])
			{
				if(rec[2] == _readDistance)
				{
					sonic[1] = rec[3];
					sonic[2] = rec[4];
					sonic[0] = _usDistSuccess;
				}
				else if(rec[2] == _readTemp)
        {
					sonic[1] = rec[3];
					sonic[2] = rec[4];
					sonic[0] = _usTempSuccess;
				}
			}
		}
		//--------------------------------------------Nothing
		else
    {
      TCE1_Stop();		//StopTimer
    }
	}
	return sonic;
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
