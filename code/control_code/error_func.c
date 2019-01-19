/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    error_func.c
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Error Detection and Treatment Source-File
* ------------------------------------------------------------------
*	Date:			    24.10.2011
* lastChanges:	24.02.2015
\**********************************************************************/

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
 * 						Error ON/OFF
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	PORT_ErrorOn
 * --------------------------------------------------------------
 *	Sets Error Signals
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
 * 					Read Error
 * ---------------------------------------------------------------*/

unsigned char Error_Read(unsigned char err)
{
	unsigned char out = 0;
	int lim = 0;

	if(err & E_T)
	{
		lim = MEM_EEPROM_ReadVar(ALARM_temp);
		if(MCP9800_PlusTemp() > lim)  out |= E_T;			//Temp
		else							            out &= ~E_T;
	}

	if(err & E_OP)
	{
		lim = ((MEM_EEPROM_ReadVar(MAX_H_druck)<<8) |
      (MEM_EEPROM_ReadVar(MAX_L_druck)));
		if(MPX_ReadCal() > lim) out |= E_OP;		//over-pressure
		else							      out &= ~E_OP;
	}

	if(err & E_UP)
	{
		lim = ((MEM_EEPROM_ReadVar(MIN_H_druck)<<8) |
      (MEM_EEPROM_ReadVar(MIN_L_druck)));
		if(MPX_ReadCal() < lim) out |= E_UP;		//under-pressure
		else							      out &= ~E_UP;
	}

	if(err & E_IT)
	{
		lim= MPX_ReadTank(AutoAir, _error);
		if(lim == ErrorMPX) out |= E_IT;		    //max in Tank
		else							  out &= ~E_IT;
	}

	return out;
}


/* ==================================================================*
 * 						Error Detection
 * ==================================================================*/

t_page Error_Detection(t_page page, int min, int sec)
{
	static unsigned char  occ = 0;
	static unsigned char  rev = 0;
	static t_page         treatPage;
	unsigned char 			  err = 0;
  ErrTreat              treat;

  //--------------------------------------------------low-pressure
	switch(page)
	{
		case AutoCircOff:  case AutoAirOff:
      if(LCD_Auto_InflowPump(page, 0, _state) == _on)     //IfInflowPump
      {
        if(!(MEM_EEPROM_ReadVar(PUMP_inflowPump)))  //ifMammutpumpe
          err|= Error_Read(E_UP);
      }
    break;

    case AutoPumpOff:
      if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff)))   //IfMammutpumpe
        err|= Error_Read(E_UP);

		case AutoZone:    case AutoMud:
		case AutoCirc:    case AutoAir:
		  err|= Error_Read(E_UP); break;

		default:				break;
	}

	//--------------------------------------------------CheckAlways
	err |= Error_Read(E_T | E_OP | E_IT);		//Err2Check
  PORT_Buzzer(_exe);						          //BuzzerExe

  //--------------------------------------------------ErrorTimerOn
  if(err && !occ)
  {
    if(TCE0_ErrorTimer(_ton))				  	  //--Error Ocur
    {
      occ = 1;
      treatPage = page;
      TCE0_ErrorTimer(_reset);					  //ResetTimer
    }
  }

  //--------------------------------------------------ErrorTreatment
  if(occ)
  {
    rev = 1;                                    //RevertFlag
    treat = Error_Treatment(treatPage, err);	  //Error Treatment
    if(treat.err_treated){
      page = treat.page;
      occ = 0;}
    else page = ErrorTreat;
  }

  //--------------------------------------------------ErrorReset
	if(!err && rev && !occ)								  //--Error Clean
	{
		rev = 0;
		Error_OFF();							            //Error-Clean
		TCE0_ErrorTimer(_reset);              //ResetTimer
		LCD_AutoSet_Symbol(page, min, sec);		//Clear Display
	}
	return page;
}



/* ==================================================================*
 * 						Error Treatment
 * ==================================================================*/

ErrTreat Error_Treatment(t_page page, unsigned char error)
{
  ErrTreat treat;
  treat.page = page;
  treat.err_treated = 0;
  static unsigned char err = 0;

  if(error) err = error;
  //--------------------------------------------------Temp
	if(err & E_T)
	{
	  Error_Symbol(E_T);                              //ErrorSymbol
    Error_Action_Temp_SetError();
    treat.page = AutoSetDown;
    treat.err_treated = 1;
    err &= ~E_T;
	}

	//--------------------------------------------------Overpressure
	if(err & E_OP)
	{
    if(page == AutoSetDown){
	    if(Error_Action_OP_SetDown()) treat.err_treated = 1;}
	  else{
	    if(Error_Action_OP_Air(page)) treat.err_treated = 1;}
    if(treat.err_treated) err &= ~E_OP;
  }

	//--------------------------------------------------Underpressure
	if(err & E_UP)
	{
    if(Error_Action_UP_Air(page)){
      treat.err_treated = 1;
      err &= ~E_OP;}
	}

	//--------------------------------------------------MaxInTank
	if(err & E_IT)
	{
    Error_Symbol(E_IT);                              //ErrorSymbol
    Error_Action_IT_SetError();
    if((page == AutoAir) || (page == AutoAirOff) ||
    (page == AutoCirc) || (page == AutoCircOff))
      treat.page = AutoSetDown;
    treat.err_treated = 1;
    err &= ~E_IT;
	}

	//--------------------------------------------------MaxOUTTank
	if(err & E_OT)
	{
    Error_Symbol(E_OT);
    Error_Action_OT_SetError();
    treat.err_treated = 1;
    err &= ~E_OT;
	}

	return treat;
}


/* ------------------------------------------------------------------*
 * 						Error Action OverPressure - SetDown
 * ------------------------------------------------------------------*/

unsigned char Error_Action_OP_SetDown(void)
{
  static unsigned char s_op = 0;

  //--------------------------------------------------open
  if(!s_op)
  {
    Error_Symbol(E_OP);          //Symbol
    Error_Action_OP_SetError();	 //AlarmAction
    OUT_Clr_Compressor();
    P_VENTIL.OUTSET = O_AIR;      //OpenAir
    s_op = 1;
  }
  //--------------------------------------------------stopOpening
  else if(s_op == 1)
  {
    if(TCE0_ErrorTimer(_ovent))
    {
      P_VENTIL.OUTCLR= O_AIR;       //StopOpening
      P_VENTIL.OUTSET= C_AIR;       //CloseAir
      s_op = 2;
    }
  }
  //--------------------------------------------------stclosing
  else if(s_op == 2)
  {
    if(TCE0_ErrorTimer(_cvent))
    {
      P_VENTIL.OUTCLR= C_AIR;       //Stclosing
      s_op = 0;
      return 1;
    }
  }
  return 0;
}


/* ------------------------------------------------------------------*
 * 						Error Action OverPressure - Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_OP_Air(t_page page)
{
  static unsigned char s_op = 0;

  //--------------------------------------------------close
  if(!s_op)
  {
    Error_Symbol(E_OP);           //Symbol
    Error_Action_OP_SetError();		//AlarmAction
    switch(page)
	  {
      case AutoPumpOff:
        if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))	  //Mammutpumpe?
          P_VENTIL.OUTSET = C_CLRW;              break;

      case AutoMud: P_VENTIL.OUTSET = C_MUD;     break;

      case AutoAirOff:
		  case AutoCircOff:
        if((LCD_Auto_InflowPump(page, 0, _state) == _on)
        && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
          P_VENTIL.OUTSET = C_RES;
        else{
          OUT_Clr_Compressor();
          P_VENTIL.OUTSET = O_AIR;}           break;

      case AutoCirc:
      case AutoAir:
		  case AutoZone:  P_VENTIL.OUTSET = C_AIR;  break;
      default:                                  break;
	  }
    s_op= 1;
  }
  //--------------------------------------------------stopClosing-open
  else if(s_op == 1)
  {
    if(TCE0_ErrorTimer(_cvent))
    {
      switch(page)
      {
        case AutoPumpOff:
          if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))	  //Mammutpumpe?
          {
            P_VENTIL.OUTCLR= C_CLRW;
            P_VENTIL.OUTSET= O_CLRW;
          } break;

        case AutoMud: P_VENTIL.OUTCLR= C_MUD;
                      P_VENTIL.OUTSET= O_MUD;     break;

        case AutoAirOff:
        case AutoCircOff:
          if((LCD_Auto_InflowPump(page, 0, _state) == _on)
          && !MEM_EEPROM_ReadVar(PUMP_inflowPump)){
            P_VENTIL.OUTCLR= C_RES;
            P_VENTIL.OUTSET= O_RES;}
          else{
            P_VENTIL.OUTCLR = O_AIR;
            P_VENTIL.OUTSET = C_AIR;}
          break;

        case AutoCirc:
        case AutoAir:
        case AutoZone:  P_VENTIL.OUTCLR = C_AIR;
                        P_VENTIL.OUTSET = O_AIR;  break;
        default:                                  break;
      }
      s_op= 2;
    }
  }

  //--------------------------------------------------stopOpening
  else if(s_op == 2)
  {
    if(TCE0_ErrorTimer(_cvent))
    {
      switch(page)
      {
        case AutoPumpOff:
          if(!MEM_EEPROM_ReadVar(PUMP_pumpOff))	  //Mammutpumpe?
            P_VENTIL.OUTCLR = O_CLRW;             break;

        case AutoMud: P_VENTIL.OUTCLR = O_MUD;    break;

        case AutoAirOff:
        case AutoCircOff:
          if((LCD_Auto_InflowPump(page, 0, _state) == _on)
          && !MEM_EEPROM_ReadVar(PUMP_inflowPump))
            P_VENTIL.OUTCLR = O_RES;
          else P_VENTIL.OUTCLR = C_AIR;           break;

        case AutoCirc:
        case AutoAir:
        case AutoZone:  P_VENTIL.OUTCLR = O_AIR;  break;
        default:                                  break;
      }
      s_op= 0;
      return 1;
    }
  }
  return 0;
}


/* ------------------------------------------------------------------*
 * 						Error Action - UnderPressure Air
 * ------------------------------------------------------------------*/

unsigned char Error_Action_UP_Air(t_page page)
{
  switch(page)
  {
    case AutoAirOff:
    case AutoCircOff:
      if((LCD_Auto_InflowPump(page, 0, _state) ==_on) &&
      (!MEM_EEPROM_ReadVar(PUMP_inflowPump)))
      {
        Error_Symbol(E_UP);
        Error_Action_UP_SetError();
        PORT_RelaisSet(R_COMP);
        PORT_RelaisSet(R_EXT_COMP);		//Compressor ON
        return 1;
      }                            break;

    case AutoZone:
    case AutoPumpOff:
    case AutoCirc:
    case AutoAir:
    case AutoMud:
      Error_Symbol(E_UP);
      Error_Action_UP_SetError();
      PORT_RelaisSet(R_COMP);
      PORT_RelaisSet(R_EXT_COMP);		//Compressor ON
      return 1;                    break;

    default:  return 1;                     break;
  }
  return 0;
}



/* ==================================================================*
 * 						Error Set
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Error Set Temperature Error
 * ------------------------------------------------------------------*/

void Error_Action_Temp_SetError(void)
{
	static unsigned char c = 0;

	c++;
	if(c == 2)
	{
    Error_ON();                                     //ErrorOn
    MEM_EEPROM_WriteAutoEntry(0, 1, Write_Error);	  //Write Error
    Modem_CallAllNumbers();
	}
	if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 * 						Error Set OverPressure
 * ------------------------------------------------------------------*/

void Error_Action_OP_SetError(void)
{
	static unsigned char c = 0;
	c++;
	if(c == 2)
	{
		if(MEM_EEPROM_ReadVar(ALARM_comp))	Error_ON();	//ErrorSignal-ON
		MEM_EEPROM_WriteAutoEntry(0, 2, Write_Error);	  //Write Error
    Modem_CallAllNumbers();
	}
	if(c > 250) c = 0;
}


/* ------------------------------------------------------------------*
 * 						Error Set UnderPressure
 * ------------------------------------------------------------------*/

void Error_Action_UP_SetError(void)
{
	static unsigned char c = 0;
	c++;
	if(c == 2)
	{
		if(MEM_EEPROM_ReadVar(ALARM_comp))	Error_ON(); //ErrorSignal-ON
		MEM_EEPROM_WriteAutoEntry(0, 4, Write_Error);	  //Write Error
    Modem_CallAllNumbers();
	}
	if(c > 250) c = 0;                             //NextError
}


/* ------------------------------------------------------------------*
 * 						Error Set IT
 * ------------------------------------------------------------------*/

void Error_Action_IT_SetError(void)
{
	static unsigned char c = 0;
	c++;
	if(c == 2)
	{
    if(MEM_EEPROM_ReadVar(ALARM_sensor)) Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 8, Write_Error);	    //WriteError
    Modem_CallAllNumbers();
	}
	if(c > 250) c = 0;                             //NextError
}

/* ------------------------------------------------------------------*
 * 						Error Set OT
 * ------------------------------------------------------------------*/

void Error_Action_OT_SetError(void)
{
	static unsigned char c = 0;
	c++;
	if(c == 2)
	{
    if(MEM_EEPROM_ReadVar(ALARM_sensor)) Error_ON();
    MEM_EEPROM_WriteAutoEntry(0, 16, Write_Error);	//WriteError
    Modem_CallAllNumbers();
	}
	if(c > 250) c= 0;                             //NextError
}



/* ==================================================================*
 * 						Error Symbols
 * ==================================================================*/

void Error_Symbol(unsigned char err)
{
  if(err & E_T)	                          //Temp
  {
    LCD_Write_Symbol_3(16, 134, n_grad);
  }

  if((err & E_OP) || (err & E_UP))        //OverPressure
  {
    LCD_ClrSpace(6, 44, 6, 35);
    LCD_Write_Symbol_2(6, 45, n_alarm);
  }

  if(err & E_IT)                          //Max in Tank
  {
    LCD_Write_Symbol_2(17, 1, n_alarm);
    LCD_TextButton(Auto, 0);
  }

  if(err & E_OT)                          //Max out Tank
  {
    LCD_Write_Symbol_2(17, 90, n_alarm);
    LCD_TextButton(Setup, 0);
  }
}





/**********************************************************************\
 * END of file
\**********************************************************************/
