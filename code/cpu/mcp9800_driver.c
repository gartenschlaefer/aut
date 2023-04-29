// --
//  MCP9800 Temperature Sensor

#include <avr/io.h>

#include "mcp9800_driver.h"

#include "lcd_driver.h"
#include "twi_func.h"


/*-------------------------------------------------------------------*
 *            init
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
 *  one shot
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


/*-------------------------------------------------------------------*
 *            receive byte
 * ------------------------------------------------------------------*/

unsigned char MCP9800_ReceiveByte(struct TWIState *twi_state, unsigned char pointer)
{
  unsigned char send[] = {pointer};
  unsigned char *rec;
  unsigned char rData;

  TWI_D_Master_WriteString(MCP_ADDR, send, 1);
  rec = TWI_D_Master_ReadString(twi_state, MCP_ADDR, 1);
  rec++;
  rData = *rec;
  return rData;
}


/*-------------------------------------------------------------------*
 *            send byte
 * ------------------------------------------------------------------*/

void MCP9800_SendByte(unsigned char pointer, unsigned char sData)
{
  unsigned char send[] = {pointer, sData};
  TWI_D_Master_WriteString(MCP_ADDR, send, 2);
}


/*-------------------------------------------------------------------*
 *            read temperature
 * ------------------------------------------------------------------*/

unsigned char MCP9800_PlusTemp(struct TWIState *twi_state)
{
  unsigned char temp = 0;

  // read temperature
  temp = MCP9800_ReceiveByte(twi_state, 0x00);

  // minus sign
  if(temp & 0x80) temp = 0;

  return (temp & 0x7F);
}


/*-------------------------------------------------------------------*
 *            write temperature
 * ------------------------------------------------------------------*/

void MCP9800_WriteTemp(struct TWIState *twi_state)
{
  // read temperature
  unsigned char temp = MCP9800_ReceiveByte(twi_state, 0x00);

  // indicate minus
  if(temp & 0x80)
  {
    LCD_WriteAnyStringFont(f_6x8_p, 17, 84, "-");
    temp = -temp;
  }

  // clear minus sign
  else{ LCD_ClrSpace(17, 84, 2, 6); }

  // write temperature
  LCD_WriteAnyValue(f_6x8_p, 2, 17, 90, (temp & 0x7F));
}