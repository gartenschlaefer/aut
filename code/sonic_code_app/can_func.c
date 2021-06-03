/*********************************************************************\
* Author:       Christian Walter
* ------------------------------------------------------------------
* Project:      UltraSonic
* Name:         can_func.c
* ------------------------------------------------------------------
* µ-Controler:  AT90CAN128/32
* Compiler:     avr-gcc (WINAVR 2010)
* Description:
* ------------------------------------------------------------------
* CAN Functions Source-File
* ------------------------------------------------------------------
* Date:         10.10.2011
* lastChanges:  10.04.2015
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "can_func.h"
#include "can_app.h"
#include "tc_func.h"



/* ==================================================================*
 *            FUNCTIONS - Init
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  CAN_Init
 * --------------------------------------------------------------
 *  -PORTSetting
 *  -set Clock to 200kbps
 *  -ClearMOb, Enable
 * ------------------------------------------------------------------*/

void CAN_Init(void)
{
  //-------------------------------------------------PORTSetting
  DDRD |= (1 << PD5);       //TxCAN Output
  DDRD &= ~(1 << PD6);      //RxCAN Input
  //--------------------------------------------------RESET
  CANGCON = (1 << SWRES);   //Reset CAN
  CANGIT =  0xFF;           //Reset INT-Flags
  CANGIE =  0x00;           //Disable Interrupts
  CANIE1 =  0x00;           //Disable MOb Interrupt1
  CANIE2 =  0x00;           //Disable MOb Interrupt2
  //--------------------------------------------------MOb
  CAN_ClearMOb();
  //--------------------------------------------------Clk16Mhz-100kbps
  CANBT1 =  0x12;       //16Mhz/(brp+1) ->  16MHz/10
  CANBT2 =  0x0C;       //PRSEG=6+1  SJW=+1
  CANBT3 =  0x37;       //PHSEG1=3+1, PHSEG2=3+1
  CANTCON = 0x00;       //NoPrescaler
  //--------------------------------------------------Enable
  CANGCON = (1 << ENASTB);            //Enable CAN
  while(!(CANGSTA & (1 << ENFG)));    //Wait until enabled

  CAN_RXMOb_Init();
  CAN_USSREG(_init, 0);
  CAN_USDDREG(_init, 0);
  CAN_USDTREG(_init, 0);
}


/* ------------------------------------------------------------------*
 *            RXMOb Init
 * ------------------------------------------------------------------*/

void CAN_RXMOb_Init(void)
{
  CANPAGE =   (1 << MOBNB1);  //Chose Page
  CANCDMOB |= (1 << DLC0);    //Datalenght

  CANIDT4 = 0x00;             //No RTRTAG and RB0TAG
  CANIDT3 = 0x00;
  CANIDT2 = 0x20;             //Write Address
  CANIDT1 = 0x00;             //Address

  CANIDM4 = 0x00;             //ID-Mask
  CANIDM3 = 0x00;
  CANIDM2 = 0x00;
  CANIDM1 = 0x00;

  CANCDMOB |= (1 << CONMOB1); //Start RX-Communication
}


/* ------------------------------------------------------------------*
 *            Clear MOb
 * ------------------------------------------------------------------*/

void CAN_ClearMOb(void)
{
  // 14 mob pages
  for(unsigned char page = 0; page < 14; page++)
  {
    // choose page
    CANPAGE= ((page << 4) & 0xF0);

    // read modify write
    unsigned char rmw = CANSTMOB;
    rmw = 0x00;
    CANSTMOB = rmw;

    // reset control and dlc register
    CANCDMOB = 0x00;

    // reset can message
    for(unsigned char i = 0; i < 8; i++)
    {
      CANMSG = 0x00;
    }
  }
}



/* ==================================================================*
 *            Functions - TX-RX-Query
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            CAN Transmit Query
 * ------------------------------------------------------------------*/

unsigned char CAN_TXOK(void)
{
  unsigned char rmw= 0;

  CANPAGE =     (1 << MOBNB0);  //Chose TXPage

  if(CANSTMOB & (1<<TXOK))
  {
    rmw = CANSTMOB;             //Read
    rmw &= ~(1 << TXOK) ;       //Modify
    CANSTMOB = rmw;             //Write
    return 1;
  }

  return 0;
}


/* ------------------------------------------------------------------*
 *            CAN Receive Query
 * ------------------------------------------------------------------*/

unsigned char CAN_RXOK(void)
{
  unsigned char rmw = 0;

  CANCDMOB |= (1 << CONMOB1); //StartReception

  if(CANSTMOB & (1 << RXOK))
  {
    rmw = CANSTMOB;           //Read
    rmw &= ~(1 << RXOK) ;     //Modify
    CANSTMOB = rmw;           //Write
    return 1;
  }

  return 0;
}



/* ==================================================================*
 *            Transmit Message Objects
 * ==================================================================
 *      [0]     - DLC
 *      [1]     - Address
 *      [2..10] - DataBytes
 * ==================================================================*/

void CAN_TXMOB(unsigned char *object)
{
  unsigned char dlc = 0;
  unsigned char address = 0;

  CANPAGE =     (1 << MOBNB0);  //ChooseTXPage
  CANCDMOB =    0x00;           //Reset Control&DLCReg&IDE&RPLV

  dlc = *object;          //DLC
  object++;               //increase Pointer
  address = *object;      //Address
  object++;               //increase Pointer

  CANCDMOB |= (0x0F & (dlc)); //Datalenght
  CANIDT4 =   0x00;           //No RTRTAG and RB0TAG
  CANIDT3 =   0x00;           //Write Address
  CANIDT2 =   0x00;           //Write Address
  CANIDT1 =   address;        //Address

  while(dlc)
  {
    CANMSG = *object;   //CAN Message@Index - autoInc
    object++;           //increase Pointer on Messages
    dlc--;              //decrease dlc
  }

  CANCDMOB |= (1 << CONMOB0);     //Start TX-Communication
}



/* ==================================================================*
 *            Receive Message Objects
 * ==================================================================
 *      [0]   - DLC
 *      [1]   - Address
 *      [2]   - Command
 *      [3..11] - DataBytes
 * ==================================================================*/

unsigned char *CAN_RXMOB(void)
{
  static unsigned char  array[11];  //MessageObject
  unsigned char         dlc;        //DLC
  unsigned char         dataID = 2; //DataID

  CANPAGE = (1 << MOBNB1);    //ChooseRXPage
  if(CAN_RXOK())
  {
    array[0] = (CANCDMOB & 0x0F); //dlc
    array[1] = CANIDT1;           //address

    dlc = array[0];
    while(dlc)
    {
      array[dataID] = CANMSG; //Data
      dataID++;               //DataID inc
      dlc--;                  //DataCounter dec
    }
    return &array[0];         //*Return ArrayPointer
  }
  else array[0] = 0x00;       //Waiting4Message
  return &array[0];           //*Return ArrayPointer
}



/* ==================================================================*
 *            CAN - Commands
 * ==================================================================*/

void CAN_RXMOb_SetAddr(unsigned char addr)
{
  CANPAGE = (1 << MOBNB1);  //Chose Page

  CANIDT4 = 0x00;           //No RTRTAG and RB0TAG
  CANIDT3 = 0x00;
  CANIDT2 = 0x00;           //Address-210x xxxx
  CANIDT1 = addr;           //Address-A987 6543

  CANIDM4 = 0x00;           //ID-Mask 0x00->NoFiltering
  CANIDM3 = 0x00;
  CANIDM2 = 0x00;
  CANIDM1 = 0xFF;

  CANCDMOB |= (1 << CONMOB1);   //Start RX-Communication
}






/*********************************************************************\
 * End of file
\**********************************************************************/
