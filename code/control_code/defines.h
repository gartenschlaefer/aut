/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    defines
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Configuration, enums, defines, structs etc
* ------------------------------------------------------------------
*	Date:			    20.06.2011
* Changes:			23.12.2015
* 							19.10.2020 - new debug stuff
\**********************************************************************/

// Include guard
#ifndef DEFINES_H
#define DEFINES_H

/* ==================================================================*
 * 						Configuration
 * ==================================================================*/

//*-*Configuration

// Start Page
#define START_PAGE AutoSetDown

// CompanyID - 0 = Purator; 1 = HECS
#define COMPANY   1

// Memory Init set to zero
#define MEM_INIT  0

// SMS Usage
#define SMS_ON		1

// Debug set to zero for no debugging: general, buzzer off, modem off, port ventile, data entry
#define DEBUG     0
#define DEB_BUZ		0
#define DEB_MODEM	0
#define DEB_PORT	0
#define DEB_ENTRY	0

// SoftwareVersion
#define SV1       1				//x.-.-
#define SV2       3				//-.x.-
#define SV3       0				//-.-.x

// TimeDefault
#define DEF_SEC   0x30    //::30 = 0x30
#define DEF_MIN   0x30    //:30: = 0x30
#define DEF_HOUR  0x21    //08:: = 0x08
#define DEF_DAY   1       //Tag1 = Monday
#define DEF_DATE  0x19    //__.x.x
#define DEF_MONTH 0x10    //x.__.x
#define DEF_YEAR  0x20    //x.x.20__



/* ==================================================================*
 * 						Types
 * ==================================================================*/

typedef signed   char   int8;
typedef unsigned char   uint8;
typedef signed   short  int16;
typedef unsigned short  uint16;
typedef signed   long   int32;
typedef unsigned long   uint32;
typedef uint16          halDataAlign_t;



/* ==================================================================*
 * 						Stuct
 * ==================================================================*/

typedef struct _TelNr
{
  unsigned char id;
  char tel;
  unsigned char pos;
}TelNr;


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
{	_clean,		_exe, 			_init,  _reset,   _add,
	_write, 	_saveValue,	_off,		_on,		  _normal,
	_start,		_count,			_old,		_new,		  _save,
	_set,		  _outSet,		_state,	_sym,		  _error,
	_enabled,	_disabled,	_stop,	_calc,	  _clear,
	_test,		_read,			_read1,	_read2,	  _ready,
	_write1,	_write2,		_shot,	_shot1,	  _shot2,
	_temp,		_temp1,     _error1,    _ton,   _ovent,
	_cvent_s, _notav,     _mbar,  _mmbar,   _right,
	_op,      _telnr,     _wrong, _debug,   _entry,
	_inc,     _dec,       _check, _low,		  _high,
	_noData,	_goOn,      _noUS,  _noBoot,  _ok,
	_success, _mcp_fail,  _cvent, _autotext
}t_FuncCmd;

#endif



