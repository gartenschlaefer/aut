/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    can_func.h
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	general Defines and Enumerations
* ------------------------------------------------------------------
*	Date:			    18.04.2013
* lastChanges:
\**********************************************************************/


/* ==================================================================*
 * 						Defines
 * ==================================================================*/
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
 * 						Function Commands
 * ------------------------------------------------------------------*/

typedef enum
{	_clean,		_exe, 			_init,  _reset, _add,
	_write, 	_saveValue,	_off,		_on,		_normal,
	_start,		_count,			_old,		_new,		_save,
	_set,		  _outSet,		_state,	_sym,		_error,
	_enabled,	_disabled,	_stop,	_calc,	_clear,
	_test,		_read,			_read1,	_read2,	_ready,
	_write1,	_write2,		_shot,	_shot1,	_shot2,
	_temp,		_error1,    _ton,   _ovent, _cvent,
	_cvent_s, _notav,     _mbar,  _mmbar, _right,
	_op,      _telnr,     _wrong, _debug, _entry,
	_inc,     _dec,       _check, _low,		_high,
	_noData,	_goOn
}t_FuncCmd;


/* ------------------------------------------------------------------*
 * 						CAN Commands
 * ------------------------------------------------------------------*/

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
