/*********************************************************************\
*	Author:			  Christian Walter
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "can_func.h"
#include "can_app.h"


/* ==================================================================*
 * 						TXMOB apps
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TXMOB Working
 * ------------------------------------------------------------------*/

void CAN_TXMOB_Working(void)
{
	unsigned char object[] = {1, 0x00, _working};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB ACK
 * ------------------------------------------------------------------*/

void CAN_TXMOB_ACK(void)
{
	unsigned char object[] = {1, 0x00, _ack};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB ACK Start Temp
 * ------------------------------------------------------------------*/

void CAN_TXMOB_ACK_Temp(void)
{
	unsigned char object[] = {1, 0x00, _startTemp};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB ACK Boot
 * ------------------------------------------------------------------*/

void CAN_TXMOB_ACK_Boot(void)
{
	unsigned char object[] = {1, 0x00, _boot};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB US-StatusReg
 * ------------------------------------------------------------------*/

void CAN_TXMOB_USSREG(void)
{
	unsigned char object[] = {2, 0x00, _readUSSREG, CAN_USSREG(_read,0)};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB Softwareversion
 * ------------------------------------------------------------------*/

void CAN_TXMOB_sVersion(void)
{
	unsigned char object[] = {3, 0x00, _sVersion, SVERS};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB US-DataDistanceReg
 * ------------------------------------------------------------------*/

void CAN_TXMOB_USDDREG(void)
{
	unsigned char object[] = {3, 0x00, _readDistance, 0x00, 0x00};
	object[3] = ((CAN_USDDREG(_read,0) >> 8) & 0xFF);
	object[4] = (CAN_USDDREG(_read,0) & 0xFF);
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB US-DataTempReg
 * ------------------------------------------------------------------*/

void CAN_TXMOB_USDTREG(void)
{
	unsigned char object[] = {3, 0x00, _readTemp, 0x00, 0x00};
	object[3] = ((CAN_USDTREG(_read,0) >> 8) & 0xFF);
	object[4] = (CAN_USDTREG(_read,0) & 0xFF);
	CAN_TXMOB(object);
}



/* ==================================================================*
 * 						RX - Parser
 * ==================================================================*/

t_UScmd CAN_RxParser(t_UScmd state)
{
	unsigned char *p_rx;
	unsigned char object[4] = {	0,		//dlc
                              0,		//address
                              0,		//cmd
                              0};		//data

	p_rx = CAN_RXMOB();		//RXMob
	if(p_rx[0])						//DataAvailable
	{
		//------------------------------------------------ResolveVariables
		object[0] = p_rx[0];			//dlc
		object[1] = p_rx[1];			//address
		object[2] = p_rx[2];			//cmd
		object[3] = p_rx[3];			//data

		//------------------------------------------------AddressFilter
		if(object[1])
		{
			//----------------------------------------------CommandParser
			switch(object[2])
			{
				case _ack:				    CAN_TXMOB_ACK();					     break;
				case _setCanAddress:  CAN_RXMOb_SetAddr(object[3]);  break;

				case _startTemp:
				  if(state == _wait){
            CAN_TXMOB_ACK_Temp();
            return _readTemp;}
          else CAN_TXMOB_Working();			break;

				case _readUSSREG: CAN_TXMOB_USSREG();		break;

				case _readDistance:
          if(CAN_USSREG(_read,0) && DISA){
				    CAN_USSREG(_reset, DISA);
            CAN_TXMOB_USDDREG();}
          else CAN_TXMOB_Working();			break;

				case _readTemp:
				  if(CAN_USSREG(_read,0) && TEMPA){
            CAN_USSREG(_reset, TEMPA);
            CAN_TXMOB_USDTREG();}
          else CAN_TXMOB_Working();			break;

				case _writeUSSREG:  CAN_USSREG(_write, object[3]);	break;
				case _sVersion:     CAN_TXMOB_sVersion();           break;
				case _boot:         CAN_TXMOB_ACK_Boot();
				                    //Boot_Jump2Bootloader();
				                    break;
				default:			                                			break;
			}
		}
	}

	return state;
}



/* ==================================================================*
 * 						UltraSonic Register
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						UltraSonic StatusRegister
 * -------------------------------------------------------------------*
 *	DISA	(1<<0)	Data Distance available
 * 	TEMPA	(1<<1)	Temp available
 * 	MDIS	(1<<2)	MeasureDistance
 *	MTEMP	(1<<3)	MeasureTemp
 *	USRDY	(1<<4)	UltraSonic ReadyBit
 *	ERR		(1<<5)	ErrorBit
 * ------------------------------------------------------------------*/

unsigned char CAN_USSREG(t_FuncCmd cmd, unsigned char write)
{
	static unsigned char sreg = 0x00;

	switch(cmd)
	{
		case _init:		sreg = 0x00;		break;
		case _set:		sreg |= write;	break;
		case _reset:	sreg &= ~write;	break;
		case _clear:	sreg = 0x00;		break;
		case _write:	sreg = write;		break;
		case _read:		return sreg;		break;
		default:							        break;
	}
	return sreg;
}


/* ------------------------------------------------------------------*
 * 						UltraSonic DataDistanceRegister
 * ------------------------------------------------------------------*/

int CAN_USDDREG(t_FuncCmd cmd, int write)
{
	static int dreg = 0x0000;

	switch(cmd)
	{
		case _init:		dreg = 0x0000;	break;
		case _set:		dreg |= write;	break;
		case _reset:	dreg &= ~write;	break;
		case _clear:	dreg = 0x0000;	break;
		case _write:	dreg = write;		break;
		case _read:		return dreg;		break;
		default:							        break;
	}
	return dreg;
}


/* ------------------------------------------------------------------*
 * 						UltraSonic DataTempRegister
 * ------------------------------------------------------------------*/

int CAN_USDTREG(t_FuncCmd cmd, int write)
{
	static int dreg = 0x0000;

	switch(cmd)
	{
		case _init:		dreg = 0x0000;	break;
		case _set:		dreg |= write;	break;
		case _reset:	dreg &= ~write;	break;
		case _clear:	dreg = 0x0000;	break;
		case _write:	dreg = write;		break;
		case _read:		return dreg;		break;
		default:							        break;
	}
	return dreg;
}




/*********************************************************************\
 * End of file
\**********************************************************************/

