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


/*-------------------------------------------------------------------*
 *  set error signals
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
  PORT_Backlight_On(ps->backlight);
  PORT_RelaisClr(R_ALARM);
}


/* ---------------------------------------------------------------*
 *          read error
 * ---------------------------------------------------------------*/

void Error_Read(struct PlantState *ps)
{
  // set pending to zero, todo: improve this
  ps->error_state->pending_err_code = 0;

  // temp
  if(MCP9800_PlusTemp(ps->twi_state) > MEM_EEPROM_ReadVar(ALARM_temp)){ ps->error_state->pending_err_code |= E_T; }

  // over-pressure
  if(MPX_ReadCal() > ((MEM_EEPROM_ReadVar(MAX_H_druck) << 8) | (MEM_EEPROM_ReadVar(MAX_L_druck)))){ ps->error_state->pending_err_code |= E_OP; }

  // under-pressure check if necessary
  unsigned char check_up_err = 0;
  switch(ps->page_state->page)
  {
    case AutoCircOff: case AutoAirOff:

      // inflow pump
      if(ps->inflow_pump_state->ip_state == _ip_on)
      {
        // mammoth pump
        if(!(MEM_EEPROM_ReadVar(PUMP_inflowPump))){ check_up_err = 1; }
      }
      break;

    case AutoPumpOff:

      // mammoth pump
      if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))){ check_up_err = 1; }
      break;

    case AutoZone: case AutoMud: case AutoCircOn: case AutoAirOn:
      check_up_err = 1;
      break;

    default: break;
  }

  // under-pressure
  if(check_up_err && (MPX_ReadCal() < ((MEM_EEPROM_ReadVar(MIN_H_druck) << 8) | (MEM_EEPROM_ReadVar(MIN_L_druck))))){ ps->error_state->pending_err_code |= E_UP; }

  // max in tank
  //if(MPX_ReadTank(AutoAirOn, _error) == ErrorMPX) ps->error_state->pending_err_code |= E_IT;
  //return err;
}


/* ---------------------------------------------------------------*
 *           Error Detection
 * ---------------------------------------------------------------*/

void Error_Detection(struct PlantState *ps)
{
  // check if errors occur
  //unsigned char err = Error_Read(ps);
  Error_Read(ps);

  // set treat page
  ps->error_state->page = ps->page_state->page;

  // run the buzzer
  PORT_Buzzer_Update(ps);

  // error timer on
  if(ps->error_state->pending_err_code && !ps->error_state->err_code)
  {
    // error timer: error must be present for some time
    if(TCE0_ErrorTimer(_ton))
    {
      // add error to error code
      ps->error_state->err_code |= ps->error_state->pending_err_code;
      TCE0_ErrorTimer(_reset);
    }
  }

  // error handling
  if(ps->error_state->err_code)
  {
    // error treatment
    Error_Treatment(ps);

    // error solved
    if(!ps->error_state->err_code)
    {
      // might change page according to error, todo: solve this better
      ps->page_state->page = ps->error_state->page;

      // reset error flag
      ps->error_state->err_reset_flag = 1;
    }

    // still unsolved, continue treatment
    else ps->page_state->page = ErrorTreat;
  }

  // error reset
  if(ps->error_state->err_reset_flag)
  {
    ps->error_state->err_reset_flag = 0;
    Error_Off(ps);
    TCE0_ErrorTimer(_reset);
    LCD_Sym_Auto_SetManager(ps);
  }
}


/* ---------------------------------------------------------------*
 *            Error Treatment
 * ---------------------------------------------------------------*/

void Error_Treatment(struct PlantState *ps)
{
  // temp
  if(ps->error_state->err_code & E_T)
  {
    Error_Action_Temp_SetError(ps);
    ps->error_state->page = AutoSetDown;

    // reset error
    ps->error_state->err_code &= ~E_T;
  }

  // over-pressure
  if(ps->error_state->err_code & E_OP)
  {
    // ps->error_state->ent
    if(Error_Action_OP_Air(ps)){ ps->error_state->err_code &= ~E_OP; }
  }

  // under-pressure
  if(ps->error_state->err_code & E_UP)
  {
    if(Error_Action_UP_Air(ps)){ ps->error_state->err_code &= ~E_UP; }
  }

  // max in tank
  if(ps->error_state->err_code & E_IT)
  {
    Error_Action_IT_SetError(ps);

    // go to set down -> pump off
    if((ps->error_state->page == AutoAirOn) || (ps->error_state->page == AutoAirOff) || (ps->error_state->page == AutoCircOn) || (ps->error_state->page == AutoCircOff)) ps->error_state->page = AutoSetDown;
    ps->error_state->err_code &= ~E_IT;
  }

  // max out of tank
  if(ps->error_state->err_code & E_OT)
  {
    Error_Action_OT_SetError(ps);
    ps->error_state->err_code &= ~E_OT;
  }
}


/* ------------------------------------------------------------------*
 *            Error Action OverPressure - Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_OP_Air(struct PlantState *ps)
{
  // start closing valves
  if(!ps->error_state->op_state)
  {
    // stop compressor and set errors
    OUT_Clr_Compressor();
    Error_Action_OP_SetError(ps);

    // page cases
    switch(ps->page_state->page)
    {
      // pump off: mammoth pump
      case AutoPumpOff: 
        if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
        { 
          P_VALVE.OUTSET = C_CLRW; 
          ps->port_state->valve_state &= ~V_CLW; 
        }
        break;

      // mud
      case AutoMud: 
        P_VALVE.OUTSET = C_MUD; 
        ps->port_state->valve_state &= ~V_MUD; 
        break;

      // air off
      case AutoAirOff:
      case AutoCircOff:
        if((ps->inflow_pump_state->ip_state == _ip_on) && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
        {
          P_VALVE.OUTSET = C_RES;
          ps->port_state->valve_state &= ~V_RES;
        }
        break;

      // air on
      case AutoCircOn:
      case AutoAirOn:
      case AutoZone: 
        P_VALVE.OUTSET = C_AIR; 
        ps->port_state->valve_state &= ~V_AIR; 
        break;

      default: break;
    }
    ps->error_state->op_state = 1;
  }

  // stop closing and start opening
  else if(ps->error_state->op_state == 1)
  {
    if(TCE0_ErrorTimer(_cvent))
    {
      switch(ps->page_state->page)
      {
        // pump off (mammoth pump)
        case AutoPumpOff: 
          if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
          { 
            P_VALVE.OUTCLR = C_CLRW; P_VALVE.OUTSET = O_CLRW; 
            ps->port_state->valve_state |= V_CLW;
          }
          break;

        // mud
        case AutoMud: 
          P_VALVE.OUTCLR = C_MUD; 
          P_VALVE.OUTSET = O_MUD; 
          ps->port_state->valve_state |= V_MUD;
          break;

        // air off
        case AutoAirOff:
        case AutoCircOff:
          if((ps->inflow_pump_state->ip_state == _ip_on) && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
          {
            P_VALVE.OUTCLR = C_RES;
            P_VALVE.OUTSET = O_RES;
            ps->port_state->valve_state |= V_RES;
          }
          break;

        // air on
        case AutoCircOn:
        case AutoAirOn:
        case AutoZone: 
          P_VALVE.OUTCLR = C_AIR; 
          P_VALVE.OUTSET = O_AIR; 
          ps->port_state->valve_state |= V_AIR;
          break;

        default: break;
      }
      ps->error_state->op_state = 2;
    }
  }

  // stop opening, set compressor on
  else if(ps->error_state->op_state == 2)
  {
    if(TCE0_ErrorTimer(_ovent))
    {
      switch(ps->page_state->page)
      {
        // pump off: mammoth pump
        case AutoPumpOff: 
          if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
          { 
            P_VALVE.OUTCLR = O_CLRW; 
            OUT_Set_Compressor(); 
          }
          break;

        // mud
        case AutoMud: 
          P_VALVE.OUTCLR = O_MUD; 
          OUT_Set_Compressor(); 
          break;

        // no air
        case AutoAirOff:
        case AutoCircOff:
          if((ps->inflow_pump_state->ip_state == _ip_on) && !MEM_EEPROM_ReadVar(PUMP_inflowPump)) 
          {
            P_VALVE.OUTCLR = O_RES;
            OUT_Set_Compressor();
          }
          break;

        // air
        case AutoCircOn:
        case AutoAirOn:
        case AutoZone: 
          P_VALVE.OUTCLR = O_AIR; 
          OUT_Set_Compressor(); 
          break;

        default: break;
      }

      // error treated
      ps->error_state->op_state = 0;
      return 1;
    }
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            Error Action - UnderPressure Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_UP_Air(struct PlantState *ps)
{
  switch(ps->page_state->page)
  {
    case AutoAirOff:
    case AutoCircOff:
      if((ps->inflow_pump_state->ip_state == _ip_on) && (!MEM_EEPROM_ReadVar(PUMP_inflowPump)))
      {
        Error_Action_UP_SetError(ps);
        OUT_Set_Compressor();
        return 1;
      }
      break;

    case AutoZone:
    case AutoPumpOff:
    case AutoCircOn:
    case AutoAirOn:
    case AutoMud: Error_Action_UP_SetError(ps); OUT_Set_Compressor(); return 1;
    default: return 1;
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            Error Set Temperature Error
 * ------------------------------------------------------------------*/

void Error_Action_Temp_SetError(struct PlantState *ps)
{
  LCD_Sym_Error(E_T);
  ps->error_state->error_counter[ERROR_ID_T]++;
  if(ps->error_state->error_counter[ERROR_ID_T] == 2)
  {
    Error_On(ps);
    ps->error_state->cycle_error_code_record |= E_T;
    Error_ModemAction(ps, E_T);
  }
  if(ps->error_state->error_counter[ERROR_ID_T] > 250){ ps->error_state->error_counter[ERROR_ID_T] = 0; }
}


/* ------------------------------------------------------------------*
 *            Error Set OverPressure
 * ------------------------------------------------------------------*/

void Error_Action_OP_SetError(struct PlantState *ps)
{
  LCD_Sym_Error(E_OP);
  ps->error_state->error_counter[ERROR_ID_OP]++;
  if(ps->error_state->error_counter[ERROR_ID_OP] == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_comp)){ Error_On(ps); }
    ps->error_state->cycle_error_code_record |= E_OP;
    Error_ModemAction(ps, E_OP);
  }
  if(ps->error_state->error_counter[ERROR_ID_OP] > 250){ ps->error_state->error_counter[ERROR_ID_OP] = 0; }
}


/* ------------------------------------------------------------------*
 *            Error Set UnderPressure
 * ------------------------------------------------------------------*/

void Error_Action_UP_SetError(struct PlantState *ps)
{
  LCD_Sym_Error(E_UP);
  ps->error_state->error_counter[ERROR_ID_UP]++;
  if(ps->error_state->error_counter[ERROR_ID_UP] == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_comp)){ Error_On(ps); }
    ps->error_state->cycle_error_code_record |= E_UP;
    Error_ModemAction(ps, E_UP);
  }
  if(ps->error_state->error_counter[ERROR_ID_UP] > 250){ ps->error_state->error_counter[ERROR_ID_UP] = 0; }
}


/* ------------------------------------------------------------------*
 *            Error Set IT - in tank error
 * ------------------------------------------------------------------*/

void Error_Action_IT_SetError(struct PlantState *ps)
{
  LCD_Sym_Error(E_IT);
  ps->error_state->error_counter[ERROR_ID_IT]++;
  if(ps->error_state->error_counter[ERROR_ID_IT] == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_sensor)){ Error_On(ps); }
    ps->error_state->cycle_error_code_record |= E_IT;
    Error_ModemAction(ps, E_IT);
  }
  if(ps->error_state->error_counter[ERROR_ID_IT] > 250){ ps->error_state->error_counter[ERROR_ID_IT] = 0; }
}


/* ------------------------------------------------------------------*
 *            Error Set OT - max out tank error
 * ------------------------------------------------------------------*/

void Error_Action_OT_SetError(struct PlantState *ps)
{
  LCD_Sym_Error(E_OT);
  ps->error_state->error_counter[ERROR_ID_OT]++;
  if(ps->error_state->error_counter[ERROR_ID_OT] == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_sensor)){ Error_On(ps); }
    ps->error_state->cycle_error_code_record |= E_OT;
    Error_ModemAction(ps, E_OT);
  }
  if(ps->error_state->error_counter[ERROR_ID_OT] > 250){ ps->error_state->error_counter[ERROR_ID_OT] = 0; }
}


/* ------------------------------------------------------------------*
 *            Error Modem
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