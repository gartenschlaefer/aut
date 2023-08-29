// --
// time state

#include "time_state.h"
#include "mcp7941_driver.h"
#include "compressor_info.h"


/* ------------------------------------------------------------------*
 *            timer update
 * ------------------------------------------------------------------*/

void TimeState_Init(struct PlantState *ps)
{
  // read timer ic vars
  ps->time_state->tic_sec = MCP7941_ReadTime(ps->twi_state, TIC_SEC);
  ps->time_state->tic_min = MCP7941_ReadTime(ps->twi_state, TIC_MIN);
  ps->time_state->tic_hou = MCP7941_ReadTime(ps->twi_state, TIC_HOUR);
  ps->time_state->tic_dat = MCP7941_ReadTime(ps->twi_state, TIC_DATE);
  ps->time_state->tic_mon = MCP7941_ReadTime(ps->twi_state, TIC_MONTH);
  ps->time_state->tic_yea = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
}


/* ------------------------------------------------------------------*
 *            timer update
 * ------------------------------------------------------------------*/

void TimeState_Update(struct PlantState *ps)
{
  // read seconds from timer ic
  unsigned char act_sec = MCP7941_ReadTime(ps->twi_state, TIC_SEC);

  // second change
  if(act_sec != ps->time_state->tic_sec)
  {
    // second update
    ps->time_state->tic_sec = act_sec;
    ps->time_state->tic_sec_update_flag = true;

    // minute change
    if(!act_sec)
    {
      // minute update
      ps->time_state->tic_min = MCP7941_ReadTime(ps->twi_state, TIC_MIN);

      // hour change
      if(!ps->time_state->tic_min)
      {
        // hour update
        ps->time_state->tic_hou = MCP7941_ReadTime(ps->twi_state, TIC_HOUR);

        // day change
        if(!ps->time_state->tic_hou)
        {
          // other updates
          ps->time_state->tic_dat = MCP7941_ReadTime(ps->twi_state, TIC_DATE);
          ps->time_state->tic_mon = MCP7941_ReadTime(ps->twi_state, TIC_MONTH);
          ps->time_state->tic_yea = MCP7941_ReadTime(ps->twi_state, TIC_YEAR);
        }
      }
    }
  }

  // no second change
  else
  {
    ps->time_state->tic_sec_update_flag = false;
  }

  // time sec update
  if(ps->time_state->tic_sec_update_flag)
  {
    TimeState_TicSecUpdate(ps);
  }
}


/* ------------------------------------------------------------------*
 *            tic update
 * ------------------------------------------------------------------*/

void TimeState_TicSecUpdate(struct PlantState *ps)
{
  // compressor info
  Compressor_Info_TicSecUpdate(ps);
}