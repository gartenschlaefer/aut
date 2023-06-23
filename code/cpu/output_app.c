// --
// outputs for relays and valves

#include <stdlib.h>
#include "output_app.h"
#include "config.h"
#include "basic_func.h"
#include "port_func.h"
#include "memory_app.h"
#include "tc_func.h"
#include "queue.h"


/* ------------------------------------------------------------------*
 *            SetDown
 * ------------------------------------------------------------------*/

void OUT_SetDown(struct PlantState *ps)
{
  OUT_Clr_Compressor(ps);
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
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))){ OUT_Valve_Action(ps, OPEN_ClearWater); }
  else{ PORT_RelaisSet(R_CLEARWATER); }
}

void OUT_Clr_PumpOff(struct PlantState *ps)
{
  // relays
  PORT_RelaisClr(R_CLEARWATER);

  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))){ OUT_Valve_Action(ps, CLOSE_ClearWater); }
}


/* ------------------------------------------------------------------*
 *            Mud
 * ------------------------------------------------------------------*/

void OUT_Set_Mud(struct PlantState *ps)
{
  OUT_Valve_Action(ps, OPEN_MudPump);
}

void OUT_Clr_Mud(struct PlantState *ps)
{
  OUT_Valve_Action(ps, CLOSE_MudPump);
}


/* ------------------------------------------------------------------*
 *            Air
 * ------------------------------------------------------------------*/

void OUT_Set_Air(struct PlantState *ps)
{
  OUT_Valve_Action(ps, OPEN_Air);
}

void OUT_Clr_Air(struct PlantState *ps)
{
  OUT_Valve_Action(ps, CLOSE_Air);
}


/* ------------------------------------------------------------------*
 *            Compressor
 * ------------------------------------------------------------------*/

void OUT_Set_Compressor(struct PlantState *ps)
{
  PORT_RelaisSet(R_COMP);
  PORT_RelaisSet(R_EXT_COMP);
  ps->compressor_state->is_on_flag = true;
}

void OUT_Clr_Compressor(struct PlantState *ps)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
  ps->compressor_state->is_on_flag = false;
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
  switch(ps->settings->settings_inflow_pump->pump)
  {
    // mammoth pump
    case 0: OUT_Valve_Action(ps, OPEN_Reserve); break;

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
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);

  if(!ps->settings->settings_inflow_pump->pump){ OUT_Valve_Action(ps, CLOSE_Reserve); }
}


/* ------------------------------------------------------------------*
 *            initialize valves to all closed
 * ------------------------------------------------------------------*/

void OUT_Valve_Init(struct PlantState *ps)
{
  if(!ps->port_state->valve_init)
  {
    // valves with springs
    if(SPRING_VALVE_ON)
    {
      // open all valves
      OUT_Valve_Action(ps, OPEN_All);

      // close all valves
      OUT_Valve_Action(ps, CLOSE_All);
    }

    // valves without springs
    else{ OUT_Valve_Action(ps, CLOSE_All); }

    // set init flag
    ps->port_state->valve_init = true;
  }
}


/* ------------------------------------------------------------------*
 *            valve action
 * ------------------------------------------------------------------*/

void OUT_Valve_Action(struct PlantState *ps, t_valve_action valve_action)
{
  // check length of queue
  int len = queue_len(ps->port_state->queue_valve_action);
  if(len > 4){ ps->port_state->valve_action_flag = true; return; }

  // new data element
  unsigned char *new_action = malloc(1);
  *new_action = valve_action;

  // add valve action
  queue_enqueue(ps->port_state->queue_valve_action, new_action);
  
  // signalize action
  ps->port_state->valve_action_flag = true;
}


/* ------------------------------------------------------------------*
 *            valve update
 * ------------------------------------------------------------------*/

void OUT_Valve_Update(struct PlantState *ps)
{
  if(ps->port_state->valve_action_flag)
  {
    // idle state -> get action
    if(ps->port_state->valve_handling == _valveHandling_idle)
    {
      if(queue_empty(ps->port_state->queue_valve_action))
      {
        ps->port_state->valve_action = VALVE_Idle;
        ps->port_state->valve_action_flag = false;
      }
      else
      {
        unsigned char *data = queue_dequeue(ps->port_state->queue_valve_action);
        ps->port_state->valve_action = *data;
        ps->port_state->valve_handling = _valveHandling_set;
        free(data);
      }
    }

    // set outputs
    else if(ps->port_state->valve_handling == _valveHandling_set)
    {
      // set outputs
      OUT_Valve_SetOutput(ps);
      
      // wait one
      OUT_Valve_Wait1(ps->port_state->valve_action);

      // next handling state
      ps->port_state->valve_handling = _valveHandling_wait1;
    }

    // wait one
    else if(ps->port_state->valve_handling == _valveHandling_wait1)
    {
      if(TCF0_Wait_Query())
      {
        // wait two
        OUT_Valve_Wait2(ps->port_state->valve_action);

        // next handling state
        ps->port_state->valve_handling = _valveHandling_wait2;
      }
    }

    // wait two
    else if(ps->port_state->valve_handling == _valveHandling_wait2)
    {
      if(TCF0_Wait_Query())
      {     
        // next handling state
        ps->port_state->valve_handling = _valveHandling_reset;
        TCF0_Stop();
      }
    }

    // reset state
    else if(ps->port_state->valve_handling == _valveHandling_reset)
    {
      OUT_Valve_ClrOutput(ps);
      ps->port_state->valve_handling = _valveHandling_idle;
    }

    else{ ps->port_state->valve_handling = _valveHandling_idle; }
  }
}


/* ------------------------------------------------------------------*
 *            wait 1
 * ------------------------------------------------------------------*/

void OUT_Valve_Wait1(t_valve_action valve_action)
{
  if((valve_action == OPEN_All) || (valve_action == CLOSE_All))
  {
    // init time
    if(SPRING_VALVE_ON){ TCF0_WaitSec_Init(4); }
    else{ TCF0_WaitSec_Init(2); }
  }
  else
  {
    // init time
    if(SPRING_VALVE_ON){ TCF0_WaitSec_Init(4); }
    else{ TCF0_WaitSec_Init(3); }
  }
}


/* ------------------------------------------------------------------*
 *            wait 2
 * ------------------------------------------------------------------*/

void OUT_Valve_Wait2(t_valve_action valve_action)
{
  switch(valve_action)
  {
    case OPEN_Reserve: case OPEN_MudPump: case OPEN_Air: case OPEN_ClearWater:
      if(SPRING_VALVE_ON){ TCF0_WaitMilliSec_Init(350); }
      else{ TCF0_WaitMilliSec_Init(5); }
      break;

    case CLOSE_Reserve: case CLOSE_MudPump: case CLOSE_Air: case CLOSE_ClearWater:
      if(SPRING_VALVE_ON){ TCF0_WaitMilliSec_Init(305); }
      else{ TCF0_WaitMilliSec_Init(100); }
      break;

    case OPEN_All:
      if(SPRING_VALVE_ON){ TCF0_WaitMilliSec_Init(50); }
      else{ TCF0_WaitMilliSec_Init(705); }
      break;

    case CLOSE_All: 
      if(SPRING_VALVE_ON){ TCF0_WaitMilliSec_Init(5); }
      else{ TCF0_WaitMilliSec_Init(800); }
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            valve set output
 * ------------------------------------------------------------------*/

void OUT_Valve_SetOutput(struct PlantState *ps)
{
  switch(ps->port_state->valve_action)
  {
    case OPEN_Reserve: P_VALVE.OUTSET = O_RES; break;
    case OPEN_MudPump: P_VALVE.OUTSET = O_MUD; break;
    case OPEN_Air: P_VALVE.OUTSET = O_AIR; break;
    case OPEN_ClearWater: P_VALVE.OUTSET = O_CLRW; break;
    case CLOSE_Reserve: OUT_Clr_Compressor(ps); P_VALVE.OUTSET = C_RES; break;
    case CLOSE_MudPump: OUT_Clr_Compressor(ps); P_VALVE.OUTSET = C_MUD; break;
    case CLOSE_Air: OUT_Clr_Compressor(ps); P_VALVE.OUTSET = C_AIR; break;
    case CLOSE_ClearWater: OUT_Clr_Compressor(ps); P_VALVE.OUTSET = C_CLRW; break;

    case OPEN_All: 
      P_VALVE.OUTSET = O_RES; TCC0_wait_ms(100);
      P_VALVE.OUTSET = O_MUD; TCC0_wait_ms(100);
      P_VALVE.OUTSET = O_AIR; TCC0_wait_ms(100);
      P_VALVE.OUTSET = O_CLRW;
      break;

    case CLOSE_All:
      P_VALVE.OUTSET = C_RES; TCC0_wait_ms(100);
      P_VALVE.OUTSET = C_MUD; TCC0_wait_ms(100);
      P_VALVE.OUTSET = C_AIR; TCC0_wait_ms(100);
      P_VALVE.OUTSET = C_CLRW;
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            valve clear output
 * ------------------------------------------------------------------*/

void OUT_Valve_ClrOutput(struct PlantState *ps)
{
  switch(ps->port_state->valve_action)
  {
    // open
    case OPEN_Reserve: P_VALVE.OUTCLR = O_RES; ps->port_state->valve_state |= V_RES; OUT_Set_Compressor(ps); break;
    case OPEN_MudPump: P_VALVE.OUTCLR = O_MUD; ps->port_state->valve_state |= V_MUD; OUT_Set_Compressor(ps); break;
    case OPEN_Air: P_VALVE.OUTCLR = O_AIR; ps->port_state->valve_state |= V_AIR; OUT_Set_Compressor(ps); break;
    case OPEN_ClearWater: P_VALVE.OUTCLR = O_CLRW; ps->port_state->valve_state |= V_CLW; OUT_Set_Compressor(ps); break;

    // close
    case CLOSE_Reserve: P_VALVE.OUTCLR = C_RES; ps->port_state->valve_state &= ~V_RES; break;
    case CLOSE_MudPump: P_VALVE.OUTCLR = C_MUD; ps->port_state->valve_state &= ~V_MUD; break;
    case CLOSE_Air: P_VALVE.OUTCLR = C_AIR; ps->port_state->valve_state &= ~V_AIR; break;
    case CLOSE_ClearWater: P_VALVE.OUTCLR = C_CLRW; ps->port_state->valve_state &= ~V_CLW; break;

    case OPEN_All:
      P_VALVE.OUTCLR = O_RES; TCC0_wait_ms(100);
      P_VALVE.OUTCLR = O_MUD; TCC0_wait_ms(100);
      P_VALVE.OUTCLR = O_AIR; TCC0_wait_ms(100);
      P_VALVE.OUTCLR = O_CLRW;
      ps->port_state->valve_state = 0x0F;

    case CLOSE_All:
      P_VALVE.OUTCLR = C_RES; TCC0_wait_ms(100);
      P_VALVE.OUTCLR = C_MUD; TCC0_wait_ms(100);
      P_VALVE.OUTCLR = C_AIR; TCC0_wait_ms(100);
      P_VALVE.OUTCLR = C_CLRW;
      ps->port_state->valve_state = 0x00;

    default: break;
  }
}