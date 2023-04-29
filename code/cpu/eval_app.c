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
 *            finds out the the placement of a no entry, return position
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
    case TEXT_BUTTON_auto: startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case TEXT_BUTTON_manual: startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case TEXT_BUTTON_setup: startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
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
    case TEXT_BUTTON_auto:    startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case TEXT_BUTTON_manual:  startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case TEXT_BUTTON_setup:   startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
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
 *          latest entry
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
    case TEXT_BUTTON_auto:    startPa = MEM_AUTO_START_SECTION; endPa = MEM_AUTO_END_SECTION; break;
    case TEXT_BUTTON_manual:  startPa = MEM_MANUAL_START_SECTION; endPa = MEM_MANUAL_END_SECTION; break;
    case TEXT_BUTTON_setup:   startPa = MEM_SETUP_START_SECTION; endPa = MEM_SETUP_END_SECTION; break;
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