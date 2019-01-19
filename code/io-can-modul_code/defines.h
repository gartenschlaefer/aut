/*
 */



/* ==================================================================*
 * 						Defines
 * ==================================================================*/

//*-*
#define SVERS           0x10    //SortwareVersion x.x
#define DEBUG_FLAG			0

/* ------------------------------------------------------------------*
 * 	Useful macros for accessing single bytes of int and long variables
 * ------------------------------------------------------------------*/

#define BYTE3(var) *((unsigned char *) &var+3)
#define BYTE2(var) *((unsigned char *) &var+2)
#define BYTE1(var) *((unsigned char *) &var+1)
#define BYTE0(var) *((unsigned char *) &var)



/* ==================================================================*
 * 						Enumerations
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						General Function Commands
 * ------------------------------------------------------------------*/

typedef enum
{	_init,		_exe,		_set,		  _reset,
	_on,		  _off,		_enabled,	_disabled,
	_state,		_start,	_stop,		_overload,
	_charge,	_ready,	_readyOn,	_readyOff,
	_receive,	_send,	_sending,	_sent,
	_txok,		_rxok,	_pending,	_read,
	_clear,		_write
}t_FuncCmd;


typedef enum
{
	//--------------------------------------------------Parser
	_setCanAddress,
	_oneShot,			  _5Shots,		    _startTemp,
	_readDistance,	_readTemp,
	_readUSSREG,		_readUSCREG1,		_readUSCREG2,
	_writeUSSREG,		_writeUSCREG1,	_writeUSCREG2,
	_sVersion,
	_working,			  _ack,				    _wait,
  _boot,          _app,           _program,
	_readProgram
}t_UScmd;






/**********************************************************************\
 * End of file
\**********************************************************************/
