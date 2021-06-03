/*********************************************************************\
* Author:       Christian Walter
* ------------------------------------------------------------------
* Project:      UltraSonic
* Name:         tc_func.c
* ------------------------------------------------------------------
* µ-Controler:  AT90CAN128/32
* Compiler:     avr-gcc (WINAVR 2010)
* Description:
* ------------------------------------------------------------------
* TimerCounter Functions Source-File
* ------------------------------------------------------------------
* Date:         06.10.2011
* lastChanges:  11.04.2015
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "tc_func.h"
#include "sonic_app.h"



/* ==================================================================*
 *            FUNCTIONS Timer Counter 16MHz
 * ==================================================================*/

void TC_Wait_1clk(void)
{
  asm volatile("nop");
}



/* ==================================================================*
 *            TC0 - 8Bit-MainWaitTimer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TC0 - 1ms Init
 * ------------------------------------------------------------------*/

void TC0_16MHzWait_1ms_Init(void)
{
  TCNT0 = 0x00;
  TCCR0A = (1<<CS00) | (1<<CS01);       //start clk/64
}


/* ------------------------------------------------------------------*
 *            TC0 - Query TC0 Overflow -> 1ms
 * ------------------------------------------------------------------*/

unsigned char TC0_16MHzWait_1ms_Query(void)
{
  if(TIFR0 & (1<<TOV0)){    //wait 1ms
    TIFR0  |= (1<<TOV0);    //Reset Flag
    return 1;}
  return 0;
}


/* ------------------------------------------------------------------*
 *            TC0 - Stop
 * ------------------------------------------------------------------*/

void TC0_16MHz_Stop(void)
{
  TCCR0A &= ~((1<<CS00) | (1<<CS01));   //stop clk/64
}


/* ------------------------------------------------------------------*
 *            TC0 - Timer App While
 * ------------------------------------------------------------------*/

void TC0_16MHzWait_msWhile(int mSec)
{
  int i=0;
  TC0_16MHzWait_1ms_Init();                 //InitTimer
  for(i=0; i<mSec; i++) while(!TC0_16MHzWait_1ms_Query());  //WaitLoop
}


/* ------------------------------------------------------------------*
 *            TC0 - Timer App IF
 * ------------------------------------------------------------------*/

unsigned char TC0_16MHzWait_msQuery(t_FuncCmd cmd, int mSec)
{
  static int i = 0;
  static int ms = 0;
  if(cmd == _init)
  {
    i = 0;
    ms = mSec;
    TC0_16MHzWait_1ms_Init();       //InitTimer
  }

  else if(cmd == _exe)
  {
    if(TC0_16MHzWait_1ms_Query()){    //Query
      i++;
      if(i > ms){                     //match
        i = 0;
        return 1;}}
  }
  return 0;
}



/* ==================================================================*
 *            TC2 - 8Bit-SecnodWaitTimer
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TC2 - 1ms Init
 * ------------------------------------------------------------------*/

void TC2_16MHzWait_1ms_Init(void)
{
  TCNT2 = 0x00;
  TCCR2A = (1<<CS22);       //start clk/64
}


/* ------------------------------------------------------------------*
 *            TC2 - Query TC2 Overflow -> 1ms
 * ------------------------------------------------------------------*/

unsigned char TC2_16MHzWait_1ms_Query(void)
{
  if(TIFR2 & (1<<TOV2))           //wait 1ms
  {
    TIFR2  |= (1<<TOV2);          //Reset Flag
    return 1;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            TC2 - Stop
 * ------------------------------------------------------------------*/

void TC2_16MHz_Stop(void)
{
  TCCR2A &= ~(1<<CS22);   //stop clk/64
}


/* ------------------------------------------------------------------*
 *            TC2 - Timer App While
 * ------------------------------------------------------------------*/

void TC2_16MHzWait_msWhile(int mSec)
{
  int i = 0;
  TC2_16MHzWait_1ms_Init();                 //InitTimer
  for(i = 0; i < mSec; i++) while(!TC2_16MHzWait_1ms_Query());  //Wait
}


/* ------------------------------------------------------------------*
 *            TC2 - Timer App IF
 * ------------------------------------------------------------------*/

unsigned char TC2_16MHzWait_msQuery(t_FuncCmd cmd, int mSec)
{
  static int i = 0;
  static int ms = 0;
  if(cmd == _init)
  {
    i = 0;
    ms = mSec;
    TC2_16MHzWait_1ms_Init();       //InitTimer
  }

  else if(cmd == _exe)
  {
    if(TC2_16MHzWait_1ms_Query())     //Query
    {
      i++;
      if(i > ms)              //match
      {
        i = 0;
        return 1;
      }
    }
  }
  return 0;
}



/* ==================================================================*
 *            TC3 - 16bit-Timer-UltraSonicMeasureMent
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            UltraSonic Pulse Init
 * ------------------------------------------------------------------*/

void TC3_Sonic_Init(void)
{
  TCCR1A =  (1 << COM1A1) |   //OCR1A-ClearOnCompare-SetOnTop
            (1 << WGM11);     //FastPWM-Mode

  TCCR1B =  (1 << WGM12)  |   //FastPWM-Mode Top=ICR1
            (1 << WGM13);     //FastPWM-Mode

  //PulseWidth
  ICR1H =   0;
  ICR1L =   PULSEWIDTH_IC;        //Set Frequenzy 16Mhz->75kHz
  OCR1AH =  0;
  OCR1AL =  PULSEWIDTH_OC;        //Set Pulse width
}


/* ------------------------------------------------------------------*
 *            UltraSonic Timer Start
 * ------------------------------------------------------------------*/

void TC3_Sonic_StartTimer(void)
{
  TC3_Sonic_ResetTimer(); //FirstResetTimer

  //StartValue
  OCR3AH = STVALUE;         //StartValue300mm - 75kHz
  OCR3AL = 0x00;

  TCCR3A =  0x00;         //Normal-Mode
  TCCR3B = (1 << CS30);     //Normal-Mode, Start Charge-Pulse clk/1

  while(!(TIFR3 & (1 << OCF3A)));   //DeadTime
  TIFR3 |= (1 << OCF3A);            //Reset OC
}


/* ------------------------------------------------------------------*
 *            UltraSonic Timer Stop
 * ------------------------------------------------------------------*/

void TC3_Sonic_StopTimer(void)
{
  TCCR3B = 0x00;      //Stopp Timer
}


/* ------------------------------------------------------------------*
 *            UltraSonic Timer Reset
 * ------------------------------------------------------------------*/

void TC3_Sonic_ResetTimer(void)
{
  TCCR3A =  0x00;       //Normal-Mode
  TCCR3B = 0x00;        //Stopp Timer
  TCNT3H = 0x00;        //Reset TCNTH
  TCNT3L = 0x00;        //Reset TCNTL
  TIFR3 |= (1<<TOV3);   //ResetFlag
  TIFR3 |= (1<<OCF3A);  //Reset OC
}


/* ------------------------------------------------------------------*
 *            Sonic Send
 * ------------------------------------------------------------------*/

void TC3_Sonic_Send(void)
{
  unsigned char p=0;

  TCCR1B |= (1<<CS10);      //Start Sonic-Pulse clk/1

  while(p<30)           //Send 30Pulses at 75kHz or 125kHz
  {
    if(TIFR1 & (1<<OCF1A))      //if-Output-Compare
    {
      TIFR1 |= (1<<OCF1A);    //Reset-Flag
      p++;            //inc PulsCounter
    }
  }

  TCCR1B &= ~(1<<CS10);     //Stop Sonic-Pulse
}



/* ==================================================================*
 *            TC1 - Reserved
 * ==================================================================*/






/*********************************************************************\
 * End of file
\**********************************************************************/
