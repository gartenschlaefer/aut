/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			TC-Functions-SourceFile
* ------------------------------------------------------------------
*	�-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Timer Counter Funktionen, 16MHz Quarz
*	Wait Sec, MilliSec and MicroSec
* ------------------------------------------------------------------														
*	Date:			21.04.2011  	
* 	lastChanges:	19.10.2011									
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_TC_func.h"
#include "SL_DS1302_driver.h"



/* ===================================================================*
 * 						FUNCTIONS 16MHz Timer
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Main Timer TCC0
 * -------------------------------------------------------------------*/

void TCC0_16MHzWaitMicroSec_Init(int microSec)
{
	int microHerz= 16;
	int time= microSec*microHerz;

	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF	
	TCC0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC0.CNT = 0;
	TCC0.PER = 60000;						//Max microSek= 3700
	TCC0.CCA = time;						//OputCompare auf x*1µs	
	TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag						
		
	TCC0_CTRLA= TC_CLKSEL_DIV1_gc;			//clk/1
}

void TCC0_16MHzWaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF	
	TCC0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC0.CNT = 0;
	TCC0.PER = 60000;						//Max milliSek= 900
	TCC0.CCA = time;						//OputCompare auf x*1µs	
	TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag			
		
	TCC0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}

void TCC0_16MHzWaitSec_Init(int Sec)
{
	int Herz= 15625;
	int time= Sec*Herz;

	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF	
	TCC0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC0.CNT = 0;
	TCC0.PER = 60000;						//Max Sek= 3s
	TCC0.CCA = time;						//OputCompare auf x*1µs	
	TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag			
		
	TCC0_CTRLA= TC_CLKSEL_DIV1024_gc;			//clk/8
}

unsigned char TCC0_16MHzWait_Query(void)
{
	if(TCC0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCC0.CNT= 0;
		TCC0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCC0_16MHz_Stop(void)
{
	TCC0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}







/* -------------------------------------------------------------------*
 * 						TWI Timer TCC1
 * -------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 * 	TWI_Master_SendeAbbruchTimer
 *	abgeschlossen:				13.05.2011
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Der Master wartet nur eine bestimmte Zeit, bis er das Senden
 *  abbricht und von Neuem startet
 * -------------------------------------------------------------------*/

void TCC1_16MHzWaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCC1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF	
	TCC1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCC1.CNT = 0;
	TCC1.PER = 60000;						//Max milliSek= 900
	TCC1.CCA = time;						//OputCompare auf x*1�s	
	TCC1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag			
		
	TCC1_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}

unsigned char TCC1_16MHzWait_Query(void)
{
	if(TCC1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCC1.CNT= 0;
		TCC1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCC1_16MHz_Stop(void)
{
	TCC1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}




/* -------------------------------------------------------------------*
 * 						Touch Timer TCD0
 * -------------------------------------------------------------------*/


void TCD0_16MHzWaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCD0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF	
	TCD0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCD0.CNT = 0;
	TCD0.PER = 60000;						//Max milliSek= 900
	TCD0.CCA = time;						//OputCompare auf x*1µs	
	TCD0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag			
		
	TCD0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}


void TCD0_16MHzWaitSec_Init(int Sec)
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

unsigned char TCD0_16MHzWait_Query(void)
{
	if(TCD0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCD0.CNT= 0;
		TCD0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCD0_16MHz_Stop(void)
{
	TCD0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}





/* -------------------------------------------------------------------*
 * 						DS1302 Timer TCD1
 * -------------------------------------------------------------------*/

void TCD1_16MHzWaitMicroSec_Init(int microSec)
{
	int microHerz= 16;
	int time= microSec*microHerz;

	TCD1_CTRLA= TC_CLKSEL_OFF_gc;			//TCD1 OFF	
	TCD1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCD1.CNT = 0;
	TCD1.PER = 60000;						//Max microSek= 3700
	TCD1.CCA = time;						//OputCompare auf x*1µs	
	TCD1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag						
		
	TCD1_CTRLA= TC_CLKSEL_DIV1_gc;			//clk/1
}

unsigned char TCD1_16MHzWait_Query(void)
{
	if(TCD1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCD1.CNT= 0;
		TCD1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCD1_16MHz_Stop(void)
{
	TCD1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}




/* -------------------------------------------------------------------*
 * 						Port/Output Timer TCE0
 * -------------------------------------------------------------------*/

void TCE0_16MHzWaitSec_Init(int Sec)
{
	int Herz= 15625;
	int time= Sec*Herz;

	TCE0_CTRLA= TC_CLKSEL_OFF_gc;			//TCE0 OFF	
	TCE0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCE0.CNT = 0;
	TCE0.PER = 60000;						//Max Sek= 3s
	TCE0.CCA = time;						//OputCompare auf x*1µs	
	TCE0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag			
		
	TCE0_CTRLA= TC_CLKSEL_DIV1024_gc;			//clk/8
}

void TCE0_16MHzWaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCE0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF	
	TCE0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCE0.CNT = 0;
	TCE0.PER = 60000;						//Max milliSek= 900
	TCE0.CCA = time;						//OputCompare auf x*1µs	
	TCE0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag			
		
	TCE0_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}

unsigned char TCE0_16MHzWait_Query(void)
{
	if(TCE0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCE0.CNT= 0;
		TCE0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCE0_16MHz_Stop(void)
{
	TCE0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}





/* -------------------------------------------------------------------*
 * 						Display Timer TCE1
 * -------------------------------------------------------------------*/

void TCE1_16MHzWaitMilliSec_Init(int milliSec)
{
	int milliHerz= 63;
	int time= milliSec*milliHerz;

	TCE1_CTRLA= TC_CLKSEL_OFF_gc;			//TCE1 OFF	
	TCE1.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCE1.CNT = 0;
	TCE1.PER = 60000;						//Max milliSek= 900
	TCE1.CCA = time;						//OputCompare auf x*1µs	
	TCE1.INTFLAGS |= (1<<TC1_CCAIF_bp);		//reset INTFlag			
		
	TCE1_CTRLA= TC_CLKSEL_DIV256_gc;			//clk/8
}

unsigned char TCE1_16MHzWait_Query(void)
{
	if(TCE1.INTFLAGS & (1<<TC1_CCAIF_bp))
	{
		TCE1.CNT= 0;
		TCE1.INTFLAGS |= (1<<TC1_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCE1_16MHz_Stop(void)
{
	TCE1_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}






/* -------------------------------------------------------------------*
 * 						AD8555 Timer TCF0
 * -------------------------------------------------------------------*/

void TCF0_16MHzWaitMicroSec_Init(int microSec)
{
	int microHerz= 16;
	int time= microSec*microHerz;

	TCF0_CTRLA= TC_CLKSEL_OFF_gc;			//TCF0 OFF	
	TCF0.CTRLB=	TC_WGMODE_NORMAL_gc;		//Mode

	TCF0.CNT = 0;
	TCF0.PER = 60000;						//Max microSek= 3700
	TCF0.CCA = time;						//OputCompare auf x*1µs	
	TCF0.INTFLAGS |= (1<<TC0_CCAIF_bp);		//reset INTFlag						
		
	TCF0_CTRLA= TC_CLKSEL_DIV1_gc;			//clk/1
}


unsigned char TCF0_16MHzWait_Query(void)
{
	if(TCF0.INTFLAGS & (1<<TC0_CCAIF_bp))
	{
		TCF0.CNT= 0;
		TCF0.INTFLAGS |= (1<<TC0_CCAIF_bp);	//reset INTFlag
		return 1;
	}
	return 0;
}

void TCF0_16MHz_Stop(void)
{
	TCF0_CTRLA= TC_CLKSEL_OFF_gc;			//TCC0 OFF
}










/* ===================================================================*
 * 						Timer Apps
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Main Timer TCC0
 * -------------------------------------------------------------------*/

void TC_Main_Wait(void)
{
	volatile unsigned char wait=0;

	TCC0_16MHzWaitMilliSec_Init(200);
	wait= TCC0_16MHzWait_Query();

	while(!wait)
	{
		wait= TCC0_16MHzWait_Query();
	}
	TCC0_16MHz_Stop();
}

unsigned char TC_MainAuto_SafetyTC(t_FuncCmd cmd)
{
	static unsigned char i=0;	

	switch(cmd)
	{
		case _init:		TCC0_16MHzWaitMilliSec_Init(10);
						i=0;								break;

		case _reset:	TCC0_16MHzWaitMilliSec_Init(10);
						i=0;	return 0;				break;
							
		case _exe:		if(TCC0_16MHzWait_Query()){	i++;				//i++	
							TCC0_16MHzWaitMilliSec_Init(10);}			//TC-Restart
						if(i>115){	i=0;								//1150 MilliSek
							DS1302_WriteByte(W_SEC, 0x00);				//Write-Sec
								return 1;}								//Return 1
						else	return 0;					break;
		default:											break;
	}
	return 0;
}


/* -------------------------------------------------------------------*
 * 						Touch Timer TCD0
 * -------------------------------------------------------------------*/

void TC_Touch_Wait(void)
{
	volatile unsigned char wait=0;

	TCD0_16MHzWaitMilliSec_Init(15);
	wait= TCD0_16MHzWait_Query();

	while(!wait)
	{
		wait= TCD0_16MHzWait_Query();
	}
	TCD0_16MHz_Stop();
}





/* -------------------------------------------------------------------*
 * 						DS1302 Timer TCD1
 * -------------------------------------------------------------------*/

void TC_DS1302_Wait(void)
{
	volatile unsigned char wait=0;

	TCD1_16MHzWaitMicroSec_Init(20);			//start Timer 10us
	wait= TCD1_16MHzWait_Query();

	while(!wait)
	{
		wait= TCD1_16MHzWait_Query();
	}
	TCD1_16MHz_Stop();
}




/* -------------------------------------------------------------------*
 * 						Port/Output/Ventil Timer TCE0
 * -------------------------------------------------------------------*/

void TC_Ventil_Wait_3s5(void)
{
	TC_Ventil_Wait_3s();
	TC_Ventil_Wait_500ms();
}


void TC_Ventil_Wait_3s(void)
{
	volatile unsigned char wait=0;

	TCE0_16MHzWaitSec_Init(3);
	wait= TCE0_16MHzWait_Query();
	while(!wait)
	{
		wait= TCE0_16MHzWait_Query();
	}
	TCE0_16MHz_Stop();
}

void TC_Ventil_Wait_2s(void)
{
	TC_Ventil_Wait_1s();
	TC_Ventil_Wait_1s();
}


void TC_Ventil_Wait_1s(void)
{
	volatile unsigned char wait=0;

	TCE0_16MHzWaitSec_Init(1);
	wait= TCE0_16MHzWait_Query();
	while(!wait)
	{
		wait= TCE0_16MHzWait_Query();
	}
	TCE0_16MHz_Stop();
}

void TC_Ventil_Wait_500ms(void)
{
	volatile unsigned char wait=0;

	TCE0_16MHzWaitMilliSec_Init(500);
	wait= TCE0_16MHzWait_Query();
	while(!wait)
	{
		wait= TCE0_16MHzWait_Query();
	}
	TCE0_16MHz_Stop();
}


/* -------------------------------------------------------------------*
 * 						Display Timer TCE1
 * -------------------------------------------------------------------*/

void TC_DisplayManual_Wait(void)
{
	volatile unsigned char wait=0;

	TCE1_16MHzWaitMilliSec_Init(50);	//Timer
	wait= TCE1_16MHzWait_Query();
	while(!wait)
	{
		wait= TCE1_16MHzWait_Query();
	}
	TCE1_16MHz_Stop();					//Stop Timer
}


void TC_DisplaySetup_Wait(void)
{
	volatile unsigned char wait=0;

	TCE1_16MHzWaitMilliSec_Init(80);	//Timer
	wait= TCE1_16MHzWait_Query();
	while(!wait)
	{
		wait= TCE1_16MHzWait_Query();
	}
	TCE1_16MHz_Stop();					//Stop Timer
}


void TC_DisplayData_Wait(void)
{
	volatile unsigned char wait=0;

	TCE1_16MHzWaitMilliSec_Init(100);	//Timer
	wait= TCE1_16MHzWait_Query();
	while(!wait)
	{
		wait= TCE1_16MHzWait_Query();
	}
	TCE1_16MHz_Stop();					//Stop Timer
}







/*********************************************************************\
 * End of SL_TC_func.c
\**********************************************************************/
