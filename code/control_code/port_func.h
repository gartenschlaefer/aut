// --
// port functions

// Include guard
#ifndef PORT_FUNC_H   
#define PORT_FUNC_H

#include<avr/io.h>

/* ==================================================================*
 *            DEFINES
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            PORT
 * ------------------------------------------------------------------*/

#define P_OPTO    (PORTH)   //PORTH
#define P_VENTIL  (PORTJ)   //PORTJ
#define P_RELAIS  (PORTK)   //PORTK


/* ------------------------------------------------------------------*
 *            Optical coupler Inputs
 * ------------------------------------------------------------------*/

#define OC1 (PIN7_bm)
#define OC2 (PIN6_bm)
#define OC3 (PIN5_bm)
#define OC4 (PIN4_bm)

#define IN_FLOAT_S3 (!(P_OPTO.IN & OC1))

struct InputHandler {
  unsigned char float_sw_alarm;
};


/* ------------------------------------------------------------------*
 *            Ventil Outpus
 * ------------------------------------------------------------------*/

//*--------------------------Normal Connection
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
//--------------------------*/

/*--------------------------Rverse
#define ALL_OPEN  0xAA
#define ALL_CLOSE 0x55
#define O_RES     0x02
#define C_RES     0x01
#define O_MUD     0x08
#define C_MUD     0x04
#define O_AIR     0x20
#define C_AIR     0x10
#define O_CLRW    0x80
#define C_CLRW    0x40
//--------------------------*/

typedef enum
{ OPEN_Reserve,   CLOSE_Reserve,
  OPEN_MudPump,   CLOSE_MudPump,
  OPEN_Air,     CLOSE_Air,
  OPEN_ClearWater,  CLOSE_ClearWater,
  CLOSE_IPAir,  
  SET_STATE_CLOSE,
  SET_STATE_OPEN,
  SET_STATE_ALL_CLOSED, 
  SET_STATE_ALL_OPEN,
  READ_STATE
}t_ventil;

#define V_AIR (1<<0)
#define V_MUD (1<<1)
#define V_CLW (1<<2) 
#define V_RES (1<<3)   


/* ------------------------------------------------------------------*
 *            Relais Outpus
 * ------------------------------------------------------------------*/

/*---------------------------------------Old
#define R_COMP      (1<<4)      //R1
#define R_PHOSPHOR    (1<<5)      //R2
#define R_ALARM     (1<<6)      //R3
#define R_VENTILATOR  (1<<7)      //R4
#define R_INFLOW1   (1<<0)      //R5
#define R_INFLOW2   (1<<1)      //R6
#define R_CLEARWATER  (1<<2)      //R7
#define R_EXT_COMP    (1<<3)      //R8
//----------------------------------------*/

//*---------------------------------------New
#define R_COMP        (1<<4)      //R1
#define R_CLEARWATER  (1<<5)      //R2
#define R_ALARM       (1<<6)      //R3
#define R_VENTILATOR  (1<<7)      //R4
#define R_INFLOW1     (1<<0)      //R5
#define R_INFLOW2     (1<<1)      //R6
#define R_PHOSPHOR    (1<<2)      //R7
#define R_EXT_COMP    (1<<3)      //R8
//----------------------------------------*/


/* ==================================================================*
 *            FUCNTION API
 * ==================================================================*/

void PORT_Init(void);
//void PORT_SoftwareRst(void);
void PORT_Bootloader(void);

void PORT_Buzzer(t_FuncCmd cmd);
void PORT_Ventilator(void);
void PORT_RunTime(struct InputHandler *in);

unsigned char PORT_Ventil(t_ventil ventil, unsigned char new_state);
void PORT_Ventil_AllOpen(void);
void PORT_Ventil_AllClose(void);
void PORT_Ventil_AutoClose(t_page page);

void PORT_RelaisSet(unsigned char relais);
void PORT_RelaisClr(unsigned char relais);
void PORT_Relais_AllOff(void);

void InputHandler_init(struct InputHandler *in);
void PORT_Debug(void);

#endif
