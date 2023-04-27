// --
// outputs for relays and valves

#include "output_app.h"

#include "config.h"
#include "port_func.h"
#include "memory_app.h"


/* ------------------------------------------------------------------*
 *            SetDown
 * ------------------------------------------------------------------*/

void OUT_SetDown(void)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);
  PORT_RelaisClr(R_CLEARWATER);
}


/* ------------------------------------------------------------------*
 *            PumpOff
 * ------------------------------------------------------------------*/

void OUT_Set_PumpOff(struct PlantState *ps)
{
  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff)))
  {
    PORT_Valve(ps, OPEN_ClearWater);
    OUT_Set_Compressor();
  }
  else{ PORT_RelaisSet(R_CLEARWATER); }
}

void OUT_Clr_PumpOff(struct PlantState *ps)
{
  // relays
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_CLEARWATER);

  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))){ PORT_Valve(ps, CLOSE_ClearWater); }
}


/* ------------------------------------------------------------------*
 *            Mud
 * ------------------------------------------------------------------*/

void OUT_Set_Mud(struct PlantState *ps)
{
  PORT_Valve(ps, OPEN_MudPump);
  OUT_Set_Compressor();
}

void OUT_Clr_Mud(struct PlantState *ps)
{
  OUT_Clr_Compressor();
  PORT_Valve(ps, CLOSE_MudPump);
}


/* ------------------------------------------------------------------*
 *            Air
 * ------------------------------------------------------------------*/

void OUT_Set_Air(struct PlantState *ps)
{
  PORT_Valve(ps, OPEN_Air);
  OUT_Set_Compressor();
}

void OUT_Clr_Air(struct PlantState *ps)
{
  OUT_Clr_Compressor();
  PORT_Valve(ps, CLOSE_Air);
}


/* ------------------------------------------------------------------*
 *            Compressor
 * ------------------------------------------------------------------*/

void OUT_Set_Compressor(void)
{
  PORT_RelaisSet(R_COMP);
  PORT_RelaisSet(R_EXT_COMP);
}

void OUT_Clr_Compressor(void)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
}


/* ------------------------------------------------------------------*
 *            Phosphor
 * ------------------------------------------------------------------*/

void OUT_Set_Phosphor(void)
{
  PORT_RelaisSet(R_PHOSPHOR);
}

void OUT_Clr_Phosphor(void)
{
  PORT_RelaisClr(R_PHOSPHOR);
}


/* ------------------------------------------------------------------*
 *            InflowPump
 * ------------------------------------------------------------------*/

void OUT_Set_InflowPump(struct PlantState *ps)
{
  switch(MEM_EEPROM_ReadVar(PUMP_inflowPump))
  {
    // mammoth pump
    case 0: PORT_Valve(ps, OPEN_Reserve); OUT_Set_Compressor(); break;

    // ext. pump 1
    case 1: PORT_RelaisSet(R_INFLOW1); break;

    // ext. pump 2
    case 2:
      if(ps->inflow_pump_state->ip_active_pump_id == 0){ ps->inflow_pump_state->ip_active_pump_id = 1; PORT_RelaisSet(R_INFLOW1); }
      else{ ps->inflow_pump_state->ip_active_pump_id = 0; PORT_RelaisSet(R_INFLOW2); }   
      break;

    default: break;
  }
}

void OUT_Clr_InflowPump(struct PlantState *ps)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);

  if(!MEM_EEPROM_ReadVar(PUMP_inflowPump)){ PORT_Valve(ps, CLOSE_Reserve); }
}


/* ------------------------------------------------------------------*
 *            InflowPump and Air
 * ------------------------------------------------------------------*/

void OUT_Clr_IPAir(struct PlantState *ps)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);
  PORT_Valve(ps, CLOSE_IPAir);
}


/* ------------------------------------------------------------------*
 *            All Off
 * ------------------------------------------------------------------*/

void OUT_Valve_CloseAll(struct PlantState *ps)
{
  OUT_Clr_Compressor();
  PORT_Valve_CloseAll(ps);
}


/* ------------------------------------------------------------------*
 *            initialize valves to all closed
 * ------------------------------------------------------------------*/

void OUT_Valve_Init(struct PlantState *ps)
{
  // valves with springs
  if(SPRING_VALVE_ON)
  {
    // open all valves
    PORT_Valve_OpenAll(ps);

    // close all valves
    OUT_Valve_CloseAll(ps);
  }

  // valves without springs
  else{ OUT_Valve_CloseAll(ps); }
}
