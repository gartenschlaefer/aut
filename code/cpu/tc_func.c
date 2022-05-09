// --
// timer

#include <avr/io.h>

#include "tc_func.h"

#include "config.h"
#include "mcp7941_driver.h"


/* ==================================================================*
 *            Main Timer TCC0
 * --------------------------------------------------------------
 *  TCC0 Allround while WaitingTimer
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCC0 - MicroSec
 * ------------------------------------------------------------------*/

void TCC0_WaitMicroSec_Init(int microSec)
{
  int microHerz = 16;
  int time = microSec * microHerz;

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

void TCC0_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

  TCC0_CTRLA = TC_CLKSEL_OFF_gc;
  TCC0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCC0.CNT = 0;
  TCC0.PER = 65500;
  TCC0.CCA = time;
  TCC0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCC0_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCC0 - Sec
 * ------------------------------------------------------------------*/

void TCC0_WaitSec_Init(int Sec)
{
  int Herz = 15625;
  int time = Sec * Herz;

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

void TCC0_Main_Wait(void)
{
  TCC0_wait_ms(200);
}

void TCC0_DisplayManual_Wait(void)
{
  TCC0_wait_ms(5);
}

void TCC0_DisplaySetup_Wait(void)
{
  TCC0_wait_ms(10);
}

void TCC0_DisplayData_Wait(void)
{
  TCC0_wait_ms(10);
}

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

void TCC1_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

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

void TCD0_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

  TCD0_CTRLA = TC_CLKSEL_OFF_gc;
  TCD0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCD0.CNT = 0;
  TCD0.PER = 65500;
  TCD0.CCA = time;
  TCD0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCD0_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCD0 - Sec: max 3s
 * ------------------------------------------------------------------*/

void TCD0_WaitSec_Init(int Sec)
{
  int Herz = 15625;
  int time = Sec * Herz;

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
 *            TimerIC Safety Timer TCD1
 * --------------------------------------------------------------
 *  If Timer IC do not work, safetyTimer TCD1 will be used
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCD1 - MilliSec
 * ------------------------------------------------------------------*/

void TCD1_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

  TCD1_CTRLA = TC_CLKSEL_OFF_gc;
  TCD1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCD1.CNT = 0;
  TCD1.PER = 65500;
  TCD1.CCA = time;
  TCD1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCD1_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCD1 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCD1_Wait_Query(void)
{
  if(TCD1.INTFLAGS & (1 << TC1_CCAIF_bp))
  {
    TCD1.CNT = 0;
    TCD1.INTFLAGS |= (1 << TC1_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCD1_Stop(void)
{
  TCD1_CTRLA = TC_CLKSEL_OFF_gc;
}


/* ------------------------------------------------------------------*
 *            TCD1 - Apps***
 * ------------------------------------------------------------------*/

unsigned char TCD1_MainAuto_SafetyTC(t_FuncCmd cmd)
{
  static unsigned char i = 0;

  switch(cmd)
  {
    case _init:   
      TCD1_WaitMilliSec_Init(100);
      i = 0;
      break;

    case _reset:  
      TCD1_WaitMilliSec_Init(100);
      i = 0;  
      break;

    case _exe:    
      if(TCD1_Wait_Query())
      {  
        i++;
        TCD1_WaitMilliSec_Init(100);
      }

      // safety second is 1200ms
      if(i > 12)
      { 
        i = 0;
        MCP7941_WriteByte(TIC_SEC, 0x00);
        return 1;
      }
      break;

    default: break;
  }
  return 0;
}



/* ==================================================================*
 *            Error Timer - TCE0
 * --------------------------------------------------------------
 *  Timer for Error gets onAction
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCE0 - Sec
 * ------------------------------------------------------------------*/

void TCE0_WaitSec_Init(int Sec)
{
  int Herz = 15625;
  int time = Sec * Herz;

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

void TCE0_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

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
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc)) TCE0_WaitSec_Init(3);

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
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc)) TCE0_WaitSec_Init(3);

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
 *            CAN Timer1 - TCE1
 * --------------------------------------------------------------
 *  Timer for CAN Communication
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCE1 - MilliSec
 * ------------------------------------------------------------------*/

void TCE1_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

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
 *            RunTime - TCF0
 * --------------------------------------------------------------
 *  Timer for UltraSonic ReadCycle
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TCF0 - Sec
 * ------------------------------------------------------------------*/

void TCF0_WaitSec_Init(int Sec)
{
  int Herz = 15625;
  int time = Sec * Herz;

  TCF0_CTRLA = TC_CLKSEL_OFF_gc;
  TCF0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCF0.CNT = 0;
  TCF0.PER = 65500;
  TCF0.CCA = time;
  TCF0.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCF0_CTRLA = TC_CLKSEL_DIV1024_gc;
}

/* ------------------------------------------------------------------*
 *            TCF0 - MilliSec
 * ------------------------------------------------------------------*/

void TCF0_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

  TCF0_CTRLA = TC_CLKSEL_OFF_gc;
  TCF0.CTRLB = TC_WGMODE_NORMAL_gc;

  TCF0.CNT = 0;
  TCF0.PER = 65500;
  TCF0.CCA = time;
  TCF0.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCF0_CTRLA = TC_CLKSEL_DIV256_gc;
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
 *            Modem Timer - TCF1
 * --------------------------------------------------------------
 *  Timer for Modem
 * --------------------------------------------------------------
 * ==================================================================*/


/* ------------------------------------------------------------------*
 *            TCF1 - Sec
 * ------------------------------------------------------------------*/

void TCF1_WaitSec_Init(int Sec)
{
  int Herz = 15625;
  int time = Sec * Herz;

  TCF1_CTRLA = TC_CLKSEL_OFF_gc;
  TCF1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCF1.CNT = 0;
  TCF1.PER = 65500;
  TCF1.CCA = time;
  TCF1.INTFLAGS |= (1 << TC0_CCAIF_bp);

  TCF1_CTRLA = TC_CLKSEL_DIV1024_gc;
}

/* ------------------------------------------------------------------*
 *            TCF1 - MilliSec
 * ------------------------------------------------------------------*/

void TCF1_WaitMilliSec_Init(int milliSec)
{
  int milliHerz = 63;
  int time = milliSec * milliHerz;

  TCF1_CTRLA = TC_CLKSEL_OFF_gc;
  TCF1.CTRLB = TC_WGMODE_NORMAL_gc;

  TCF1.CNT = 0;
  TCF1.PER = 65500;
  TCF1.CCA = time;
  TCF1.INTFLAGS |= (1 << TC1_CCAIF_bp);

  TCF1_CTRLA = TC_CLKSEL_DIV256_gc;
}


/* ------------------------------------------------------------------*
 *            TCF1 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCF1_Wait_Query(void)
{
  if(TCF1.INTFLAGS & (1 << TC1_CCAIF_bp))
  {
    TCF1.CNT = 0;
    TCF1.INTFLAGS |= (1 << TC1_CCAIF_bp);
    return 1;
  }
  return 0;
}

void TCF1_Stop(void)
{
  TCF1_CTRLA = TC_CLKSEL_OFF_gc;
}

