// --
// touch driver

#include "touch_driver.h"

#include "lcd_driver.h"
#include "adc_func.h"
#include "tc_func.h"
#include "memory_app.h"
#include "basic_func.h"


/*-------------------------------------------------------------------*
 *  safes calibration data in EEPROM
 * ------------------------------------------------------------------*/

void Touch_Cal_Main(void)
{
  int calX = 0;
  int calY = 0;

  //-----------------------------------------------NoTouchValue-----
  LCD_Clean();
  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "Do not touch!");
  LCD_WriteAnyStringFont(f_6x8_p, 5, 0, "If touched, restart!");

  // save no touch value to EEPROM
  MEM_EEPROM_WriteVar(TOUCH_X_min, (Touch_Cal_X_ReadData() >> 4));
  MEM_EEPROM_WriteVar(TOUCH_Y_min, (Touch_Cal_Y_ReadData() >> 4));

  TCD0_WaitSec_Init(2);
  while(!(TCD0_Wait_Query()));

  // watchdog restart
  BASIC_WDT_RESET;

  //-----------------------------------------------TouchValue-----
  LCD_Clean();
  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "Touch me!");
  while(!(TCD0_Wait_Query()));

  // x, y calibration
  BASIC_WDT_RESET;
  calX = Touch_Cal_X_Init();
  BASIC_WDT_RESET;
  calY = Touch_Cal_Y_Init();
  BASIC_WDT_RESET;

  MEM_EEPROM_WriteVar(TOUCH_X_max, (calX >> 4));
  MEM_EEPROM_WriteVar(TOUCH_Y_max, (calY >> 4));

  LCD_Clean();
  LCD_WriteAnyStringFont(f_6x8_p, 2,0,"Touchpanel Calibrated");
  LCD_WriteAnyStringFont(f_6x8_p, 5,0,"EEPROM written");
  TCD0_Stop();
}


/*-------------------------------------------------------------------*
 *  calibrate x-axis
 * ------------------------------------------------------------------*/

int Touch_Cal_X_Init(void)
{
  int calData = 0;
  int xCal = 0;

  Touch_Clean();
  LCD_Clean();
  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "Touchpanel X-Calibrating");
  TCC0_Touch_Wait();

  for(int i = 0; i < CAL_READS; i++)
  {
    calData = Touch_Cal_X_ReadData();
    if(calData > xCal) xCal = calData;
  }

  Touch_Clean();
  return xCal;
}


/*-------------------------------------------------------------------*
 *  calibrate y-axis
 * ------------------------------------------------------------------*/

int Touch_Cal_Y_Init(void)
{
  int calData = 0;
  int yCal = 0;

  Touch_Clean();
  LCD_Clean();
  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "Touchpanel Y-Calibrating");
  TCC0_Touch_Wait();

  for(int i = 0; i < CAL_READS; i++)
  {
    calData = Touch_Cal_Y_ReadData();
    if(calData > yCal) yCal = calData;
  }

  Touch_Clean();
  return yCal;
}


/*-------------------------------------------------------------------*
 *  x-space will be calibrated
 * ------------------------------------------------------------------*/

int Touch_Cal_X_Value(int x_space)
{
  int xCal = 0;
  int maxCal = 0;
  int minCal = 0;

  maxCal = MEM_EEPROM_ReadVar(TOUCH_X_max);
  minCal = MEM_EEPROM_ReadVar(TOUCH_X_min);

  xCal = x_space - minCal;
  if(xCal < 0) xCal = 0;
  xCal = ((xCal * 155) / maxCal);

  return xCal;
}


/*-------------------------------------------------------------------*
 *  y-space will be calibrated
 * ------------------------------------------------------------------*/

int Touch_Cal_Y_Value(int y_space)
{
  int yCal = 0;
  int maxCal = 0;
  int minCal = 0;

  maxCal = MEM_EEPROM_ReadVar(TOUCH_Y_max);
  minCal = MEM_EEPROM_ReadVar(TOUCH_Y_min);

  yCal = y_space - minCal;
  if(yCal < 0)  yCal = 0;
  yCal = (((yCal) * 105) / maxCal);

  return yCal;
}


/*-------------------------------------------------------------------*
 *  Touch_Clean
 * --------------------------------------------------------------
 *  Set all Touch Pins Low,
 *  The Voltage of the capacitor will be decharged
 * ------------------------------------------------------------------*/

void Touch_Clean(void)
{
  // output config
  PORTA.DIRSET = LEFT | RIGHT | TOP | BOTTOM;

  // set output to low
  PORTA.OUTCLR = LEFT | RIGHT | TOP | BOTTOM;
}


/*-------------------------------------------------------------------*
 *  Touch_Y_Measure
 * --------------------------------------------------------------
 *  Set TOP   and BOTTOM  as Output, TOP = 0, BOTTOM=1
 *  Set LEFT  and RIGHT   as Input, read at LEFT (ADC0)
 * ------------------------------------------------------------------*/

void Touch_Y_Measure(void)
{
  // output
  PORTA.DIRSET = TOP | BOTTOM;

  // input
  PORTA.DIRCLR = LEFT | RIGHT;

  // set voltage
  PORTA.OUTCLR = TOP;
  PORTA.OUTSET = BOTTOM;
}


/*-------------------------------------------------------------------*
 *  Touch_Y_Read
 * --------------------------------------------------------------
 *  Wait until Conversion Complete and return data
 *  read at LEFT (ADC3),  Convert CH0
 * ------------------------------------------------------------------*/

 int Touch_Y_Read(void)
 {
  int data = 0;

  // start conversion
  ADCA.CTRLA |= ADC_CH0START_bm;

  while(!(ADCA.INTFLAGS & (1 << ADC_CH0IF_bp)));

  // reset INT-flag
  ADCA.INTFLAGS |= (1 << ADC_CH0IF_bp);

  // data
  data = ADCA.CH0RES;

  return data;
}


/*-------------------------------------------------------------------*
 *  Touch_Cal_Y_ReadData
 * --------------------------------------------------------------
 *  Set UP Measure through Touch_Y_Measure
 *  read data on LEFT ADC0 through  Touch_Y_Read
 *  Clean Up Measure through Touch_Clean
 * ------------------------------------------------------------------*/

int Touch_Cal_Y_ReadData(void)
{
  int yData = 0;

  // setup Pins
  Touch_Clean();
  TCC0_Touch_Wait();
  Touch_Y_Measure();
  TCC0_Touch_Wait();

  // read at ADC0
  yData = Touch_Y_Read();
  Touch_Clean();

  return yData;
}


/*-------------------------------------------------------------------*
 *  Touch_X_Measure
 * --------------------------------------------------------------
 *  Set LEFT  and RIGHT   as Output,  LEFT=1, RIGTH = 0
 *  Set TOP   and BOTTOM  as Input,   read at TOP (ADC1)
 * ------------------------------------------------------------------*/

void Touch_X_Measure(void)
{
  // output
  PORTA.DIRSET = LEFT | RIGHT;

  // input
  PORTA.DIRCLR = TOP | BOTTOM;

  // voltage channel
  PORTA.OUTCLR = LEFT;
  PORTA.OUTSET = RIGHT;
}


/*-------------------------------------------------------------------*
 *  Touch_X_Read
 * --------------------------------------------------------------
 *  Wait until Conversion Complete and return data
 *  read at TOP (ADC2), Convert CH1
 * ------------------------------------------------------------------*/

 int Touch_X_Read(void)
 {
  int data= 0;

  // start conversion
  ADCA.CTRLA |= ADC_CH1START_bm;

  while(!(ADCA.INTFLAGS & (1 << ADC_CH1IF_bp)));
  ADCA.INTFLAGS |= (1 << ADC_CH1IF_bp);
  data = ADCA.CH1RES;

  return data;
}


/*-------------------------------------------------------------------*
 *  Touch_Cal_X_ReadData
 * --------------------------------------------------------------
 *  set up measure, read data at TOP ADC1, clean
 * ------------------------------------------------------------------*/

 int Touch_Cal_X_ReadData(void)
 {
  int xData = 0;

  // setup port
  Touch_Clean();
  TCC0_Touch_Wait();
  Touch_X_Measure();
  TCC0_Touch_Wait();

  // read at ADC0
  xData = Touch_X_Read();
  Touch_Clean();

  return xData;
}


unsigned char *Touch_Read(void)
{
  // state, y, x
  static unsigned char touch[3] = {_clean, 0x00, 0x00 };

  // x-measure
  if(touch[0] == _clean)
  {
    Touch_Clean();
    TCD0_WaitMilliSec_Init(5);
    touch[0] = _read1;
  }

  else if((touch[0] == _read1) && TCD0_Wait_Query())
  {
    Touch_X_Measure();
    TCD0_WaitMilliSec_Init(5);
    touch[0] = _write1;
  }

  else if((touch[0] == _write1) && TCD0_Wait_Query())
  {
    touch[2] = (Touch_X_Read() >> 4);
    Touch_Clean();
    TCD0_WaitMilliSec_Init(5);
    touch[0] = _read2;
  }

  // y-measure
  else if((touch[0] == _read2) && TCD0_Wait_Query())
  {
    Touch_Y_Measure();
    TCD0_WaitMilliSec_Init(5);
    touch[0] = _write2;
  }

  else if((touch[0] == _write2) && TCD0_Wait_Query())
  {
    touch[1] = (Touch_Y_Read() >> 4);
    Touch_Clean();
    TCD0_Stop();
    touch[0] = _ready;
  }

  // repeat loop and safety
  else if(touch[0] == _ready || (touch[0] != _clean && touch[0] != _read1 && touch[0] != _write1 && touch[0] != _read2 && touch[0] != _write2))
  {
    touch[0] = _clean;
  }

  return &touch[0];
}