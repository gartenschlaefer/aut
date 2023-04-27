// --
// evaluation and build-in functions

#include "eval_app.h"

#include "config.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "lcd_sym.h"
#include "memory_app.h"
#include "mcp7941_driver.h"
#include "tc_func.h"



/* ------------------------------------------------------------------*
 *  Eval_SetupPlus/Minus: evaluation of Circulate Page, write variables
 * ------------------------------------------------------------------*/

int Eval_SetupPlus(int value, int max)
{
  if(value < max){ value++; }
  else{ value = value; }
  return value;
}

int Eval_SetupMinus(int value, int min)
{
  if(value > min){ value--; }
  else{ value = value; }
  return value;
}


/* ------------------------------------------------------------------*
 *  Eval_SetupCirculateMark
 * ------------------------------------------------------------------*/

void Eval_SetupCircSensorMark(unsigned char sensor)
{
  switch(sensor)
  {
    case 0:   
      LCD_WriteAnySymbol(s_29x17, 15, 0, p_sensor);
      LCD_FillSpace (15, 39, 4, 31);
      LCD_WriteAnyStringFont(f_6x8_n, 16, 40, "Time:"); 
      break;

    case 1:   
      LCD_WriteAnySymbol(s_29x17, 15, 0, n_sensor);
      LCD_ClrSpace  (15, 39, 4, 31);
      LCD_WriteAnyStringFont(f_6x8_p, 16, 40, "Time:");
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *  Eval_SetupCirculateTextMark: first clear, then mark
 * ------------------------------------------------------------------*/

void Eval_SetupCircTextMark(unsigned char on, unsigned char *p_var)
{
  unsigned char var[4] = {0};
  unsigned char i = 0;

  LCD_ClrSpace(15, 70, 4, 20);

  for(i = 0; i < 4; i++)
  {
    var[i]= *p_var;
    p_var++;
  }

  LCD_OnValue(var[0]);
  LCD_OffValue(var[1]);
  LCD_WriteAnyValue(f_6x8_p, 3, 16, 72, ((var[3] << 8) | var[2]));

  switch (on)
  {
    case 0: LCD_OnValueNeg(var[0]); break;
    case 1: LCD_OffValueNeg(var[1]); break;
    case 2: 
      LCD_FillSpace (15, 70, 4, 20);
      LCD_WriteAnyValue(f_6x8_n, 3, 16, 72, ((var[3] << 8) | var[2])); 
      break;

    default: break;
  }
}


void Eval_SetupAirTextMark(unsigned char on, unsigned char *p_var)
{
  unsigned char var[4] = {0};
  unsigned char i = 0;

  LCD_ClrSpace(15, 39, 4, 51);
  LCD_WriteAnyStringFont(f_6x8_p, 16, 40,"Time:");

  for(i = 0; i < 4; i++)
  {
    var[i] = *p_var;
    p_var++;
  }

  LCD_OnValue(var[0]);
  LCD_OffValue(var[1]);
  LCD_WriteAnyValue(f_6x8_p, 3, 16,72, ((var[3] << 8) | var[2]));

  switch (on)
  {
    case 0: LCD_OnValueNeg(var[0]); break;
    case 1: LCD_OffValueNeg(var[1]); break;
    case 2:
      LCD_FillSpace (15, 70, 4, 20);
      LCD_WriteAnyValue(f_6x8_n, 3, 16,72, ((var[3] << 8) | var[2]));
      break;

    default: break;
  }
}


/* ------------------------------------------------------------------*
 *  Eval_SetupPumpMark: first clear, then mark
 * ------------------------------------------------------------------*/

void Eval_SetupPumpMark(unsigned char mark)
{
  LCD_WriteAnySymbol(s_29x17, 15, 45, p_compressor);
  LCD_WriteAnySymbol(s_19x19, 15, 90, p_pump);
  LCD_WriteAnySymbol(s_35x23, 15, 120, p_pump2);

  switch (mark)
  {
    case 0: LCD_WriteAnySymbol(s_29x17, 15, 45, n_compressor); break;
    case 1: LCD_WriteAnySymbol(s_19x19, 15, 90, n_pump); break;
    case 2: LCD_WriteAnySymbol(s_35x23, 15, 120, n_pump2); break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *  Eval_SetupWatchMark: first clear, then mark
 * ------------------------------------------------------------------*/

void Eval_SetupWatchMark(t_DateTime time, unsigned char *p_dT)
{
  unsigned char i = 0;
  unsigned char var;

  for(i = 5; i < 11; i++)
  {
    LCD_DateTime(i, *p_dT);
    p_dT++;
  }

  p_dT = p_dT-6+time;
  var = *p_dT;

  switch (time)
  {
    case n_h:     LCD_DateTime(n_h, var);     break;
    case n_min:   LCD_DateTime(n_min, var);     break;
    case n_day:   LCD_DateTime(n_day, var);     break;
    case n_month: LCD_DateTime(n_month, var);   break;
    case n_year:  LCD_DateTime(n_year, var);    break;
    default: break;
  }
}


/* ------------------------------------------------------------------*
 *  Eval_Memory_NoAutoEntry(): finds out the the placement of a no entry, return position
 * ------------------------------------------------------------------*/

unsigned char *Eval_Memory_NoEntry(t_text_buttons data)
{
  static unsigned char memCount[3] = {MEM_AUTO_START_SECTION, 0, 0};

  unsigned char *p_count = memCount;
  unsigned char eep = 0;
  unsigned char i = 0;
  unsigned char stop = 0;

  unsigned char startPa = MEM_AUTO_START_SECTION;
  unsigned char endPa = MEM_AUTO_END_SECTION;

  // determine start and end page
  switch(data)
  {
    case Auto:    startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case Manual:  startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case Setup:   startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
    default: break;
  }

  // start page
  memCount[0] = startPa;

  // pages
  for(eep = startPa; eep <= endPa; eep++)
  {
    // update page
    memCount[0] = eep;

    // entries
    for(i = 0; i < 4; i++)
    {
      memCount[1] = i;
      if(!(MEM_EEPROM_ReadData(eep, i, DATA_day)))
      {
        stop = 1;
        break;
      }
    }

    // loop termination if null found
    if(stop)
    {
      // null indicator
      memCount[2] = 1;
      break;
    }

    //No null entries found
    else memCount[2] = 0;
  }

  return p_count;
}


/* ---------------------------------------------------------------*
 *          old entry
 * ---------------------------------------------------------------*/

unsigned char *Eval_Memory_OldestEntry(t_text_buttons data)
{
  static unsigned char old[2] = {MEM_AUTO_START_SECTION, 0};

  unsigned char *p_old = old;
  unsigned char eep = 0;
  unsigned char i = 0;

  unsigned char startPa = MEM_AUTO_START_SECTION;
  unsigned char endPa = MEM_AUTO_END_SECTION;

  unsigned char day = 31;
  unsigned char month = 12;
  unsigned char year = 60;
  unsigned char h = 23;
  unsigned char min =  59;

  unsigned char rDay = 0;
  unsigned char rMonth = 0;
  unsigned char rYear = 0;
  unsigned char rH = 0;
  unsigned char rMin = 0;

  // determine start and end page
  switch(data)
  {
    case Auto:    startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case Manual:  startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case Setup:   startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
    default: break;
  }

  // start page
  old[0] = startPa;

  // pages
  for(eep = startPa; eep <= endPa; eep++)
  {
    // entries
    for(i = 0; i < 4; i++)
    {
      rDay=   MEM_EEPROM_ReadData(eep, i, DATA_day);
      rMonth= MEM_EEPROM_ReadData(eep, i, DATA_month);
      rYear=  MEM_EEPROM_ReadData(eep, i, DATA_year);
      rH=   MEM_EEPROM_ReadData(eep, i, DATA_hour);
      rMin= MEM_EEPROM_ReadData(eep, i, DATA_minute);

      if( (rDay)      &&  (
        (rYear  < year) ||
        (rMonth < month &&  rYear <= year)  ||
        (rDay < day &&  rMonth <= month &&  rYear <= year)  ||
        (rH   < h   &&  rDay <= day   &&  rMonth <= month &&  rYear <= year)  ||
        (rMin <= min  &&  rH <= h     &&  rDay <= day   &&  rMonth <= month &&  rYear <= year)))
        {
            year=   rYear;
            month=  rMonth;
            day=  rDay;
            h=    rH;
            min=  rMin;
            old[0]= eep;
            old[1]= i;    }
    }
  }

  return p_old;
}


/* ---------------------------------------------------------------*
 *          Latest Entry
 * ---------------------------------------------------------------*/

unsigned char *Eval_Memory_LatestEntry(t_text_buttons data)
{
  static unsigned char latest[2] = {MEM_AUTO_START_SECTION, 0};

  unsigned char *p_latest = latest;
  unsigned char eep = 0;
  unsigned char i = 0;

  unsigned char startPa = MEM_AUTO_START_SECTION;
  unsigned char endPa = MEM_AUTO_END_SECTION;

  unsigned char day = 0;
  unsigned char month = 0;
  unsigned char year = 0;
  unsigned char h = 0;
  unsigned char min = 0;

  unsigned char rDay = 0;
  unsigned char rMonth = 0;
  unsigned char rYear = 0;
  unsigned char rH = 0;
  unsigned char rMin = 0;

  // determine start and end page
  switch(data)
  {
    case Auto:    startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case Manual:  startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case Setup:   startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
    default: break;
  }

  // start page
  latest[0] = startPa;

  // pages
  for(eep = startPa; eep <= endPa; eep++)
  {
    // entries
    for(i = 0; i < 4; i++)
    {
      rDay = MEM_EEPROM_ReadData(eep, i, DATA_day);
      rMonth = MEM_EEPROM_ReadData(eep, i, DATA_month);
      rYear = MEM_EEPROM_ReadData(eep, i, DATA_year);
      rH = MEM_EEPROM_ReadData(eep, i, DATA_hour);
      rMin = MEM_EEPROM_ReadData(eep, i, DATA_minute);

      if( (rDay)      &&  (
        (rYear  > year) ||
        (rMonth > month &&  rYear >= year)  ||
        (rDay > day &&  rMonth >= month &&  rYear >= year)  ||
        (rH   > h   &&  rDay >= day   &&  rMonth >= month &&  rYear >= year)  ||
        (rMin >= min  &&  rH >= h     &&  rDay >= day   &&  rMonth >= month &&  rYear >= year)))
        {
          year = rYear;
          month = rMonth;
          day = rDay;
          h = rH;
          min = rMin;
          latest[0] = eep;
          latest[1] = i;
        }
    }
  }

  return p_latest;
}

