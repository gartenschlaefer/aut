// --
// error detection and treatment

#include <string.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "output_app.h"
#include "memory_app.h"

#include "mcp9800_driver.h"
#include "mpx_driver.h"
#include "lcd_sym.h"

#include "tc_func.h"
#include "error_func.h"
#include "port_func.h"
#include "modem_driver.h"


/* ==================================================================*
 *            Error ON/OFF
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  PORT_ErrorOn
 * --------------------------------------------------------------
 *  Sets Error Signals
 * ------------------------------------------------------------------*/

void Error_ON(void)
{
  PORT_Buzzer(_error);
  LCD_Backlight(_error);
  PORT_RelaisSet(R_ALARM);
}

void Error_OFF(void)
{
  PORT_Buzzer(_off);
  LCD_Backlight(_on);
  PORT_RelaisClr(R_ALARM);
}


/* ---------------------------------------------------------------*
 *          Read Error
 * ---------------------------------------------------------------*/

unsigned char Error_Read(t_page page)
{
  unsigned char err = 0;

  // temp
  if(MCP9800_PlusTemp() > MEM_EEPROM_ReadVar(ALARM_temp))
    err |= E_T;

  // over-pressure
  if(MPX_ReadCal() > ((MEM_EEPROM_ReadVar(MAX_H_druck) << 8) | (MEM_EEPROM_ReadVar(MAX_L_druck))))
    err |= E_OP;

  // under-pressure check if necessary
  unsigned char check_up_err = 0;
  switch(page)
  {
    // air off
    case AutoCircOff:
    case AutoAirOff:
      // inflow pump
      if(LCD_Auto_InflowPump(page, 0, _state) == _on)
      {
        // mammut pump
        if(!(MEM_EEPROM_ReadVar(PUMP_inflowPump)))
          check_up_err = 1;
      }
      break;

    // pump off
    case AutoPumpOff:
      // mammut pump
      if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff)))
        check_up_err = 1;
      break;

    // air on
    case AutoZone:
    case AutoMud:
    case AutoCirc:
    case AutoAir:
      check_up_err = 1;
      break;

    default:
      break;
  }

  // under-pressure
  if(check_up_err && MPX_ReadCal() < ((MEM_EEPROM_ReadVar(MIN_H_druck)<<8) | (MEM_EEPROM_ReadVar(MIN_L_druck))))
    err |= E_UP;

  // max in Tank
  if(MPX_ReadTank(AutoAir, _error) == ErrorMPX)
    err |= E_IT;

  return err;
}


/* ==================================================================*
 *            Error Detection
 * ==================================================================*/

t_page Error_Detection(t_page page, int min, int sec)
{
  static unsigned char occ = 0;
  static unsigned char rev = 0;
  static t_page treatPage;
  unsigned char err = 0;
  ErrTreat treat;

  // check if errors occur
  err = Error_Read(page);

  // run the buzzer
  PORT_Buzzer(_exe);

  // error timer on
  if(err && !occ)
  {
    if(TCE0_ErrorTimer(_ton))
    {
      occ = 1;
      treatPage = page;
      TCE0_ErrorTimer(_reset);
    }
  }

  // error treatment
  if(occ)
  {
    rev = 1;
    treat = Error_Treatment(treatPage, err);
    if(treat.err_treated){
      page = treat.page;
      occ = 0;
    }
    else
      page = ErrorTreat;
  }

  // error reset
  if(!err && rev && !occ)
  {
    rev = 0;
    Error_OFF();
    TCE0_ErrorTimer(_reset);
    LCD_AutoSet_Symbol(page, min, sec);
  }
  return page;
}



/* ==================================================================*
 *            Error Treatment
 * ==================================================================*/

ErrTreat Error_Treatment(t_page page, unsigned char error)
{
  ErrTreat treat;
  treat.page = page;
  treat.err_treated = 0;
  static unsigned char err = 0;

  if(error) err = error;

  // temp
  if(err & E_T)
  {
    Error_Action_Temp_SetError();
    treat.page = AutoSetDown;
    treat.err_treated = 1;
    err &= ~E_T;
  }

  // over-pressure
  if(err & E_OP)
  {
    if(Error_Action_OP_Air(page)){
      treat.err_treated = 1;
      err &= ~E_OP;
    }
  }

  // under-pressure
  if(err & E_UP)
  {
    if(Error_Action_UP_Air(page)){
      treat.err_treated = 1;
      err &= ~E_OP;
    }
  }

  // max in tank
  if(err & E_IT)
  {
    Error_Action_IT_SetError();
    if((page == AutoAir) || (page == AutoAirOff) || (page == AutoCirc) || (page == AutoCircOff))
      treat.page = AutoSetDown;
    treat.err_treated = 1;
    err &= ~E_IT;
  }

  //--------------------------------------------------MaxOUTTank
  if(err & E_OT)
  {
    Error_Action_OT_SetError();
    treat.err_treated = 1;
    err &= ~E_OT;
  }

  return treat;
}


/* ------------------------------------------------------------------*
 *            Error Action OverPressure - Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_OP_Air(t_page page)
{
  static unsigned char s_op = 0;

  // close
  if(!s_op)
  {
    Error_Action_OP_SetError();
    switch(page)
    {
      // set down
      case AutoSetDown:
        OUT_Clr_Compressor();
        P_VENTIL.OUTSET = O_AIR;
        PORT_Ventil(SET_STATE_OPEN, V_AIR);
        break;

      // pump off
      case AutoPumpOff:
        // mammut pump
        if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
        {
          P_VENTIL.OUTSET = C_CLRW;
          PORT_Ventil(SET_STATE_CLOSE, V_CLW);
        }
        break;

      case AutoMud:
        P_VENTIL.OUTSET = C_MUD;
        PORT_Ventil(SET_STATE_CLOSE, V_MUD);
        break;

      case AutoAirOff:
      case AutoCircOff:
        if((LCD_Auto_InflowPump(page, 0, _state) == _on) && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
        {
          P_VENTIL.OUTSET = C_RES;
          PORT_Ventil(SET_STATE_CLOSE, V_RES);
        }
        else
        {
          OUT_Clr_Compressor();
          P_VENTIL.OUTSET = O_AIR;
          PORT_Ventil(SET_STATE_OPEN, V_AIR);
        }
        break;

      case AutoCirc:
      case AutoAir:
      case AutoZone:
        P_VENTIL.OUTSET = C_AIR;
        PORT_Ventil(SET_STATE_CLOSE, V_AIR);
        break;

      default:
        break;
    }
    s_op = 1;
  }

  // stop closing and open
  else if(s_op == 1)
  {
    if(TCE0_ErrorTimer(_cvent))
    {
      switch(page)
      {
        // set down
        case AutoSetDown:
          P_VENTIL.OUTCLR= O_AIR;
          P_VENTIL.OUTSET= C_AIR;
          PORT_Ventil(SET_STATE_CLOSE, V_AIR);
          break;

        case AutoPumpOff:
          // mamut pump
          if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
          {
            P_VENTIL.OUTCLR= C_CLRW;
            P_VENTIL.OUTSET= O_CLRW;
            PORT_Ventil(SET_STATE_OPEN, V_CLW);
          }
          break;

        case AutoMud:
          P_VENTIL.OUTCLR= C_MUD;
          P_VENTIL.OUTSET= O_MUD;
          PORT_Ventil(SET_STATE_OPEN, V_MUD);
          break;

        case AutoAirOff:
        case AutoCircOff:
          if((LCD_Auto_InflowPump(page, 0, _state) == _on) && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
          {
            P_VENTIL.OUTCLR= C_RES;
            P_VENTIL.OUTSET= O_RES;
            PORT_Ventil(SET_STATE_OPEN, V_RES);
          }
          else
          {
            P_VENTIL.OUTCLR = O_AIR;
            P_VENTIL.OUTSET = C_AIR;
            PORT_Ventil(SET_STATE_CLOSE, V_AIR);
          }
          break;

        case AutoCirc:
        case AutoAir:
        case AutoZone:
          P_VENTIL.OUTCLR = C_AIR;
          P_VENTIL.OUTSET = O_AIR;
          PORT_Ventil(SET_STATE_OPEN, V_AIR);
          break;

        default:
          break;
      }
      s_op = 2;
    }
  }

  // stop opening
  else if(s_op == 2)
  {
    if(TCE0_ErrorTimer(_cvent))
    {
      switch(page)
      {
        // set down
        case AutoSetDown:
          P_VENTIL.OUTCLR = C_AIR;
          break;

        case AutoPumpOff:
          // mammut pump
          if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))
            P_VENTIL.OUTCLR = O_CLRW;
          break;

        case AutoMud:
          P_VENTIL.OUTCLR = O_MUD;
          break;

        case AutoAirOff:
        case AutoCircOff:
          if((LCD_Auto_InflowPump(page, 0, _state) == _on) && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
            P_VENTIL.OUTCLR = O_RES;
          else
            P_VENTIL.OUTCLR = C_AIR;
          break;

        case AutoCirc:
        case AutoAir:
        case AutoZone:
          P_VENTIL.OUTCLR = O_AIR;
          break;

        default:
          break;
      }

      s_op = 0;
      return 1;
    }
  }
  return 0;
}


/* ------------------------------------------------------------------*
 *            Error Action - UnderPressure Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_UP_Air(t_page page)
{
  switch(page)
  {
    case AutoAirOff:
    case AutoCircOff:
      if((LCD_Auto_InflowPump(page, 0, _state) ==_on) && (!MEM_EEPROM_ReadVar(PUMP_inflowPump)))
      {
        Error_Action_UP_SetError();
        PORT_RelaisSet(R_COMP);
        PORT_RelaisSet(R_EXT_COMP);
        return 1;
      }
      break;

    case AutoZone:
    case AutoPumpOff:
    case AutoCirc:
    case AutoAir:
    case AutoMud:
      Error_Action_UP_SetError();
      PORT_RelaisSet(R_COMP);
      PORT_RelaisSet(R_EXT_COMP);
      return 1;

    default:
      return 1;
  }
  return 0;
}



/* ==================================================================*
 *            Error Set
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Error Set Temperature Error
 * ------------------------------------------------------------------*/

void Error_Action_Temp_SetError(void)
{
  static unsigned char c = 0;

  Error_Symbol(E_T);
  c++;
  if(c == 2)
  {
    Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 1, Write_Error);
    Error_ModemAction(E_T);
  }
  if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 *            Error Set OverPressure
 * ------------------------------------------------------------------*/

void Error_Action_OP_SetError(void)
{
  static unsigned char c = 0;

  Error_Symbol(E_OP);
  c++;
  if(c == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_comp))  Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 2, Write_Error);
    Error_ModemAction(E_OP);
  }
  if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 *            Error Set UnderPressure
 * ------------------------------------------------------------------*/

void Error_Action_UP_SetError(void)
{
  static unsigned char c = 0;

  Error_Symbol(E_UP);
  c++;
  if(c == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_comp))  Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 4, Write_Error);
    Error_ModemAction(E_UP);
  }
  if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 *            Error Set IT - in tank error
 * ------------------------------------------------------------------*/

void Error_Action_IT_SetError(void)
{
  static unsigned char c = 0;

  Error_Symbol(E_IT);
  c++;
  if(c == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_sensor)) Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 8, Write_Error);
    Error_ModemAction(E_IT);
  }
  if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 *            Error Set OT - max out tank error
 * ------------------------------------------------------------------*/

void Error_Action_OT_SetError(void)
{
  static unsigned char c = 0;

  Error_Symbol(E_OT);
  c++;
  if(c == 2)
  {
    if(MEM_EEPROM_ReadVar(ALARM_sensor)) Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 16, Write_Error);
    Error_ModemAction(E_OT);
  }
  if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 *            Error Modem
 * ------------------------------------------------------------------*/

void Error_ModemAction(unsigned char error)
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
  Modem_Alert(msg);
}


/* ==================================================================*
 *            Error Symbols
 * ==================================================================*/

void Error_Symbol(unsigned char err)
{
  // temp
  if(err & E_T)
  {
    LCD_Write_Symbol_3(16, 134, n_grad);
  }

  // over-pressure or under-pressure
  if((err & E_OP) || (err & E_UP))
  {
    LCD_ClrSpace(6, 44, 6, 35);
    LCD_Write_Symbol_2(6, 45, n_alarm);
  }

  // max in tank
  if(err & E_IT)
  {
    LCD_Write_Symbol_2(17, 1, n_alarm);
    LCD_TextButton(Auto, 0);
  }

  // max out tank
  if(err & E_OT)
  {
    LCD_Write_Symbol_2(17, 90, n_alarm);
    LCD_TextButton(Setup, 0);
  }
}
