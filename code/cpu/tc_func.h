// --
// timer

// include guard
#ifndef TC_FUNC_H   
#define TC_FUNC_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define TC_OSC_F        16000000
#define TC_OSC_DIV1024  15625
#define TC_FPS          60
#define TC_FPS_HALF     30
#define TC_CAN_MS       100


/* ------------------------------------------------------------------*
 *            TCC0 - Main Timer
 * ------------------------------------------------------------------*/

void TCC0_WaitMicroSec_Init(int micro_sec);
void TCC0_WaitMilliSec_Init(int milli_sec);
void TCC0_WaitSec_Init(int sec);
void TCC0_Stop(void);
unsigned char TCC0_Wait_Query(void);

void TCC0_wait_us(int us);
void TCC0_wait_ms(int ms);
void TCC0_wait_sec(int sec);
void TCC0_wait_openValve(void);
void TCC0_wait_closeValve(void);

void TCC0_Touch_Wait(void);


/* ------------------------------------------------------------------*
 *            TCC1 - TWI Timer
 * ------------------------------------------------------------------*/

void TCC1_WaitMilliSec_Init(int milli_sec);
unsigned char TCC1_Wait_Query(void);
void TCC1_Stop(void);


/* ------------------------------------------------------------------*
 *            TCD0 - Touch Timer
 * ------------------------------------------------------------------*/

void TCD0_WaitMilliSec_Init(int milli_sec);
void TCD0_WaitSec_Init(int sec);
unsigned char TCD0_Wait_Query(void);
void TCD0_Stop(void);


/* ------------------------------------------------------------------*
 *            TCD1 - Safety Timer
 * ------------------------------------------------------------------*/


/* ------------------------------------------------------------------*
 *            TCE0 - Error Timer
 * ------------------------------------------------------------------*/

void TCE0_WaitSec_Init(int sec);
void TCE0_WaitMilliSec_Init(int milli_sec);
unsigned char TCE0_Wait_Query (void);
void TCE0_Stop(void);
unsigned char TCE0_ErrorTimer(t_FuncCmd error);


/* ------------------------------------------------------------------*
 *            TCE0 - CAN Timer 1
 * ------------------------------------------------------------------*/

void TCE1_WaitMilliSec_Init(int milli_sec);
unsigned char TCE1_Wait_Query(void);
void TCE1_Stop(void);


/* ------------------------------------------------------------------*
 *            TCF0 - RunTime
 * ------------------------------------------------------------------*/

void TCF0_WaitSec_Init(int sec);
unsigned char TCF0_Wait_Query(void);
void TCF0_Stop(void);


/* ------------------------------------------------------------------*
 *            TCF1 - Modem
 * ------------------------------------------------------------------*/

void TCF1_FrameTimer_Init(void);
void TCF1_FrameTimer_WaitUntilFrameEnded(struct FrameCounter *frame_counter);

#endif