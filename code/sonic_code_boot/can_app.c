/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    can_app.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	CAN Application File for CAN-Participiants
* ------------------------------------------------------------------
*	Date:			    08.04.2013
* lastChanges:  10.04.2015
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "can_func.h"
#include "can_app.h"
#include "boot_func.h"
#include "boot_app.h"


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
 * 						TXMOB ACK
 * ------------------------------------------------------------------*/

void CAN_TXMOB_ACK_Boot(void)
{
	unsigned char object[] = {1, 0x00, _boot};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB program Ack
 * ------------------------------------------------------------------*/

void CAN_TXMOB_ACK_Program(void)
{
	unsigned char object[] = {1, 0x00, _program};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB read program Ack
 * ------------------------------------------------------------------*/

void CAN_TXMOB_ACK_ReadProgram(void)
{
	unsigned char object[] = {1, 0x00, _readProgram};
	CAN_TXMOB(object);
}


/* ------------------------------------------------------------------*
 * 						TXMOB Softwareversion
 * ------------------------------------------------------------------*/

void CAN_TXMOB_sVersion(void)
{
	unsigned char object[] = {3, 0x00, _sVersion, SONIC_TYPE, SVERS};
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



/* ==================================================================*
 * 						RX - Parser
 * ==================================================================*/

t_UScmd CAN_RxParser(t_UScmd state)
{
	unsigned char *p_rx;

	p_rx = CAN_RXMOB();		//RXMob
	if(p_rx[0])						//DataAvailable
	{
		//------------------------------------------------AddressFilter
		if(p_rx[1])
		{
			//----------------------------------------------CommandParser
			switch(p_rx[2])
			{
				case _boot: CAN_TXMOB_ACK_Boot();     break;

				case _app:  CAN_TXMOB_ACK();
                    Boot_Jump2App();          break;

        case _program:
          if(state == _wait){
            CAN_TXMOB_ACK_Program();
            return _program;}                 break;

        case _readProgram:
          if(state == _wait){
            CAN_TXMOB_ACK_ReadProgram();
            return _readProgram;}             break;

        case _sVersion: CAN_TXMOB_sVersion(); break;

        case _setCanAddress:
        case _oneShot:
        case _5Shots:
        case _startTemp:
        case _readUSSREG:
        case _readDistance:
        case _readTemp:
          Boot_Jump2App();                    break;

				default:			                        break;
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





/*********************************************************************\
 * End of file
\**********************************************************************/

