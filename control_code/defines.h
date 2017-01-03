/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    defines
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Configuration, enums, defines, structs etc
* ------------------------------------------------------------------
*	Date:			    20.06.2011
* lastChanges:	23.12.2015
\**********************************************************************/

/* ==================================================================*
 * 						Configuration
 * ==================================================================*/

//*-*Configuration

// Start Page
#define START_PAGE AutoSetDown

// Company
#define COMPANY   0	      //CompanyID - 0=Purator; 1=HECS

// Memory Init
#define MEM_INIT  0       //and LCD Calibration

// Debug
#define DEBUG     1

// SoftwareVersion
#define SV1       1	      //x.-.-
#define SV2       2	      //-.x.-
#define SV3       9	      //-.-.x

// TimeDefault
#define DEF_SEC   0x30    //::30 = 0x30
#define DEF_MIN   0x30    //:30: = 0x30
#define DEF_HOUR  0x12    //08:: = 0x08
#define DEF_DAY   7       //Tag1 = Montag
#define DEF_DATE  0x19    //__.x.x
#define DEF_MONTH 0x09    //x.__.x
#define DEF_YEAR  0x16    //x.x.20__



/* ==================================================================*
 * 						Types
 * ==================================================================*/

typedef signed   char   int8;
typedef unsigned char   uint8;
typedef signed   short  int16;
typedef unsigned short  uint16;
typedef signed   long   int32;
typedef unsigned long   uint32;
typedef unsigned char   bool;
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
	_temp,		_error1,    _ton,   _ovent,   _cvent,
	_cvent_s, _notav,     _mbar,  _mmbar,   _right,
	_op,      _telnr,     _wrong, _debug,   _entry,
	_inc,     _dec,       _check, _low,		  _high,
	_noData,	_goOn,      _noUS,  _noBoot,  _ok,
	_success
}t_FuncCmd;




/**********************************************************************\
 * End of file
\**********************************************************************/



