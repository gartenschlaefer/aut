// --
//  Telit GSM modem GC864Quad_v2

#include <avr/io.h>

#include "modem_driver.h"

#include "config.h"
#include "lcd_driver.h"
#include "usart_func.h"
#include "tc_func.h"
#include "at24c_app.h"
#include "basic_func.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Modem_Init(struct PlantState *ps)
{
  // read
  AT24C_TeleNr_ReadToModem(ps);
}


/* ------------------------------------------------------------------*
 *            port
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
 *            modem check
 * ------------------------------------------------------------------*/

unsigned char Modem_Update(struct PlantState *ps)
{
  // Startup wait
  if(ps->modem->startup_delay < MO_STARTUP_DELAY)
  {
    ps->modem->startup_delay++;
    return 1;
  }

  // status LED
  Modem_ReadSLED(ps->page_state->page);

  // Check if off
  if(MO_PW_OFF)
  {
    ps->modem->turned_on = 0;
    Modem_TurnOn(ps->modem);
    ps->modem->startup_delay = 0;
  }

  // PWR on
  else if(!MO_PW_OFF)
  {
    ps->modem->turned_on = 1;
    ps->modem->turn_on_state = 0;
    ps->modem->turn_on_error = 0;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            modem turn on
 * ------------------------------------------------------------------*/

unsigned char Modem_TurnOn(struct Modem *mo)
{
  if(mo->turn_on_state == 0)
  {
    MO_PORT_ON;
    TCC0_WaitSec_Init(1);
    mo->turn_on_state = 1;
  }

  else if(mo->turn_on_state == 1 && TCC0_Wait_Query())
  {
    MO_PORT_OFF;
    if(MO_PW_OFF)
    {
      TCC0_WaitSec_Init(2);
      mo->turn_on_state = 2;
    }
    else
    {
      TCC0_Stop();
      mo->turn_on_state = 0;
    }
  }

  else if(mo->turn_on_state == 2 && TCC0_Wait_Query())
  {
    // timeout
    if(MO_PW_OFF)
    {
      mo->turn_on_error++;
    }
    else
    {
      TCC0_Stop();
    }
    TCC0_Stop();
    mo->turn_on_state = 0;
  }

  return 0;
}


/* ------------------------------------------------------------------*
 *            turn off
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
 *            modem ReadPWR-Monitor
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
 *            modem ReadSLED
 * ------------------------------------------------------------------*/

void Modem_ReadSLED(t_page page)
{
  if(page == DataMain)
  {
    if(!(PORTF.IN & PIN3_bm))
    {
      LCD_WriteAnyFont(f_4x6_p, 1, 152, 26);
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
      LCD_WriteAnyFont(f_4x6_p, 18, 137, 26);
    }
    else
    {
      LCD_ClrSpace(18, 137, 2, 4);
    }
  }

}


/* ------------------------------------------------------------------*
 *            modem ReadCTS
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
 *            modem call
 * ------------------------------------------------------------------*/

void Modem_Call(struct PlantState *ps, unsigned char id)
{
  struct TeleNr *tele_nr;

  // get correct pointer
  if(id == 1){ tele_nr = ps->modem->tele_nr1; }
  else if(id == 2){ tele_nr = ps->modem->tele_nr2; }
  else{ return; }

  // no number selected
  if(tele_nr->nr[0] == 11){ return; }

  // header
  USART_WriteString("AT+FCLASS=8");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_ms(100);
  USART_WriteString("ATD+");

  // write number
  for(unsigned char i = 0; ((tele_nr->nr[i] != 11) && (i < 16)); i++){ USART_WriteByte(tele_nr->nr[i] + 48); }

  // end sign
  USART_WriteByte(CHAR_CR);

  // wait and hang up
  LCD_WriteAnyFont(f_4x6_p, 16, 119, tele_nr->id);
  TCC0_wait_sec(1);
  unsigned char point_pos = 0;
  for(unsigned char i = 0; i < MO_HANG_UP_TIME; i++)
  {
    BASIC_WDT_RESET;

    // clear ... field
    if(point_pos > 5){ point_pos = 0; LCD_ClrSpace(16, 119, 2, 41); }

    // ...
    LCD_WriteAnyFont(f_4x6_p, 16, 123 + point_pos * 4, 22);
    point_pos++;
    TCC0_wait_sec(1);
  }

  // disconnect call
  USART_WriteString("ATH");
  USART_WriteByte(CHAR_CR);
  TCC0_wait_sec(1);
  LCD_ClrSpace(16, 119, 2, 41);
}


/* ------------------------------------------------------------------*
 *            modem SMS
 * ------------------------------------------------------------------*/

void Modem_SMS(struct PlantState *ps, unsigned char id, char msg[])
{
  struct TeleNr *tele_nr;

  // get correct pointer
  if(id == 1){ tele_nr = ps->modem->tele_nr1; }
  else if(id == 2){ tele_nr = ps->modem->tele_nr2; }
  else{ return; }

  // no number selected
  if(tele_nr->nr[0] == 11){ return; }

  // watchdog
  BASIC_WDT_RESET;

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

  // write number
  for(unsigned char i = 0; ((tele_nr->nr[i] != 11) && (i < 16)); i++){ USART_WriteByte(tele_nr->nr[i] + 48); }

  USART_WriteByte(0x22);
  USART_WriteByte(CHAR_CR);
  BASIC_WDT_RESET;
  TCC0_wait_ms(500);

  // message here
  USART_WriteString(msg);

  // end routine
  USART_WriteByte(CHAR_CR);
  USART_WriteByte(CHAR_CTR_Z);
  TCC0_wait_ms(500);
}


/* ------------------------------------------------------------------*
 *            modem Call / SMS / alert
 * ------------------------------------------------------------------*/

void Modem_Call_AllNumbers(struct PlantState *ps)
{
  Modem_Call(ps, 1);
  Modem_Call(ps, 2);
}


void Modem_SMS_AllNumbers(struct PlantState *ps, char msg[])
{
  Modem_SMS(ps, 1, msg);
  TCC0_wait_sec(5);
  Modem_SMS(ps, 2, msg);
}


void Modem_Alert(struct PlantState *ps, char msg[])
{
  //*** debug modem alert off
  if(DEB_MODEM){ return; }

  if(SMS_ON){ Modem_SMS_AllNumbers(ps, msg); }
  else{ Modem_Call_AllNumbers(ps); }
}


/* ------------------------------------------------------------------*
 *            modem send test
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
      BASIC_WDT_RESET;
      Modem_WriteSMS_Test("msg 30");
      TCC0_wait_sec(3);

      BASIC_WDT_RESET;
      Modem_WriteSMS_Test("msg 31");
      TCC0_wait_sec(3);

      BASIC_WDT_RESET;
      Modem_WriteSMS_Test("msg 32");
      TCC0_wait_sec(3);

      BASIC_WDT_RESET;
      Modem_WriteSMS_Test("msg 33");
      TCC0_wait_sec(3);

      call_done = 1;
  }
}


void Modem_Test(struct PlantState *ps)
{
  // some text
  LCD_WriteAnyStringFont(f_6x8_p, 1, 10, "Modem");
  LCD_WriteAnyStringFont(f_6x8_p, 4, 30,  "PWR");
  LCD_WriteAnyStringFont(f_6x8_p, 12, 30, "CTS");

  // timers
  TCC0_wait_sec(1);
  TCC0_WaitSec_Init(3);

  // test page
  ps->page_state->page = PinModem;

  // RTS off
  PORTF.OUTCLR = PIN4_bm;

  // wait variable
  int wait_var = 0;

  // loop
  while(1)
  {
    // watchdog
    BASIC_WDT_RESET;

    // check modem status -> turn on
    Modem_Update(ps);

    // read status
    Modem_ReadPWR();
    Modem_ReadCTS();

    // time instance
    if(TCC0_Wait_Query())
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
          LCD_WriteAnyStringFont(f_6x8_p, 15, 30, "sent");
          Modem_SendTest();
        }

        // read buffer
        else
        {
          unsigned char *p_rec = USART_Rx_Buffer_Read(ps->usart_state);
          unsigned char len = *p_rec;
          unsigned char i = 0;
          unsigned char j = 0;
          p_rec++;

          // print length
          LCD_WriteAnyValue(f_6x8_p, 2, 20, 5, len);

          // print message
          while(len)
          {
            LCD_WriteAnyValue(f_6x8_p, 3, i*2, 70+30*j, *p_rec);
            p_rec++;
            len--;
            i++;
            if (i > 11){ i = 0; j++; }
            if (j > 2){ j = 0; }
          }

          // clear buffer
          USART_Rx_Buffer_Clear(ps->usart_state);
        }
      }

      else
      {
        wait_var = 0;
      }

      // see if it still runs
      LCD_DeathMan(ps, 0, 0);
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
