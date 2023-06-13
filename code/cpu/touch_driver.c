// --
// touch driver

#include "touch_driver.h"

#include "lcd_driver.h"
#include "adc_func.h"
#include "tc_func.h"
#include "memory_app.h"
#include "basic_func.h"


/*-------------------------------------------------------------------*
 *            safes calibration data in EEPROM
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
  LCD_WriteAnyStringFont(f_6x8_p, 2, 0, "Touchpanel Calibrated");
  LCD_WriteAnyStringFont(f_6x8_p, 5, 0, "EEPROM written");
  TCD0_Stop();
}


/*-------------------------------------------------------------------*
 *            calibrate x-axis
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
 *            calibrate y-axis
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
 *            x-space will be calibrated
 * ------------------------------------------------------------------*/

int Touch_Cal_X_Value(float x_space)
{
  // calibration data
  unsigned char max_cal = MEM_EEPROM_ReadVar(TOUCH_X_max);
  unsigned char min_cal = MEM_EEPROM_ReadVar(TOUCH_X_min);

  float x_cal = x_space - min_cal;
  if(x_cal < 0) x_cal = 0;
  x_cal = ((x_cal * 155.0) / max_cal);

  return f_round_int(x_cal);
}


/*-------------------------------------------------------------------*
 *            y-space will be calibrated
 * ------------------------------------------------------------------*/

int Touch_Cal_Y_Value(float y_space)
{
  // calibration data
  unsigned char max_cal = MEM_EEPROM_ReadVar(TOUCH_Y_max);
  unsigned char min_cal = MEM_EEPROM_ReadVar(TOUCH_Y_min);

  float y_cal = y_space - min_cal;
  if(y_cal < 0){ y_cal = 0; }
  y_cal = ((y_cal * 105.0) / max_cal);

  return f_round_int(y_cal);
}


/*-------------------------------------------------------------------*
 *  set all Touch Pins Low, the voltage of the capacitor will be discharged
 * ------------------------------------------------------------------*/

void Touch_Clean(void)
{
  // output config
  PORTA.DIRSET = LEFT | RIGHT | TOP | BOTTOM;

  // set output to low
  PORTA.OUTCLR = LEFT | RIGHT | TOP | BOTTOM;
}


/*-------------------------------------------------------------------*
 *  Set TOP and BOTTOM as Output, TOP = 0, BOTTOM = 1
 *  Set LEFT and RIGHT as Input, read at LEFT (ADC0)
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
 *            read at LEFT (ADC3), convert CH0
 * ------------------------------------------------------------------*/

 int Touch_Y_Read(void)
 {
  // start conversion
  ADCA.CTRLA |= ADC_CH0START_bm;

  while(!(ADCA.INTFLAGS & (1 << ADC_CH0IF_bp)));

  // reset INT-flag
  ADCA.INTFLAGS |= (1 << ADC_CH0IF_bp);

  // data
  int data = ADCA.CH0RES;

  return data;
}


/*-------------------------------------------------------------------*
 *  Set UP Measure through Touch_Y_Measure
 *  read data on LEFT ADC0, clean up measure
 * ------------------------------------------------------------------*/

int Touch_Cal_Y_ReadData(void)
{
  // setup Pins
  Touch_Clean();
  TCC0_Touch_Wait();
  Touch_Y_Measure();
  TCC0_Touch_Wait();

  // read at ADC0
  int yData = Touch_Y_Read();
  Touch_Clean();

  return yData;
}


/*-------------------------------------------------------------------*
 *  Set LEFT and RIGHT as Output, LEFT = 1, RIGTH = 0
 *  Set TOP and BOTTOM as Input, read at TOP (ADC1)
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
 *            read at TOP (ADC2), Convert CH1
 * ------------------------------------------------------------------*/

 int Touch_X_Read(void)
 {
  // start conversion
  ADCA.CTRLA |= ADC_CH1START_bm;

  while(!(ADCA.INTFLAGS & (1 << ADC_CH1IF_bp)));
  ADCA.INTFLAGS |= (1 << ADC_CH1IF_bp);
  int data = ADCA.CH1RES;

  return data;
}


/*-------------------------------------------------------------------*
 *            set up measure, read data at TOP ADC1, clean
 * ------------------------------------------------------------------*/

 int Touch_Cal_X_ReadData(void)
 {
  // setup port
  Touch_Clean();
  TCC0_Touch_Wait();
  Touch_X_Measure();
  TCC0_Touch_Wait();

  // read at ADC0
  int data = Touch_X_Read();
  Touch_Clean();

  return data;
}


/*-------------------------------------------------------------------*
 *            main read
 * ------------------------------------------------------------------*/

void Touch_Read(struct TouchState *touch_state)
{
  // x-measure
  if(touch_state->state == _touch_clean)
  {
    Touch_Clean();
    TCD0_WaitMilliSec_Init(5);
    touch_state->state = _touch_setup_x;
  }

  else if((touch_state->state == _touch_setup_x) && TCD0_Wait_Query())
  {
    Touch_X_Measure();
    TCD0_WaitMilliSec_Init(5);
    touch_state->state = _touch_read_x;
  }

  else if((touch_state->state == _touch_read_x) && TCD0_Wait_Query())
  {
    touch_state->x = (unsigned char)((Touch_X_Read() >> 4) & 0x00FF);
    Touch_Clean();
    TCD0_WaitMilliSec_Init(5);
    touch_state->state = _touch_setup_y;
  }

  // y-measure
  else if((touch_state->state == _touch_setup_y) && TCD0_Wait_Query())
  {
    Touch_Y_Measure();
    TCD0_WaitMilliSec_Init(5);
    touch_state->state = _touch_read_y;
  }

  else if((touch_state->state == _touch_read_y) && TCD0_Wait_Query())
  {
    touch_state->y = (unsigned char)((Touch_Y_Read() >> 4) & 0x00FF);
    Touch_Clean();
    TCD0_Stop();
    touch_state->state = _touch_ready;
  }

  // repeat loop
  else if(touch_state->state == _touch_ready)
  {
    touch_state->state = _touch_clean;
  }
}