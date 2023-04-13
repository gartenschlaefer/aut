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

void OUT_Set_PumpOff(void)
{
  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff)))
  {
    PORT_Valve(OPEN_ClearWater, 0);
    OUT_Set_Compressor();
  }
  else PORT_RelaisSet(R_CLEARWATER);
}

void OUT_Clr_PumpOff(void)
{
  // relays
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_CLEARWATER);

  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))) PORT_Valve(CLOSE_ClearWater, 0);
}


/* ------------------------------------------------------------------*
 *            Mud
 * ------------------------------------------------------------------*/

void OUT_Set_Mud(void)
{
  PORT_Valve(OPEN_MudPump, 0);
  OUT_Set_Compressor();
}

void OUT_Clr_Mud(void)
{
  OUT_Clr_Compressor();
  PORT_Valve(CLOSE_MudPump, 0);
}


/* ------------------------------------------------------------------*
 *            Air
 * ------------------------------------------------------------------*/

void OUT_Set_Air(void)
{
  PORT_Valve(OPEN_Air, 0);
  OUT_Set_Compressor();
}

void OUT_Clr_Air(void)
{
  OUT_Clr_Compressor();
  PORT_Valve(CLOSE_Air, 0);
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
    case 0: PORT_Valve(OPEN_Reserve, 0); OUT_Set_Compressor(); break;

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

void OUT_Clr_InflowPump(void)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);

  if(!MEM_EEPROM_ReadVar(PUMP_inflowPump)) PORT_Valve(CLOSE_Reserve, 0);
}


/* ------------------------------------------------------------------*
 *            InflowPump and Air
 * ------------------------------------------------------------------*/

void OUT_Clr_IPAir(void)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);
  PORT_Valve(CLOSE_IPAir, 0);
}


/* ------------------------------------------------------------------*
 *            All Off
 * ------------------------------------------------------------------*/

void OUT_Valve_CloseAll(void)
{
  OUT_Clr_Compressor();
  PORT_Valve_CloseAll();
}


/* ------------------------------------------------------------------*
 *            initialize valves to all closed
 * ------------------------------------------------------------------*/

void OUT_Valve_Init(void)
{
  // valves with springs
  if(SPRING_VALVE_ON)
  {
    // open all valves
    PORT_Valve_OpenAll();

    // close all valves
    OUT_Valve_CloseAll();
  }

  // valves without springs
  else{ OUT_Valve_CloseAll(); }
}


/* ------------------------------------------------------------------*
 *            valve auto close
 * ------------------------------------------------------------------*/

void OUT_Valve_AutoClose(t_page page)
{
  switch(page)
  {
    case AutoSetDown: break;
    case AutoZone: OUT_Clr_Air(); break;
    case AutoPumpOff: OUT_Clr_PumpOff(); break;
    case AutoMud: OUT_Clr_Mud(); break;
    case AutoCircOn: OUT_Clr_IPAir(); break;
    case AutoAirOn: OUT_Clr_IPAir(); break;
    default: break;
  }
}