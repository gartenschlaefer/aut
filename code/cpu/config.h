// --
// defines
// author: Christian Walter
// uC: ATxmega128A1

// include guard
#ifndef DEFINES_H
#define DEFINES_H

/* ------------------------------------------------------------------*
 *            configuration
 * ------------------------------------------------------------------*/

//*-* configuration

// start page for automatic mode
#define AUTO_START_PAGE AutoSetDown

// company id: 0 = Purator; 1 = HECS
#define COMPANY 1

// memory init set to zero
#define MEM_INIT false

// use new relay connections (old ones are only for special cases)
#define USE_NEW_RELAY_CONNECTIONS 1

// SMS usage
#define SMS_ON true

// spring valve
#define SPRING_VALVE_ON true

// debug: set to zero for no debugging with buzzer off, modem off, port valve, data entry
#define DEBUG     false
#define DEB_BUZ   false
#define DEB_MODEM false
#define DEB_PORT  false
#define DEB_ENTRY false

// software version x.x.x
// 1.3.0 - SMS support
#define SV1 1
#define SV2 3
#define SV3 11

// default time: release time
#define DEF_SEC   0x30    // ::30 = 0x30
#define DEF_MIN   0x30    // :30: = 0x30
#define DEF_HOUR  0x21    // 08:: = 0x08
#define DEF_DAY   1       // day 1 = Monday
#define DEF_DATE  0x19    // __.x.x
#define DEF_MONTH 0x10    // x.__.x
#define DEF_YEAR  0x20    // x.x.20__

#endif