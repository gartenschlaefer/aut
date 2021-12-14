// --
// ultrasonic applications

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
 *            Functions
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            UltraSonic - LCD-Data - Shot
 * ------------------------------------------------------------------*/

void Sonic_Data_Shot(void)
{
  unsigned char run = 1;
  unsigned char *rec;

  // Temp
  CAN_SonicQuery(_init, _startTemp);
  while(run)
  {
    rec = CAN_SonicQuery(_exe, 0);

    // error
    if(rec[0] >= _usErrTimeout1)
    {
      LCD_Data_SonicWrite(_noUS, 0);
      run = 0;
    }

    // ok
    else if(rec[0] == _usTempSuccess)
    {
      LCD_Data_SonicWrite(_temp, (rec[1] << 8) | rec[2]);
      run = 0;
    }

    // wrong one
    else if(rec[0] == _usDistSuccess || rec[0] == _usWait)
    {
      CAN_SonicQuery(_init, _startTemp);
    }
  }

  // 5shots
  run = 1;
  CAN_SonicQuery(_init, _5Shots);
  while(run)
  {
    rec = CAN_SonicQuery(_exe, 0);

    // error
    if(rec[0] >= _usErrTimeout1)
    {
      LCD_Data_SonicWrite(_noUS, 0);
      run = 0;
    }

    // ok
    else if(rec[0] == _usDistSuccess)
    {
      LCD_Data_SonicWrite(_shot, (rec[1] << 8) | rec[2]);
      run = 0;
    }

    // wrong one
    else if(rec[0] == _usTempSuccess || rec[0] == _usWait)
    {
      CAN_SonicQuery(_init, _5Shots);
    }
  }
}


/* ------------------------------------------------------------------*
 *            UltraSonic - LCDData - Auto
 * ------------------------------------------------------------------*/

void Sonic_Data_Auto(void)
{
  unsigned char *rec;

  rec = CAN_SonicQuery(_exe, 0);

  // error
  if(rec[0] >= _usErrTimeout1)
  {
    LCD_Data_SonicWrite(_noUS, 0);
  }

  // distance
  else if(rec[0] == _usDistSuccess)
  {
    LCD_Data_SonicWrite(_shot1, (rec[1] << 8) | rec[2]);
    CAN_SonicQuery(_init, _startTemp);
  }

  // temperature
  else if(rec[0] == _usTempSuccess)
  {
    LCD_Data_SonicWrite(_temp1, (rec[1] << 8) | rec[2]);
    CAN_SonicQuery(_init, _5Shots);
  }
}


/* ------------------------------------------------------------------*
 *            UltraSonic - ReadTank
 * ------------------------------------------------------------------*/

t_page Sonic_ReadTank(t_page page, t_FuncCmd cmd)
{
  static unsigned char state = 1;
  static int sonic = 0;
  unsigned char *rec;

  // deactivated sonic
  if(!MEM_EEPROM_ReadVar(SONIC_on)) return page;

  // init
  if(cmd == _init)
  {
    state = 1;
  }

  // exe
  else if(cmd == _exe)
  {
    // read
    if(state == 0)
    {
      rec = CAN_SonicQuery(_exe, 0);

      // error
      if(rec[0] >= _usErrTimeout1)
      {
        CAN_SonicQuery(_init, _5Shots);
        LCD_Sym_NoUS(page, _write);
      }

      // distance
      else if(rec[0] == _usDistSuccess)
      {
        sonic = (rec[1] << 8) | rec[2];
        LCD_Auto_SonicVal(page, sonic);
        page = Sonic_ChangePage(page, sonic);
        LCD_Sym_NoUS(page, _clear);
        state = 1;
      }

      // temperature
      else if(rec[0] == _usTempSuccess || rec[0] == _usWait)
      {
        CAN_SonicQuery(_init, _5Shots);
      }
    }

    // tc init
    else if(state == 1)
    {
      TCF0_WaitSec_Init(2);
      state = 2;
    }

    // next shot
    else if(state >= 2)
    {
      if(TCF0_Wait_Query()) state++;    //2s
      if(state > Sonic_getRepeatTime(page))
      {
        CAN_SonicQuery(_init, _5Shots);
        state = 0;
      }
    }
  }

  // write
  else if(cmd == _write)
  {
    if(!LCD_Sym_NoUS(page, _check)) LCD_Auto_SonicVal(page, sonic);
  }

  return page;
}


/* ------------------------------------------------------------------*
 *            UltraSonic - Repeat Time
 * ------------------------------------------------------------------*/

unsigned char Sonic_getRepeatTime(t_page page)
{
  unsigned char repeat_time = 7;
  switch(page)
  {
      case AutoZone: break;
      case AutoSetDown: repeat_time = 30; break;
      case AutoPumpOff: repeat_time = 7; break;
      case AutoMud: repeat_time = 30; break;

      case AutoAir:
      case AutoCirc: repeat_time = 30; break;

      case AutoAirOff:
      case AutoCircOff: break;

      default: break;
  }
  //*** debug SonicTime*2s
  if(DEBUG) repeat_time = 5;
  return repeat_time;
}


/* ------------------------------------------------------------------*
 *            UltraSonic - ReadTank - ChangePages
 * ------------------------------------------------------------------*/

t_page Sonic_ChangePage(t_page page, int sonic)
{
  static int oldSonic = 0;
  static unsigned char error = 0;
  int zero = 0;
  int lvO2 = 0;
  int lvCi = 0;

  //--------------------------------------------------checkOldValue
  // init
  if(!oldSonic) oldSonic = sonic;

  // limits
  if((sonic > (oldSonic + D_LIM)) || (sonic < (oldSonic - D_LIM)))
  {
    error++;
  }
  else
  {
    error = 0;
    oldSonic = sonic;
  }

  // tries to accept the new distance
  if(error > 4)
  {
    error = 0;
    oldSonic = sonic;
  }
  if(error) return page;

  // percentage
  zero = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) | (MEM_EEPROM_ReadVar(SONIC_L_LV)));
  lvO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | (MEM_EEPROM_ReadVar(TANK_L_O2)));
  lvCi = ((MEM_EEPROM_ReadVar(TANK_H_Circ) << 8) | (MEM_EEPROM_ReadVar(TANK_L_Circ)));

  // change page
  switch(page)
  {
    case AutoZone:
      if(sonic < (zero - (lvO2 * 10)))
        page = AutoSetDown;
      else{
        LCD_Auto_InflowPump(page, 0, _reset);
        LCD_Write_AirVar(page, 0, _reset);
        page = AutoCirc;
        LCD_Write_AirVar(page, 0, _init);}
      break;

    case AutoCirc:
    case AutoCircOff:
      if(sonic < (zero - (lvCi * 10))){
        LCD_Auto_InflowPump(page, 0, _reset);
        LCD_Write_AirVar(page, 0, _reset);
        page = AutoAir;
        LCD_Write_AirVar(page, 0, _init);}
      break;

    case AutoAir:
    case AutoAirOff:
      if(sonic < (zero - (lvO2 * 10))){
        LCD_Auto_InflowPump(page, 0, _reset);
        LCD_Write_AirVar(page, 0, _reset);
        page = AutoSetDown;
        LCD_Write_AirVar(AutoCirc, 0, _init);}
      break;

    default: break;
  }

  return page;
}


/* ------------------------------------------------------------------*
 *            UltraSonic - LevelCal - For Calibration in Setup
 * ------------------------------------------------------------------*/

int Sonic_LevelCal(t_FuncCmd cmd)
{
  static int level = 0;
  unsigned char run = 1;
  unsigned char *rec;

  switch(cmd)
  {
    // read from EEPROM
    case _init:
      level = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) | (MEM_EEPROM_ReadVar(SONIC_L_LV)));
      LCD_WriteValue4(17, 40, level);
      break;

    // save to EEPROM
    case _save:
      if(level){
        MEM_EEPROM_WriteVar(SONIC_L_LV, level & 0x00FF);
        MEM_EEPROM_WriteVar(SONIC_H_LV, ((level >> 8) & 0x00FF));}
      break;

    // measure
    case _new:
      CAN_SonicQuery(_init, _5Shots);
      while(run)
      {
        rec = CAN_SonicQuery(_exe, 0);

        // error
        if(rec[0] >= _usErrTimeout1)
        {
          LCD_Sym_NoUS(SetupCal, _write);
          level = 0;
          run = 0;
        }

        // ok
        else if(rec[0] == _usDistSuccess)
        {
          level = (rec[1] << 8) | rec[2];
          LCD_Sym_NoUS(SetupCal, _clear);
          run = 0;
        }

        // wrong one
        else if(rec[0] == _usTempSuccess || rec[0] == _usWait)
        {
          CAN_SonicQuery(_init, _5Shots);
        }
      }
      break;

    case _write:
      LCD_WriteValue4(17, 40, level); break;
    default: break;
  }
  return level;
}


/* ------------------------------------------------------------------*
 *            UltraSonic - SoftwareVersion
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

  // application version
  switch(rec[1])
  {
    case 0: LCD_WriteStringFont(1, 2, "75kHz");  break;
    case 1: LCD_WriteStringFont(1, 2, "125kHz"); break;
    case 2: LCD_WriteStringFont(1, 2, "Boot  "); break;
    default:                                    break;
  }

  // S
  LCD_WriteMyFont(1, 52, 21);
  ver = ((rec[2] & 0xF0) >> 4);
  LCD_WriteMyFont(1, 57, ver);

  // .
  LCD_WriteMyFont(1,61, 22);
  ver = (rec[2] & 0x0F);
  LCD_WriteMyFont(1, 65, ver);

  return rec[1];
}


/* ------------------------------------------------------------------*
 *            UltraSonic - LCD-Data - Bootloader
 * ------------------------------------------------------------------*/

void Sonic_Data_Boot(t_FuncCmd cmd)
{
  // safety timer
  TCE1_WaitMilliSec_Init(25);

  // sonic boot in data section clicked
  if(cmd == _on)
  {
    // boot
    if(Sonic_sVersion() != 2)
    {
      CAN_TxCmd(_boot);
      while(CAN_RxACK() != _boot){
        if(TCE1_Wait_Query()){
          LCD_Data_SonicWrite(_noUS, 0);
          return;}}
    }
  }
  else if(cmd == _off)
  {
    if(Sonic_sVersion() == 2)
    {
      CAN_TxCmd(_app);
      while(CAN_RxACK() != _ack){
        if(TCE1_Wait_Query()){
          LCD_Data_SonicWrite(_noUS, 0);
          return;}}
    }
  }
  Sonic_sVersion();
}


/* ------------------------------------------------------------------*
 *            UltraSonic - LCD-Data - Bootloader Read
 * ------------------------------------------------------------------*/

void Sonic_Data_BootRead(void)
{
  unsigned char state = 0;

  CAN_SonicReadProgram(_init);
  while((state != 4) && !(state >= 10))
    state = CAN_SonicReadProgram(_exe);

  switch(state)
  {
    case 4:  LCD_Data_SonicWrite(_success, 0); break;
    case 11: LCD_Data_SonicWrite(_noBoot, 0); break;
    case 12: LCD_Data_SonicWrite(_error, 0); break;
    case 13: LCD_Data_SonicWrite(_noData, 0); break;
    default: break;
  }
  TCE1_Stop();
}


/* ------------------------------------------------------------------*
 *            UltraSonic - LCD-Data - Bootloader Write
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