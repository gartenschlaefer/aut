/*********************************************************************\
* Author:       Christian Walter
* ------------------------------------------------------------------
* Project:      UltraSonic
* Name:         sonic_app.c
* ------------------------------------------------------------------
* µ-Controler:  AT90CAN128/32
* Compiler:     avr-gcc (WINAVR 2010)
* Description:
* ------------------------------------------------------------------
* UltraSonic send and receive applications
* ------------------------------------------------------------------
* Date:         12.04.2015
* lastChanges:  13.08.2019
\**********************************************************************/


#include<avr/io.h>

#include "defines.h"
#include "adc_func.h"
#include "tc_func.h"
#include "can_func.h"

#include "can_app.h"
#include "sonic_app.h"



/* ==================================================================*
 *            App Functions
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Distance - 5 Measurements
 * ------------------------------------------------------------------*/

t_UScmd Sonic_5Shots(t_FuncCmd cmd)
{
  unsigned char sonic = 0;
  unsigned long calc = 0;
  static unsigned char state = 0;
  static unsigned char a = 0;
  static unsigned char time[5][3] = {
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}};

  //--------------------------------------------------Init
  if(cmd == _init)
  {
    state = 0;
    CAN_USSREG(_reset, DISA);               //WriteStatusReg
  }

  //--------------------------------------------------Exe
  else if(cmd == _exe)
  {
    //--------------------------------------------------SendPulses
    if(state == 0)
    {
      Sonic_StartMeasurement(); //SendPulses&Start
      state = 1;
    }

    //--------------------------------------------------EchoRecord
    else if(state == 1)
    {
      sonic = ADC_ReadByteComplete();

      // read timer
      time[a][0]= TCNT3L;
      time[a][1]= TCNT3H;

      // adc sonic echo trigger lever
      if(sonic >= SONIC_TRIGGER)
      {
        // stop timer
        TC3_Sonic_StopTimer();

        // all trials done
        if(a >= N_SHOTS - 1)
        {
          state = 2;
        }
        // more trials
        else
        {
          //SafetyTimer
          TC2_16MHzWait_msQuery(_init, TSAFE);
          state = 4;
          a++;
        }
      }

      // third timer var
      else if(TIFR3 & (1 << TOV3))  //UltraSonicTimeOut-80cm
      {
        TIFR3 |= (1 << TOV3);
        time[a][2]++;
      }

      // send time limit
      else if (time[a][2] >= SEND_LIMIT_H && time[a][1] >= SEND_LIMIT_L)
      {
        // stop timer
        TC3_Sonic_StopTimer();
        time[a][2] = 0xFF;

        // all trials done
        if(a >= N_SHOTS - 1)
        {
          state = 2;
        }
        // more trials
        else
        {
          //SafetyTimer
          TC2_16MHzWait_msQuery(_init, TSAFE);
          state = 4;
          a++;
        }
      }
    }

    //--------------------------------------------------Calc+StoreData
    else if(state == 2)
    {
      int n_failed_trials = 0;

      // average
      for(a = 0; a < N_SHOTS; a++)
      {
        // too far, limit reached
        if (time[a][2] == 0xFF)
        {
          n_failed_trials++;
          time[a][2] = 0;
          continue;
        }
        // calculation
        calc = calc + Sonic_Time2mm(&time[a][0]);
        time[a][2] = 0;
      }

      // normalize
      calc = calc / (N_SHOTS - n_failed_trials);

      // write registers and reset
      CAN_USDDREG(_write, calc);
      CAN_USSREG(_set, DISA);
      TC2_16MHzWait_msQuery(_init, TSAFE);
      a = 0;
      state = 3;
    }

    //--------------------------------------------------Success
    else if(state == 3)
    {
      if(TC2_16MHzWait_msQuery(_exe, TSAFE))
      {
        state = 0;
        return _wait;           //Back2Wait
      }
    }

    //--------------------------------------------------NoSuccess
    else if(state == 4)
    {
      if(TC2_16MHzWait_msQuery(_exe, TSAFE)) state = 0;
    }
  }
  return _5Shots;
}


/* ------------------------------------------------------------------*
 *            Distance - SingleShot
 * ------------------------------------------------------------------*/

t_UScmd Sonic_OneShot(void)
{
  int result = 0;
  unsigned char sonic = 0;
  static unsigned char state = 0;
  static unsigned char time[3] = {0x00, 0x00, 0x00};

  //--------------------------------------------------SendPulses
  if(state == 0)
  {
    Sonic_StartMeasurement(); //SendPulses&Start
    state = 1;
  }

  //--------------------------------------------------EchoRecord
  else if(state == 1)
  {
    sonic = ADC_ReadByteComplete();

    // read timer
    time[0]= TCNT3L;
    time[1]= TCNT3H;

    // adc sonic echo trigger lever
    if(sonic >= SONIC_TRIGGER)
    {
      TC3_Sonic_StopTimer();  //TimerStop
      state = 2;              //NextStep
    }

    // third timer var
    else if(TIFR3 & (1<<TOV3))  //UltraSonicTimeOut-80cm
    {
      TIFR3 |= (1 << TOV3);     //ResetFlag
      time[2]++;                //DistanceByte3
    }

    // send limit
    else if (time[2] >= SEND_LIMIT_H && time[1] >= SEND_LIMIT_L)
    {
      TC3_Sonic_StopTimer();
      time[2] = 0xFF;
      state = 2;
    }
  }

  //--------------------------------------------------StoreData
  else if(state == 2)
  {
    result = Sonic_Time2mm(&time[0]);
    CAN_USDDREG(_write, result);
    CAN_USSREG(_set, DISA);
    time[2] = 0;
    TC2_16MHzWait_msQuery(_init, TSAFE);
    state = 3;
  }

  //--------------------------------------------------SafetyTimer
  else if(state == 3)
  {
    if(TC2_16MHzWait_msQuery(_exe, TSAFE))
    {
      state = 0;
      return _wait;           //Back2Wait
    }
  }
  return _oneShot;
}


/* ------------------------------------------------------------------*
 *            Temperature
 * ------------------------------------------------------------------*/

t_UScmd Sonic_Temp(void)
{
  static unsigned char state = 0;
  int adc = 0;

  //--------------------------------------------------Init
  if(state == 0)
  {
    ADC_ADC3_Init();                  //ADC3-Temp-Init
    TC2_16MHzWait_msQuery(_init, 5);  //Time2Change2InternalVoltage
    state = 1;
  }
  //--------------------------------------------------StartConv
  else if(state == 1)
  {
    if(TC2_16MHzWait_msQuery(_exe, 5))
    {
      ADCSRA |= (1 << ADSC);    //startConversion
      state = 2;
    }
  }
  //--------------------------------------------------ReadADC
  else if(state == 2)
  {
    adc = ADC_Read10Bit();        //ReadADC
    if(!(adc & 0x8000))           //Received
    {
      adc = Sonic_Temp_Calc(adc);   //Calc
      CAN_USDTREG(_write, adc);
      CAN_USSREG(_set, TEMPA);
      state = 0;
      return _wait;               //Back2Wait
    }
  }
  return _readTemp;
}


/* ------------------------------------------------------------------*
 *            Temperature - Calc
 * ------------------------------------------------------------------*/

int Sonic_Temp_Calc(int adc)
{
  int zero = 200;       //500mV= 0°C  //Ref= 2,56V
  int k = 4;            //k= 10mV/°C
  int calc = 0;         //CalcVar
  unsigned char h = 0;  //°C
  unsigned char l = 0;  //.°C

  if(adc > 200)         //+
  {
    calc = adc - zero;  //set2Zero
    h = calc / k;       //TempHigh@°C
    l = calc % 4;       //TempLow
    l = l * 25;         //TempLow2.°C
  }
  else if(adc < 200)    //-
  {
    calc = zero - adc;  //set2Zero
    h = calc / k;       //TempHigh@°C
    h = (0x80 | h);     //Minus
    l = calc % 4;       //TempLow
    l = l * 25;         //TempLow2.°C
  }
  else if(adc == 200)   //zero
  {
    h = 0;
    l = 0;
  }
  return ((h << 8) | l);
}



/* ==================================================================*
 *            UltraSonic Init Measurement
 * ==================================================================*/

void Sonic_StartMeasurement(void)
{
    ADC_ADC1_TurnOff();       //SetLow
    TC3_Sonic_Init();         //Init PulseTimerCounter

    PORTE &= ~(1 << PE3);     //Disable Low
    TC3_Sonic_Send();         //Send Sonic Impulses
    PORTE |= (1 << PE3);      //Disable High

    TC3_Sonic_StartTimer();   //Start Timer
    ADC_ADC1_Init();          //ADC-UltraSonic enable
}



/* ==================================================================*
 *            UltraSonic Time2mmCalc
 * ==================================================================*/

int Sonic_Time2mm(unsigned char *p_time)
{
  unsigned long cycle = 0;
  unsigned long us = 0;
  unsigned long um = 0;
  unsigned long mm = 0;
  char temp = 0;
  long c = 0;

  // time limit
  if ( p_time[2] == 0xFF)
  {
    return 0;
  }

  cycle = p_time[2];
  cycle = ((cycle << 8) | p_time[1]);
  cycle = ((cycle << 8) | p_time[0]);
  temp = ((CAN_USDTREG(_read, 0) >> 8) & 0x00FF);

  // calculate sonic velocity
  if(temp & 0x80){
    temp &= 0x7F;
    c = 3315 - (6 * temp);}         //331,5m/s - 0,6 * °C
  else c = 3315 + (6 * temp);       //331,5m/s + 0,6 * °C

  // calculate distance 2 ways
  us = ((cycle * 625) / 10000);     //MicroSecs   0,0625=1Zyclus
  um = ((us * c) / 20);             //MicroMeters s=c*T=343,5m/s*t
  mm = (um / 1000) + SONIC_OFFSET;  //MilliMeters

  return mm;
}

