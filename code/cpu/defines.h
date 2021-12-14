// --
// defines
// author: Christian Walter
// uC: ATxmega128A1

// include guard
#ifndef DEFINES_H
#define DEFINES_H

/* ==================================================================*
 *            Configuration
 * ==================================================================*/

//*-* Configuration

// start page
#define START_PAGE AutoSetDown

// company id: 0 = Purator; 1 = HECS
#define COMPANY   1

// memory init set to zero
#define MEM_INIT  0

// SMS usage
#define SMS_ON    1

// debug: set to zero for no debugging with buzzer off, modem off, port ventile, data entry
#define DEBUG     0
#define DEB_BUZ   0
#define DEB_MODEM 0
#define DEB_PORT  0
#define DEB_ENTRY 0

// software version
#define SV1       1       //x.-.-
#define SV2       3       //-.x.-
#define SV3       11       //-.-.x

// default time: release time
#define DEF_SEC   0x30    //::30 = 0x30
#define DEF_MIN   0x30    //:30: = 0x30
#define DEF_HOUR  0x21    //08:: = 0x08
#define DEF_DAY   1       //Tag1 = Monday
#define DEF_DATE  0x19    //__.x.x
#define DEF_MONTH 0x10    //x.__.x
#define DEF_YEAR  0x20    //x.x.20__


/* ==================================================================*
 *            Stucts
 * ==================================================================*/

typedef struct _TelNr
{
  unsigned char id;
  char tel;
  unsigned char pos;
}TelNr;


/* ==================================================================*
 *            Enumerations
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Function Commands
 * ------------------------------------------------------------------*/

typedef enum
{ _clean, _exe, _init, _reset, _add, _write, _saveValue, _off, _on, _count, _new, _save,
  _set, _state, _sym, _error, _disabled, _clear, _read, _read1, _read2, _ready, _write1, 
  _write2, _shot, _shot1, _temp, _temp1, _ton, _ovent, _cvent, _notav, _mbar, _mmbar, 
  _right, _op, _telnr, _wrong, _debug, _entry, _dec, _check, _noData, _noUS, _noBoot, 
  _ok, _success, _mcp_fail, _autotext
}t_FuncCmd;

#endif