// --
// spi

#include "spi_func.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void SPI_Init(void)
{
  // output: CS, MOSI, SCK
  PORTE.DIR =   PIN4_bm | PIN5_bm | PIN7_bm;
  PORTE.DIRCLR = PIN6_bm;
  PORTE.OUTCLR =  PIN5_bm | PIN6_bm | PIN7_bm;

  // SPI setup
  SPIE.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc | SPI_PRESCALER_DIV64_gc;

  // interrupt off
  SPIE.INTCTRL = SPI_INTLVL_OFF_gc;

  // no cs
  CS_SET;
}


/* ------------------------------------------------------------------*
 *            write byte
 * ------------------------------------------------------------------*/

unsigned char SPI_WriteByte(unsigned char wByte)
{
  unsigned char dummy = 0;

  // start transmission
  SPIE.DATA = wByte;
  while(!(SPIE.STATUS & SPI_IF_bm));

  // read dummy: reset SPIE
  dummy = SPIE.DATA;

  // to disregard compiler warning                     
  dummy = dummy;
  return 0;
}


/* ------------------------------------------------------------------*
 *            write string
 * ------------------------------------------------------------------*/

unsigned char SPI_WriteString(unsigned char *sendData, unsigned char i)
{
  unsigned char sendPuffer = 0;

  while(i)
  {
    // update send puffer
    sendPuffer = *sendData;

    // send via SPI
    SPI_WriteByte(sendPuffer);
    sendData++;
    i--;
  }

  return F_SPI_SENT;
}


/* ------------------------------------------------------------------*
 *            read byte
 * ------------------------------------------------------------------*/

unsigned char SPI_ReadByte(void)
{
  // write dummy byte
  SPI_WriteByte(0xFF);
  return SPIE.DATA;;
}