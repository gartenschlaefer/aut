// --
//  MCP9800 Temperature Sensor

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "mcp9800_driver.h"
#include "twi_func.h"



/* ==================================================================*
 *            FUNCTIONS
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  MCP9800_Init
 * ------------------------------------------------------------------*/

void MCP9800_Init(void)
{
  unsigned char init = ( MCP_ONESHOT_OFF | MCP_RES | MCP_FAULTQ | MCP_ALERTPOL | MCP_COMP_INT | MCP_SHUTDOWN_OFF);

  // write config register
  MCP9800_SendByte(MCP_CONFREG, init);

  // alert as input
  MCP_PORT.DIRCLR = MCP_ALERT;
}


/*-------------------------------------------------------------------*
 *  MCP9800_OneShot
 * --------------------------------------------------------------
 *  One Conversion in Shutdown Mode, remain in Shutdown Mode, go back to init
 * ------------------------------------------------------------------*/

void MCP9800_OneShot(void)
{
  unsigned char shutdown = ( MCP_ONESHOT_OFF | MCP_RES | MCP_FAULTQ | MCP_ALERTPOL | MCP_COMP_INT | MCP_SHUTDOWN_ON);

  // shutdown mode
  MCP9800_SendByte(MCP_CONFREG, shutdown);

  // one shot
  MCP9800_SendByte(MCP_CONFREG, (shutdown | MCP_ONESHOT_ON));
}



/* ==================================================================*
 *            FUNCTIONS Write and Read
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  MCP9800 receive byte
 * ------------------------------------------------------------------*/

unsigned char MCP9800_ReceiveByte(unsigned char pointer)
{
  unsigned char send[] = {pointer};
  unsigned char *rec;
  unsigned char rData;

  TWI2_Master_WriteString(MCP_ADDR, send, 1);
  rec = TWI2_Master_ReadString(MCP_ADDR, 1);
  rec++;
  rData = *rec;
  return rData;
}


/*-------------------------------------------------------------------*
 *  MCP9800 send byte
 * ------------------------------------------------------------------*/

void MCP9800_SendByte(unsigned char pointer, unsigned char sData)
{
  unsigned char send[] = {pointer, sData};
  TWI2_Master_WriteString(MCP_ADDR, send, 2);
}



/* ==================================================================*
 *            FUNCTIONS - Apps
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  MPC9800 - Read Temperature (only PlusTemp)
 * ------------------------------------------------------------------*/

unsigned char MCP9800_PlusTemp(void)
{
  unsigned char temp = 0;

  // read temperature
  temp = MCP9800_ReceiveByte(0x00);

  // minus sign
  if(temp & 0x80) temp = 0;

  return (temp & 0x7F);
}


/*-------------------------------------------------------------------*
 *  MPC9800 - WriteTemp
 * ------------------------------------------------------------------*/

void MCP9800_WriteTemp(void)
{
  unsigned char temp = 0;
  static unsigned char run = 0;

  run++;

  // 1 second
  if(run > 10)
  {
    // reset run
    run = 0;

    // read temperature
    temp = MCP9800_ReceiveByte(0x00);

    // indicate minus
    if(temp & 0x80)
    {
      LCD_WriteStringFont(17,84,"-");
      temp = -temp;
    }

    // clear minus sign
    else LCD_ClrSpace(17, 84, 2, 6);

    // write temperature
    LCD_WriteValue2(17, 90, (temp & 0x7F));
  }
}