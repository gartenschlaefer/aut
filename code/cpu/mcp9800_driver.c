// --
//  MCP9800 Temperature Sensor

#include <avr/io.h>

#include "mcp9800_driver.h"

#include "lcd_driver.h"
#include "twi_func.h"
#include "error_func.h"


/*-------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void MCP9800_Init(struct PlantState *ps)
{
  unsigned char init = ( MCP_ONESHOT_OFF | MCP_RES | MCP_FAULTQ | MCP_ALERTPOL | MCP_COMP_INT | MCP_SHUTDOWN_OFF);

  // write config register
  MCP9800_SendByte(MCP_CONFREG, init);

  // alert as input
  MCP_PORT.DIRCLR = MCP_ALERT;

  // read temperature
  ps->temp_sensor->actual_temp = MCP9800_ReceiveByte(ps->twi_state, 0x00);
}


/*-------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void MCP9800_Temp_Update(struct PlantState *ps)
{
  if(ps->frame_counter->sixty_sec_counter % TEMP_UPDATE_SEC_MOD)
  {
    char temp = (char)MCP9800_ReceiveByte(ps->twi_state, 0x00);
    ps->temp_sensor->actual_temp = temp;
    ps->temp_sensor->new_temp_flag = true;


    // error check
    if(temp > (char)ps->settings->settings_alarm->temp){ ps->error_state->pending_err_code |= E_T; }
    else{ ps->error_state->pending_err_code &= ~E_T; }
  }
  else{ ps->temp_sensor->new_temp_flag = false; }
}


/*-------------------------------------------------------------------*
 *            one shot
 * --------------------------------------------------------------
 *  one conversion in Shutdown Mode, remain in Shutdown Mode, go back to init
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

unsigned char MCP9800_ReceiveByte(struct TWIState *twi_state, unsigned char byte)
{
  unsigned char send[] = { byte };
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

void MCP9800_SendByte(unsigned char byte, unsigned char sData)
{
  unsigned char send[] = {byte, sData};
  TWI_D_Master_WriteString(MCP_ADDR, send, 2);
}