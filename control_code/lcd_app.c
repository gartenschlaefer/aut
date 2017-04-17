/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Display-App-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  AVR Studio mit avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Application-File for the EADOGXL160-7 Display
* ------------------------------------------------------------------
*	Date:			    13.07.2011
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "output_app.h"
#include "touch_app.h"
#include "eval_app.h"
#include "memory_app.h"

#include "touch_driver.h"
#include "mpx_driver.h"
#include "mcp9800_driver.h"
#include "mcp7941_driver.h"

#include "error_func.h"
#include "tc_func.h"
#include "basic_func.h"
#include "port_func.h"

#include "lcd_sym.h"
#include "sonic_app.h"


/* ==================================================================*
 * 						Auto Pages
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	LCD_AutoPage
 * --------------------------------------------------------------
 *	Loads Auto-Page in Display
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage(t_page page)
{
	static t_page	sPage = START_PAGE;
	static int min = 5;
	static int sec = 0;
	static int lcd_reset = 0;
	static unsigned char initVar = 0;
	int *p_min = &min;
	int *p_sec = &sec;

	switch(page)
	{
		case AutoPage:
      LCD_AutoSet_Page();
      page = sPage;

      if(!initVar ||
      (!min && !(MEM_EEPROM_ReadVar(SENSOR_inTank))))
      {
        initVar = 1;
        LCD_Write_AirVar(AutoCirc, 0, _init);
        LCD_Auto_InflowPump(page, 0, _init);
        LCD_Auto_Phosphor(0, _init);
        OUT_CloseOff();
        LCD_AutoSet(page, p_min, p_sec);
      }
      else
      {
        // SaveTime
        int sMin = *p_min;
        int sSec = *p_sec;
        LCD_AutoSet(page, p_min, p_sec);
        *p_min = sMin;
        *p_sec = sSec;

        // rewrite countdown
        if(page != AutoAir && page != AutoAirOff &&
          page != AutoCirc && page != AutoCircOff){
            LCD_AutoCountDown(*p_min, *p_sec);}
      }
      break;

		case AutoZone:
      sPage = page;
      page = LCD_AutoPage_Zone(p_min, p_sec);		break;

		case AutoSetDown:
      sPage = page;
      page = LCD_AutoPage_SetDown(p_min, p_sec);	break;

		case AutoPumpOff:
      sPage = page;
      page = LCD_AutoPage_PumpOff(p_min, p_sec);	break;

		case AutoMud:
      sPage = page;
      page = LCD_AutoPage_Mud(p_min, p_sec);		  break;

		case AutoCirc:
		case AutoCircOff:
      sPage = page;
      page = LCD_AutoPage_Circ(page, p_min, p_sec);
      if(page == AutoCirc && sPage == AutoCircOff)      sPage = page;
      else if(page == AutoCircOff && sPage == AutoCirc) sPage = page;
      break;

		case AutoAir:
		case AutoAirOff:
      sPage = page;
      page = LCD_AutoPage_Air(page, p_min, p_sec);
      if(page == AutoAir && sPage == AutoAirOff)      sPage = page;
      else if(page == AutoAirOff && sPage == AutoAir) sPage = page;
      break;

		default:											break;
	}

  // change pages
	page = Touch_AutoLinker(Touch_Matrix(), page, p_min, p_sec);
	page = Sonic_ReadTank(page, _exe);
	if(page != sPage) LCD_AutoSet(page, p_min, p_sec);

	// execute in all auto pages
  Watchdog_Restart();
	LCD_Backlight(_exe);
  LCD_Auto_InflowPump(page, sec, _exe);
  LCD_Auto_Phosphor(sec, _exe);
	MPX_ReadAverage(Auto, _exe);

  //--------------------------------------------------lcd_reset
	if(lcd_reset == 10)										//30s default: 32000
	{
		LCD_Init();                          	//InitLCD
		TCC0_wait_ms(2);						          //Wait
	}
  else if(lcd_reset == 10000)
  {
    LCD_MarkTextButton(Auto);
  }
	else if(lcd_reset == 20000)
  {
    if(!COMPANY)   LCD_Write_Purator(0,0);
    else LCD_Write_HECS(0,0);
    MCP7941_LCD_WriteTime(_init);
    MCP7941_LCD_WriteDate();
  }
  else if(lcd_reset > 30000)
  {
    LCD_AutoSet_Symbol(page, min, sec);
    lcd_reset = 0;
  }
	lcd_reset++;

	return page;
}


/* ------------------------------------------------------------------*
 * 						Auto ZoneCalc
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_Zone(int *p_min, int *p_sec)
{
	t_page page = AutoZone;
	LCD_WriteAutoVar_Comp(*p_min, *p_sec);		//Variables
	Eval_Oxygen(_count, *p_min);				      //Count Oxygen Hours
	page = Error_Detection(page, *p_min, *p_sec);
	if((Eval_CountDown(p_min, p_sec)) && (page != ErrorTreat))
	{
		page = MPX_ReadTank(AutoZone, _exe);
		OUT_Clr_Air();
		return page;
  }
	return AutoZone;
}


/* ------------------------------------------------------------------*
 * 						Auto Set-Down
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_SetDown(int *p_min, int *p_sec)
{
	t_page page = AutoSetDown;
	LCD_WriteAutoVar(*p_min, *p_sec);							//Variables
  page = Error_Detection(page, *p_min, *p_sec);
	if((Eval_CountDown(p_min, p_sec)) && (page != ErrorTreat))
	  return AutoPumpOff;		//Next Auto Page
	return AutoSetDown;
}


/* ------------------------------------------------------------------*
 * 						Auto Pump-Off
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_PumpOff(int *p_min, int *p_sec)
{
	t_page page = AutoPumpOff;
	LCD_WriteAutoVar_Comp(*p_min, *p_sec);			//Variables
  page = Error_Detection(page, *p_min, *p_sec);
	if((Eval_CountDown(p_min, p_sec)) && (page != ErrorTreat))
	{
		OUT_Clr_PumpOff();
		return AutoMud;				//Next Auto Page
	}
	return AutoPumpOff;
}


/* ------------------------------------------------------------------*
 * 						Auto Mud
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_Mud(int *p_min, int *p_sec)
{
	t_page page = AutoMud;
	LCD_WriteAutoVar_Comp(*p_min, *p_sec);			//Write Variables
  page = Error_Detection(page, *p_min, *p_sec);
	if((Eval_CountDown(p_min, p_sec)) && (page != ErrorTreat))
	{
		OUT_Clr_Mud();
		//------------------------------------------------WriteEntry
		Eval_Oxygen(_entry, 0);				                //O2 Write2Entry
		MEM_EEPROM_WriteAutoEntry(0, 0, Write_Entry); //WriteAutoEntry
		Eval_Oxygen(_clear, 0);					              //02=0

		if(MEM_EEPROM_ReadVar(CAL_Redo_on) &&
		!(MEM_EEPROM_ReadVar(SONIC_on))){
      return AutoZone;}
		else return AutoCirc;					//Next Auto Page
	}
	return AutoMud;
}


/* ------------------------------------------------------------------*
 * 						Auto Circulate
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_Circ(t_page page, int *p_min, int *p_sec)
{
  int min = *p_min;
  int sec = *p_sec;

  t_page sPage = page;                        //SafeBack4ErrTreat
  page = Error_Detection(page, min, sec);     //ErrorDet
	page = LCD_Write_AirVar(page, sec,  _exe);  //WriteVar

	if(Eval_CountDown(p_min, p_sec) && (page != ErrorTreat) &&
	(!MEM_EEPROM_ReadVar(SENSOR_inTank) || LCD_Sym_NoUS(page, _check)))
	{
		LCD_Auto_InflowPump(page, 0, _reset);		  //ResetIP
		if(page == AutoCirc) OUT_Clr_Air();	      //ClearAir
		return AutoAir;									          //NextAutoPage
  }
  if(page == ErrorTreat) page = sPage;        //SafeBack4ErrTreat
	return page;
}


/* ------------------------------------------------------------------*
 * 						Auto Air
 * ------------------------------------------------------------------*/

t_page LCD_AutoPage_Air(t_page page, int *p_min, int *p_sec)
{
  int min = *p_min;
  int sec = *p_sec;

  page = Error_Detection(page, min, sec);   //ErrorDet
	page = LCD_Write_AirVar(page, sec, _exe);	//WriteVar

	if((Eval_CountDown(p_min, p_sec)) && (page != ErrorTreat))
	{
		LCD_Auto_InflowPump(page, 0, _reset);		//ResetIP
		if(page == AutoAir) OUT_Clr_Air();		  //ClearAir
		return AutoSetDown;							        //Next-AutoPage
  }
	return page;
}



/* ==================================================================*
 * 						Auto Built-in-Page
 * ==================================================================*/
/* ------------------------------------------------------------------*
 * 						Auto Air
 * ------------------------------------------------------------------*/

t_page LCD_Write_AirVar(t_page page, int sec, t_FuncCmd cmd)
{
	static int	cOld = 0;
	static int  airMin = 0;
	static int  airSec = 0;

	t_page cPage = AutoAir;

	//--------------------------------------------------------Init
	if(cmd == _init)
	{
		airMin = LCD_AutoRead_StartTime(page);	//Read On
		airSec = 0;
	}

	//--------------------------------------------------------Set
	else if(cmd == _set)
	{
		if(page == AutoAir || page == AutoCirc)
    {
      OUT_Set_Air();
    }
	}

  //--------------------------------------------------------Set
	else if(cmd == _reset)
	{
    if(page == AutoAir || page == AutoCirc)
    {
      OUT_Clr_Air();
    }
	}

	//--------------------------------------------------------WriteSym
	else if(cmd == _write)
	{
    LCD_AutoAirSym(page);               //Symbols
		LCD_AutoCountDown(airMin, airSec);  //CountDownVar
	}

  //--------------------------------------------------------exe
	else if(cmd == _exe)
	{
    if(!airMin && !airSec)			//--Change2Off
    {
      switch(page)
      {
        case AutoAir:
        case AutoCirc:
          cPage = page;
          page = MPX_ReadTank(page, _exe);			    //MPX
          MPX_ReadTank(page, _write);					      //MPX
          OUT_Clr_Air();						                //ClearAir
          if(cPage != page)
          {
            LCD_Auto_InflowPump(page, 0, _reset);	  //IP-Reset
            return page;
          }
          //SetPages
          if(page == AutoAir) page = AutoAirOff;
          else if(page == AutoCirc) page = AutoCircOff;
          LCD_AutoAirSym(page);
          LCD_Auto_InflowPump(page, 0, _set);				//IP-Set
          airMin = LCD_AutoRead_StartTime(page);	  //Read Off
          airSec = 0;
          break;

        case AutoAirOff:
        case AutoCircOff:
          LCD_Auto_InflowPump(page, 0, _reset);	      //IP-Reset
          OUT_Set_Air();						                  //SetAir
          //SetPages
          if(page == AutoAirOff)  page = AutoAir;
          else if(page == AutoCircOff)  page = AutoCirc;
          LCD_AutoAirSym(page);
          airMin = LCD_AutoRead_StartTime(page);	    //Read On
          airSec = 0;
          break;

        case ManualCirc:
          if(page == ManualCirc)  page = ManualCircOff;   //Set2Off
          MPX_ReadTank(ManualCirc, _exe);		              //MPX
          MPX_ReadTank(ManualCirc, _write);		            //MPX
          OUT_Clr_Air();						                      //ClearAir
          airMin = LCD_AutoRead_StartTime(page);	        //Read Off
          airSec = 0;
          break;

        case ManualCircOff:
          if(page == ManualCircOff)  page = ManualCirc; //Set2On
          OUT_Set_Air();
          airMin = LCD_AutoRead_StartTime(page);	      //ReadOn
          airSec = 0;
          break;
        default:						  break;
			}
		}

		//------------------------------------------------CountDown
		if(sec != cOld)
		{
		  cOld = sec;
      if(!airSec && (page != ErrorTreat))
      {
        airSec = 60;
        if(airMin) airMin--;
      }
      if(airSec)  airSec--;
    }

    //------------------------------------------------ManualReturn
    if(page == ManualCirc || page == ManualCircOff) return page;

		//------------------------------------------------WriteAutoVar
		if((page == AutoCirc)	|| (page == AutoAir) ||
		(page == ErrorTreat))
		{
      LCD_WriteAutoVar_Comp(airMin, airSec);			//compVar
      Eval_Oxygen(_count, airMin);
    }
    else LCD_WriteAutoVar(airMin, airSec);				//normalVar
	}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Auto Inflow Pump
 * ------------------------------------------------------------------*/

t_FuncCmd LCD_Auto_InflowPump(t_page page, int sec, t_FuncCmd cmd)
{
	static int ip_count = 0;
	static unsigned char t_ip[3] = {0,0,0};	//Time[h=2:min=1:sec=0]
	static t_FuncCmd ip_state = _off;		    //Init-Var

	//--------------------------------------------------Init
	if(cmd == _init)
	{
		t_ip[1] = MEM_EEPROM_ReadVar(ON_inflowPump);	//Read ON-Time
		if(!t_ip[1])
		{
			t_ip[0] = 0;							//?-Disable
			ip_state = _disabled;			//InfolwPump=Disabled
			return ip_state;					//Return
		}
		t_ip[2] = 0;		    //h
		t_ip[1] = 1;		    //min
		t_ip[0] = 2;		    //sec
		ip_state = _off;    //StateOff
	}

	//--------------------------------------------------State
	else if(cmd == _state)
	{
		return ip_state;
	}

	//--------------------------------------------------OutSet
	else if(cmd == _sym)
	{
    LCD_SymbolAuto_IP(page, ip_state);
		LCD_WriteAutoVar_IP(0x07, t_ip);
	}

	else if(ip_state == _disabled)	return ip_state;	//Disabled?

	//--------------------------------------------------Set
	else if(cmd == _set)
	{
		if(ip_state == _on &&
    (page == AutoAirOff || page == AutoCircOff))
		{
      LCD_SymbolAuto_IP(page, ip_state);
      OUT_Set_InflowPump();
    }
	}

	//--------------------------------------------------Reset
	else if(cmd == _reset)
	{
		if(ip_state == _on)
		{
      LCD_SymbolAuto_IP(page, ip_state);
      OUT_Clr_InflowPump();
    }
	}

	//--------------------------------------------------Execution
	else if(cmd == _exe)
	{
		// AutoChange2Off
		if(ip_state == _on && page != ErrorTreat && !t_ip[1] && !t_ip[0])
		{
			ip_state = _off;
			t_ip[2] = MEM_EEPROM_ReadVar(T_IP_off_h);
			t_ip[1] = MEM_EEPROM_ReadVar(OFF_inflowPump);
			t_ip[0] = 2;
			LCD_SymbolAuto_IP(page, _off);
			LCD_WriteAutoVar_IP(0x07, t_ip);     //WriteAll
			OUT_Clr_InflowPump();
    }

		// AutoChange2On
		else if(ip_state == _off  &&  page != ErrorTreat
		&& !t_ip[2] && !t_ip[1]   && !t_ip[0]
    && (page == AutoAirOff || page == AutoCircOff))
		{
			ip_state = _on;
			t_ip[2] = 0;
			t_ip[1] = MEM_EEPROM_ReadVar(ON_inflowPump);
			t_ip[0] = 2;

			LCD_SymbolAuto_IP(page, ip_state);
			LCD_WriteAutoVar_IP(0x07, t_ip);     //WriteAll
			OUT_Set_InflowPump();
    }

		// CountDown
		if(ip_state == _off ||
    ((page == AutoAirOff || page == AutoCircOff) && ip_state == _on))
		{
      if(ip_count != sec)
      {
        ip_count = sec;
        if(!t_ip[0])
        {
          if(t_ip[1] || t_ip[2])
          {
            t_ip[0] = 60;
          }
          if((!t_ip[1]) && t_ip[2])
          {
            t_ip[1] = 60;
            // decrease h
            t_ip[2]--;
            LCD_WriteAutoVar_IP(0x06, t_ip);
          }
          // decrease min
          if(t_ip[1]) t_ip[1]--;
          LCD_WriteAutoVar_IP(0x02, t_ip);
        }
        // decrease sec
        if(t_ip[0]) t_ip[0]--;
        LCD_WriteAutoVar_IP(0x01, t_ip);		    //WriteSec
      }
    }
	}

	return ip_state;
}



/* ------------------------------------------------------------------*
 * 						Auto Phosphor
 * ------------------------------------------------------------------*/

void LCD_Auto_Phosphor(int s_sec, t_FuncCmd cmd)
{
	static int count = 0;
	static unsigned char min = 0;
	static unsigned char sec = 5;
	static t_FuncCmd	 state = _off;

	//--------------------------------------------------Initialization
	if(cmd == _init)
	{
		min = MEM_EEPROM_ReadVar(ON_phosphor);
		if(!min)
    {
		  sec= 0;
      state = _disabled;
      return;
    }
		min = 1;
		sec = 5;
		state = _off;
	}

  //--------------------------------------------------Symbols
	else if(cmd == _sym)
	{
		LCD_SymbolAuto_Ph(state);
    LCD_WriteAutoVar_Ph(min, sec);
	}

	//--------------------------------------------------Disabled
	else if(state == _disabled)		return;

	//--------------------------------------------------Execution
	else if(cmd == _exe)
	{
		//Counter
		if(count != s_sec)
    {
      count = s_sec;
      if(!sec)
      {
        sec = 60;
        if(min) min--;
        LCD_WriteValue2_MyFont(13,135, min);
      }
      if(sec) sec--;
      LCD_WriteValue2_MyFont(13,147, sec);
    }

    //Change to OFF
		if(state == _on && !min && !sec)
		{
			state =_off;
			min = MEM_EEPROM_ReadVar(OFF_phosphor);
			sec = 0;
			LCD_Write_Symbol_3(6, 134, p_phosphor);
			OUT_Clr_Phosphor();
		}

		//Change to ON
		else if(state == _off && !min && !sec)
		{
			state = _on;
			min = MEM_EEPROM_ReadVar(ON_phosphor);
			sec = 0;
			LCD_Write_Symbol_3(6, 134, n_phosphor);
			OUT_Set_Phosphor();
		}
	}
}



/* ==================================================================*
 * 						Auto Set Pages
 * ==================================================================*/

void LCD_AutoSet(t_page page, int *p_min, int *p_sec)
{
	switch(page)
	{
		case AutoPage:
      LCD_AutoSet_Symbol(page, *p_min, *p_sec);
      OUT_CloseOff();
      break;

		case AutoZone:
      *p_sec = 0;
		  *p_min = 2;
      LCD_AutoSet_Symbol(page, *p_min, *p_sec);
      OUT_Set_Air();
      break;

		case AutoSetDown:
      *p_sec = 0;
		  *p_min = MEM_EEPROM_ReadVar(TIME_setDown);
      LCD_AutoSet_Symbol(page, *p_min, *p_sec);
      OUT_SetDown();
      break;

		case AutoPumpOff:
      *p_sec = 0;
		  *p_min = MEM_EEPROM_ReadVar(ON_pumpOff);
      LCD_AutoSet_Symbol(page, *p_min, *p_sec);
      OUT_Set_PumpOff();
      break;

		case AutoMud:
		  *p_min = MEM_EEPROM_ReadVar(ON_MIN_mud);
      *p_sec = MEM_EEPROM_ReadVar(ON_SEC_mud);

      //do not open Mud
      if((*p_min == 0) && (*p_sec == 0))
      {
        *p_sec = 1;
        break;
      }
      LCD_AutoSet_Symbol(page, *p_min, *p_sec);
      OUT_Set_Mud();
      break;

		case AutoCirc:
		case AutoCircOff:
    case AutoAir:
		case AutoAirOff:
		  // time
		  *p_sec = 0;
		  if(page == AutoCirc || page == AutoCircOff){
        *p_min = ((	MEM_EEPROM_ReadVar(TIME_H_circ)<<8) |
        MEM_EEPROM_ReadVar(TIME_L_circ));}
      else{
        *p_min = ((	MEM_EEPROM_ReadVar(TIME_H_air)<<8) |
        MEM_EEPROM_ReadVar(TIME_L_air));}

      // set Output
      LCD_AutoSet_Symbol(page, *p_min, *p_sec);
      LCD_Write_AirVar(page, 0, _set);
      LCD_Auto_InflowPump(page, 0, _set);
      break;

		default:
		  break;
	}
}


/* ------------------------------------------------------------------*
 * 						Auto Set Symbols
 * ------------------------------------------------------------------*/

void LCD_AutoSet_Symbol(t_page page, int min, int sec)
{
  //ClearText
  LCD_ClrSpace(13, 0, 6, 160);
  LCD_AutoText();

  //ClearSymbol
  LCD_ClrSpace(5, 0, 2, 35);
	switch(page)
	{
		case AutoPage:	  LCD_AutoSet_Page();							  break;
		case AutoZone:		LCD_AutoSet_Zone(min, sec);		  break;
		case AutoSetDown:	LCD_AutoSet_SetDown(min, sec);	break;
		case AutoPumpOff: LCD_AutoSet_PumpOff(min, sec);  break;
		case AutoMud:			LCD_AutoSet_Mud(min, sec);		  break;

		case AutoAir:
		case AutoAirOff:
		case AutoCirc:
		case AutoCircOff:	LCD_Write_AirVar(page, 0, _write);	break;
		default:													                    break;
	}
	//AddOns
  LCD_Auto_Phosphor(0, _sym);
  LCD_Auto_InflowPump(page, 0, _sym);
}



/* ==================================================================*
 * 						Manual Pages
 * ==================================================================*/

t_page LCD_ManualPage(t_page page)
{
	static int mMin = 5;
	static int mSec = 0;
  static int lcd_reset = 0;
	int *p_min;
	int *p_sec;

	p_min = &mMin;
	p_sec = &mSec;

	switch(page)
	{
		case ManualPage:
      page = ManualMain;
      mMin = 5;
      mSec = 0;
      LCD_ManualSet_Page(mMin, mSec);							    break;

		case ManualMain:
      page = LCD_ManualPage_Main(p_min, p_sec);		    break;

		case ManualCirc:
		case ManualCircOff:
      page = LCD_ManualPage_Circ(page, p_min, p_sec); break;

		case ManualAir:
      page = LCD_ManualPage_Air(p_min, p_sec);  break;

		case ManualSetDown:
      page = LCD_ManualPage_SetDown(p_min, p_sec);	  break;

		case ManualPumpOff:
		case ManualPumpOff_On:
      page = LCD_ManualPage_PumpOff(p_min, p_sec);	  break;

		case ManualMud:
      page = LCD_ManualPage_Mud(p_min, p_sec);			  break;

		case ManualCompressor:
      page = LCD_ManualPage_Compressor(p_min, p_sec);	break;

		case ManualPhosphor:
      page = LCD_ManualPage_Phosphor(p_min, p_sec);		break;

		case ManualInflowPump:
      page = LCD_ManualPage_InflowPump(p_min, p_sec);	break;

		default:														              break;
	}

	Watchdog_Restart();
	LCD_Backlight(_exe);

  //--------------------------------------------------lcd_reset
	if(lcd_reset > 32000)										//30s
	{
		lcd_reset = 0;
		LCD_Init();                          	//InitLCD
		TCC0_wait_ms(2);						          //Wait
    LCD_MarkTextButton(Manual);
	}
	lcd_reset++;
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual CountDown
 * ------------------------------------------------------------------*/

t_page LCD_ManualCD(t_page page, int *p_min, int *p_sec)
{
  if(Eval_CountDown(p_min, p_sec)){
	  MEM_EEPROM_WriteManualEntry(0,0, _write);		//Write Entry
		return AutoPage;}														//Back to AutoPage
  return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Main
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_Main(int *p_min, int *p_sec)
{
	t_page page = ManualMain;
	page = Touch_ManualLinker(Touch_Matrix(), page);
	page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page!= ManualMain)
	  LCD_ManualSet(page, p_min, p_sec);	//nextPage
	LCD_WriteManualVar(*p_min, *p_sec);	  //Variables
	return page;
}



/* ------------------------------------------------------------------*
 * 						Manual Circulate
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_Circ(t_page page, int *p_min, int *p_sec)
{
	t_page sPage = page;
	page = Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualCirc && page != ManualCircOff)
	{
	  if(sPage == ManualCirc) OUT_Clr_Air();  //CloseIfOpen
    LCD_ManualSet(page, p_min, p_sec);		  //nextPage
    return page;
  }

	LCD_WriteManualVar(*p_min, *p_sec);		        //Variables
	page = LCD_Write_AirVar(page, *p_sec, _exe);	//WriteVar
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Air
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_Air(int *p_min, int *p_sec)
{
	t_page page = ManualAir;
	page = Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualAir)
	{
	  OUT_Clr_Air();
    LCD_ManualSet(page, p_min, p_sec);	    //nextPage
    return page;
  }
	LCD_WriteManualVar(*p_min, *p_sec);		      //Variables
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Set Down
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_SetDown(int *p_min, int *p_sec)
{
	t_page page = ManualSetDown;
	page =	Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualSetDown)	LCD_ManualSet(page, p_min, p_sec);	//nextPage
	LCD_WriteManualVar(*p_min, *p_sec);									//Variables
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Pump Off
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_PumpOff(int *p_min, int *p_sec)
{
	t_page page = ManualPumpOff;
	static unsigned char on = 0;
	static unsigned char count = 0;

	page = Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page == ManualPumpOff_On){
	  on = 1;		                        //PumpOffOn
    *p_min = 29;
	  *p_sec = 58;
		LCD_ManualText(*p_min, *p_sec);}

	else if(page != ManualPumpOff){				  //--Else
		LCD_ClrSpace(14, 2, 8, 120);
		if(on)	OUT_Clr_PumpOff();					  //Clr PumpOff
		LCD_ManualSet(page, p_min, p_sec);		//nextPage
		LCD_ManualText(*p_min, *p_sec);
		on = 0;
		return page;}

	if(!on){
	  TCC0_DisplayManual_Wait();
	  count++;							//--Blink
		if(count == 125)
		  LCD_WriteStringFontNeg(17,15,"PRESS OK!:");
		if(count == 250){
		  LCD_WriteStringFont(17,15,"PRESS OK!:");}}

	if(on) LCD_WriteManualVar(*p_min, *p_sec);		//Variables
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Mud
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_Mud(int *p_min, int *p_sec)
{
	t_page page = ManualMud;
	page = Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualMud){
	  OUT_Clr_Mud();
    LCD_ManualSet(page, p_min, p_sec);}	//nextPage

	LCD_WriteManualVar(*p_min, *p_sec);
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Compressor
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_Compressor(int *p_min, int *p_sec)
{
	t_page page = ManualCompressor;
	page = Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualCompressor){
	  OUT_Clr_Compressor();
    LCD_ManualSet(page, p_min, p_sec);}	//nextPage

	LCD_WriteManualVar(*p_min, *p_sec);
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Phosphor
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_Phosphor(int *p_min, int *p_sec)
{
	t_page page = ManualPhosphor;
	page =	Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualPhosphor){
	  OUT_Clr_Phosphor();
    LCD_ManualSet(page, p_min, p_sec);}	//nextPage

	LCD_WriteManualVar(*p_min, *p_sec);
	return page;
}


/* ------------------------------------------------------------------*
 * 						Manual Inflow Pump
 * ------------------------------------------------------------------*/

t_page LCD_ManualPage_InflowPump(int *p_min, int *p_sec)
{
	t_page page = ManualInflowPump;
	page =	Touch_ManualLinker(Touch_Matrix(), page);
  page = LCD_ManualCD(page, p_min, p_sec);     //CountDown
	if(page != ManualInflowPump){
	  OUT_Clr_InflowPump();
    LCD_ManualSet(page, p_min, p_sec);}	//nextPage

	LCD_WriteManualVar(*p_min, *p_sec);
	return page;
}



/* ==================================================================*
 * 						Manual Set Page
 * ==================================================================*/

void LCD_ManualSet(t_page page, int *p_min, int *p_sec)
{
	switch(page)
	{
		case ManualMain:
		  *p_min = 5;
      *p_sec = 0;			break;

		case ManualCirc:
		  LCD_Write_AirVar(AutoCirc, 0, _init);
      OUT_Set_Air();
      *p_min = 60;
      *p_sec = 0;			break;

		case ManualAir:
		  LCD_Write_AirVar(AutoAir, 0, _init);
		  OUT_Set_Air();
      *p_min = 60;
      *p_sec = 0;			break;

		case ManualSetDown:
		  *p_min = 60;
      *p_sec = 0;			break;

		case ManualPumpOff:
		  LCD_ClrSpace(15, 2, 5, 120);
      LCD_WriteStringFont(17,15,"PRESS OK!:");
      LCD_Write_Symbol_3(15, 85, p_ok);
      *p_min = 30;
      *p_sec = 0;
      LCD_WriteValue2(17,124, *p_min);
      LCD_WriteValue2(17,142, *p_sec);		break;

		case ManualMud:
		  OUT_Set_Mud();
      *p_min = 5;
      *p_sec = 0;			break;

		case ManualCompressor:
		  OUT_Set_Compressor();
      *p_min = 0;
      *p_sec = 10;		break;

		case ManualPhosphor:
		  OUT_Set_Phosphor();
      *p_min = 30;
      *p_sec = 0;			break;

		case ManualInflowPump:
		  OUT_Set_InflowPump();
      *p_min = 5;
      *p_sec = 0;			break;
		default:				  break;
	}
}



/* ==================================================================*
 * 						Setup Pages
 * ==================================================================*/

t_page LCD_SetupPage(t_page page)
{
	static int sMin = 5;
	static int sSec = 0;

	int *p_min;
	int *p_sec;

	p_min = &sMin;
	p_sec = &sSec;

	switch(page)
	{
		case SetupPage:
      //-------------------------------------------Main-Page----
      page = SetupMain;
      sMin = 5;
      sSec = 0;
      LCD_SetupSet_Page();								break;	//Setup

		case SetupMain:
      page = LCD_SetupPage_Main(p_min, p_sec);		break;	//Main

		case SetupCirculate:
      page = LCD_SetupPage_Circulate(p_min, p_sec);	break;	//Circulate

		case SetupAir:
      page = LCD_SetupPage_Air(p_min, p_sec);			break;	//Air

		case SetupSetDown:
      page = LCD_SetupPage_SetDown(p_min, p_sec);		break;	//Set Down

		case SetupPumpOff:
      page = LCD_SetupPage_PumpOff(p_min, p_sec);		break;	//Pump Off

		case SetupMud:
      page = LCD_SetupPage_Mud(p_min, p_sec);			break;	//Mud

		case SetupCompressor:
      page = LCD_SetupPage_Compressor(p_min, p_sec);	break;	//Compressor

		case SetupPhosphor:
      page = LCD_SetupPage_Phosphor(p_min, p_sec);	break;	//Phosphor

		case SetupInflowPump:
      page = LCD_SetupPage_InflowPump(p_min, p_sec);	break; 	//inflowPump

		case SetupCal:
      page = LCD_SetupPage_Cal(p_min, p_sec);			break; 	//Cal

		case SetupCalPressure:
      page = LCD_SetupPage_Cal(p_min, p_sec);			break; 	//Cal

		case SetupAlarm:
      page = LCD_SetupPage_Alarm(p_min, p_sec);		break; 	//Alarm

		case SetupWatch:
      page = LCD_SetupPage_Watch(p_min, p_sec);		break; 	//Watch

		case SetupZone:
      page = LCD_SetupPage_Zone(p_min, p_sec);		break; 	//Zone

		default:													break;
	}

	Watchdog_Restart();
	TCC0_DisplaySetup_Wait();									            //Wait

	if(Eval_CountDown(p_min, p_sec)){                     //TimeOut
	  if(page == SetupCalPressure) PORT_Ventil_AllClose();
	  return AutoPage;}
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Circulate
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Main(int *p_min, int *p_sec)
{
	t_page page = SetupMain;
	page =	Touch_SetupLinker(Touch_Matrix(), page);
	if(page!= SetupMain)	LCD_SetupSet(page, p_min, p_sec);		//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Circulate
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Circulate(int *p_min, int *p_sec)
{
	t_page page = SetupCirculate;
	page =	Touch_SetupCirculateLinker(Touch_Matrix(), page);
	if(page!= SetupCirculate)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Air
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Air(int *p_min, int *p_sec)
{
	t_page page = SetupAir;
	page =	Touch_SetupAirLinker(Touch_Matrix(), page);
	if(page!= SetupAir)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup SetDown
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_SetDown(int *p_min, int *p_sec)
{
	t_page page = SetupSetDown;
	page =	Touch_SetupSetDownLinker(Touch_Matrix(), page);
	if(page!= SetupSetDown)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup PumpOff
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_PumpOff(int *p_min, int *p_sec)
{
	t_page page = SetupPumpOff;
	page =	Touch_SetupPumpOffLinker(Touch_Matrix(), page);
	if(page!= SetupPumpOff)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Mud
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Mud(int *p_min, int *p_sec)
{
	t_page page = SetupMud;
	page =	Touch_SetupMudLinker(Touch_Matrix(), page);
	if(page!= SetupMud)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Compressor
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Compressor(int *p_min, int *p_sec)
{
	t_page page = SetupCompressor;
	page =	Touch_SetupCompressorLinker(Touch_Matrix(), page);
	if(page!= SetupCompressor)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Phosphor
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Phosphor(int *p_min, int *p_sec)
{
	t_page page = SetupPhosphor;
	page =	Touch_SetupPhosphorLinker(Touch_Matrix(), page);
	if(page!= SetupPhosphor)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup InflowPump
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_InflowPump(int *p_min, int *p_sec)
{
	t_page page = SetupInflowPump;
	page =	Touch_SetupInflowPumpLinker(Touch_Matrix(), page);
	if(page!= SetupInflowPump)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Cal
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Cal(int *p_min, int *p_sec)
{
	t_page page = 		    SetupCal;
	static unsigned char 	iniTime = 0;

	if(iniTime)	page = SetupCalPressure;
	page = Touch_SetupCalLinker(Touch_Matrix(), page);	//TouchApp

	if(page == SetupCalPressure)		//Cal minimum Pressure
	{
    //------------------------------------------------Sonic
    if(MEM_EEPROM_ReadVar(SONIC_on))
    {
        Sonic_LevelCal(_new);
        Sonic_LevelCal(_write);
        LCD_Write_Symbol_2(15,1, p_level);
        page = SetupCal;
    }
    //------------------------------------------------PressureSensor
    else
    {
      if(!iniTime){
        iniTime = 1;				      //Reset JumpAuto-Time
        *p_min = 4;
        *p_sec = 120;             //2minCal
        OUT_Set_Air();}	          //Open Air
      LCD_WriteValue3(17, 100, *p_sec);			//Write Time
      if(!*p_sec){
        iniTime = 0;							          //--ReadPressure
        MPX_LevelCal(_new);		              //Read WaterLevel
        MPX_LevelCal(_write);		            //Write Niveau Pressure
        OUT_Clr_Air();						          //Close Air
        LCD_ClrSpace(17, 100, 2, 20);       //ClearCountdown
        LCD_Write_Symbol_2(15,1, p_level);  //Symbol
        page = SetupCal;}                   //Back to Normal
    }
	}
	else if(page != SetupCal){
		iniTime = 0;
		LCD_SetupSet(page, p_min, p_sec);}	  //nextPage

	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Alarm
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Alarm(int *p_min, int *p_sec)
{
	t_page page = SetupAlarm;
	page =	Touch_SetupAlarmLinker(Touch_Matrix(), page);	//Touch Apps
	MCP9800_WriteTemp();
	if(page!= SetupAlarm)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Watch
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Watch(int *p_min, int *p_sec)
{
	t_page page = SetupWatch;
	page =	Touch_SetupWatchLinker(Touch_Matrix(), page);
	if(page!= SetupWatch)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Setup Zone
 * ------------------------------------------------------------------*/

t_page LCD_SetupPage_Zone(int *p_min, int *p_sec)
{
	t_page page = SetupZone;
	page =	Touch_SetupZoneLinker(Touch_Matrix(), page);
	if(page!= SetupZone)	LCD_SetupSet(page, p_min, p_sec);	//nextPage
	return page;
}



/* ==================================================================*
 * 						Setup Set Page
 * ==================================================================*/

void LCD_SetupSet(t_page page, int *p_min, int *p_sec)
{
	*p_min = 5;
	*p_sec = 0;

	switch(page)
	{
		case SetupMain:			  LCD_SetupSet_Page();		    break;
		case SetupCirculate:	LCD_SetupSet_Circulate(); 	break;
		case SetupAir: 			  LCD_SetupSet_Air();			    break;
		case SetupSetDown:		LCD_SetupSet_SetDown();		  break;
		case SetupPumpOff:		LCD_SetupSet_PumpOff();		  break;
		case SetupMud:			  LCD_SetupSet_Mud();			    break;
		case SetupCompressor:	LCD_SetupSet_Compressor();	break;
		case SetupPhosphor:		LCD_SetupSet_Phosphor();	  break;
		case SetupInflowPump:	LCD_SetupSet_InflowPump();	break;
		case SetupCal:		    LCD_SetupSet_Cal();         break;
		case SetupAlarm:		  LCD_SetupSet_Alarm();		    break;
		case SetupWatch:		  LCD_SetupSet_Watch();		    break;
		case SetupZone:			  LCD_SetupSet_Zone();		    break;
		default:											                    break;
	}
}



/* ==================================================================*
 * 						Data Pages
 * ==================================================================*/

t_page LCD_DataPage(t_page page)
{
	static int dMin = 5;
	static int dSec = 0;

	int *p_min;
	int *p_sec;

	p_min = &dMin;
	p_sec = &dSec;

	switch(page)
	{
		case DataPage:
      page = DataMain;
      dMin = 5;
      dSec = 0;
      LCD_DataSet_Page();								break;	//Setup

		case DataMain:
      page = LCD_DataPage_Main(p_min, p_sec);		break;	//Main

		case DataAuto:
      page = LCD_DataPage_Auto(p_min, p_sec);		break;	//Auto

		case DataManual:
      page = LCD_DataPage_Manual(p_min, p_sec);	break;	//Manual

		case DataSetup:
      page = LCD_DataPage_Setup(p_min, p_sec);	break;	//Setup

    //------------------------------------------------UltraSonic
		case DataSonic:
		case DataSonicAuto:
    case DataSonicBoot:
		case DataSonicBootR:
    case DataSonicBootW:
				page = LCD_DataPage_Sonic(page, p_min, p_sec);
				break;

		default:												break;
	}

	Watchdog_Restart();
	TCC0_DisplayData_Wait();								            //Wait 100ms
	if(Eval_CountDown(p_min, p_sec))	return AutoPage;  //Back2AutoPage

	return page;
}


/* ------------------------------------------------------------------*
 * 						Data Main
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Main(int *p_min, int *p_sec)
{
	t_page page = DataMain;
	page = Touch_DataLinker(Touch_Matrix(), page);				//Touch
	if(page!= DataMain)	LCD_DataSet(page, p_min, p_sec);  //nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data Auto
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Auto(int *p_min, int *p_sec)
{
	t_page page = DataAuto;
	page = Touch_DataAutoLinker(Touch_Matrix(), page);			//Touch
	if(page!= DataAuto)	LCD_DataSet(page, p_min, p_sec);		//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data Manual
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Manual(int *p_min, int *p_sec)
{
	t_page page = DataManual;
	page = Touch_DataManualLinker(Touch_Matrix(), page);		//Touch
	if(page!= DataManual)	LCD_DataSet(page, p_min, p_sec);	//nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data Setup
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Setup(int *p_min, int *p_sec)
{
	t_page page = DataSetup;
	page = Touch_DataSetupLinker(Touch_Matrix(), page);		  //Touch
	if(page!= DataSetup)	LCD_DataSet(page, p_min, p_sec);  //nextPage
	return page;
}


/* ------------------------------------------------------------------*
 * 						Data UltraSonic
 * ------------------------------------------------------------------*/

t_page LCD_DataPage_Sonic(t_page page, int *p_min, int *p_sec)
{
  *p_min = 5;           //Freeze Time
	page = Touch_DataSonicLinker(Touch_Matrix(), page);	//Touch

	switch(page)
	{
	  case DataSonicAuto:   Sonic_Data_Auto();      break;
	  case DataSonicBootR:  Sonic_Data_BootRead();
	                        page = DataSonic;       break;
	  case DataSonicBootW:  Sonic_Data_BootWrite();
                          page = DataSonic;       break;
	  default:                                      break;
	}

	if(page != DataSonic && page != DataSonicAuto &&
  page != DataSonicBootR && page != DataSonicBootW)
	  LCD_DataSet(page, p_min, p_sec);	//nextPage
	return page;
}



/* ==================================================================*
 * 						Data Set Pages
 * ==================================================================*/

void LCD_DataSet(t_page page, int *p_min, int *p_sec)
{
	*p_min = 5;
	*p_sec = 0;

	switch(page)
	{
		case DataMain:		LCD_DataSet_Page();		break;
		case DataAuto:		LCD_DataSet_Auto();		break;
		case DataManual: 	LCD_DataSet_Manual();	break;
		case DataSetup:		LCD_DataSet_Setup();	break;

		case DataSonic:
      LCD_DataSet_Sonic();	  break;
		default:									break;
	}
}



/* ==================================================================*
 * 						Pin Page
 * ==================================================================*/

t_page LCD_PinPage(t_page page)
{
	t_page sPage = page;
	unsigned char matrix = 0;

	static int pinMin = 5;
	static int pinSec = 0;
	static unsigned char init = 0;

	int *p_min;
	int *p_sec;

	p_min= &pinMin;
	p_sec= &pinSec;

	if(!init)
	{
		init = 1;
		LCD_PinSet_Page();
		pinMin = 5;
		pinSec = 0;
	}

	Watchdog_Restart();
	TCC0_DisplayManual_Wait();						//Wait

	matrix = Touch_Matrix();
	page = Touch_PinLinker(matrix, page);

	if(sPage != page){	init=0;
	  LCD_Clean();
    return page;}

	if(Eval_CountDown(p_min, p_sec)){	  init=0;
		LCD_Clean();
	  return AutoPage;}				//Back to AutoPage

	return page;
}



/* ==================================================================*
 * 						Entry Pages
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Auto Entry Clear Pages
 * ------------------------------------------------------------------*/

void LCD_Entry_Clr(void)
{
  LCD_ClrSpace(4,1,24,135);
}


/* ------------------------------------------------------------------*
 * 						Auto Entry
 * ------------------------------------------------------------------*/

void LCD_WriteAutoEntryPage(t_eeDataPage page)
{
	unsigned char entry=0;
	unsigned char eep=0;
	unsigned char *p_ct= Eval_Memory_LatestEntry(Auto);
	unsigned char i=0;

	eep= *p_ct;
	p_ct++;
	entry= *p_ct;

	switch(page)
	{
		case Page1: LCD_Entry_Clr();
                LCD_wPage(Auto, eep, entry);	break;

		case Page2:	LCD_Entry_Clr();  eep= LCD_eep_minus(eep,2);
						    LCD_wPage(Auto, eep, entry);	break;

		case Page3:	LCD_Entry_Clr();  eep= LCD_eep_minus(eep,4);
						    LCD_wPage(Auto, eep, entry);	break;

		case Page4:	LCD_Entry_Clr();  eep= LCD_eep_minus(eep,6);
						    LCD_wPage(Auto, eep, entry);	break;

		case Page5:	LCD_Entry_Clr();  eep= LCD_eep_minus(eep,8);
						    LCD_wPage(Auto, eep, entry);	break;

		case Page6:	LCD_Entry_Clr();	eep= LCD_eep_minus(eep,10);
						    LCD_wPage(Auto, eep, entry);	break;

		case Page7:	LCD_Entry_Clr();	eep= LCD_eep_minus(eep,12);
						    LCD_wPage(Auto, eep, entry);	break;

		case Page8:	LCD_Entry_Clr();	eep= LCD_eep_minus(eep,14);
      for(i=0; i<4; i++)
      {
        LCD_WriteAutoEntry(5+(2*i), eep, entry);
        if(entry<1)
        {
          entry=4;		eep--;
          if(eep < 2)		eep= 16;
        }
        entry--;
      }
      LCD_WriteStringFont(15,1,"End");	break;

		default:		break;
	}
}



/* ------------------------------------------------------------------*
 * 						Data Manual Entry Page
 * ------------------------------------------------------------------*/

void LCD_WriteManualEntryPage(t_eeDataPage page)
{
	unsigned char entry=0;
	unsigned char eep=0;
	unsigned char *p_ct= Eval_Memory_LatestEntry(Manual);
	unsigned char i=0;

	eep= *p_ct;
	p_ct++;
	entry= *p_ct;

	switch(page)
	{
		case Page1:	LCD_Entry_Clr();
						    LCD_wPage(Manual, eep, entry);	break;

		case Page2:	LCD_Entry_Clr();		eep= LCD_eep_minus(eep,2);
      for(i=0; i<4; i++)
      {
        LCD_WriteManualEntry(5+(2*i), eep, entry);
        if(entry<1)
        {
          entry=4;		eep--;
          if(eep < 17)	eep= 20;
        }
        entry--;
      }
      LCD_WriteStringFont(15,1,"End");	break;
		default:										      	break;
	}
}



/* ------------------------------------------------------------------*
 * 						Data Setup Entry Page
 * ------------------------------------------------------------------*/

void LCD_WriteSetupEntryPage(t_eeDataPage page)
{
	unsigned char entry=0;
	unsigned char eep=0;
	unsigned char *p_ct= Eval_Memory_LatestEntry(Setup);
	unsigned char i=0;

	eep= *p_ct;
	p_ct++;
	entry= *p_ct;

	switch(page)
	{
		case Page1: LCD_Entry_Clr();
						    LCD_wPage(Setup, eep, entry);			break;

		case Page2:	LCD_Entry_Clr();	eep= LCD_eep_minus(eep,2);
      for(i=0; i<4; i++)
      {
        LCD_WriteSetupEntry(5+(2*i), eep, entry);
        if(entry<1)
        {
          entry=4;		eep--;
          if(eep < 22)	eep= 25;
        }
        entry--;
      }
      LCD_WriteStringFont(15,1,"End");	break;
		default:											      break;
	}
}




/* ==================================================================*
 * 						Data Built-in Pages
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	LCD_wPage
 * --------------------------------------------------------------
 * 	t_textSymbols data	-	Display Page Selection
 *  unsigned char eep	  -	EEPROM-Page
 *	unsigned char entry	-	entry
 * --------------------------------------------------------------
 *	Writes One Entry line, call in entryPages
 * ------------------------------------------------------------------*/

void LCD_wPage(t_textButtons data, unsigned char eep, unsigned char entry)
{
	unsigned char i=0;
	unsigned char startPa=0;
	unsigned char endPa=0;

	switch(data)
	{
		case Auto:		startPa= 2;		endPa= 16;		break;			//8Pages
		case Manual:	startPa= 17; 	endPa= 20;		break;			//2Pages
		case Setup:		startPa= 22;	endPa= 25;		break;			//2Pages
		default:										            break;
	}

	//-----------------------------------------------------------------Half-Page
	for(i=0; i<4; i++)
	{
		//-------------------------------------------Write-Entry-------
		switch(data)
		{
			case Auto:		LCD_WriteAutoEntry(5+(2*i), eep, entry);	break;
			case Manual:	LCD_WriteManualEntry(5+(2*i), eep, entry);	break;
			case Setup:		LCD_WriteSetupEntry(5+(2*i), eep, entry);	break;
			default:													break;
		}
		//-------------------------------------------Update-Varibles---
		if(entry<1)
		{
			entry=4;			eep--;
			if(eep < startPa)	eep= endPa;
		}
		entry--;
	}

	//-----------------------------------------------------------------Complete-Page
	for(i=4; i<8; i++)
	{
		//-------------------------------------------Write-Entry-------
		switch(data)
		{
			case Auto:		LCD_WriteAutoEntry(5+(2*i), eep, entry);	break;
			case Manual:	LCD_WriteManualEntry(5+(2*i), eep, entry);	break;
			case Setup:		LCD_WriteSetupEntry(5+(2*i), eep, entry);	break;
			default:													break;
		}
		//-------------------------------------------Update-Varibles---
		if(entry<1)
		{
			entry=4;			eep--;
			if(eep < startPa)	eep= endPa;
		}
		entry--;
	}
}


/* ------------------------------------------------------------------*
 * 						Minus
 * ------------------------------------------------------------------*/

unsigned char LCD_eep_minus(unsigned char eep, unsigned char cnt)
{
	unsigned char i=0;
	for(i=0; i<cnt; i++)
	{
		eep--;
		if(eep<2) eep=16;
	}
	return eep;
}



/* ==================================================================*
 * 						FUNCTIONS Misc
 * ==================================================================*/

void LCD_Calibration(void)
{
	int xRead = 0;
	int yRead = 0;
	int x = 0;
	int y = 0;

  LCD_Backlight(_on);
	Touch_Cal();
	LCD_Clean();

	LCD_WriteStringFont(2, 0, "X-Cal:");
	LCD_WriteStringFont(5, 0, "Y-Cal:");
	LCD_WriteStringFont(10, 0, "X-Value:");
	LCD_WriteStringFont(13, 0, "Y-Value:");
	LCD_WriteStringFont(18, 0, "X-Value-Cal:");
	LCD_WriteStringFont(21, 0, "Y-Value-Cal:");

	LCD_WriteValue3(2, 60, 	MEM_EEPROM_ReadVar(TOUCH_X_min));
	LCD_WriteValue3(5, 60, 	MEM_EEPROM_ReadVar(TOUCH_Y_min));
	LCD_WriteValue3(2, 100, MEM_EEPROM_ReadVar(TOUCH_X_max));
	LCD_WriteValue3(5, 100, MEM_EEPROM_ReadVar(TOUCH_Y_max));

	while(1)
	{
		Watchdog_Restart();

		xRead = (Touch_X_ReadData()>>4);
		yRead = (Touch_Y_ReadData()>>4);
		y = Touch_Y_Cal(yRead);
		x = Touch_X_Cal(xRead);

		LCD_WriteValue3(10, 100, xRead);
		LCD_WriteValue3(13, 100, yRead);
		LCD_WriteValue3(18, 100, x);
		LCD_WriteValue3(21, 100, y);
	}
}





/**********************************************************************\
 * End of file
\**********************************************************************/

