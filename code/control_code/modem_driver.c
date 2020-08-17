/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Interception ICT
*	Name:			    Modem-GC864-driver-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
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
#include "lcd_app.h"

#include "basic_func.h"
#include "usart_func.h"
#include "tc_func.h"
#include "at24c_app.h"
#include "modem_driver.h"


/* ==================================================================*
 * 						FUNCTIONS Basics
 * ==================================================================*/

 /* ------------------------------------------------------------------*
 * 						Modem Object init
 * ------------------------------------------------------------------*/
// TODO (christian#1#): Modem_object_init ...
//
void Modem_init(struct Modem *mo)
{
   mo->turned_on = 0;
   mo->turn_on_state = 0;
   mo->turn_on_error = 0;
   mo->startup_delay = 0;
}


/* ------------------------------------------------------------------*
 * 						Init
 * ------------------------------------------------------------------*/

void Modem_Port_Init(void)
{
	PORTF.DIR = PIN0_bm |		//PF0-ModemOn-Output
              PIN1_bm	|		//PF1-ModemReset-Output
              PIN4_bm	|		//PF4-ModemRTS-Output
              PIN7_bm;		//PF7-ModemTxD-Output

	PORTCFG.MPCMASK =	PIN2_bm	|		//PF2-ModemPWRmonitor-Input
                    PIN3_bm	|		//PF3-ModemSLED-Input
                    PIN5_bm	|		//PF5-ModemCTS-Input
                    PIN6_bm;		//PF6-ModemRxD-Input

  PORTF.PIN2CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_INVEN_bm;	//Pins PULL UP
  //PORTF.PIN2CTRL = PORT_OPC_WIREDORPULL_gc;
  //PORTF.PIN2CTRL = PORT_OPC_PULLUP_gc;
  //PORTF.PIN2CTRL = PORT_OPC_BUSKEEPER_gc;

	PORTF.OUTCLR = (PIN0_bm | PIN1_bm | PIN4_bm | PIN7_bm);
	PORTF.OUTSET = (PIN2_bm | PIN3_bm | PIN5_bm | PIN6_bm);

}


/* ------------------------------------------------------------------*
 * 						Modem check
 * ------------------------------------------------------------------*/

unsigned char Modem_Check(t_page page, struct Modem *mo)
{
  // Startup wait
  if(mo->startup_delay < MO_STARTUP_DELAY)
  {
    mo->startup_delay++;
    return 1;
  }

  // status LED
  Modem_ReadSLED(page);

  // Check if off
  if(MO_PW_OFF)
  {
    mo->turned_on = 0;
    Modem_TurnOn(mo);
    mo->startup_delay = 0;
  }

  // PWR on
  else if(!MO_PW_OFF)
  {
    mo->turned_on = 1;
    mo->turn_on_state = 0;
    mo->turn_on_error = 0;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 * 						Modem TurnOn
 * ------------------------------------------------------------------*/

unsigned char Modem_TurnOn(struct Modem *mo)
{
  if(mo->turn_on_state == 0)
  {
    MO_PORT_ON;
    TCF1_WaitSec_Init(1);
    mo->turn_on_state = 1;
  }

  else if(mo->turn_on_state == 1 && TCF1_Wait_Query())
  {
    MO_PORT_OFF;
    if(MO_PW_OFF)
    {
      TCF1_WaitSec_Init(2);
      mo->turn_on_state = 2;
    }
    else
    {
      TCF1_Stop();
      mo->turn_on_state = 0;
    }
  }

  else if(mo->turn_on_state == 2 && TCF1_Wait_Query())
  {
    // timeout
    if(MO_PW_OFF)
    {
      mo->turn_on_error++;
    }
    else
    {
      TCF1_Stop();
    }
    TCF1_Stop();
    mo->turn_on_state = 0;
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

void Modem_ReadSLED(t_page page)
{
  if(page == DataMain)
	{
    //***TODO: Cool Symbol for modem
    /*
    if(PORTF.IN & PIN3_bm)
    {
      LCD_WriteMyFont(1, 40, 26);
    }
    else
    {
      LCD_ClrSpace(1, 40, 2, 4);
    }
    */
  }

  else if(page == PinModem)
	{
    if(PORTF.IN & PIN3_bm)
    {
      LCD_WriteMyFont(18, 137, 26);
    }
    else
    {
      LCD_ClrSpace(18, 137, 2, 4);
    }
  }

}


/* ------------------------------------------------------------------*
 * 						Modem ReadCTS
 * ------------------------------------------------------------------*/

unsigned char Modem_CTS_ready(void)
{
	// sending data to modem is allowed
	if(!(PORTF.IN & PIN5_bm))
	{
		return 1;
	}
	return 0;
}


void Modem_ReadCTS(void)
{
	if(Modem_CTS_ready())
	{
		LCD_FillSpace(12, 10, 2, 8);
	}
	else
	{
		LCD_ClrSpace(12, 10, 2, 8);
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
  nr.pos = 0;
  TCC0_wait_us(25);
  //NoNumber
  if(Modem_TelNr(_read, nr) == 11) return 1;

  //--------------------------------------------------DialNumber
	USART_WriteString("AT+FCLASS=8");   //EnablePhone
	USART_WriteByte(CHAR_CR);						  //CR
	TCC0_wait_ms(100);                  //wait
	USART_WriteString("ATD+");		      //CallHeader

  for(nr.pos= 0; ((nr.tel != 11) && (nr.pos < 16)); nr.pos++)
  {
    nr.tel= Modem_TelNr(_read, nr);
    if(nr.tel != 11) USART_WriteByte(nr.tel+48);
  }
  USART_WriteByte(CHAR_CR);						    //CR

  //--------------------------------------------------Wait+HangUp
  LCD_WriteMyFont(16, 119, nr.id);
  TCC0_wait_sec(1);
  unsigned char point_pos = 0;
  for(nr.pos = 0; nr.pos < MO_HANG_UP_TIME; nr.pos++)
  {
    Watchdog_Restart();
    if(point_pos > 5)
    {
      point_pos = 0;
      LCD_ClrSpace(16,119,2,41);
    }
    LCD_WriteMyFont(16, 123 + point_pos * 4, 22);  //...
    point_pos++;
    TCC0_wait_sec(1);
  }
  //DisconnectCall
  USART_WriteString("ATH");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_sec(1);
  LCD_ClrSpace(16,119,2,41);
  return 0;
}


/* ------------------------------------------------------------------*
 * 						Modem Call
 * ------------------------------------------------------------------*/

void Modem_CallAllNumbers(void)
{
  //*** modify this if calls should be enabled in debuging
  if(DEBUG) return;
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
	// data allowed from modem
	PORTF.OUTSET = PIN4_bm;				//RTS-Off

	if (Modem_CTS_ready())
	{

		//USART_WriteString("AT");
		//USART_WriteString("AT#SELINT?");
		USART_WriteString("AT#SIMDET=1");

		//USART_WriteByte(CHAR_LF);
		USART_WriteByte(CHAR_CR);


    //USART_WriteString("AT+ICF?");
    //USART_WriteByte(CHAR_CR);

		// dial number
		//Modem_DialNumber()

		//Modem_GetSoftwareversion();
		//LCD_WriteValue3(14, 130, 42);

		/*---------------------------------------------PortSpeed
		USART_WriteString("AT+IPR=115200");
		USART_WriteString("");
		USART_WriteByte(CHAR_CR);						//CR
		//---------------------------------------------*/

		/*---------------------------------------------ShutDown
		USART_WriteString("AT#SHDN");
		USART_WriteByte(CHAR_CR);						//CR
		//---------------------------------------------*/
	}

	// data allowed from modem
	PORTF.OUTCLR = PIN4_bm;				//RTS-Off
}



void Modem_Test(void)
{
	LCD_WriteStringFont(1, 10, "Hallo Modem");
	//Modem_Port_Init();

	struct Modem modem;
  Modem_init(&modem);

	LCD_WriteStringFont(4, 30,	"PWR");
	LCD_WriteStringFont(12, 30,	"CTS");

	LCD_WriteValue3(2, 130, 'A');
	LCD_WriteValue3(4, 130, 'T');
	LCD_WriteValue3(8, 130, 'O');
	LCD_WriteValue3(10, 130, 'K');

	TCC0_wait_sec(1);
	//TCF1_WaitMilliSec_Init(500);
	TCF1_WaitSec_Init(3);
	// test page
  t_page page = PinModem;

  // RTS off
  PORTF.OUTCLR = PIN4_bm;

  //unsigned char row = 1;

  // loop
	while(1)
	{
	  Watchdog_Restart();

	  // check modem status -> turn on
	  Modem_Check(page, &modem);

    // debug
    LCD_WriteValue2(20, 10, modem.turned_on);
    LCD_WriteValue2(20, 35, modem.turn_on_state);
    LCD_WriteValue2(22, 10, modem.turned_on);
    LCD_WriteValue2(22, 35, modem.startup_delay);

    // Read status
		Modem_ReadPWR();
		Modem_ReadCTS();

    //int i = 0;

    //unsigned char *p_rec;
    //p_rec = &len;  p_rec++;

    //p_rec = USART_Rx_Buffer(_read, 0);
    //LCD_WriteValue3(22, 130, *p_rec);

  	// int msg = 0;
		// msg = USART_ReadByte();

		// // check if message
		// if((msg & 0xFF00))
		// {
		// 	LCD_WriteValue3(2*row, 90, (msg & 0x00FF));
		// 	row++;
		// 	if(row > 10)
		// 	{
		// 		row = 1;
		// 	}
		// }

		if(TCF1_Wait_Query())
		{
			// send something
			Modem_SendTest();
			LCD_DeathMan(0, 0);
		}

		// if(TCF1_Wait_Query())
		// {
		// 	p_rec = USART_Rx_Buffer(_read, 0);
		// 	len = *p_rec;
		// 	p_rec++;

		// 	LCD_WriteValue2(20, 5, len);

		// 	while(i < len)
		// 	{
		// 		LCD_WriteValue3(i*2, 100, *p_rec);
		// 		p_rec++;
		// 		i++;
		// 	}

		// 	USART_Rx_Buffer(_clear, 0);
		// 	i = 0;
		// 	LCD_DeathMan(0,0);
		// 	Modem_SendTest();
		// }

	}
}
//*/


void Modem_WriteSMS(void)
{
	// text mode
	USART_WriteString("AT+CMGF=1");
	USART_WriteByte(CHAR_CR);

	//Parameters
	USART_WriteString("AT+CSMP=17,168,0,240");
	USART_WriteByte(CHAR_CR);
	TCC0_wait_ms(10);

	USART_WriteString("AT+CNMI=1,1,0,0,0");
	USART_WriteByte(CHAR_CR);
	TCC0_wait_ms(10);

	//LCD_Pin_WriteOK(1);

	USART_WriteString("AT+CMGW=");
	USART_WriteByte(0x22);
	USART_WriteString("+436802104231");
	USART_WriteByte(0x22);
	USART_WriteString(">TEST");
	USART_WriteByte(0x1A);
	USART_WriteByte(CHAR_CR);
	TCC0_wait_ms(10);

	USART_WriteString("AT+CMSS=1");
	USART_WriteByte(CHAR_CR);
	TCC0_wait_ms(10);
}


void Modem_DialNumber(void)
{
	USART_WriteString("AT+FCLASS=8");
	USART_WriteByte(CHAR_CR);
	TCC0_wait_sec(1);
	USART_WriteString("ATD+436802104231");
	USART_WriteByte(CHAR_CR);
}


void Modem_GetSoftwareversion(void)
{
	USART_WriteString("AT+CGMR");
	USART_WriteByte(CHAR_CR);
}



