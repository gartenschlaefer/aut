// --
// error detection and treatment

#include <string.h>

#include "error_func.h"

#include "lcd_driver.h"
#include "lcd_sym.h"
#include "lcd_app.h"
#include "memory_app.h"
#include "mcp9800_driver.h"
#include "mpx_driver.h"
#include "tc_func.h"
#include "port_func.h"
#include "output_app.h"
#include "modem_driver.h"
#include "config.h"


/*-------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Error_Init(struct PlantState *ps)
{
  ;
}


/*-------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void Error_Update(struct PlantState *ps)
{
  ;
}


/*-------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void Error_ChangePage(struct PlantState *ps)
{
  ;
}


/*-------------------------------------------------------------------*
 *            set error signals
 * ------------------------------------------------------------------*/

void Error_On(struct PlantState *ps)
{
  ps->port_state->buzzer_on = true;
  PORT_Backlight_Error(ps->backlight);
  PORT_RelaisSet(R_ALARM);
}

void Error_Off(struct PlantState *ps)
{
  ps->port_state->buzzer_on = false;
  BUZZER_OFF;
  PORT_Backlight_On(ps->backlight);
  PORT_RelaisClr(R_ALARM);
}


/* ---------------------------------------------------------------*
 *          read error
 * ---------------------------------------------------------------*/

void Error_Read(struct PlantState *ps)
{
  // set pending to zero
  ps->error_state->pending_err_code = 0;

  // under-pressure check if necessary
  bool check_compressor_errors = ps->compressor_state->is_on_flag && !ps->port_state->valve_action_flag;

  // temp
  // if(MCP9800_PlusTemp(ps->twi_state) > ps->settings->settings_alarm->temp){ ps->error_state->pending_err_code |= E_T; }

  // compressor errors
  if(check_compressor_errors)
  {
    // over-pressure
    if(ps->mpx_state->actual_mpx_av > ps->settings->settings_compressor->max_pressure){ ps->error_state->pending_err_code |= E_OP; }

    // under-pressure
    if(ps->mpx_state->actual_mpx_av < ps->settings->settings_compressor->min_pressure){ ps->error_state->pending_err_code |= E_UP; }
  }

  // todo: max in tank
  // must also consider sonic
  //if(ps->mpx_state->actual_mpx_av >= (ps->settings->settings_zone->level_to_set_down + ps->settings->settings_calibration->tank_level_min_pressure)){ ps->error_state->pending_err_code |= E_IT; }
}


/* ---------------------------------------------------------------*
 *           Error Detection
 * ---------------------------------------------------------------*/

void Error_Detection_Update(struct PlantState *ps)
{
  // check if errors occur
  Error_Read(ps);

  // run the buzzer if on
  PORT_Buzzer_Update(ps);

  // go through each error
  for(unsigned i = 0; i < NUM_ERRORS; i++)
  { 
    // error treatment if under treatmet
    if(ps->error_state->error_code  & (1 << i))
    {
      // error treatment
      Error_Treatment(ps, i);

      // treated error
      if(!(ps->error_state->error_code  & (1 << i))){ ps->error_state->reset_error_indicator |= (1 << i); }
    }

    // on counter
    else
    {
      // second update
      if(ps->time_state->tic_sec_update_flag)
      { 
        // count error on
        if(ps->error_state->pending_err_code & (1 << i)){ ps->error_state->error_on_counter[i]++; }
        else{ ps->error_state->error_on_counter[i] = 0; continue; }

        // print the symbols
        if(ps->error_state->error_on_counter[i] == ERROR_ON_SYM_SEC){ LCD_Sym_Error(i); }

        // set error on
        else if(ps->error_state->error_on_counter[i] >= ERROR_ON_TIME_SEC)
        {
          ps->error_state->error_on_counter[i] = 0;
          ps->error_state->pending_err_code &= ~(1 << i);
          ps->error_state->error_code |= (1 << i);
        }
      }
    }

    // reset error
    if(ps->error_state->reset_error_indicator & (1 << i))
    {
      if(!(ps->error_state->pending_err_code & (1 << i)) && !(ps->error_state->error_code & (1 << i)))
      {
        // reset error
        ps->error_state->error_on_counter[i] = 0;
        Error_Off(ps);
        LCD_Sym_Auto_SetManager(ps, ps->page_state->page);
        ps->error_state->reset_error_indicator &= ~(1 << i);
      }
    }
  }
}


/* ---------------------------------------------------------------*
 *            Error Treatment
 * ---------------------------------------------------------------*/

void Error_Treatment(struct PlantState *ps, unsigned char error_id)
{
  bool set_error_flag = true;
  switch(error_id)
  {
    case ERROR_ID_T:
      // nothing to treat
      ps->error_state->error_code &= ~E_T; 
      break;

    case ERROR_ID_OP:
      // treat this error as valve error
      Error_Action_OP_Air(ps);
      ps->error_state->error_code &= ~(1 << error_id);
      break;

    case ERROR_ID_UP:
      // should not happen
      set_error_flag = ps->settings->settings_alarm->compressor;
      //OUT_Set_Compressor(ps);
      ps->error_state->error_code &= ~(1 << error_id);
      break;

    case ERROR_ID_IT:
      // todo: must be implemented or removed
      set_error_flag = ps->settings->settings_alarm->sensor;
      ps->error_state->error_code &= ~(1 << error_id);
      break;

    case ERROR_ID_OT: 
      // todo: must be implemented or removed
      set_error_flag = ps->settings->settings_alarm->sensor;
      break;
    default: break;
  }

  // set error
  ps->error_state->error_counter[error_id]++;
  if(ps->error_state->error_counter[error_id] == 2)
  {
    if(set_error_flag){ Error_On(ps); }
    ps->error_state->cycle_error_code_record |= error_id;
    Error_ModemAction(ps, error_id);
  }
  if(ps->error_state->error_counter[error_id] > ERROR_COUNT_NEW_ACTION){ ps->error_state->error_counter[error_id] = 0; }

}


/* ------------------------------------------------------------------*
 *            Error Action OverPressure - Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_OP_Air(struct PlantState *ps)
{
  t_valve_action valve_actions[3] = { _va_none, _va_none, _va_none };

  // page cases
  switch(ps->page_state->page)
  {
    case AutoPumpOff: if(!ps->settings->settings_pump_off->pump){ valve_actions[0] = valve_actions[2] = OPEN_ClearWater; valve_actions[1] = CLOSE_ClearWater; } break;
    case AutoMud: valve_actions[0] = valve_actions[2] = OPEN_MudPump; valve_actions[1] = CLOSE_MudPump; break;

    case AutoAir:
    case AutoCirc:
      if(ps->air_circ_state->ac_state == _ac_on){ valve_actions[0] = valve_actions[2] = OPEN_Air; valve_actions[1] = CLOSE_Air; }
      else{ if((ps->inflow_pump_state->ip_state == _ip_on) && !ps->settings->settings_inflow_pump->pump){ valve_actions[0] = valve_actions[2] = OPEN_Reserve; valve_actions[1] = CLOSE_Reserve; } }
      break;

    case AutoZone: valve_actions[0] = valve_actions[2] = OPEN_Air; valve_actions[1] = CLOSE_Air; break;
    default: break;
  }

  // add actions
  for(unsigned char i = 0; i < 3; i++){ if(valve_actions[i] != _va_none){ OUT_Valve_Action(ps, valve_actions[i]); } }
  return 1;
}

// unsigned char Error_Action_OP_Air(struct PlantState *ps)
// {
//   // start closing valves
//   if(ps->error_state->op_state == _error_op_close_start)
//   {
//     // stop compressor and set errors
//     OUT_Clr_Compressor();
//     Error_Action_OP_SetError(ps);

//     // close first (only if no spring valves)
//     if(!SPRING_VALVE_ON)
//     {
//       // page cases
//       switch(ps->page_state->page)
//       {
//         case AutoPumpOff: if(!ps->settings->settings_pump_off->pump){ OUT_Valve_Action(ps, CLOSE_ClearWater); } break;
//         case AutoMud: OUT_Valve_Action(ps, CLOSE_MudPump); break;

//         case AutoAir:
//         case AutoCirc:
//           if(ps->air_circ_state->ac_state == _ac_on){ OUT_Valve_Action(ps, CLOSE_Air); }

//           if((ps->inflow_pump_state->ip_state == _ip_on) && !ps->settings->settings_inflow_pump->pump){ OUT_Valve_Action(ps, CLOSE_Reserve); }
//           break;

//         // air on
//         case AutoCircOn: case AutoAirOn: 

//         case AutoZone: OUT_Valve_Action(ps, CLOSE_Air); break;

//         default: break;
//       }
//     }

//     // close process timing
//     ps->error_state->op_state = _error_op_close_process;
//   }

//   // wait until done
//   else if(ps->error_state->op_state == _error_op_close_process)
//   {
//     if(!ps->port_state->valve_action_flag){ ps->error_state->op_state = _error_op_stop_close_start_open; }
//   }

//   // stop closing and start opening
//   else if(ps->error_state->op_state == _error_op_stop_close_start_open)
//   {
//     switch(ps->page_state->page)
//     {
//       // pump off (mammoth pump)
//       case AutoPumpOff: if(!ps->settings->settings_pump_off->pump){ OUT_Valve_Action(ps, OPEN_ClearWater); } break;

//       // mud
//       case AutoMud: OUT_Valve_Action(ps, OPEN_MudPump); break;

//       // air off
//       case AutoAirOff:
//       case AutoCircOff:
//         if((ps->inflow_pump_state->ip_state == _ip_on) && ps->settings->settings_inflow_pump->pump){ OUT_Valve_Action(ps, OPEN_Reserve); }
//         break;

//       // air on
//       case AutoCircOn: case AutoAirOn: case AutoZone: OUT_Valve_Action(ps, OPEN_Air); break;

//       default: break;
//     }

//     // open process
//     ps->error_state->op_state = _error_op_open_process;
//   }

//   // open process
//   else if(ps->error_state->op_state == _error_op_open_process)
//   {
//     if(!ps->port_state->valve_action_flag)
//     { 
//       ps->error_state->op_state = _error_op_open_stop; 
//     }
//   }

//   // stop opening, set compressor on
//   else if(ps->error_state->op_state == _error_op_open_stop)
//   {
//     // error treated, and cycle again if needed
//     ps->error_state->op_state = _error_op_close_start;
//     return 1;
//   }

//   // any other state
//   else
//   {
//     ps->error_state->op_state = _error_op_close_start;
//   }
//   return 0;
// }


/* ------------------------------------------------------------------*
 *            Error Action - UnderPressure Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_UP_Air(struct PlantState *ps)
{
  Error_Action_UP_SetError(ps);
  OUT_Set_Compressor(ps);
  return 1;
}


/* ------------------------------------------------------------------*
 *            error modem
 * ------------------------------------------------------------------*/

void Error_ModemAction(struct PlantState *ps, unsigned char error)
{
  // SMS message
  char msg[20] = "Error: ";

  // error appendix
  switch(error)
  {
    case E_T: strcat(msg, "Temperature"); break;
    case E_OP: strcat(msg, "Over-Pressure"); break;
    case E_UP: strcat(msg, "Under-Pressure"); break;
    case E_IT: strcat(msg, "Inner Tank"); break;
    case E_OT: strcat(msg, "Outer Tank"); break;
    default: break;
  }

  // Alert Modem
  Modem_Alert(ps, msg);
}