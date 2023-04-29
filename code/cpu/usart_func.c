// --
// usart - serial interface

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart_func.h"
#include "modem_driver.h"


/* ------------------------------------------------------------------*
 *            Interrupts
 * ------------------------------------------------------------------*/

ISR(USARTF1_RXC_vect)
{
  // read data from internal register
  unsigned char data = USARTF1.DATA;

  // add byte to buffer
  USART_Rx_Buffer_AddByte(&global_usart_state, data);
}


/* ------------------------------------------------------------------*
 *            USART init
 * ------------------------------------------------------------------*
 *  Description:
 *    DetTime-Modem -> 9,22 ms
 *    115200 8N1  -> 107chars
 *   57600 8N1   ->  54chars
 *   9600 8N1    ->   9chars
 *    8N1 ->  8DataBits, noParityBit, 1StopBit
 *    9600bps   Bscale=3    Bsel=12
 *    57600bps  Bscale=-3   Bsel=131
 *    115200bps Bscale=-4   Bsel=123
 * ------------------------------------------------------------------*/

void USART_Init(void)
{
  // Port infos
  // RX - PF6
  // TX - PF7

  // global interrupts dissable
  cli();

  // ports init
  Modem_Port_Init();

  // Interrupts, MediumLevel, LowLevel Interrupts
  //PMIC.CTRL = PMIC_LOLVLEN_bm;
  PMIC.CTRL = PMIC_MEDLVLEN_bm;

  // interrupt enable
  USARTF1.CTRLA = USART_RXCINTLVL_MED_gc;

  // transmission enable
  USARTF1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

  // modes: asynch, noparity, 2stop bits, 8bit
  USARTF1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc |   //AsynchronousMode
                  USART_PMODE_DISABLED_gc     |   //NoParity
                  //USART_SBMODE_bm     |         //2StopBits
                  USART_CHSIZE_8BIT_gc;

  //*------------------------------------------*
  USARTF1.BAUDCTRLA = 123;                //115200 - Bsel=123
  USARTF1.BAUDCTRLB = ((1<<7) | (4<<4));  //115200 - Bscale=-4
  //------------------------------------------*/
  /*------------------------------------------*
  USARTF1.BAUDCTRLA = 131;                //57600 - Bsel=131
  USARTF1.BAUDCTRLB = ((1<<7) | (3<<4));  //57600 - Bscale=-3
  //------------------------------------------*/
  /*------------------------------------------/
  USARTF1.BAUDCTRLA = 12;                 //9600 - Bsel=12
  USARTF1.BAUDCTRLB = (3<<4);             //9600 - Bscale=3
  //------------------------------------------*/

  // global interrupts enable
  sei();
}


/* ------------------------------------------------------------------*
 *            USART read byte
 * ------------------------------------------------------------------*/

int USART_ReadByte(void)
{
  // check status
  if(USARTF1.STATUS & USART_RXCIF_bm)
  {
    // read data
    unsigned char read= USARTF1.DATA;

    // reset flag
    USARTF1.STATUS |= USART_RXCIF_bm;

    // return message
    return (0x0100 | read);
  }

  // no message
  return 0x0000;
}


/* ------------------------------------------------------------------*
 *            write byte
 * ------------------------------------------------------------------*/

void USART_WriteByte(char write)
{
  while(!(USARTF1.STATUS & USART_DREIF_bm));
  USARTF1_DATA = write;
  while(!(USARTF1.STATUS & USART_TXCIF_bm));
  USARTF1.STATUS |= USART_TXCIF_bm;
}


/* ------------------------------------------------------------------*
 *            write stirng
 * ------------------------------------------------------------------*/

void USART_WriteString(char write[])
{
  unsigned char i = 0;

  while(write[i] != 0)
  {
    USART_WriteByte(write[i]);
    i++;
  }
}


/* ------------------------------------------------------------------*
 *            USART RX buffer
 * ------------------------------------------------------------------*/

void USART_Rx_Buffer_AddByte(struct USARTState *usart_state, unsigned char data_byte)
{
  if(usart_state->pos >= 49){ return; }
  usart_state->rx_buffer[usart_state->pos+1] = data_byte;
  usart_state->pos++;
}


/* ------------------------------------------------------------------*
 *            USART RX buffer
 * ------------------------------------------------------------------*/

void USART_Rx_Buffer_Clear(struct USARTState *usart_state)
{
  for(usart_state->pos = 0; usart_state->pos < 50; usart_state->pos++){ usart_state->rx_buffer[usart_state->pos] = 0x00; }
  usart_state->pos = 0;
}


/* ------------------------------------------------------------------*
 *            USART RX Buffer
 * ------------------------------------------------------------------*/

unsigned char *USART_Rx_Buffer_Read(struct USARTState *usart_state)
{
  // amount of entries
  usart_state->rx_buffer[0] = usart_state->pos;

  // return data
  return &usart_state->rx_buffer[0];
}