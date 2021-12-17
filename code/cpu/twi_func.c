// --
// twi - two wire interface

#include <avr/io.h>

#include "defines.h"
#include "lcd_driver.h"

#include "tc_func.h"
#include "twi_func.h"


/* ==================================================================*
 *                TWIC: TWI1 only for Display EA-DOGXL160-7
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Init
 * ------------------------------------------------------------------*/

void TWI_Master_Init(void)
{
  // master disable
  TWIC.MASTER.CTRLA &= ~TWI_MASTER_ENABLE_bm;

  // SDA, SCL output -> gnd
  PORTC.DIRSET = PIN0_bm | PIN1_bm;
  PORTC.OUTCLR = PIN0_bm | PIN1_bm;

  TCC1_WaitMilliSec_Init(10);
  while(TCC1_Wait_Query());

  TWIC.MASTER.CTRLB = TWI_MASTER_TIMEOUT_DISABLED_gc;

  //-----------------------------------------------------Boud = (fsys / 2 * ftwi) - 5--
  TWIC.MASTER.BAUD = 35;                //200kHz/16MHz
  //TWIC.MASTER.BAUD = 21;              //300kHz/16MHz
  //TWIC.MASTER.BAUD = 15;              //400kHz/16MHz

  TWIC.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
  TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
  TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
}




/* ------------------------------------------------------------------*
 *            Error
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_Error(void)
{
  // arbitration
  if(TWIC.MASTER.STATUS & (1 << TWI_MASTER_ARBLOST_bp))
  {
    TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI_Master_Reset();
    return E_TWI_ARBLOST;
  }

  // bus error
  if(TWIC.MASTER.STATUS & (1 << TWI_MASTER_BUSERR_bp))
  {
    TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI_Master_Reset();
    return E_TWI_BUSERR;
  }

  // nack
  if(TWIC.MASTER.STATUS & (1 << TWI_MASTER_RXACK_bp))
  {
    TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI_Master_Reset();
    return E_TWI_NACK;
  }

  // no errors
  return 0;
}



/* ==================================================================*
 *            FUNCTIONS Write Read
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Write String
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_WriteString(unsigned char address, unsigned char *sendData, unsigned char i)
{
  unsigned char error = 0;
  unsigned char send = 0;

  // max duration of transmission
  TCC1_WaitMilliSec_Init(300);

  if( (TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_IDLE_gc )
  {
    // send address
    error = TWI_Master_AddressWriteMode(address);
    if(error) return error;
    //------------------------------------------------------------------------
    //ACK Received
    //--------------------------------------------------start-Transaction-----
    while(i)
    {
      send = *sendData;
      error = TWI_Master_Send(send);
      if(error) return error;
      sendData++;
      i--;
    }
    //------------------------------------------------Send Stop Condition-------
    TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
    TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
    return F_TWI_DATA_SENT;
  }

  TWI_Master_Reset();
  TCC1_Stop();
  return E_TWI_WAIT;
}


/* ------------------------------------------------------------------*
 *            ReadString
 * ------------------------------------------------------------------*/

unsigned char *TWI_Master_ReadString(unsigned char address, unsigned char i)
{
  static unsigned char receiveData[10] = {0x00,0xFF};
  unsigned char error1 = 0;
  unsigned char *p_data;

  // max duration
  TCC1_WaitMilliSec_Init(300);

  if( ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_IDLE_gc) || ((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_OWNER_gc) )
  {
    // send address
    TWIC.MASTER.ADDR = (C_TWI_ADDRESS(address) | C_TWI_READ);
    //--------------------------------------------------------ACK Received

    // read data
    while(i)
    {
      while(!(TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm))
      {
        // timer
        if(TCC1_Wait_Query())
        {
          TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
          receiveData[0] = E_TWI_NO_SENT;
          p_data = &receiveData[0];
          return p_data;
        }
      }

      // error
      error1 = TWI_Master_Error();
      if(error1)
      {
        receiveData[0] = error1;
        p_data = &receiveData[0];
        return p_data;
      }

      // receive byte
      receiveData[i] = TWIC.MASTER.DATA;

      // ACK or NACK?
      if(i == 1) 
      {
        //----------------------------------------SendNACK------------
        TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
        //-----------------------------------------------------------
      }
      else 
      {
        //----------------------------------------SendACK------------
        TWIC.MASTER.CTRLC =  (TWIC.MASTER.CTRLC & ~(TWI_MASTER_ACKACT_bm)) | TWI_MASTER_CMD_RECVTRANS_gc;
        //-----------------------------------------------------------
      }
      i--;
    }
    TCC1_Stop();
    receiveData[0] = 0;
    p_data = &receiveData[0];
    return p_data;
  }

  // set idle
  if((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_UNKNOWN_gc)) TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
  TCC1_Stop();
  receiveData[0] = E_TWI_WAIT;
  p_data = &receiveData[0];
  return p_data;
}



/* ==================================================================*
 *            Build-In Functions
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Master Send Data
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_Send(unsigned char send)
{
  unsigned char error = 0;

  TWIC.MASTER.DATA = send;
  while(!(TWIC.MASTER.STATUS & (1 << TWI_MASTER_WIF_bp)))
  {
    // timer
    if(TCC1_Wait_Query())
    {
      TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
      TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
      TWI_Master_Init();
      LCD_Init();
      TCC1_Stop();
      return E_TWI_NO_SENT;
    }
  }

  // error
  error = TWI_Master_Error();
  if(error) return error;

  return 0;
}


/* ------------------------------------------------------------------*
 *            Master Send Address
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_AddressWriteMode(unsigned char f_address)
{
  unsigned char error = 0;

  // write address
  TWIC.MASTER.ADDR = (C_TWI_ADDRESS(f_address) | C_TWI_WRITE);
  while(!(TWIC.MASTER.STATUS & (1 << TWI_MASTER_WIF_bp)))
  {
    // timer
    if(TCC1_Wait_Query())
    {
      TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
      TWI_Master_Reset();
      TCC1_Stop();

      // stop
      return E_TWI_NO_SENT;
    }
  }

  // error
  error = TWI_Master_Error();
  if(error) return error;

  return 0;
}


/* ------------------------------------------------------------------*
 *            TWI Reset
 * ------------------------------------------------------------------*/

void TWI_Master_Reset(void)
{
  TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
  TWI_Master_Init();
}



/* ==================================================================*
 *                TWID
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TWI2 - Master Init
 * ------------------------------------------------------------------*/

void TWI2_Master_Init(void)
{
  TWID.MASTER.CTRLA &= ~TWI_MASTER_ENABLE_bm;
  PORTD.DIRSET = PIN0_bm | PIN1_bm;
  PORTD.OUTCLR = PIN0_bm | PIN1_bm;

  TCC1_WaitMilliSec_Init(10);
  while(TCC1_Wait_Query());

  TWID.MASTER.CTRLB = TWI_MASTER_TIMEOUT_DISABLED_gc;

  //---------------------------------------------Boud = (fsys / 2 * ftwi) - 5)
  //TWID.MASTER.BAUD = 155;             //50kHz/16MHz
  //TWID.MASTER.BAUD = 75;              //100kHz/16MHz
  TWID.MASTER.BAUD = 35;                //200kHz/16MHz

  TWID.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
  TWID.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
  TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
  TCC1_Stop();
}


/* ------------------------------------------------------------------*
 *            Error
 * ------------------------------------------------------------------*/

unsigned char TWI2_Master_Error(void)
{
  // arbitration
  if(TWID.MASTER.STATUS & (1 << TWI_MASTER_ARBLOST_bp))
  {
    TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI2_Master_Init();
    return E_TWI_ARBLOST;
  }

  // bus
  else if(TWID.MASTER.STATUS & (1 << TWI_MASTER_BUSERR_bp))
  {
    TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI2_Master_Init();
    return E_TWI_BUSERR;
  }

  // nack
  else if(TWID.MASTER.STATUS & (1 << TWI_MASTER_RXACK_bp))
  {
    TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI2_Master_Init();
    return E_TWI_NACK;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            Write String
 * ------------------------------------------------------------------*/

unsigned char TWI2_Master_WriteString(unsigned char address, unsigned char *sendData, unsigned char i)
{
  unsigned char error = 0;
  unsigned char send = 0;

  TCC1_WaitMilliSec_Init(10);

  // wait for idle
  while((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) != TWI_MASTER_BUSSTATE_IDLE_gc)
  {
    if(TCC1_Wait_Query()){ TWI2_Master_Init(); TCC1_WaitMilliSec_Init(10); error++; }
    if(error > 3) return E_TWI_NO_SENT;
  }

  // send address
  error = TWI2_Master_AddressWriteMode(address);
  if(error) return error;

  // transaction
  while(i)
  {
    send = *sendData;
    error = TWI2_Master_Send(send);
    if(error) return error;
    sendData++;
    i--;
  }

  // stop condition
  TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
  TWID.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
  return F_TWI_DATA_SENT;
}


/* ------------------------------------------------------------------*
 *            Master Send Address
 * ------------------------------------------------------------------*/

unsigned char TWI2_Master_AddressWriteMode(unsigned char f_address)
{
  unsigned char error = 0;

  TCC1_WaitMilliSec_Init(10);
  TWID.MASTER.ADDR = (C_TWI_ADDRESS(f_address) | C_TWI_WRITE);
  while(!(TWID.MASTER.STATUS & (1 << TWI_MASTER_WIF_bp)))
  {
    if(TCC1_Wait_Query()){ TWI2_Master_Init(); return E_TWI_NO_SENT; }
  }
  error = TWI2_Master_Error();
  if(error) return error;
  return 0;
}


/* ------------------------------------------------------------------*
 *            Master Send Data
 * ------------------------------------------------------------------*/

unsigned char TWI2_Master_Send(unsigned char send)
{
  unsigned char error = 0;

  TCC1_WaitMilliSec_Init(10);
  TWID.MASTER.DATA = send;
  while(!(TWID.MASTER.STATUS & (1 << TWI_MASTER_WIF_bp)))
  {
    if(TCC1_Wait_Query()){ TWI2_Master_Init(); return E_TWI_NO_SENT; }
  }
  error = TWI2_Master_Error();
  if(error) return error;
  return 0;
}


/* ------------------------------------------------------------------*
 *            Read String
 * ------------------------------------------------------------------*/

unsigned char *TWI2_Master_ReadString(unsigned char address, unsigned char i)
{
  static unsigned char receiveData[10];
  unsigned char error1 = 0;
  unsigned char *p_data;
  unsigned char a = 1;

  TCC1_WaitMilliSec_Init(300);

  if(((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_IDLE_gc) || ((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_OWNER_gc))
  {
    // address
    TWID.MASTER.ADDR = (C_TWI_ADDRESS(address) | C_TWI_READ);

    // read data
    while(i)
    {
      while(!(TWID.MASTER.STATUS & TWI_MASTER_RIF_bm))
      {
        if(TCC1_Wait_Query())
        {
          // nack, stop condition
          TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;

          // error code
          receiveData[0] = E_TWI_NO_SENT;
          p_data = &receiveData[0];
          return p_data;
        }
      }

      // error detection
      error1 = TWI2_Master_Error();
      if(error1)
      {
        receiveData[0] = error1;
        p_data = &receiveData[0];
        return p_data;
      }

      // receive byte
      receiveData[a] = TWID.MASTER.DATA;

      // ack, stop
      if(i == 1) TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
      else TWID.MASTER.CTRLC = (TWID.MASTER.CTRLC & ~(TWI_MASTER_ACKACT_bm)) | TWI_MASTER_CMD_RECVTRANS_gc;

      // update indices
      a++;
      i--;
    }
    TCC1_Stop();

    // no errors
    receiveData[0] = 0;
    p_data = &receiveData[0];
    return p_data;
  }

  // set idle
  if((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_UNKNOWN_gc)) TWID.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;

  TCC1_Stop();
  receiveData[0] = E_TWI_WAIT;
  p_data = &receiveData[0];
  return p_data;
}