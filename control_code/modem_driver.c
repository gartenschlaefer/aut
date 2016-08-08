/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Interception ICT
*	Name:			    Modem-GC864-driver-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceFile for Telit-GSM-Modem GC864Quad_v2
* ------------------------------------------------------------------
*	Date:			    01.12.2012
* lastChanges:  30.12.2014
\**********************************************************************/


#include <avr/io.h>

#include "defines.h"
#include "lcd_driver.h"

#include "basic_func.h"
#include "usart_func.h"
#include "tc_func.h"

#include "at24c_app.h"



/* ==================================================================*
 * 						FUNCTIONS Basics
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Init
 * ------------------------------------------------------------------*/

void Modem_Init(void)
{
	PORTF.DIR = PIN0_bm |		//PF0-ModemOn-Output
              PIN1_bm	|		//PF1-ModemReset-Output
              PIN4_bm	|		//PF4-ModemRTS-Output
              PIN7_bm;		//PF7-ModemTxD-Output

	PORTCFG.MPCMASK =	PIN2_bm	|		//PF2-ModemPWRmonitor-Input
                    PIN3_bm	|		//PF3-ModemSLED-Input
                    PIN5_bm	|		//PF5-ModemCTS-Input
                    PIN6_bm;		//PF6-ModemRxD-Input

  PORTF.PIN2CTRL = PORT_OPC_WIREDANDPULL_gc;	//Pins PULL UP
	PORTF.OUT = 		0x00;						            //Clr Modem Outputs
	PORTF.OUTCLR = PIN4_bm;						          //RTS-Off
}


/* ------------------------------------------------------------------*
 * 						Modem TurnOn
 * ------------------------------------------------------------------*/

unsigned char Modem_TurnOn(void)
{
  static unsigned char state=0;
	if(PORTF.IN & PIN2_bm)				  //IfPWR-Off?
	{
	  if(!state)
	  {
	    PORTF.OUTSET=	PIN0_bm;		    //TurnOnModem
	    TCF1_WaitSec_Init(3);
      state= 1;
    }

    else if(state == 1 && TCF1_Wait_Query())
    {
		  TCF1_WaitSec_Init(2);
		  state= 2;
		}

		else if(state == 2 && TCF1_Wait_Query())
		{
      PORTF.OUTCLR=	PIN0_bm;		    //ClrOnSignal
      TCF1_Stop();
      if(PORTF.IN & PIN2_bm)  state= 3;
      else return _on;
		}

		else if(state == 3)
		{
      return _notav;   //NotAvailable
		}
	}
	else                  //IfPWR-On
	{
    if(state)
    {
      PORTF.OUTCLR=	PIN0_bm;		    //ClrOnSignal
      TCF1_Stop();
      state= 0;
    }
	  return _on;
  }
	return 0;
}


/* ------------------------------------------------------------------*
 * 						Modem TurnOff
 * ------------------------------------------------------------------*/

void Modem_TurnOff(void)
{
	if(!(PORTF.IN & PIN2_bm))			    //PWR-On?
	{
		PORTF.OUTSET=	PIN0_bm;		      //TurnOff Modem
		while(!(PORTF.IN & PIN2_bm));		//Wait until turned off
		TCC0_wait_sec(1);
		PORTF.OUTCLR=	PIN0_bm;		      //ClrOn Signal
	}
}


/* ------------------------------------------------------------------*
 * 						Modem ReadPWR-Monitor
 * ------------------------------------------------------------------*/

void Modem_ReadPWR(void)
{
	if(PORTF.IN & PIN2_bm)
	{
		LCD_FillSpace(4, 10, 2, 8);
	}
	else
	{
		LCD_ClrSpace(4, 10, 2, 8);
	}
}


/* ------------------------------------------------------------------*
 * 						Modem ReadSLED
 * ------------------------------------------------------------------*/

void Modem_ReadSLED(void)
{
	if(PORTF.IN & PIN3_bm)
	{
		LCD_FillSpace(18, 137, 1, 4);
	}
	else
	{
		LCD_ClrSpace(18, 137, 1, 4);
	}
}


/* ------------------------------------------------------------------*
 * 						Modem ReadCTS
 * ------------------------------------------------------------------*/

void Modem_ReadCTS(void)
{
	if(PORTF.IN & PIN5_bm)
	{
		LCD_FillSpace(12, 10, 2, 8);
	}
	else
	{
		LCD_ClrSpace(12, 10, 2, 8);
	}
}


/* ------------------------------------------------------------------*
 * 						Modem ReadRxD
 * ------------------------------------------------------------------*/

void Modem_ReadRxD(void)
{
	if(PORTF.IN & PIN6_bm)
	{
		LCD_FillSpace(16, 10, 2, 8);
	}
	else
	{
		LCD_ClrSpace(16, 10, 2, 8);
	}
}



/* ------------------------------------------------------------------*
 * 						Modem Tel.Nr
 * ------------------------------------------------------------------*/

char Modem_TelNr(t_FuncCmd cmd, TelNr nr)
{
  static char tmp[16]={ 11,11,11,11,11, 11,11,11,11,11,
                        11,11,11,11,11, 11};
  unsigned char i=0;

  //--------------------------------------------------Init
  if(cmd == _init)
  {
    for(i=0; i<16; i++)
    {
      tmp[i]= AT24C_ReadVar(i + 16 * (nr.id - 1));
      TCC0_wait_us(50);
    }
  }

  //--------------------------------------------------WriteTmp
  else if(cmd == _write)
  {
    tmp[nr.pos]= nr.tel;
  }

  //--------------------------------------------------ReadEEPROM
  else if(cmd == _read)
  {
    if(!nr.id)  return 'X';     //NoNumberSelected
    return AT24C_ReadVar(nr.pos + 16 * (nr.id - 1));
  }

  //--------------------------------------------------ReadTmp
  else if(cmd == _read2)
  {
    if(!nr.id)  return 'X';     //NoNumberSelected
    return tmp[nr.pos];
  }

  //--------------------------------------------------Write2EEPROM
  else if(cmd == _save)
  {
    if(!nr.id)  return 'X';     //NoNumberSelected
    for(i=0; i<16; i++)
    {
      AT24C_WriteVar(i + 16*(nr.id - 1), tmp[i]);
      TCC0_wait_ms(10);
    }
  }

  //--------------------------------------------------ResetTemp
  else if(cmd == _reset)
  {
    if(!nr.id)  return 'X';     //NoNumberSelected
    for(i=0; i<16; i++)
    {
      tmp[i]= 11;
      TCC0_wait_ms(10);
    }
  }
  return AT24C_ReadVar(nr.pos);
}



/* ------------------------------------------------------------------*
 * 						Modem Call
 * ------------------------------------------------------------------*/

unsigned char Modem_Call(TelNr nr)
{
  nr.pos= 0;
  TCC0_wait_us(25);                     //wait
  if(Modem_TelNr(_read, nr) == 11) return 1;            //NoNumber

  //--------------------------------------------------DialNumber
	USART_WriteString("AT+FCLASS=8");   //EnablePhone
	USART_WriteByte(0x0D);						  //CR
	TCC0_wait_ms(100);                  //wait
	USART_WriteString("ATD+");		      //CallHeader

  for(nr.pos= 0; ((nr.tel != 11) && (nr.pos < 16)); nr.pos++)
  {
    nr.tel= Modem_TelNr(_read, nr);
    if(nr.tel != 11) USART_WriteByte(nr.tel+48);
  }
  USART_WriteByte(0x0D);						    //CR

  //--------------------------------------------------Wait+HangUp
  LCD_WriteMyFont(16, 119, nr.id);      //TelNr
  TCC0_wait_sec(1);                     //wait
  for(nr.pos = 0; nr.pos < 9; nr.pos++)     //10
  {
    Watchdog_Restart();                   //WatchdogRestart
    LCD_WriteMyFont(16, 123+nr.pos*4, 22);  //...
    TCC0_wait_sec(1);                       //wait
  }
  USART_WriteString("ATH");             //DisconnectCall
  USART_WriteByte(0x0D);						    //CR
  TCC0_wait_sec(1);                     //wait
  LCD_ClrSpace(16,119,2,41);            //ClearSpace
  return 0;
}


/* ------------------------------------------------------------------*
 * 						Modem Call
 * ------------------------------------------------------------------*/
void Modem_CallAllNumbers(void)
{
  TelNr nr;
  nr.id = 1;
  Modem_Call(nr);
  nr.id = 2;
  Modem_Call(nr);
}


/* ------------------------------------------------------------------*
 * 						Modem SendTest
 * ------------------------------------------------------------------*/

void Modem_SendTest(void)
{
	//PORTF.OUTSET= PIN4_bm;				//RTS-Off
	PORTF.OUTCLR= PIN4_bm;
	TCC0_wait_ms(1);
	/*---------------------------------------------AT
	USART_WriteString("AT");
	USART_WriteByte(0xD0);			//CR
	//---------------------------------------------*/
	//*---------------------------------------------AT
	USART_WriteByte('A');			//0x41
	TCC0_wait_ms(1);
	USART_WriteByte('T');			//0x54
	TCC0_wait_ms(1);
	USART_WriteByte(0x0D);			//CR
	//---------------------------------------------*/

	/*---------------------------------------------SMS
	USART_WriteString("AT+CMGF=1");				//TextMode
	USART_WriteByte(0x0D);						//CR

	USART_WriteString("AT+CSMP=17,168,0,240");	//Parameters
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CNMI=1,1,0,0,0");
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);LCD_Pin_WriteOK(1);

	USART_WriteString("AT+CMGW=");
	USART_WriteByte(0x22);						//"
	USART_WriteString("+436802104231");
	USART_WriteByte(0x22);						//"
	USART_WriteString(">TEST");
	USART_WriteByte(0x1A);						//Ctrl-Z
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=1");
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=2");
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=3");
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=4");
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=5");
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=6");
	USART_WriteByte(0x0D);						//CR
	//---------------------------------------------*/


	/*---------------------------------------------PortSpeed
	USART_WriteString("AT+IPR=115200");
	USART_WriteString("");
	USART_WriteByte(0x0D);						//CR
	//---------------------------------------------*/

	/*---------------------------------------------SoftwareVersion
	USART_WriteString("AT+CGMR");
	USART_WriteByte(0x0D);						//CR
	//---------------------------------------------*/

	/*---------------------------------------------ShutDown
	USART_WriteString("AT#SHDN");
	USART_WriteByte(0x0D);						//CR
	//---------------------------------------------*/

	/*---------------------------------------------DialNumber
	USART_WriteString("AT+FCLASS=8");			//EnablePhone
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_sec(1);
	USART_WriteString("ATD+436802104231");		//Call
	USART_WriteByte(0x0D);						//CR
	//---------------------------------------------*/

	/*---------------------------------------------DialNumber - AT+FCLASS=8 - ATD+436802104231
	USART_WriteByte('A');
	USART_WriteByte('T');
	USART_WriteByte('+');
	USART_WriteByte('F');
	USART_WriteByte('C');
	USART_WriteByte('L');
	USART_WriteByte('A');
	USART_WriteByte('S');
	USART_WriteByte('S');
	USART_WriteByte('=');
	USART_WriteByte('8');
	USART_WriteByte(0x0D);						//CR
	TCC0_wait_sec(1);

	USART_WriteByte('A');
	USART_WriteByte('T');
	USART_WriteByte('D');
	USART_WriteByte('+');
	USART_WriteByte('4');
	USART_WriteByte('3');
	USART_WriteByte('6');
	USART_WriteByte('8');
	USART_WriteByte('0');
	USART_WriteByte('2');
	USART_WriteByte('1');
	USART_WriteByte('0');
	USART_WriteByte('4');
	USART_WriteByte('2');
	USART_WriteByte('3');
	USART_WriteByte('1');
	USART_WriteByte(0x0D);						//CR
	//---------------------------------------------*/

	TCC0_wait_ms(1);
	//PORTF.OUTSET= PIN4_bm;				//RTS-Off
}


//*--------------------------------------------------Modem
void Modem_Test(void)
{
  /*
	unsigned char 	len=1;
	int	i=0;
	unsigned char 	*p_rec;
  p_rec= &len;  p_rec++;
  */

	LCD_WriteStringFont(1, 10, "Hallo Modemm");
	Modem_Init();

	LCD_WriteStringFont(4, 30,	"PWR");
	LCD_WriteStringFont(8, 30,	"SLED");
	LCD_WriteStringFont(12, 30,	"CTS");
	LCD_WriteStringFont(16, 30,	"RxD");

	LCD_WriteValue3(2, 130, 'A');
	LCD_WriteValue3(4, 130, 'T');
	LCD_WriteValue3(8, 130, 'O');
	LCD_WriteValue3(10, 130, 'K');

	TCC0_wait_sec(1);
	Modem_SendTest();

	//TCF1_WaitMilliSec_Init(100);

	while(1)
	{
	  unsigned char mod;
	  mod= Modem_TurnOn();
	  if(mod == _on)    LCD_WriteStringFont(0, 0, "on");
    if(mod == _notav) LCD_WriteStringFont(0, 0, "notav");

		Modem_ReadPWR();
		Modem_ReadSLED();
		Modem_ReadCTS();
		Modem_ReadRxD();

		/*
		i= USART_ReadByte();
		if((i & 0xFF00))
		{
			LCD_WriteValue3(2*len, 90, (i & 0x00FF));
			len++;
		}

		if(TCF1_Wait_Query())
		{
			p_rec= USART_Rx_Buffer(_read, 0);
			len= *p_rec;
			p_rec++;

			LCD_WriteValue2(20, 5, len);

			while(i<len)
			{
				LCD_WriteValue3(i*2, 100, *p_rec);
				p_rec++;
				i++;
			}

			USART_Rx_Buffer(_clear, 0);
			i=0;
			LCD_DeathMan(0,0);
			Modem_SendTest();
		}
		*/
	}
}




/**********************************************************************\
 * End of file
\**********************************************************************/



