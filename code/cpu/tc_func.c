// --
// timer

#include <avr/io.h>

#include "tc_func.h"

#include "config.h"
#include "mcp7941_driver.h"
#include "utils.h"


/* ==================================================================*
 *            TCC0 - main timer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCC0 - micro second
 * ------------------------------------------------------------------*/

void TCC0_WaitMicroSec_Init(int micro_sec)
{
  int microHerz = 16;
  int time = micro_sec * microHerz;

  TCC0_CTRLA = TC_CLKSEL_OFF_gc;
  TCC0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCC0.CNT = 0;
  TCC0.PER = 65500;
  TCC0.CCA = time;
  TCC0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCC0_CTRLA = TC_CLKSEL_DIV1_gc;
}


/* ------------------------------------------------------------------*
 *            TCC0 - milli second
 * ------------------------------------------------------------------*/

void TCC0_WaitMilliSec_Init(int milli_sec)
{
  int time = f_round_int(milli_sec * TC_OSC_DIV256_MS);

  TCC0_CTRLA = TC_CLKSEL_OFF_gc;
  TCC0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCC0.CNT = 0;
  TCC0.PER = 65500;
  TCC0.CCA = time;
  TCC0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCC0_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCC0 - sec
 * ------------------------------------------------------------------*/

void TCC0_WaitSec_Init(int sec)
{
  int time = sec * TC_OSC_DIV1024;

  TCC0_CTRLA = TC_CLKSEL_OFF_gc;
  TCC0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCC0.CNT = 0;
  TCC0.PER = 65500;
  TCC0.CCA = time;
  TCC0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCC0_CTRLA = TC_CLKSEL_DIV1024_gc;
}


/* ------------------------------------------------------------------*
 *            TCC0 - query and stop
 * ------------------------------------------------------------------*/

unsigned char TCC0_Wait_Query(void)
{
  if(TCC0.INTFLAGS & (1 << TC0_CCAIF_bp))
  {
    TCC0.CNT = 0;
    TCC0.INTFLAGS |= (1 << TC0_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCC0_Stop(void)
{
  TCC0_CTRLA = TC_CLKSEL_OFF_gc;
}


/* ------------------------------------------------------------------*
 *            TCC0 - Apps
 * ------------------------------------------------------------------*/

void TCC0_wait_us(int us)
{
  TCC0_WaitMicroSec_Init(us);
  while(!(TCC0_Wait_Query()));
  TCC0_Stop();
}

void TCC0_wait_ms(int ms)
{
  TCC0_WaitMilliSec_Init(ms);
  while(!(TCC0_Wait_Query()));
  TCC0_Stop();
}

void TCC0_wait_sec(int sec)
{
  while(sec)
  {
    TCC0_WaitSec_Init(1);
    while(!(TCC0_Wait_Query()));
    sec--;
  }
  TCC0_Stop();
}


/* ------------------------------------------------------------------*
 *            TCC0 - touch wait
 * ------------------------------------------------------------------*/

void TCC0_Touch_Wait(void)
{
  TCC0_wait_ms(15);
}


/* ------------------------------------------------------------------*
 *            TCC0 - open and close valve
 * ------------------------------------------------------------------*/

void TCC0_wait_openValve(void)
{
  // spring or usual valves
  if(SPRING_VALVE_ON){ TCC0_wait_sec(4); TCC0_wait_ms(350); }
  else{ TCC0_wait_sec(3); }
}


void TCC0_wait_closeValve(void)
{
  // spring valves
  if(SPRING_VALVE_ON){ TCC0_wait_sec(4); TCC0_wait_ms(300); }
  else{ TCC0_wait_sec(3); TCC0_wait_ms(100); }
}




/* ==================================================================*
 *            TWI timer TCC1
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCC1 - milli second
 * ------------------------------------------------------------------*/

void TCC1_WaitMilliSec_Init(int milli_sec)
{
  int time = f_round_int(milli_sec * TC_OSC_DIV256_MS);

  TCC1_CTRLA = TC_CLKSEL_OFF_gc;
  TCC1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCC1.CNT = 0;
  TCC1.PER = 65500;
  TCC1.CCA = time;
  TCC1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCC1_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCC1 - query and stop
 * ------------------------------------------------------------------*/

unsigned char TCC1_Wait_Query(void)
{
  if(TCC1.INTFLAGS & (1 << TC1_CCAIF_bp))
  {
    TCC1.CNT = 0;
    TCC1.INTFLAGS |= (1 << TC1_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCC1_Stop(void)
{
  TCC1_CTRLA = TC_CLKSEL_OFF_gc;
}




/* ==================================================================*
 *            Touch timer TCD0
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCD0 - milli second
 * ------------------------------------------------------------------*/

void TCD0_WaitMilliSec_Init(int milli_sec)
{
  int time = f_round_int(milli_sec * TC_OSC_DIV256_MS);

  TCD0_CTRLA = TC_CLKSEL_OFF_gc;
  TCD0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCD0.CNT = 0;
  TCD0.PER = 65500;
  TCD0.CCA = time;
  TCD0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCD0_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCD0 - sec: max 3s
 * ------------------------------------------------------------------*/

void TCD0_WaitSec_Init(int sec)
{
  int time = sec * TC_OSC_DIV1024;

  TCD0_CTRLA = TC_CLKSEL_OFF_gc;
  TCD0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCD0.CNT = 0;
  TCD0.PER = 65500;
  TCD0.CCA = time;
  TCD0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCD0_CTRLA = TC_CLKSEL_DIV1024_gc;
}


/* ------------------------------------------------------------------*
 *            TCD0 - query and stop
 * ------------------------------------------------------------------*/

unsigned char TCD0_Wait_Query(void)
{
  if(TCD0.INTFLAGS & (1 << TC0_CCAIF_bp))
  {
    TCD0.CNT = 0;
    TCD0.INTFLAGS |= (1 << TC0_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCD0_Stop(void)
{
  TCD0_CTRLA = TC_CLKSEL_OFF_gc;
}



/* ==================================================================*
 *            TCD1 - unused
 * ==================================================================*/



/* ==================================================================*
 *            TCE0 - unused
 * ==================================================================*/


/* ==================================================================*
 *            TCE1 - CAN timer1
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCE1 - milli second
 * ------------------------------------------------------------------*/

void TCE1_WaitMilliSec_Init(int milli_sec)
{
  int time = f_round_int(milli_sec * TC_OSC_DIV256_MS);

  TCE1_CTRLA = TC_CLKSEL_OFF_gc;
  TCE1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCE1.CNT = 0;
  TCE1.PER = 65500;
  TCE1.CCA = time;
  TCE1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCE1_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCE1 - query and stop
 * ------------------------------------------------------------------*/

unsigned char TCE1_Wait_Query(void)
{
  if(TCE1.INTFLAGS & (1 << TC1_CCAIF_bp))
  {
    TCE1.CNT = 0;
    TCE1.INTFLAGS |= (1 << TC1_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCE1_Stop(void)
{
  TCE1_CTRLA = TC_CLKSEL_OFF_gc;
}



/* ==================================================================*
 *            TCF0 - valve timer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCF0 - sec
 * ------------------------------------------------------------------*/

void TCF0_WaitSec_Init(int sec)
{
  int time = sec * TC_OSC_DIV1024;

  TCF0_CTRLA = TC_CLKSEL_OFF_gc;
  TCF0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCF0.CNT = 0;
  TCF0.PER = 65500;
  TCF0.CCA = time;
  TCF0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCF0_CTRLA = TC_CLKSEL_DIV1024_gc;
}


/* ------------------------------------------------------------------*
 *            TCF0 - milli second
 * ------------------------------------------------------------------*/

void TCF0_WaitMilliSec_Init(int milli_sec)
{
  int time = f_round_int(milli_sec * TC_OSC_DIV256_MS);

  TCF0_CTRLA = TC_CLKSEL_OFF_gc;
  TCF0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCF0.CNT = 0;
  TCF0.PER = 65500;
  TCF0.CCA = time;
  TCF0.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCF0_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCF0 - query and stop
 * ------------------------------------------------------------------*/

unsigned char TCF0_Wait_Query(void)
{
  if(TCF0.INTFLAGS & (1 << TC1_CCAIF_bp))
  {
    TCF0.CNT = 0;
    TCF0.INTFLAGS |= (1 << TC1_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCF0_Stop(void)
{
  TCF0_CTRLA = TC_CLKSEL_OFF_gc;
}



/* ==================================================================*
 *            frame timer - TCF1
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCF1 - timer init
 * ------------------------------------------------------------------*/

void TCF1_FrameTimer_Init(void)
{
  // calculate frame time: 0.016666666666666666
  int frame_time_count = f_round_int(250000.0 / TC_FPS);

  // settings
  TCF1_CTRLA = TC_CLKSEL_OFF_gc;
  TCF1.CTRLB = TC_WGMODE_NORMAL_gc;
  TCF1.CNT = 0;
  TCF1.PER = 65500;
  TCF1.CCA = frame_time_count;
  TCF1.INTFLAGS |= (1 << TC0_CCAIF_bp);
  TCF1_CTRLA = TC_CLKSEL_DIV64_gc;
}


/* ------------------------------------------------------------------*
 *            TCF1 - query and stop
 * ------------------------------------------------------------------*/

void TCF1_FrameTimer_WaitUntilFrameEnded(struct FrameCounter *frame_counter)
{
  // get delta t
  frame_counter->delta_t = TCF1.CNT;
  frame_counter->fps = 250000.0 / frame_counter->delta_t;

  // wait until fps time reached
  while(!(TCF1.INTFLAGS & (1 << TC1_CCAIF_bp)));
  TCF1.CNT = 0;
  TCF1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  // keep track of frame
  frame_counter->frame++;
  if(frame_counter->frame >= TC_FPS)
  { 
    frame_counter->frame = 0;
    frame_counter->sixty_sec_counter++;
    if(frame_counter->sixty_sec_counter >= 60){ frame_counter->sixty_sec_counter = 0; }
  }
}