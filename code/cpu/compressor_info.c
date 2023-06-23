// --
// compressor

#include "compressor_info.h"
#include "mcp7941_driver.h"
#include "lcd_sym.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Compressor_Info_Init(struct PlantState *ps)
{
  struct Thms *op_time = ps->compressor_state->operation_time;
  op_time->hou = MCP7941_Read_Comp_OpHours(ps->twi_state);
  op_time->min = 0;
  op_time->sec = 0;
  ps->compressor_state->cycle_o2_min = 0;
}


/* ------------------------------------------------------------------*
 *            resets
 * ------------------------------------------------------------------*/

void Compressor_Info_Reset_CycleO2(struct PlantState *ps){ ps->compressor_state->cycle_o2_min = 0; }
void Compressor_Info_Reset_OpHours(struct PlantState *ps){ MCP7941_Write_Comp_OpHours(0); ps->compressor_state->operation_time->hou = 0; }


/* ------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void Compressor_Info_Update(struct PlantState *ps)
{
  // sec change
  if(ps->time_state->tic_sec_update_flag)
  {
    // no update rules
    if(!ps->compressor_state->is_on_flag){ return; }

    struct Thms *op_time = ps->compressor_state->operation_time;

    // update second
    op_time->sec++;

    // min update
    if(op_time->sec >= 60)
    {
      op_time->sec = 0;
      op_time->min++;

      // o2 counting
      t_page p = ps->page_state->page;
      if(p == AutoCirc || p == AutoAir){ ps->compressor_state->cycle_o2_min++; }
    }

    // hour update
    if(op_time->min >= 60)
    {
      op_time->min = 0;
      op_time->hou++;

      // update compressor hours in memory
      MCP7941_Write_Comp_OpHours(op_time->hou);
      LCD_Sym_Auto_Compressor_OpHours(op_time->hou);
    }
  }
}