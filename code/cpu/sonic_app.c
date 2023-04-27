// --
// ultrasonic applications

#include "sonic_app.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_sym.h"
#include "lcd_app.h"
#include "tc_func.h"
#include "can_app.h"
#include "memory_app.h"
#include "output_app.h"
#include "at24c_driver.h"
#include "basic_func.h"

/* ------------------------------------------------------------------*
 *            sonic init
 * ------------------------------------------------------------------*/

void Sonic_Init(struct PlantState *ps)
{
  ps->sonic_state->level_cal = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) | (MEM_EEPROM_ReadVar(SONIC_L_LV)));
}


/* ------------------------------------------------------------------*
 *            sonic - LCD-Data - Shot
 * ------------------------------------------------------------------*/

void Sonic_Data_Shot(struct PlantState *ps)
{
  unsigned char run = 1;

  // temp
  Sonic_Query_Temp_Init(ps);

  // update temp request
  while(run)
  {
    // update query
    Sonic_Query_Temp_Update(ps);

    // error
    if(ps->sonic_state->query_state >= _usErrTimeout1)
    {
      LCD_Sym_Data_SonicWrite(_noUS, 0);
      run = 0;
    }

    // ok
    else if(ps->sonic_state->query_state == _usTempSuccess)
    {
      LCD_Sym_Data_SonicWrite(_temp, ps->sonic_state->temp);
      run = 0;
    }
  }

  // 5shots
  run = 1;
  Sonic_Query_Dist_Init(ps);

  // 5 shots request
  while(run)
  {
    Sonic_Query_Dist_Update(ps);

    // error
    if(ps->sonic_state->query_state >= _usErrTimeout1)
    {
      LCD_Sym_Data_SonicWrite(_noUS, 0);
      run = 0;
    }

    // ok
    else if(ps->sonic_state->query_state == _usDistSuccess)
    {
      LCD_Sym_Data_SonicWrite(_shot, ps->sonic_state->d_mm);
      run = 0;
    }
  }
}


/* ------------------------------------------------------------------*
 *            sonic - LCDData - Auto
 * ------------------------------------------------------------------*/

void Sonic_Data_Auto(struct PlantState *ps)
{
  Sonic_Query_Temp_Update(ps);
  Sonic_Query_Dist_Update(ps);

  // error
  if(ps->sonic_state->query_state >= _usErrTimeout1)
  {
    LCD_Sym_Data_SonicWrite(_noUS, 0);
  }

  // distance
  else if(ps->sonic_state->query_state == _usDistSuccess)
  {
    LCD_Sym_Data_SonicWrite(_shot1, ps->sonic_state->d_mm);
    Sonic_Query_Temp_Init(ps);
  }

  // temperature
  else if(ps->sonic_state->query_state == _usTempSuccess)
  {
    LCD_Sym_Data_SonicWrite(_temp1, ps->sonic_state->temp);
    Sonic_Query_Dist_Init(ps);
  }
}


/* ------------------------------------------------------------------*
 *            sonic - ReadTank
 * ------------------------------------------------------------------*/

void Sonic_ReadTank(struct PlantState *ps)
{
  // deactivated sonic
  if(!MEM_EEPROM_ReadVar(SONIC_on)){ return; }

  // read
  if(ps->sonic_state->read_tank_state == SONIC_TANK_listen)
  {
    // update query
    Sonic_Query_Dist_Update(ps);

    // error
    if(ps->sonic_state->query_state >= _usErrTimeout1)
    {
      Sonic_Query_Dist_Init(ps);
      LCD_Sym_Sonic_NoUS(ps->page_state->page, _write);
    }

    // distance
    else if(ps->sonic_state->query_state == _usDistSuccess)
    {
      LCD_Sym_Auto_SonicVal(ps->page_state->page, ps->sonic_state->d_mm);
      Sonic_ChangePage(ps);
      LCD_Sym_Sonic_NoUS(ps->page_state->page, _clear);
      ps->sonic_state->read_tank_state = SONIC_TANK_timer_init;
    }
  }

  // tc init
  else if(ps->sonic_state->read_tank_state == SONIC_TANK_timer_init)
  {
    TCF0_WaitSec_Init(2);
    ps->sonic_state->read_tank_state = 2;
  }

  // next shot
  else if(ps->sonic_state->read_tank_state >= 2)
  {
    if(TCF0_Wait_Query()){ ps->sonic_state->read_tank_state++; }
    if(ps->sonic_state->read_tank_state > Sonic_GetRepeatTime(ps->page_state->page))
    {
      Sonic_Query_Dist_Init(ps);
      ps->sonic_state->read_tank_state = SONIC_TANK_listen;
      TCF0_Stop();
    }
  }
}


/* ------------------------------------------------------------------*
 *            sonic - Repeat Time
 * ------------------------------------------------------------------*/

unsigned char Sonic_GetRepeatTime(t_page page)
{
  unsigned char repeat_time = 30;

  // different repeat times
  switch(page)
  {
      case AutoPumpOff: repeat_time = 10; break;
      case AutoAirOff: 
      case AutoCircOff: repeat_time = 15; break;

      default: break;
  }
  //*** debug SonicTime*2s
  if(DEBUG){ repeat_time = 5; }
  return repeat_time;
}


/* ------------------------------------------------------------------*
 *            sonic - ReadTank - ChangePages
 * ------------------------------------------------------------------*/

void Sonic_ChangePage(struct PlantState *ps)
{
  // sonic handle
  int sonic = ps->sonic_state->d_mm;

  //--------------------------------------------------checkOldValue
  // init
  if(!ps->sonic_state->d_mm_prev){ ps->sonic_state->d_mm_prev = sonic; }

  // limits
  if((sonic > (ps->sonic_state->d_mm_prev + D_LIM)) || (sonic < (ps->sonic_state->d_mm_prev - D_LIM))){ ps->sonic_state->d_error++; }
  else{ ps->sonic_state->d_error = 0; ps->sonic_state->d_mm_prev = sonic; }

  // tries to accept the new distance
  if(ps->sonic_state->d_error > 4)
  {
    ps->sonic_state->d_error = 0;
    ps->sonic_state->d_mm_prev = sonic;
  }
  if(ps->sonic_state->d_error) return;

  // percentage
  int zero = ((MEM_EEPROM_ReadVar(SONIC_H_LV) << 8) | (MEM_EEPROM_ReadVar(SONIC_L_LV)));
  int lvO2 = ((MEM_EEPROM_ReadVar(TANK_H_O2) << 8) | (MEM_EEPROM_ReadVar(TANK_L_O2)));
  int lvCi = ((MEM_EEPROM_ReadVar(TANK_H_Circ) << 8) | (MEM_EEPROM_ReadVar(TANK_L_Circ)));

  // change page
  switch(ps->page_state->page)
  {
    case AutoZone:
      if(sonic < (zero - (lvO2 * 10))){ ps->page_state->page = AutoSetDown; }
      else{ ps->page_state->page = AutoCircOn; }
      break;

    case AutoCircOn:
    case AutoCircOff:
      if(sonic < (zero - (lvCi * 10))){ ps->page_state->page = AutoAirOn; }
      break;

    case AutoAirOn:
    case AutoAirOff:
      if(sonic < (zero - (lvO2 * 10))){ ps->page_state->page = AutoSetDown; }
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            sonic level calibration
 * ------------------------------------------------------------------*/

void Sonic_LevelCal(struct PlantState *ps)
{
  unsigned char run = 1;

  // query calibration
  Sonic_Query_Dist_Init(ps);

  // do until done
  while(run)
  {
    // read query
    Sonic_Query_Dist_Update(ps);

    // error
    if(ps->sonic_state->query_state >= _usErrTimeout1)
    {
      LCD_Sym_Sonic_NoUS(SetupCal, _write);
      ps->sonic_state->level_cal = 0;
      run = 0;
    }

    // ok
    else if(ps->sonic_state->query_state == _usDistSuccess)
    {
      ps->sonic_state->level_cal = ps->sonic_state->d_mm;
      LCD_Sym_Sonic_NoUS(SetupCal, _clear);
      run = 0;
    }
  }

}


/* ------------------------------------------------------------------*
 *            sonic - LCD-Data - Bootloader
 * ------------------------------------------------------------------*/

void Sonic_Data_Boot_On(struct PlantState *ps)
{
  // get sonic version
  Sonic_ReadVersion(ps);

  // safety timer
  TCE1_WaitMilliSec_Init(25);

  // boot
  if(ps->sonic_state->app_type != SONIC_APP_boot)
  {
    // command to activate boot mode
    CAN_TxCmd(CAN_CMD_sonic_boot);
    while(CAN_RxB0_Ack(ps->can_state) != CAN_CMD_sonic_boot)
    {
      CAN_RxB0_Read(ps->can_state);
      if(TCE1_Wait_Query()){ LCD_Sym_Data_SonicWrite(_noUS, 0); break; }
    }
    CAN_RxB0_Clear(ps->can_state);
  }
}

void Sonic_Data_Boot_Off(struct PlantState *ps)
{
  // get sonic version
  Sonic_ReadVersion(ps);

  // safety timer
  TCE1_WaitMilliSec_Init(25);

  // check if in boot application
  if(ps->sonic_state->app_type == SONIC_APP_boot)
  {
    // send command to return to usual application
    CAN_TxCmd(CAN_CMD_sonic_app);
    while(CAN_RxB0_Ack(ps->can_state) != CAN_CMD_sonic_app)
    {
      CAN_RxB0_Read(ps->can_state);
      if(TCE1_Wait_Query()){ LCD_Sym_Data_SonicWrite(_noUS, 0); break;}
    }
    CAN_RxB0_Clear(ps->can_state);
  }
  LCD_Sym_Data_Sonic_ReadSversion(ps);
}


/* ------------------------------------------------------------------*
 *            sonic - LCD-Data - Bootloader Read
 * ------------------------------------------------------------------*/

void Sonic_Data_BootRead(struct PlantState *ps)
{
  unsigned char state = 0;

  // read program
  while((state != 4) && !(state >= 10)){ state = Sonic_ReadProgram(ps, state); }

  // check success
  switch(state)
  {
    case 4: LCD_Sym_Data_SonicWrite(_success, 0); break;
    case 11: LCD_Sym_Data_SonicWrite(_noBoot, 0); break;
    case 12: LCD_Sym_Data_SonicWrite(_error, 0); break;
    case 13: LCD_Sym_Data_SonicWrite(_noData, 0); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            sonic - LCD-Data - Bootloader Write
 * ------------------------------------------------------------------*/

void Sonic_Data_BootWrite(struct PlantState *ps)
{
  unsigned char state = 0;

  // write program
  while((state != 4) && !(state >= 10)){ state = Sonic_WriteProgram(ps, state); }

  // check success
  switch(state)
  {
    case 4: LCD_Sym_Data_SonicWrite(_ok, 0); break;
    case 11: LCD_Sym_Data_SonicWrite(_noBoot, 0); break;
    case 12: LCD_Sym_Data_SonicWrite(_error, 0); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *            Read Software Version
 * ------------------------------------------------------------------*/

void Sonic_ReadVersion(struct PlantState *ps)
{
  // init
  unsigned char err = 0;
  TCE1_WaitMilliSec_Init(25);
  CAN_TxCmd(CAN_CMD_sonic_read_sversion);
  unsigned char run = 1;

  // read and wait
  while(run)
  {
    // read
    CAN_RxB0_Read(ps->can_state);

    // resend cmd
    if(TCE1_Wait_Query())
    {
      err++;
      if(err >= 4){ TCE1_Stop(); return; }
      CAN_RxB0_Clear(ps->can_state);
      CAN_TxCmd(CAN_CMD_sonic_read_sversion);
    }

    // data available
    if(ps->can_state->rxb0_data_av)
    {
      // see if correct request
      if(ps->can_state->rxb0_buffer[2] == CAN_CMD_sonic_read_sversion)
      {
        // update software version
        ps->sonic_state->app_type = ps->can_state->rxb0_buffer[3];
        ps->sonic_state->software_version = ps->can_state->rxb0_buffer[4];
        TCE1_Stop();
        CAN_RxB0_Clear(ps->can_state);
        run = 0;
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            query inits
 * ------------------------------------------------------------------*/

void Sonic_Query_Dist_Init(struct PlantState *ps)
{
  TCE1_WaitMilliSec_Init(25);
  CAN_TxCmd(_usDistReq);
  ps->sonic_state->query_state = _usDistReq;
}

void Sonic_Query_Temp_Init(struct PlantState *ps)
{
  TCE1_WaitMilliSec_Init(25);
  CAN_TxCmd(_usTempReq);
  ps->sonic_state->query_state = _usTempReq;
}


/* ------------------------------------------------------------------*
 *            query temperature
 * ------------------------------------------------------------------*/

void Sonic_Query_Temp_Update(struct PlantState *ps)
{
  // request temp
  if(ps->sonic_state->query_state == _usTempReq)
  {
    // Error check
    if(TCE1_Wait_Query())
    {
      ps->sonic_state->query_error_count++;
      if(ps->sonic_state->query_error_count >= 5){ ps->sonic_state->query_state = _usErrTimeout1; return; }
      CAN_TxCmd(CAN_CMD_sonic_start_temp);
    }

    // check ack
    unsigned char ack = CAN_RxB0_Ack(ps->can_state);

    // temp ack
    if(ack == CAN_CMD_sonic_start_temp)
    {
      ps->sonic_state->query_state = _usTempAckOK;
      CAN_RxB0_Clear(ps->can_state);
      CAN_TxCmd(CAN_CMD_sonic_read_USSREG);
      TCE1_WaitMilliSec_Init(25);
    }

    // working (just wait)
    else if(ack == CAN_CMD_sonic_working)
    {
      ps->sonic_state->query_error_count = 0;
      TCE1_WaitMilliSec_Init(25);
      CAN_RxB0_Clear(ps->can_state);
    }
  }

  // check data availability
  else if(ps->sonic_state->query_state == _usTempAckOK)
  {
    // Error check
    if(TCE1_Wait_Query())
    {
      ps->sonic_state->query_error_count++;
      if(ps->sonic_state->query_error_count >= 5){ ps->sonic_state->query_state = _usErrTimeout2; return; }
      CAN_TxCmd(CAN_CMD_sonic_read_USSREG);
    }

    // Check if Data available
    if(ps->can_state->rxb0_data_av)
    {
      if(ps->can_state->rxb0_buffer[2] == CAN_CMD_sonic_read_USSREG)
      {
        ps->sonic_state->query_error_count = 0;
        CAN_RxB0_Clear(ps->can_state);
        // temp available?
        if(ps->can_state->rxb0_buffer[3] & TEMPA)
        {
          ps->sonic_state->query_state = _usTempAv;
          CAN_TxCmd(CAN_CMD_sonic_read_temp);
          TCE1_WaitMilliSec_Init(25);
        }
      }
    }
  }

  // read data
  else if(ps->sonic_state->query_state == _usTempAv)
  {
    // Error Check
    if(TCE1_Wait_Query())
    {
      ps->sonic_state->query_error_count++;
      if(ps->sonic_state->query_error_count >= 5){ ps->sonic_state->query_state = _usErrTimeout3; return; }
      CAN_TxCmd(CAN_CMD_sonic_read_temp);
    }

    // check data
    if(ps->can_state->rxb0_data_av)
    {
      if(ps->can_state->rxb0_buffer[2] == CAN_CMD_sonic_read_temp)
      {
        ps->sonic_state->temp = (ps->can_state->rxb0_buffer[3] << 8) | ps->can_state->rxb0_buffer[4];
        ps->sonic_state->query_state = _usTempSuccess;
        CAN_RxB0_Clear(ps->can_state);
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            query distance
 * ------------------------------------------------------------------*/

void Sonic_Query_Dist_Update(struct PlantState *ps)
{
  // request temp
  if(ps->sonic_state->query_state == _usDistReq)
  {
    // Error check
    if(TCE1_Wait_Query())
    {
      ps->sonic_state->query_error_count++;
      if(ps->sonic_state->query_error_count >= 5){ ps->sonic_state->query_state = _usErrTimeout1; return; }
      CAN_RxB0_Clear(ps->can_state);
      CAN_TxCmd(CAN_CMD_sonic_start_5shots);
    }

    // check ack
    unsigned char ack = CAN_RxB0_Ack(ps->can_state);

    // temp ack
    if(ack == CAN_CMD_sonic_start_5shots)
    {
      ps->sonic_state->query_state = _usDistAckOK;
      CAN_RxB0_Clear(ps->can_state);
      CAN_TxCmd(CAN_CMD_sonic_read_USSREG);
      TCE1_WaitMilliSec_Init(25);
    }

    // working (just wait)
    else if(ack == CAN_CMD_sonic_working)
    {
      ps->sonic_state->query_error_count = 0;
      TCE1_WaitMilliSec_Init(25);
      CAN_RxB0_Clear(ps->can_state);
    }
  }

  // check data availability
  else if(ps->sonic_state->query_state == _usDistAckOK)
  {
    // Error check
    if(TCE1_Wait_Query())
    {
      ps->sonic_state->query_error_count++;
      if(ps->sonic_state->query_error_count >= 5){ ps->sonic_state->query_state = _usErrTimeout2; return; }
      CAN_TxCmd(CAN_CMD_sonic_read_USSREG);
    }

    // Check if Data available
    if(ps->can_state->rxb0_data_av)
    {
      if(ps->can_state->rxb0_buffer[2] == CAN_CMD_sonic_read_USSREG)
      {
        ps->sonic_state->query_error_count = 0;
        CAN_RxB0_Clear(ps->can_state);
        // distance available?
        if(ps->can_state->rxb0_buffer[3] & DISA)
        {
          ps->sonic_state->query_state = _usDistAv;
          CAN_TxCmd(CAN_CMD_sonic_read_dist);
          TCE1_WaitMilliSec_Init(25);
        }
      }
    }
  }

  // read data
  else if(ps->sonic_state->query_state == _usDistAv)
  {
    // Error Check
    if(TCE1_Wait_Query())
    {
      ps->sonic_state->query_error_count++;
      if(ps->sonic_state->query_error_count >= 5){ ps->sonic_state->query_state = _usErrTimeout3; return; }
      CAN_RxB0_Clear(ps->can_state);
      CAN_TxCmd(CAN_CMD_sonic_read_dist);
    }

    // check data
    if(ps->can_state->rxb0_data_av)
    {
      if(ps->can_state->rxb0_buffer[2] == CAN_CMD_sonic_read_dist)
      {
        ps->sonic_state->d_mm = (ps->can_state->rxb0_buffer[3] << 8) | ps->can_state->rxb0_buffer[4];
        ps->sonic_state->query_state = _usDistSuccess;
        CAN_RxB0_Clear(ps->can_state);
      }
    }
  }
}


/* ------------------------------------------------------------------*
 *            Sonic Read Application Program
 * ------------------------------------------------------------------*/

unsigned char Sonic_ReadProgram(struct PlantState *ps, unsigned char state)
{
  unsigned char data[128];

  // init
  if(state == 0)
  { 
    TCE1_WaitMilliSec_Init(TC_CAN_MS);
    CAN_TxCmd(CAN_CMD_sonic_read_program);
    state = 1;
  }

  // read program cmd ack
  else if(state == 1)
  { 
    // error no boot
    if(TCE1_Wait_Query()){ state = 11; }

    // read program
    if(CAN_RxB0_Ack(ps->can_state) == CAN_CMD_sonic_read_program)
    {
      CAN_RxB0_Clear(ps->can_state);
      TCE1_WaitMilliSec_Init(TC_CAN_MS);
      CAN_TxCmd(CAN_CMD_sonic_read_program);
      state = 2;
    }
  }

  // read application
  else if(state == 2)
  {
    for(int page = 0; page < 32; page++)  //32Pages = 4kB
    {
      LCD_WriteAnyValue(f_4x6_p, 3, 17, 50, page);

      // fill buffer page 128
      for(int byte8 = 0; byte8 < 128; byte8 += 8)
      {
        // wait until data arrives
        while(!ps->can_state->rxb0_data_av)
        {
          if(TCE1_Wait_Query()){ TCE1_Stop(); return state = 12; }
          CAN_RxB0_Read(ps->can_state);
        }

        // check file
        if(!page && !byte8)
        {
          if((ps->can_state->rxb0_buffer[2] != 0x0C) && (ps->can_state->rxb0_buffer[3] != 0x94)){ TCE1_Stop(); return state = 13; }
        }

        // read data
        for(int byte = 0; byte < 8; byte++){ data[byte8 + byte] = ps->can_state->rxb0_buffer[byte + 2]; }

        // safety timer and read program cmd
        TCE1_WaitMilliSec_Init(TC_CAN_MS);
        CAN_RxB0_Clear(ps->can_state);
        CAN_TxCmd(CAN_CMD_sonic_read_program);
      }

      // watchdog restart
      BASIC_WDT_RESET;

      // set page address and write
      int adr = ((AT24C_BOOT_PAGE_OS + page) << 8);
      AT24C_WritePage(adr, &data[0]);
    }
    state = 4;

    // end
    CAN_TxCmd(CAN_CMD_sonic_ack);
    TCE1_Stop();
  }

  return state;
}


/* ------------------------------------------------------------------*
 *            Sonic Write Application Program
 * ------------------------------------------------------------------*/

unsigned char Sonic_WriteProgram(struct PlantState *ps, unsigned char state)
{
  unsigned char tx[10] = {8, 0x01, 0, 1, 2, 3, 4, 5, 6, 7};

  // init
  if(state == 0)
  {
    TCE1_WaitMilliSec_Init(TC_CAN_MS);
    CAN_TxCmd(CAN_CMD_sonic_program);
    state = 1;
  }

  // check bootloader
  else if(state == 1)
  {
    if(TCE1_Wait_Query()){ state = 11; }
    if(CAN_RxB0_Ack(ps->can_state) == CAN_CMD_sonic_program)
    {
      TCE1_WaitMilliSec_Init(TC_CAN_MS);
      state = 2;
    }
  }
  //------------------------------------------------WriteApp
  else if(state == 2)
  {
    for(int page = 0; page < 32; page++)  //32Pages = 4kB
    {
      BASIC_WDT_RESET;
      LCD_WriteAnyValue(f_4x6_p, 3, 17, 50, page);
      //--------------------------------------------Write1EEPage
      for(int byte8 = 0; byte8 < 128; byte8 += 8)
      {
        // TODO (chris#1#): check File
        int adr = (((AT24C_BOOT_PAGE_OS + page) << 8) | byte8);
        unsigned char *p_data = AT24C_Read8Byte(ps->twi_state, adr);
        for(unsigned char i = 0; i < 8; i++)
        {
          tx[i + 2] = p_data[i];
        }
        CAN_TxB0_Write(&tx[0]);
        TCE1_WaitMilliSec_Init(TC_CAN_MS);

        while(CAN_RxB0_Ack(ps->can_state) != CAN_CMD_sonic_program)
        {
          CAN_RxB0_Read(ps->can_state);
          if(TCE1_Wait_Query()){ TCE1_Stop(); return state = 12; }
        }
      }
    }
    state = 4;
    CAN_TxCmd(CAN_CMD_sonic_ack);
    TCE1_Stop();
  }
  return state;
}