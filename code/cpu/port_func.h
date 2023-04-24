// --
// port functions

// include guard
#ifndef PORT_FUNC_H   
#define PORT_FUNC_H

#include <avr/io.h>
#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            PORT
 * ------------------------------------------------------------------*/

#define P_OPTO    (PORTH)
#define P_VALVE  (PORTJ)
#define P_RELAIS  (PORTK)


/* ------------------------------------------------------------------*
 *            optical coupler inputs
 * ------------------------------------------------------------------*/

#define OC1 (PIN7_bm)
#define OC2 (PIN6_bm)
#define OC3 (PIN5_bm)
#define OC4 (PIN4_bm)

#define IN_FLOAT_S3 (!(P_OPTO.IN & OC1))


/* ------------------------------------------------------------------*
 *            valve
 * ------------------------------------------------------------------*/

// valve outputs
#define ALL_OPEN  0x55
#define ALL_CLOSE 0xAA
#define O_RES     0x01
#define C_RES     0x02
#define O_MUD     0x04
#define C_MUD     0x08
#define O_AIR     0x10
#define C_AIR     0x20
#define O_CLRW    0x40
#define C_CLRW    0x80

#define V_AIR (1 << 0)
#define V_MUD (1 << 1)
#define V_CLW (1 << 2) 
#define V_RES (1 << 3)   


/* ------------------------------------------------------------------*
 *            relay output connections
 * ------------------------------------------------------------------*/

// new
#if USE_NEW_RELAY_CONNECTIONS

  // define new relay connections R1...R8
  #define R_COMP        (1 << 4)
  #define R_CLEARWATER  (1 << 5)
  #define R_ALARM       (1 << 6)
  #define R_VENTILATOR  (1 << 7)
  #define R_INFLOW1     (1 << 0)
  #define R_INFLOW2     (1 << 1)
  #define R_PHOSPHOR    (1 << 2)
  #define R_EXT_COMP    (1 << 3)

// old
#else
  
  // define very old relay connections R1...R8
  #define R_COMP        (1 << 5)
  #define R_PHOSPHOR    (1 << 4)
  #define R_ALARM       (1 << 6)
  #define R_VENTILATOR  (1 << 7)
  #define R_INFLOW1     (1 << 0)
  #define R_INFLOW2     (1 << 1)
  #define R_CLEARWATER  (1 << 2)
  #define R_EXT_COMP    (1 << 3)

  // // define old relay connections R1...R8
  // #define R_COMP        (1 << 4)
  // #define R_PHOSPHOR    (1 << 5)
  // #define R_ALARM       (1 << 6)
  // #define R_VENTILATOR  (1 << 7)
  // #define R_INFLOW1     (1 << 0)
  // #define R_INFLOW2     (1 << 1)
  // #define R_CLEARWATER  (1 << 2)
  // #define R_EXT_COMP    (1 << 3)

#endif


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void PORT_Init(void);
void PORT_Bootloader(void);

void PORT_Buzzer(t_FuncCmd cmd);
void PORT_Ventilator(void);
void PORT_Auto_RunTime(struct PlantState *ps);

unsigned char PORT_Valve(t_valve valve, unsigned char new_state);
void PORT_Valve_OpenAll(void);
void PORT_Valve_CloseAll(void);

void PORT_RelaisSet(unsigned char relais);
void PORT_RelaisClr(unsigned char relais);

void PORT_Debug(void);

#endif