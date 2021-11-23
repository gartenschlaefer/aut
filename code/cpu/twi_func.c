// --
// twi - two wire interface

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"

#include "tc_func.h"
#include "twi_func.h"


/* ==================================================================*
 *                TWIC
 * ==================================================================*/
/* ==================================================================*
 *        TWI1 only for Display EA-DOGXL160-7
 * ==================================================================*/

/* ==================================================================*
 *            FUNCTIONS Init + Error
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Init
 * ------------------------------------------------------------------*/

void TWI_Master_Init(void)
{
  TWIC.MASTER.CTRLA &=  ~TWI_MASTER_ENABLE_bm;        //Master Disable
  PORTC.DIRSET =      PIN0_bm | PIN1_bm;          //SDA, SCL Output
  PORTC.OUTCLR =      PIN0_bm | PIN1_bm;          //SDA, SCL GND

  TCC1_WaitMilliSec_Init(10);
  while(TCC1_Wait_Query());               //wait

  TWIC.MASTER.CTRLB=    TWI_MASTER_TIMEOUT_DISABLED_gc;   //TimeoutDisabled

  //-----------------------------------------------------Boud=(fsys/2*ftwi)-5--
  TWIC.MASTER.BAUD = 35;                //200kHz/16MHz
  //TWIC.MASTER.BAUD = 21;              //300kHz/16MHz
  //TWIC.MASTER.BAUD = 15;              //400kHz/16MHz

  TWIC.MASTER.CTRLA=  TWI_MASTER_ENABLE_bm;     //enable Master
  TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;  //set Idle
  TWIC.MASTER.CTRLC=  TWI_MASTER_CMD_STOP_gc;     //stopCondition
}




/* ------------------------------------------------------------------*
 *            Error
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_Error(void)
{
  if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_ARBLOST_bp)) //-Arbitration
  {
    TWIC.MASTER.CTRLC=  TWI_MASTER_ACKACT_bm |    //NACK
              TWI_MASTER_CMD_STOP_gc;   //stopCondition

    TWI_Master_Reset();               //TWI Reset
    return E_TWI_ARBLOST;
  }

  if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_BUSERR_bp))  //-Bus-Error
  {
    TWIC.MASTER.CTRLC=  TWI_MASTER_ACKACT_bm |    //NACK
              TWI_MASTER_CMD_STOP_gc;   //stopCondition

    TWI_Master_Reset();               //TWI Reset
    return E_TWI_BUSERR;
  }

  if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_RXACK_bp)) //-Nack
  {
    TWIC.MASTER.CTRLC=  TWI_MASTER_ACKACT_bm |    //NACK
              TWI_MASTER_CMD_STOP_gc;   //stopCondition

    TWI_Master_Reset();               //TWI Reset
    return E_TWI_NACK;
  }
  return 0;                     //No Errors
}



/* ==================================================================*
 *            FUNCTIONS Write Read
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Write String
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_WriteString(   unsigned char   address,
                    unsigned char *sendData,
                    unsigned char   i )
{
  unsigned char error=0;
  unsigned char send=0;

  TCC1_WaitMilliSec_Init(300);    //maxDurationOfTransmission

  if( (TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_IDLE_gc))
  {
    //--------------------------------------------------Send-Address----------
    error= TWI_Master_AddressWriteMode(address);  //Send Address
    if(error) return error;         //return Error if occured
    //------------------------------------------------------------------------
    //ACK Received
    //--------------------------------------------------Beginn-Transaction-----
    while(i)
    {
      send= *sendData;            //update send Puffer
      error= TWI_Master_Send(send); //send Databyte
      if(error) return error;     //return Error if occured
      sendData++;                 //increase Pointer
      i--;                    //decrease Counter
    }
    //------------------------------------------------Send Stop Condition-------
    TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;     //stopCondition
    TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;  //set Idle
    return    F_TWI_DATA_SENT;            //DataSent-Flag
  }

  TWI_Master_Reset();                 //TWI Reset
  TCC1_Stop();                  //Stop Timer
  return E_TWI_WAIT;                  //ReturnWait
}



/* ------------------------------------------------------------------*
 *            ReadString
 * ------------------------------------------------------------------*/

unsigned char *TWI_Master_ReadString(unsigned char address, unsigned char i)
{
  static unsigned char  receiveData[10]= {0x00,0xFF};   //max length[], init
  unsigned char       error1= 0;
  unsigned char       *p_data;

  TCC1_WaitMilliSec_Init(300);    //maximale Dauer der Ã?bertragung

  if( (TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_IDLE_gc) ||
  ( (TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_OWNER_gc)))
  {
    //--------------------------------------------------------Send-Address----------
    TWIC.MASTER.ADDR= (C_TWI_ADDRESS(address) | C_TWI_READ);
    //--------------------------------------------------------ACK Received

    //--------------------------------------------------------Read-Data-------------
    while(i)
    {
      while(!(TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm))  //wait
      {
        //Timer
        if(TCC1_Wait_Query())             //Timer
        {
          TWIC.MASTER.CTRLC=  TWI_MASTER_ACKACT_bm |    //NACK
                    TWI_MASTER_CMD_STOP_gc;   //stopCondition

          receiveData[0]= E_TWI_NO_SENT;    //Write ErrorCode
          p_data= &receiveData[0];      //update Pointer
          return p_data;            //Abbruch
        }
      }
      //.........................................................ERROR
      error1= TWI_Master_Error();     //Error Detection
      if(error1)
      {
        receiveData[0]= error1;   //Write ErrorCode
        p_data= &receiveData[0];      //update Pointer
        return p_data;            //return Error if occured
      }
      //..............................................................

      receiveData[i]= TWIC.MASTER.DATA; //Receive Byte

      //ACK or NACK?
      if(i==1) {
      //----------------------------------------SendNACK------------
      TWIC.MASTER.CTRLC=  TWI_MASTER_ACKACT_bm |  //NACK
                TWI_MASTER_CMD_STOP_gc; //stopCondition
      //-----------------------------------------------------------
      }
      else {
      //----------------------------------------SendACK------------
      TWIC.MASTER.CTRLC=  (TWIC.MASTER.CTRLC & ~(TWI_MASTER_ACKACT_bm)) |
                TWI_MASTER_CMD_RECVTRANS_gc;
      //-----------------------------------------------------------
      }
      i--;              //decrease Counter
    }
    //--------------------------------------------------------------
    TCC1_Stop();        //Stop Timer

    receiveData[0]= 0;        //NO Errors occured
    p_data= &receiveData[0];    //update Pointer
    return p_data;          //return Pointer
  }

  if((TWIC.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == (TWI_MASTER_BUSSTATE_UNKNOWN_gc))
  {
    TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;  //set Idle
  }

  TCC1_Stop();        //Stop Timer

  //Wait
  receiveData[0]= E_TWI_WAIT;   //Write ErrorCode
  p_data= &receiveData[0];    //update Pointer
  return p_data;          //return Error if occured
}






/* ==================================================================*
 *            Build-In Functions
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Master Send Data
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_Send(unsigned char send)
{
  unsigned char error=0;

  TWIC.MASTER.DATA= send;                 //Send Data
  while(!(TWIC.MASTER.STATUS & (1<<TWI_MASTER_WIF_bp))) //wait
  {
    //Timer
    if(TCC1_Wait_Query())             //Timer
    {
      TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;     //send Stop
      TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;  //set Idle
      TWI_Master_Init();                  //Master Init
      LCD_Init();                                 //Init LCD
      TCC1_Stop();                  //Stop Timer

      return E_TWI_NO_SENT;             //Abbruch
    }
  }
  //........................................ERROR.................
  error= TWI_Master_Error();    //Error Detection
  if(error) return error;   //return Error
  //..............................................................
  return 0;
}


/* ------------------------------------------------------------------*
 *            Master Send Adress
 * ------------------------------------------------------------------*/

unsigned char TWI_Master_AddressWriteMode(unsigned char f_address)
{
  unsigned char error=0;

  TWIC.MASTER.ADDR= (C_TWI_ADDRESS(f_address) | C_TWI_WRITE); //writeAddress
  while(!(TWIC.MASTER.STATUS & (1<<TWI_MASTER_WIF_bp)))   //wait
  {
    //Timer
    if(TCC1_Wait_Query())               //Timer
    {
      TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;     //send Stop
      TWI_Master_Reset();                 //TWI Reset
      TCC1_Stop();                  //Stop Timer

      return E_TWI_NO_SENT;               //Abbruch
    }
  }
  //........................................ERROR.................
  error = TWI_Master_Error();   //Error Detection
  if(error) return error;   //return Error
  //..............................................................
  return 0;
}


/* ------------------------------------------------------------------*
 *            TWI Reset
 * ------------------------------------------------------------------*/

void TWI_Master_Reset(void)
{
  TWIC.MASTER.STATUS= TWI_MASTER_BUSSTATE_IDLE_gc;  //set Idle
  TWI_Master_Init();                  //Master Init
}



/* ==================================================================*
 *                TWID
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            TWI2 - Master Init
 * ------------------------------------------------------------------*/

void TWI2_Master_Init(void)
{
  TWID.MASTER.CTRLA &= ~TWI_MASTER_ENABLE_bm; //Master Disable
  PORTD.DIRSET = PIN0_bm | PIN1_bm;           //SDA, SCL Output
  PORTD.OUTCLR = PIN0_bm | PIN1_bm;           //SDA, SCL GND

  TCC1_WaitMilliSec_Init(10);
  while(TCC1_Wait_Query());               //wait

  TWID.MASTER.CTRLB = TWI_MASTER_TIMEOUT_DISABLED_gc; //TimeoutDisabled

  //---------------------------------------------Boud=(fsys/2*ftwi)-5)
  //TWID.MASTER.BAUD = 155;             //50kHz/16MHz
  //TWID.MASTER.BAUD = 75;              //100kHz/16MHz
  TWID.MASTER.BAUD = 35;                //200kHz/16MHz

  TWID.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;         //enable Master
  TWID.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc; //set Idle
  TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;       //stopCondition
  TCC1_Stop();          //Stop Timer
}


/* ------------------------------------------------------------------*
 *            Error
 * ------------------------------------------------------------------*/

unsigned char TWI2_Master_Error(void)
{
  if(TWID.MASTER.STATUS & (1 << TWI_MASTER_ARBLOST_bp)) //Arbitration
  {
    TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI2_Master_Init();
    return E_TWI_ARBLOST;
  }

  else if(TWID.MASTER.STATUS & (1 << TWI_MASTER_BUSERR_bp)) //Bus
  {
    TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI2_Master_Init();               //TWI Reset
    return E_TWI_BUSERR;
  }

  else if(TWID.MASTER.STATUS & (1 << TWI_MASTER_RXACK_bp))  //Nack
  {
    TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    TWI2_Master_Init();               //TWI Reset
    return E_TWI_NACK;
  }
  return 0;   //Keine Errors
}


/* ------------------------------------------------------------------*
 *            Write String
 * ------------------------------------------------------------------*/

unsigned char TWI2_Master_WriteString(unsigned char address,
unsigned char *sendData,
unsigned char i)
{
  unsigned char error = 0;
  unsigned char send = 0;

  TCC1_WaitMilliSec_Init(10);
  //------------------------------------------------waitTillIdle
  while((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) !=
  (TWI_MASTER_BUSSTATE_IDLE_gc))
  {
    if(TCC1_Wait_Query()){
      TWI2_Master_Init();                         //TWI Reset
      TCC1_WaitMilliSec_Init(10);
      error++;}
    if(error > 3) return E_TWI_NO_SENT;
  }
  //------------------------------------------------Send-Address
  error = TWI2_Master_AddressWriteMode(address);  //SendAdr
  if(error) return error;                         //Error
  //------------------------------------------------Transaction
  while(i)
  {
    send = *sendData;                 //updatePuffer
    error = TWI2_Master_Send(send);   //sendDatabyte
    if(error) return error;           //Error
    sendData++;
    i--;
  }
  //------------------------------------------------StopCondition
  TWID.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;       //stopCondition
  TWID.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc; //setIdle
  return F_TWI_DATA_SENT;                           //DataSent Flag

  TWI2_Master_Init();   //TWI Reset
  return E_TWI_WAIT;
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
    if(TCC1_Wait_Query()){
      TWI2_Master_Init();                         //TWI Reset
      return E_TWI_NO_SENT;}
  }
  error = TWI2_Master_Error();    //Error Detection
  if(error) return error;   //return Error
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
  while(!(TWID.MASTER.STATUS & (1 << TWI_MASTER_WIF_bp))) //wait
  {
    if(TCC1_Wait_Query()){
      TWI2_Master_Init();                 //Master Init
      return E_TWI_NO_SENT;}
  }
  error = TWI2_Master_Error();    //Error Detection
  if(error) return error;   //return Error
  return 0;
}


/* ------------------------------------------------------------------*
 *            Read String
 * ------------------------------------------------------------------*/

unsigned char *TWI2_Master_ReadString(unsigned char address,
unsigned char i)
{
  static unsigned char receiveData[10];   //maxlength[]
  unsigned char error1= 0;
  unsigned char *p_data;
  unsigned char a = 1;

  TCC1_WaitMilliSec_Init(300);    //maxDuration

  if((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) ==
  (TWI_MASTER_BUSSTATE_IDLE_gc) ||
  ((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) ==
  (TWI_MASTER_BUSSTATE_OWNER_gc)))
  {
    TWID.MASTER.ADDR= (C_TWI_ADDRESS(address) | C_TWI_READ);  //Adr
    //------------------------------------------------Read-Data
    while(i)
    {
      while(!(TWID.MASTER.STATUS & TWI_MASTER_RIF_bm))
      {
        if(TCC1_Wait_Query())
        {
          TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm |    //NACK
            TWI_MASTER_CMD_STOP_gc;         //stopCondition

          receiveData[0] = E_TWI_NO_SENT;   //Write ErrorCode
          p_data = &receiveData[0];         //update Pointer
          return p_data;
        }
      }
      //----------------------------------------------ERROR
      error1 = TWI2_Master_Error();   //Error Detection
      if(error1){
        receiveData[0] = error1;      //Write ErrorCode
        p_data = &receiveData[0];     //update Pointer
        return p_data;}
      //----------------------------------------------ReceiveByte
      receiveData[a]= TWID.MASTER.DATA;
      //----------------------------------------------ACK
      if(i == 1) TWID.MASTER.CTRLC = TWI_MASTER_ACKACT_bm |    //Nack
        TWI_MASTER_CMD_STOP_gc;

      else TWID.MASTER.CTRLC = (TWID.MASTER.CTRLC &  //Ack
        ~(TWI_MASTER_ACKACT_bm)) | TWI_MASTER_CMD_RECVTRANS_gc;
      a++;
      i--;
    }
    TCC1_Stop();
    receiveData[0] = 0;         //NO Errors occured
    p_data = &receiveData[0];   //update Pointer
    return p_data;              //return Pointer
  }

  if((TWID.MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) ==
  (TWI_MASTER_BUSSTATE_UNKNOWN_gc))
    TWID.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc; //set Idle

  TCC1_Stop();
  receiveData[0] = E_TWI_WAIT;  //Write ErrorCode
  p_data = &receiveData[0];     //update Pointer
  return p_data;
}