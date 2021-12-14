// --
//  Telit-GSM-Modem GC864Quad_v2

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
 *            FUNCTIONS Basics
 * ==================================================================*/

 /* ------------------------------------------------------------------*
 *            Modem Object init
 * ------------------------------------------------------------------*/

void Modem_init(struct Modem *mo)
{
   mo->turned_on = 0;
   mo->turn_on_state = 0;
   mo->turn_on_error = 0;
   mo->startup_delay = 0;
}


/* ------------------------------------------------------------------*
 *            Init
 * ------------------------------------------------------------------*/

void Modem_Port_Init(void)
{
  // outputs
  PORTF.DIR = PIN0_bm |   //PF0-ModemOn-Output
              PIN1_bm |   //PF1-ModemReset-Output
              PIN4_bm |   //PF4-ModemRTS-Output
              PIN7_bm;    //PF7-ModemTxD-Output

  // inputs
  PORTCFG.MPCMASK = PIN2_bm |   //PF2-ModemPWRmonitor-Input
                    PIN3_bm |   //PF3-ModemSLED-Input
                    PIN5_bm |   //PF5-ModemCTS-Input
                    PIN6_bm;    //PF6-ModemRxD-Input

  //Pins PULL UP and inverted
  PORTF.PIN2CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_INVEN_bm;

  // config
  PORTF.OUTCLR = (PIN0_bm | PIN1_bm | PIN4_bm | PIN7_bm);
  PORTF.OUTSET = (PIN2_bm | PIN3_bm | PIN5_bm | PIN6_bm);

}


/* ------------------------------------------------------------------*
 *            Modem check
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
 *            Modem TurnOn
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
 *            Modem TurnOff
 * ------------------------------------------------------------------*/

void Modem_TurnOff(void)
{
  if(!(PORTF.IN & PIN2_bm))         //PWR-On?
  {
    PORTF.OUTSET= PIN0_bm;          //TurnOff Modem
    while(!(PORTF.IN & PIN2_bm));   //Wait until turned off
    TCC0_wait_sec(1);
    PORTF.OUTCLR= PIN0_bm;          //ClrOn Signal
  }
}


/* ------------------------------------------------------------------*
 *            Modem ReadPWR-Monitor
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
 *            Modem ReadSLED
 * ------------------------------------------------------------------*/

void Modem_ReadSLED(t_page page)
{
  if(page == DataMain)
  {
    if(!(PORTF.IN & PIN3_bm))
    {
      LCD_WriteMyFont(1, 152, 26);
    }
    else
    {
      LCD_ClrSpace(1, 152, 2, 4);
    }
  }

  else if(page == PinModem)
  {
    if(!(PORTF.IN & PIN3_bm))
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
 *            Modem ReadCTS
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
 *            Modem Tel.Nr
 * ------------------------------------------------------------------*/

char Modem_TelNr(t_FuncCmd cmd, TelNr nr)
{
  static char tmp[16] = { 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 };
  unsigned char i = 0;

  //--------------------------------------------------Init
  if(cmd == _init)
  {
    for(i = 0; i < 16; i++)
    {
      tmp[i] = AT24C_ReadVar(i + 16 * (nr.id - 1));
      TCC0_wait_us(50);
    }
  }

  //--------------------------------------------------WriteTmp
  else if(cmd == _write)
  {
    tmp[nr.pos] = nr.tel;
  }

  //--------------------------------------------------ReadEEPROM
  else if(cmd == _read)
  {
    if(!nr.id)  return 'X';
    return AT24C_ReadVar(nr.pos + 16 * (nr.id - 1));
  }

  //--------------------------------------------------ReadTmp
  else if(cmd == _read2)
  {
    if(!nr.id)  return 'X';
    return tmp[nr.pos];
  }

  //--------------------------------------------------Write2EEPROM
  else if(cmd == _save)
  {
    if(!nr.id)  return 'X';
    for(i = 0; i < 16; i++)
    {
      AT24C_WriteVar(i + 16*(nr.id - 1), tmp[i]);
      TCC0_wait_ms(10);
    }
  }

  //--------------------------------------------------ResetTemp
  else if(cmd == _reset)
  {
    if(!nr.id)  return 'X';
    for(i = 0; i < 16; i++)
    {
      tmp[i] = 11;
      TCC0_wait_ms(10);
    }
  }
  return AT24C_ReadVar(nr.pos);
}



/* ------------------------------------------------------------------*
 *            Modem Call
 * ------------------------------------------------------------------*/

unsigned char Modem_Call(TelNr nr)
{
  nr.pos = 0;
  TCC0_wait_us(25);

  // NoNumber selected
  if(Modem_TelNr(_read, nr) == 11) return 1;

  //--------------------------------------------------DialNumber
  USART_WriteString("AT+FCLASS=8");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(100);
  USART_WriteString("ATD+");

  for(nr.pos= 0; ((nr.tel != 11) && (nr.pos < 16)); nr.pos++)
  {
    nr.tel= Modem_TelNr(_read, nr);
    if(nr.tel != 11) USART_WriteByte(nr.tel+48);
  }
  USART_WriteByte(CHAR_CR);

  //--------------------------------------------------Wait+HangUp
  LCD_WriteMyFont(16, 119, nr.id);
  TCC0_wait_sec(1);
  unsigned char point_pos = 0;
  for(nr.pos = 0; nr.pos < MO_HANG_UP_TIME; nr.pos++)
  {
    WDT_RESET;
    if(point_pos > 5)
    {
      point_pos = 0;
      LCD_ClrSpace(16, 119, 2, 41);
    }

    // ...
    LCD_WriteMyFont(16, 123 + point_pos * 4, 22);
    point_pos++;
    TCC0_wait_sec(1);
  }

  //DisconnectCall
  USART_WriteString("ATH");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_sec(1);
  LCD_ClrSpace(16, 119, 2, 41);

  return 0;
}


void Modem_SMS(TelNr nr, char msg[])
{

  // text mode
  USART_WriteString("AT+CMGF=1");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(500);

  USART_WriteString("AT+CNMI=1,1,0,0,0");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(500);

  USART_WriteString("AT+CMGS=");
  USART_WriteByte(0x22);
  USART_WriteString("+");

  // get number and write
  for(nr.pos= 0; ((nr.tel != 11) && (nr.pos < 16)); nr.pos++)
  {
    nr.tel= Modem_TelNr(_read, nr);
    if(nr.tel != 11) USART_WriteByte(nr.tel+48);
  }

  USART_WriteByte(0x22);
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(500);

  // message here
  USART_WriteString(msg);

  // end routine
  USART_WriteByte(CHAR_CR);
  USART_WriteByte(CHAR_CTR_Z);
  TCC0_wait_ms(500);
}


/* ------------------------------------------------------------------*
 *            Modem Call / SMS / Alert
 * ------------------------------------------------------------------*/

void Modem_CallAllNumbers(void)
{
  TelNr nr;
  nr.id = 1;
  Modem_Call(nr);
  nr.id = 2;
  Modem_Call(nr);
}


void Modem_SMSAllNumbers(char msg[])
{
  TelNr nr;

  // fist number
  nr.id = 1;

  WDT_RESET;
  Modem_SMS(nr, msg);
  TCC0_wait_sec(5);

  // second number
  nr.id = 2;
  WDT_RESET;
  Modem_SMS(nr, msg);
}


void Modem_Alert(char msg[])
{
  //*** debug modem alert off
  if(DEB_MODEM) return;

  if(SMS_ON)
  {
    Modem_SMSAllNumbers(msg);
  }
  else
  {
    Modem_CallAllNumbers();
  }
}




/* ------------------------------------------------------------------*
 *            Modem SendTest
 * ------------------------------------------------------------------*/

// helper var
int call_done = 0;

void Modem_SendTest(void)
{
  // data not allowed from modem
  //PORTF.OUTSET = PIN4_bm;

  // data allowed from modem
  PORTF.OUTCLR = PIN4_bm;

  if (!call_done)
  {
      // dial number
      //Modem_DialNumber();

      // sms
      WDT_RESET;
      Modem_WriteSMS_Test("msg 30");
      TCC0_wait_sec(3);

      WDT_RESET;
      Modem_WriteSMS_Test("msg 31");
      TCC0_wait_sec(3);

      WDT_RESET;
      Modem_WriteSMS_Test("msg 32");
      TCC0_wait_sec(3);

      WDT_RESET;
      Modem_WriteSMS_Test("msg 33");
      TCC0_wait_sec(3);

      call_done = 1;
  }
}


void Modem_Test(void)
{

  // some text
  LCD_WriteStringFont(1, 10, "Modem");
  LCD_WriteStringFont(4, 30,  "PWR");
  LCD_WriteStringFont(12, 30, "CTS");

  // modem init
  struct Modem modem;
  Modem_init(&modem);

  // timers
  TCC0_wait_sec(1);
  TCF1_WaitSec_Init(3);

  // test page
  t_page page = PinModem;

  // RTS off
  PORTF.OUTCLR = PIN4_bm;

  // wait variable
  int wait_var = 0;

  // loop
  while(1)
  {
    // watchdog
    WDT_RESET;

    // check modem status -> turn on
    Modem_Check(page, &modem);

    // Read status
    Modem_ReadPWR();
    Modem_ReadCTS();

    // time instance
    if(TCF1_Wait_Query())
    {
      // add counter
      wait_var++;

      // wait for ready modem
      if (Modem_CTS_ready())
      {

        // send
        if (wait_var == 5)
        {
          // send something
          LCD_WriteStringFont(15, 30, "sent");
          Modem_SendTest();
        }

        // read buffer
        else
        {
          unsigned char *p_rec = USART_Rx_Buffer(_read, 0);
          unsigned char len = *p_rec;
          unsigned char i = 0;
          unsigned char j = 0;
          p_rec++;

          // print length
          LCD_WriteValue2(20, 5, len);

          // print message
          while(len)
          {
            LCD_WriteValue3(i*2, 70+30*j, *p_rec);
            p_rec++;
            len--;
            i++;
            if (i > 11)
            {
              i = 0;
              j++;
            }
            if (j > 2)
            {
              j = 0;
            }

          }

          // clear buffer
          USART_Rx_Buffer(_clear, 0);
        }
      }

      else
      {
        wait_var = 0;
      }

      // see if it still runs
      LCD_DeathMan(0, 0);
    }
  }
}


void Modem_WriteSMS_Test(char msg[])
{

  // text mode
  USART_WriteString("AT+CMGF=1");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(500);

  USART_WriteString("AT+CNMI=1,1,0,0,0");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(500);

  USART_WriteString("AT+CMGS=");
  USART_WriteByte(0x22);
  USART_WriteString("+436802104231");
  USART_WriteByte(0x22);
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(500);

  // start routine
  USART_WriteByte(CHAR_CR);
  USART_WriteByte(CHAR_LF);
  USART_WriteByte(CHAR_STX);

  // message here
  USART_WriteString(msg);

  // end routine
  USART_WriteByte(CHAR_CR);
  USART_WriteByte(CHAR_LF);
  USART_WriteByte(CHAR_ETX);
  USART_WriteByte(CHAR_CTR_Z);
  TCC0_wait_ms(500);
}


void Modem_DialNumber(void)
{
  USART_WriteString("AT+FCLASS=8");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(100);
  USART_WriteString("ATD+436802104231");
  USART_WriteByte(CHAR_CR);
}


void Modem_GetSoftwareversion(void)
{
  USART_WriteString("AT+CGMR");
  USART_WriteByte(CHAR_CR);
}


void Modem_Shutdown(void)
{
  USART_WriteString("AT#SHDN");
  USART_WriteByte(CHAR_CR);
}
