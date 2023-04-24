// --
// timer

#include <avr/io.h>

#include "tc_func.h"

#include "config.h"
#include "mcp7941_driver.h"


/* ==================================================================*
 *            TCC0 - Main Timer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCC0 - MicroSec
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
 *            TCC0 - MilliSec
 * ------------------------------------------------------------------*/

void TCC0_WaitMilliSec_Init(int milli_sec)
{
  int milliHerz = 63;
  int time = milli_sec * milliHerz;

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
 *            TCC0 - Query and Stop
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
 *            TCC0 - PageTimer
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
 *            TWI Timer TCC1
 * --------------------------------------------------------------
 *  TWI Timer for Safety
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCC1 - MilliSec
 * ------------------------------------------------------------------*/

void TCC1_WaitMilliSec_Init(int milli_sec)
{
  int milliHerz = 63;
  int time = milli_sec * milliHerz;

  TCC1_CTRLA = TC_CLKSEL_OFF_gc;
  TCC1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCC1.CNT = 0;
  TCC1.PER = 65500;
  TCC1.CCA = time;
  TCC1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCC1_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCC1 - Query and Stop
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
 *            Touch Timer TCD0
 * --------------------------------------------------------------
 *  Touch Timer for working with touch separatly
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCD0 - MilliSec
 * ------------------------------------------------------------------*/

void TCD0_WaitMilliSec_Init(int milli_sec)
{
  int milliHerz = 63;
  int time = milli_sec * milliHerz;

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
 *            TCD0 - Query and Stop
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
 *            TCE0 - Error Timer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCE0 - sec
 * ------------------------------------------------------------------*/

void TCE0_WaitSec_Init(int sec)
{
  int time = sec * TC_OSC_DIV1024;

  TCE0_CTRLA = TC_CLKSEL_OFF_gc;
  TCE0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCE0.CNT = 0;
  TCE0.PER = 65500;
  TCE0.CCA = time;
  TCE0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCE0_CTRLA = TC_CLKSEL_DIV1024_gc;
}


/* ------------------------------------------------------------------*
 *            TCE0 - MilliSec
 * ------------------------------------------------------------------*/

void TCE0_WaitMilliSec_Init(int milli_sec)
{
  int milliHerz = 63;
  int time = milli_sec * milliHerz;

  TCE0_CTRLA = TC_CLKSEL_OFF_gc;
  TCE0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCE0.CNT = 0;
  TCE0.PER = 65500;
  TCE0.CCA = time;
  TCE0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCE0_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCE0 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCE0_Wait_Query(void)
{
  if(TCE0.INTFLAGS & (1 << TC0_CCAIF_bp))
  {
    TCE0.CNT = 0;
    TCE0.INTFLAGS |= (1 << TC0_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCE0_Stop(void)
{
  TCE0_CTRLA = TC_CLKSEL_OFF_gc;
}


/* ------------------------------------------------------------------*
 *            TCE0 - Apps***
 * ------------------------------------------------------------------*/

unsigned char TCE0_ErrorTimer(t_FuncCmd cmd)
{
  static unsigned char i = 0;

  if(cmd == _ton)
  {
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc)){ TCE0_WaitSec_Init(3); }

    // increase time counter
    if(TCE0_Wait_Query()) i++;

    // error time ends: 4*3s
    if(i > 2)
    {
      i = 0;
      TCE0_Stop();
      return 1;
    }
  }

  // open valve
  else if(cmd == _ovent)
  {
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc)){ TCE0_WaitSec_Init(3); }

    if(TCE0_Wait_Query())
    {
      i = 0;
      TCE0_Stop();
      return 1;
    }
  }

  // close valve
  else if(cmd == _cvent)
  {
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc))
    {
      i = 0;
      TCE0_WaitSec_Init(3);
    }

    if(TCE0_Wait_Query())
    {
      if(!i)
      {
        i = 1;
        TCE0_WaitMilliSec_Init(100);
      }
      else
      {
        i = 0;
        TCE0_Stop();
        return 1;
      }
    }
  }

  else if(cmd == _reset)
  {
    TCE0_Stop();
    i = 0;
  }

  return 0;
}



/* ==================================================================*
 *            TCE1 - CAN Timer1
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCE1 - MilliSec
 * ------------------------------------------------------------------*/

void TCE1_WaitMilliSec_Init(int milli_sec)
{
  int milliHerz = 63;
  int time = milli_sec * milliHerz;

  TCE1_CTRLA = TC_CLKSEL_OFF_gc;
  TCE1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCE1.CNT = 0;
  TCE1.PER = 65500;
  TCE1.CCA = time;
  TCE1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCE1_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCE1 - Query and Stop
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
 *            TCF0 - Sonic Timer
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
 *            TCF0 - Query and Stop
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
 *            Frame Timer - TCF1
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCF1 - timer init
 * ------------------------------------------------------------------*/

void TCF1_FrameTimer_Init(void)
{
  // calculate frame time: 0.016666666666666666
  float frame_time_count_f = 250000.0 / TC_FPS;
  int frame_time_count = (int)frame_time_count_f;

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
 *            TCF1 - Query and Stop
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
