/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    sonic_app.c
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	UltraSonic Applications
* ------------------------------------------------------------------
*	Date:			    21.05.2014
* lastChanges:  12.08.2015
\**********************************************************************/


#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "lcd_sym.h"

#include "tc_func.h"
#include "basic_func.h"
#include "at24c_driver.h"
#include "can_app.h"
#include "sonic_app.h"
#include "memory_app.h"


/* ==================================================================*
 * 						Functions
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						UltraSonic App - 4Safety
 * ------------------------------------------------------------------*/

int Sonic_App(t_US cmd)
{
	static int usSreg = 0;			  //StatusReg
	static int usDreg = 0;			  //DistanceReg
	static int usTreg = 0;			  //TempReg

	static t_US state = US_wait;  //StatusCAN
	unsigned char *rec = 0;				//ReadPointer

	//---------------------------------------------DistanceInit-OneShot
	if((cmd == D1_ini) && (state == US_wait))
	{
		rec = CAN_SonicDistance(_init, _oneShot);
		usSreg &= ~DERR;
		state = D1_wo;
	}

	//---------------------------------------------DistanceInit-5Shots
	else if((cmd == D5_ini) && (state == US_wait))
	{
		rec = CAN_SonicDistance(_init, _5Shots);
		usSreg &= ~DERR;
		state = D5_wo;
	}

	//---------------------------------------------TempInit
	else if((cmd == T_ini) && (state == US_wait))
	{
		rec = CAN_SonicTemp(_init);
		usSreg &= ~TERR;
		state = T_wo;
	}

	//--------------------------------------------------US-Exe
	else if(cmd == US_exe)
	{
		//------------------------------------------------ShotExe
		if(state == D1_wo)
		{
			rec = CAN_SonicDistance(_exe, _oneShot);  //ReadSonic
			if(rec[0] >= 10)                          //Error
			{
			 	usSreg |= DERR;
				state = US_wait;
			}
			else if(rec[0] == 3)                   //OK
			{
				usDreg = ((rec[1] << 8) | rec[2]);  //DistanceReg
				usSreg |= DISA;                     //DistanceAv
				state = US_wait;
			}
		}
    //------------------------------------------------Shot5Exe
		else if(state == D5_wo)
		{
			rec = CAN_SonicDistance(_exe, _5Shots); //ReadSonic
			if(rec[0] >= 10)                        //Error
			{
			 	usSreg |= DERR;
				state = US_wait;
			}
			else if(rec[0] == 3)                  //OK
			{
				usDreg = ((rec[1] << 8) | rec[2]);  //DistanceReg
				usSreg |= DISA;                     //DistanceAv
				state = US_wait;
			}
		}
    //------------------------------------------------TempExe
		else if(state == T_wo)
		{
			rec = CAN_SonicTemp(_exe);  //ReadSonic
			if(rec[0] >= 10)            //Error
			{
			 	usSreg |= TERR;
				state = US_wait;
			}
			else if(rec[0] == 3)        //OK
			{
				usTreg = ((rec[1] << 8) | rec[2]);  //TempReg
				usSreg |= TEMPA;                    //TempAv
				state = US_wait;
			}
		}
		return usSreg;
	}
	//---------------------------------------------US-Reset
	else if(cmd == US_reset)
	{
		state = US_wait;
		usSreg = 0x00;
		usDreg = 0;
		usTreg = 0;
	}
	//---------------------------------------------US-Read
	else if(cmd == R_sreg)	return usSreg;
	else if(cmd == R_dreg){	usSreg &= ~DISA;	return usDreg;}
	else if(cmd == R_treg){	usSreg &= ~TEMPA;	return usTreg;}

	return state;
}



/* ------------------------------------------------------------------*
 * 						UltraSonic - LCDData - Shot
 * ------------------------------------------------------------------*/

void Sonic_Data_Shot(void)
{
	unsigned char run = 1;

	Sonic_App(US_reset);
	Sonic_App(T_ini);
	while(run)
	{
		Sonic_App(US_exe);
		if(Sonic_App(R_sreg) & TEMPA)
		{
			LCD_Data_SonicWrite(_temp, Sonic_App(R_treg)); //printTemp
			run = 0;
		}
		else if(Sonic_App(R_sreg) & TERR)
		{
			LCD_Data_SonicWrite(_noUS, 0);
			run = 0;
		}
	}

  run = 1;
  Sonic_App(US_reset);
	Sonic_App(D5_ini);
	while(run)
	{
		Sonic_App(US_exe);
		if(Sonic_App(R_sreg) & DISA)
		{
			LCD_Data_SonicWrite(_shot, Sonic_App(R_dreg));   //printDistance
			run = 0;
		}
		else if(Sonic_App(R_sreg) & DERR)
		{
			LCD_Data_SonicWrite(_noUS, 0);
			run = 0;
		}
	}
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - LCDData - Auto
 * ------------------------------------------------------------------*/

void Sonic_Data_Auto(void)
{
	Sonic_App(US_exe);
  if(Sonic_App(R_sreg) & TEMPA)
	{
		LCD_Data_SonicWrite(_temp, Sonic_App(R_treg));
		Sonic_App(D5_ini);
	}
	else if(Sonic_App(R_sreg) & TERR) LCD_Data_SonicWrite(_noUS, 0);

	else if(Sonic_App(R_sreg) & DISA)
	{
		LCD_Data_SonicWrite(_shot1, Sonic_App(R_dreg));
		Sonic_App(T_ini);
	}
	else if(Sonic_App(R_sreg) & DERR) LCD_Data_SonicWrite(_noUS, 0);
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - ReadTank
 * ------------------------------------------------------------------*/

t_page Sonic_ReadTank(t_page page, t_FuncCmd cmd)
{
	static unsigned char state = 1;
	static int sonic = 0;

	if(!MEM_EEPROM_ReadVar(SONIC_on))
	  return page;

	//--------------------------------------------------exe
	else if(cmd == _exe)
	{
    //------------------------------------------------AirOffCheck
    if(page != AutoAirOff && page != AutoCircOff &&
    page != AutoSetDown)
    {
      state = 1;
      return page;
    }

	  //------------------------------------------------Read
    if(state == 0)								            //Read
    {
      Sonic_App(US_exe);
      if(Sonic_App(R_sreg) & DISA)						//DistanceAvailable
      {
        sonic = Sonic_App(R_dreg);	          //ReadReg
        LCD_Auto_SonicVal(sonic);             //WriteValue
        page = Sonic_ChangePage(page, sonic);	//ChangePage
        LCD_Sym_NoUS(page, _clear);           //ClearNoUs
        state = 2;
      }
      else if(Sonic_App(R_sreg) & DERR)   //NoUs
      {
        Sonic_App(US_reset);
        Sonic_App(D5_ini);
        state = 0;
        LCD_Sym_NoUS(page, _write);       //WriteNoUs
      }
    }
    //------------------------------------------------TC-Init
    else if(state == 1)
    {
      TCF0_WaitSec_Init(2);
      state = 2;
    }
    //------------------------------------------------NextShot
    else if(state >= 2)
    {
      if(TCF0_Wait_Query()) state++;		//2s
      //***SonicTime*2s - def:12
      if(state >= 12)
      {
        Sonic_App(US_reset);
        Sonic_App(D5_ini);
        state = 0;
      }
    }
	}

	//--------------------------------------------------write
	else if(cmd == _write)
	{
		if(!LCD_Sym_NoUS(page, _check)) LCD_Auto_SonicVal(sonic);
	}

	return page;
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - ReadTank - ChangePages
 * ------------------------------------------------------------------*/

t_page Sonic_ChangePage(t_page page, int sonic)
{
	if(!MEM_EEPROM_ReadVar(SONIC_on))	return page;	//DisabledUS

  static int oldSonic = 0;
  static unsigned char error = 0;
  int zero = 0;
  int lvO2 = 0;
  int lvCi = 0;

  //--------------------------------------------------checkOldValue
  if(!oldSonic) oldSonic = sonic;   //Init
  else
  {
    if(sonic > (oldSonic + 50)) error++;
    else if(sonic < (oldSonic - 50)) error++;
    else error = 0;

    if(error > 3) {
      error = 0;
      oldSonic = sonic;}

    if(error) return page;
  }

  //--------------------------------------------------Percentage
  zero = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) |
          (MEM_EEPROM_ReadVar(SONIC_L_LV)));
  lvO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8)		|
				  (MEM_EEPROM_ReadVar(TANK_L_O2)));
  lvCi = ((MEM_EEPROM_ReadVar(TANK_H_Circ) << 8)		|
				  (MEM_EEPROM_ReadVar(TANK_L_Circ)));

// TODO (chris#1#): do not jump back to air

	switch(page)
	{
		case AutoSetDown:
			if(sonic < (zero - (lvO2 * 10)))	    page = AutoSetDown;
			else if(sonic < (zero - (lvCi * 10))) page = AutoAir;
			else                                  page = AutoCirc;
			break;

		case AutoCircOff:
			if(sonic < (zero - (lvCi * 10)))  page = AutoAir;
			break;

		case AutoAirOff:
			if(sonic < (zero - (lvO2 * 10)))	page = AutoSetDown;
			break;

		default: break;
	}

	return page;
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - LevelCal - For Calibration in Setup
 * ------------------------------------------------------------------*/

int Sonic_LevelCal(t_FuncCmd cmd)
{
	static int level = 0;

	switch(cmd)
	{
		//------------------------------------------------ReadFromEEPROM
		case _init:
      level = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) |
               (MEM_EEPROM_ReadVar(SONIC_L_LV)));
      LCD_WriteValue4(17, 40, level);
      break;

		//------------------------------------------------Save2EEPROM
		case _save:
		  if(level){
        MEM_EEPROM_WriteVar(SONIC_L_LV, level & 0x00FF);
        MEM_EEPROM_WriteVar(SONIC_H_LV, ((level >> 8) & 0x00FF));}
      break;

    //------------------------------------------------Meassure
		case _new:
      Sonic_App(D5_ini);			              //Distance Init
      TCF0_WaitSec_Init(1);
      while(!(Sonic_App(R_sreg) & DISA))	  //DistanceAvailable
      {
        Sonic_App(US_exe);
        if(TCF0_Wait_Query()){
          LCD_Sym_NoUS(SetupCal, _write);     //WriteNoUs
          level = 0;
          return 0;}
      }
      level = Sonic_App(R_dreg);	            //ReadReg
      LCD_Sym_NoUS(SetupCal, _clear);         //WriteNoUs
      break;

		case _write:
		  LCD_WriteValue4(17, 40, level);	break;    //WriteInSetupCalPage
		default:									        break;
	}
	return level;
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - SoftwareVersion
 * ------------------------------------------------------------------*/

unsigned char Sonic_sVersion(void)
{
  unsigned char *rec;
  unsigned char ver = 0;
  rec = CAN_SonicVersion(_init);
  while(!rec[0])
  {
    rec = CAN_SonicVersion(_exe);
    if(rec[0] >= 11) return 10;
  }
  switch(rec[1])
  {
    case 0: LCD_WriteStringFont(1,2, "75kHz");  break;
    case 1: LCD_WriteStringFont(1,2, "125kHz"); break;
    case 2: LCD_WriteStringFont(1,2, "Boot  "); break;
    default:                                    break;
  }

  LCD_WriteMyFont(1,52, 21);      //S
  ver = ((rec[2] & 0xF0) >> 4);
	LCD_WriteMyFont(1, 57, ver);
  LCD_WriteMyFont(1,61, 22);      //.
  ver = (rec[2] & 0x0F);
	LCD_WriteMyFont(1, 65, ver);

	return rec[1];
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - LCDData - Bootloader
 * ------------------------------------------------------------------*/

void Sonic_Data_Boot(t_FuncCmd cmd)
{
  TCE1_WaitMilliSec_Init(25);	    //SafetyTimer
  if(cmd == _on)
  {
    if(Sonic_sVersion() != 2)       //Boot
    {
      CAN_TxCmd(_boot);				        //CANTxCmd
      while(CAN_RxACK() != _boot){
        if(TCE1_Wait_Query()){
          LCD_Data_SonicWrite(_noUS, 0);
          return;}}
    }
  }
  else if(cmd == _off)
  {
    if(Sonic_sVersion() == 2)       //Boot
    {
      CAN_TxCmd(_app);				        //CANTxCmd
      while(CAN_RxACK() != _ack){
        if(TCE1_Wait_Query()){
          LCD_Data_SonicWrite(_noUS, 0);
          return;}}
    }
  }
  Sonic_sVersion();
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - LCDData - Bootloader Read
 * ------------------------------------------------------------------*/

void Sonic_Data_BootRead(void)
{
  unsigned char state = 0;

  CAN_SonicReadProgram(_init);
  while((state != 4) && !(state >= 10))
    state = CAN_SonicReadProgram(_exe);

  switch(state)
  {
    case 4:  LCD_Data_SonicWrite(_success, 0);     break;
    case 11: LCD_Data_SonicWrite(_noBoot, 0); break;
    case 12: LCD_Data_SonicWrite(_error, 0);  break;
    case 13: LCD_Data_SonicWrite(_noData, 0); break;
    default: break;
  }
  TCE1_Stop();
}


/* ------------------------------------------------------------------*
 * 						UltraSonic - LCDData - Bootloader Write
 * ------------------------------------------------------------------*/

void Sonic_Data_BootWrite(void)
{
  unsigned char state = 0;

  CAN_SonicWriteProgram(_init);
  while((state != 4) && !(state >= 10))
    state = CAN_SonicWriteProgram(_exe);

  switch(state)
  {
    case 4:  LCD_Data_SonicWrite(_ok, 0);     break;
    case 11: LCD_Data_SonicWrite(_noBoot, 0); break;
    case 12: LCD_Data_SonicWrite(_error, 0);  break;
    default: break;
  }
  TCE1_Stop();
}





/**********************************************************************\
 * End of file
\**********************************************************************/

