/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    boot_func.c
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Bootloader Functions Applications
* ------------------------------------------------------------------
*	Date:			    02.01.2016
* lastChanges:
\**********************************************************************/

#include<avr/io.h>

/* ===================================================================*
 * 						Header
 * ===================================================================*/

#include "defines.h"
#include "tc_func.h"
#include "basic_func.h"
#include "boot_func.h"
#include "boot_app.h"
#include "can_func.h"
#include "can_app.h"


/* ===================================================================*
 * 						FUNCTIONS Apps
 * ===================================================================*/
/* -------------------------------------------------------------------*
 *  At90can32
 *  Application Section:  0x0000 - 0x2FFF   24kB    96 pages
 *  Bootloader Section:	  0x3000 - 0x3FFF		8kB     32 pages
 *  Pages:                32
 *  Words / Page:         128
 * -------------------------------------------------------------------*/

/* ===================================================================*
 * 						Bootloader Programm Application Section
 * ===================================================================*/

t_UScmd Boot_Program_App(void)
{
  unsigned char *p_rx = 0;
  unsigned char i = 0;
  unsigned char word = 0;
  unsigned char page = 0;
  int data = 0;

  TC0_Wait_msQuery(_init, TC_CAN_MS);        //SafetyTimer
  //--------------------------------------------------64Pages(8kB)
  for(page = 0; page < 64; page++)
  {
    //--------------------------------------------------1Page
    for(word = 0; word < 128; word += 4)
    {
      WDT_RESET;
      p_rx = CAN_RXMOB();		  //RXMob
      while(!p_rx[0]){
        p_rx = CAN_RXMOB();		//RXMob
        if(TC0_Wait_msQuery(_exe, TC_CAN_MS))
          return _wait;}
      //----------------------------------------------DataReceived
      if(p_rx[0] == 8)
      {
        for(i = 0; i < 4; i++){
          data = p_rx[(i * 2) + 2] | (p_rx[(i * 2) + 3] << 8);
          Boot_LoadPageBuffer_Word((word + i), data);}	  //loadOneWord

        TC0_Wait_msQuery(_init, TC_CAN_MS);
        CAN_TXMOB_ACK_Program();
      }
      //----------------------------------------------End
      else if((p_rx[0] == 1) && (p_rx[2] == _ack))
      {
        Boot_WritePage(page);
        return _wait;
      }
    }
    Boot_WritePage(page);
  }
  return _wait;
}



/* ===================================================================*
 * 						Read Application in UltraSonic
 * ===================================================================*/

t_UScmd Boot_Read_App(void)
{
  int word = 0;
  unsigned char page = 0;
  unsigned char *p_rx = 0;


  TC0_Wait_msQuery(_init, TC_CAN_MS);        //SafetyTimer
  //--------------------------------------------------64Pages(8kB)
  for(page = 0; page < 64; page++)
  {
    //------------------------------------------------1Page-256W
    for(word = 0; word < 128; word += 4)
    {
      p_rx = CAN_RXMOB();		  //RXMob
      while(!p_rx[0]){
        p_rx = CAN_RXMOB();		//RXMob
        if(TC0_Wait_msQuery(_exe, TC_CAN_MS))
          return _wait;}

      if(p_rx[2] == _readProgram)
      {
        Boot_Write4Words2CAN(page, word);  //Read4WordsFromApp
        WDT_RESET;
        TC0_Wait_msQuery(_init, TC_CAN_MS);
      }
      else if(p_rx[2] == _ack)  return _wait;
    }
  }
  return _wait;
}


/* ------------------------------------------------------------------*
 * 						Write 4 Words to CAN Bus
 * ------------------------------------------------------------------*/

void Boot_Write4Words2CAN(unsigned char page, unsigned char word)
{
  unsigned char send[10] = {8, 0, 1, 2, 3, 4, 5, 6, 7, 8};
  unsigned char i = 0;

  for(i = 0; i < 4; i++)
  {
    send[(i * 2) + 2] = Boot_Read(page, word, 0);	  //readLByte
		send[(i * 2) + 3] = Boot_Read(page, word, 1);	  //read HByte
    word++;
  }
  CAN_TXMOB(send);
}







/***********************************************************************
 *	End of file
 ***********************************************************************/


