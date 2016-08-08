/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    TC-Functions-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Timer Counter Functions, 16MHz Quarz
* ------------------------------------------------------------------
*	Date:			21.04.2011
* lastChanges:	07.10.2013
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "mcp7941_driver.h"
#include "tc_func.h"



/* ==================================================================*
 * 						Main Timer TCC0
 * --------------------------------------------------------------
 *	TCC0 Allround while WaitingTimer
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCC0 - MicroSec
 * ------------------------------------------------------------------*/

void TCC0_WaitMicroSec_Init(int microSec)
{
	int microHerz= 16;
	int time= microSec*microHerz;

	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCC0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC0.CNT = 0;
	TCC0.PER = 60000;						//Max microSek= 3700
	TCC0.CCA = time;						//OputCompare auf
	TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCC0_CTRLA= TC_CLKSEL_DIV1_gc;			//clk/1
}


/* ------------------------------------------------------------------*
 * 						TCC0 - MilliSec
 * ------------------------------------------------------------------*/

void TCC0_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCC0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC0.CNT = 0;
	TCC0.PER = 60000;						//Max milliSek= 900
	TCC0.CCA = time;						//OputCompare
	TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCC0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCC0 - Sec
 * ------------------------------------------------------------------*/

void TCC0_WaitSec_Init(int Sec)
{
	int Herz = 15625;
	int time = Sec * Herz;

	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCC0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC0.CNT = 0;
	TCC0.PER = 60000;						//Max Sek= 3s
	TCC0.CCA = time;						//OputCompare
	TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCC0_CTRLA = TC_CLKSEL_DIV1024_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCC0 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCC0_Wait_Query(void)
{
	if(TCC0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCC0.CNT= 0;
		TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCC0_Stop(void)
{
	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}


/* ------------------------------------------------------------------*
 * 						TCC0 - Apps
 * ------------------------------------------------------------------*/

void TCC0_wait_us(int us)
{
	TCC0_WaitMicroSec_Init(us);	//startTimer
	while(!(TCC0_Wait_Query()));	//Query
	TCC0_Stop();					//stopTimer
}

void TCC0_wait_ms(int ms)
{
	TCC0_WaitMilliSec_Init(ms);	//startTimer
	while(!(TCC0_Wait_Query()));	//Query
	TCC0_Stop();					//stopTimer
}

void TCC0_wait_sec(unsigned char sec)
{
	while(sec)
	{
		TCC0_WaitSec_Init(1);			//startTimer
		while(!(TCC0_Wait_Query()));	//Query
		sec--;
	}
	TCC0_Stop();					//stopTimer
}


/* ------------------------------------------------------------------*
 * 						TCC0 - PageTimer
 * ------------------------------------------------------------------*/

void TCC0_Main_Wait(void)
{
	TCC0_wait_ms(200);		//Wait
}

void TCC0_DisplayManual_Wait(void)
{
	TCC0_wait_ms(5);		//Wait
}

void TCC0_DisplaySetup_Wait(void)
{
	TCC0_wait_ms(10);		//Wait
}

void TCC0_DisplayData_Wait(void)
{
	TCC0_wait_ms(10);		//Wait
}

void TCC0_Touch_Wait(void)
{
	TCC0_wait_ms(15);		//Wait
}

/* ------------------------------------------------------------------*
 * 						TCC0 - misc
 * ------------------------------------------------------------------*/

void TCC0_wait_3s5(void)
{
	TCC0_wait_sec(3);
	TCC0_wait_ms(500);
}




/* ==================================================================*
 * 						TWI Timer TCC1
 * --------------------------------------------------------------
 *	TWI Timer for Safety
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCC1 - MilliSec
 * ------------------------------------------------------------------*/

void TCC1_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCC1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCC1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC1.CNT = 0;
	TCC1.PER = 60000;						//Max milliSek= 900
	TCC1.CCA = time;						//OputCompare auf x*1µs
	TCC1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag

	TCC1_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCC1 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCC1_Wait_Query(void)
{
	if(TCC1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCC1.CNT= 0;
		TCC1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCC1_Stop(void)
{
	TCC1_CTRLA = TC_CLKSEL_OFF_gc;			//TCC0 OFF
}




/* ==================================================================*
 * 						Touch Timer TCD0
 * --------------------------------------------------------------
 *	Touch Timer for working with touch separatly
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCD0 - MilliSec
 * ------------------------------------------------------------------*/

void TCD0_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCD0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCD0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCD0.CNT = 0;
	TCD0.PER = 60000;						//Max milliSek= 900
	TCD0.CCA = time;						//OputCompare auf x*1Âµs
	TCD0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCD0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCD0 - Sec
 * ------------------------------------------------------------------*/

void TCD0_WaitSec_Init(int Sec)
{
	int Herz= 15625;
	int time= Sec*Herz;

	TCD0_CTRLA= TC_CLKSEL_OFF_gc;			//TCD0 OFF
	TCD0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCD0.CNT = 0;
	TCD0.PER = 60000;						//Max Sek= 3s
	TCD0.CCA = time;						//OputCompare auf x*1µs
	TCD0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCD0_CTRLA= TC_CLKSEL_DIV1024_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCD0 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCD0_Wait_Query(void)
{
	if(TCD0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCD0.CNT= 0;
		TCD0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCD0_Stop(void)
{
	TCD0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}



/* ==================================================================*
 * 						TimerIC Safety Timer TCD1
 * --------------------------------------------------------------
 *	If Timer IC don´t work, safetyTimer TCD1 will be used
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCD1 - MilliSec
 * ------------------------------------------------------------------*/

void TCD1_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCD1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCD1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCD1.CNT = 0;
	TCD1.PER = 60000;						//Max milliSek= 900
	TCD1.CCA = time;						//OputCompare auf x*1µs
	TCD1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag

	TCD1_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCD1 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCD1_Wait_Query(void)
{
	if(TCD1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCD1.CNT= 0;
		TCD1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCD1_Stop(void)
{
	TCD1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}


/* ------------------------------------------------------------------*
 * 						TCD1 - Apps***
 * ------------------------------------------------------------------*/

unsigned char TCD1_MainAuto_SafetyTC(t_FuncCmd cmd)
{
	static unsigned char i=0;

	switch(cmd)
	{
		case _init:		TCD1_WaitMilliSec_Init(100);
						i=0;								break;

		case _reset:	TCD1_WaitMilliSec_Init(100);
						i=0;	return 0;					break;

		case _exe:		if(TCD1_Wait_Query()){	i++;				//i++
							TCD1_WaitMilliSec_Init(100);}			//TC-Restart
						if(i>12){	i=0;								//1200 MilliSek
							MCP7941_WriteByte(TIC_SEC, 0x00);			//Write-Sec
								return 1;}								//Return 1
						else	return 0;					break;
		default:											break;
	}
	return 0;
}



/* ==================================================================*
 * 						Error Timer - TCE0
 * --------------------------------------------------------------
 *	Timer for Error gets onAction
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCE0 - Sec
 * ------------------------------------------------------------------*/

void TCE0_WaitSec_Init(int Sec)
{
	int Herz= 15625;
	int time= Sec*Herz;

	TCE0_CTRLA= TC_CLKSEL_OFF_gc;			//TCE0 OFF
	TCE0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCE0.CNT = 0;
	TCE0.PER = 60000;						//Max Sek= 3s
	TCE0.CCA = time;						//OputCompare
	TCE0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCE0_CTRLA= TC_CLKSEL_DIV1024_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCE0 - MilliSec
 * ------------------------------------------------------------------*/

void TCE0_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCE0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
	TCE0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCE0.CNT = 0;
	TCE0.PER = 60000;						//Max milliSek= 900
	TCE0.CCA = time;						//OputCompare
	TCE0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCE0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCE0 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCE0_Wait_Query(void)
{
	if(TCE0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCE0.CNT= 0;
		TCE0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCE0_Stop(void)
{
	TCE0_CTRLA= TC_CLKSEL_OFF_gc;			//TCE0 OFF
}


/* ------------------------------------------------------------------*
 * 						TCE0 - Apps***
 * ------------------------------------------------------------------*/

unsigned char TCE0_ErrorTimer(t_FuncCmd cmd)
{
	static unsigned char i=0;

	if(cmd == _ton)
	{
		if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc))
			TCE0_WaitSec_Init(3);				      //Init3Sec

		if(TCE0_Wait_Query())	i++;			    //increaseCounter
		if(i>2)
		{
      i=0;
      TCE0_Stop();
			return 1;							            //12sec
    }
	}
  //--------------------------------------------------openVentil
  else if(cmd == _ovent)
  {
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc))
      TCE0_WaitSec_Init(3);

    if(TCE0_Wait_Query())
    {
      i=0;
      TCE0_Stop();
      return 1;
    }
  }
  //--------------------------------------------------closeVentil
  else if(cmd == _cvent)
  {
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc)){
      i=0;
      TCE0_WaitSec_Init(3);}

    if(TCE0_Wait_Query()){
      if(!i){
        i=1;
        TCE0_WaitMilliSec_Init(300);}
      else{
        i=0;
        TCE0_Stop();
        return 1;}}
  }

  //--------------------------------------------------openVentil
  else if(cmd == _cvent_s)
  {
    if(!(TCE0_CTRLA & TC_CLKSEL_DIV1024_gc))
      TCE0_WaitSec_Init(1);

    if(TCE0_Wait_Query())
    {
      i=0;
      TCE0_Stop();
      return 1;
    }
  }

  else if(cmd == _reset)
	{
		TCE0_Stop();
		i=0;
	}

	return 0;
}



/* ==================================================================*
 * 						CAN Timer1 - TCE1
 * --------------------------------------------------------------
 *	Timer for CAN Communication
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCE1 - MilliSec
 * ------------------------------------------------------------------*/

void TCE1_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCE1_CTRLA= TC_CLKSEL_OFF_gc;			//TCCE1 OFF
	TCE1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCE1.CNT = 0;
	TCE1.PER = 60000;						//Max milliSek= 900
	TCE1.CCA = time;						//OputCompare auf x*1µs
	TCE1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag

	TCE1_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCE1 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCE1_Wait_Query(void)
{
	if(TCE1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCE1.CNT= 0;
		TCE1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCE1_Stop(void)
{
	TCE1_CTRLA= TC_CLKSEL_OFF_gc;			//TCCE1 OFF
}



/* ==================================================================*
 * 						RunTime - TCF0
 * --------------------------------------------------------------
 *	Timer for UltraSonic ReadCycle
 * --------------------------------------------------------------
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						TCF0 - Sec
 * ------------------------------------------------------------------*/

void TCF0_WaitSec_Init(int Sec)
{
	int Herz= 15625;
	int time= Sec*Herz;

	TCF0_CTRLA= TC_CLKSEL_OFF_gc;			//TCF0 OFF
	TCF0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCF0.CNT = 0;
	TCF0.PER = 60000;						//Max Sek= 3s
	TCF0.CCA = time;						//OputCompare
	TCF0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag

	TCF0_CTRLA= TC_CLKSEL_DIV1024_gc;			//clk/8
}

/* ------------------------------------------------------------------*
 * 						TCF0 - MilliSec
 * ------------------------------------------------------------------*/

void TCF0_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCF0_CTRLA= TC_CLKSEL_OFF_gc;			//TCCF0 OFF
	TCF0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCF0.CNT = 0;
	TCF0.PER = 60000;						//Max milliSek= 900
	TCF0.CCA = time;						//OputCompare auf x*1µs
	TCF0.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag

	TCF0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCF0 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCF0_Wait_Query(void)
{
	if(TCF0.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCF0.CNT= 0;
		TCF0.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCF0_Stop(void)
{
	TCF0_CTRLA= TC_CLKSEL_OFF_gc;			//TCF0 OFF
}



/* ==================================================================*
 * 						Modem Timer - TCF1
 * --------------------------------------------------------------
 *	Timer for Modem
 * --------------------------------------------------------------
 * ==================================================================*/


/* ------------------------------------------------------------------*
 * 						TCF1 - Sec
 * ------------------------------------------------------------------*/

void TCF1_WaitSec_Init(int Sec)
{
	int Herz= 15625;
	int time= Sec*Herz;

	TCF1_CTRLA= TC_CLKSEL_OFF_gc;			  //TCF1 OFF
	TCF1.CTRLB=	TC_WGMODE_NORMAL_gc;	  //Mode

	TCF1.CNT = 0;
	TCF1.PER = 60000;						        //Max Sek= 3s
	TCF1.CCA = time;						        //OputCompare
	TCF1.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag

	TCF1_CTRLA= TC_CLKSEL_DIV1024_gc;		//clk/8
}

/* ------------------------------------------------------------------*
 * 						TCF1 - MilliSec
 * ------------------------------------------------------------------*/

void TCF1_WaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCF1_CTRLA= TC_CLKSEL_OFF_gc;			//TCF1 OFF
	TCF1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCF1.CNT = 0;
	TCF1.PER = 60000;						//Max milliSek= 900
	TCF1.CCA = time;						//OputCompare auf x*1µs
	TCF1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag

	TCF1_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


/* ------------------------------------------------------------------*
 * 						TCF1 - Query and Stop
 * ------------------------------------------------------------------*/

unsigned char TCF1_Wait_Query(void)
{
	if(TCF1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCF1.CNT= 0;
		TCF1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCF1_Stop(void)
{
	TCF1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}



/*=====================================================================\
* ------------------------------------------------------------------
*	Changes:
* ------------------------------------------------------------------
*	Date				Description
* ------------------------------------------------------------------
*	18.08.2013	-	App wait functions -> without volatile var
*					      Add Modem Timer TCF1
*	07.10.2013	-	CleanUp
*	25.10.2013 	-	ErrorTimer
* ------------------------------------------------------------------
\*====================================================================*/



/**********************************************************************\
 * End of file
\**********************************************************************/
