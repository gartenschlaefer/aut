// --
// touch applications

// include guard
#ifndef TOUCH_DRIVER_H   
#define TOUCH_DRIVER_H

/* ==================================================================*
 *            Defines
 * ==================================================================*/

#define TOP     (1 << PIN2)
#define BOTTOM  (1 << PIN4)
#define LEFT    (1 << PIN3)
#define RIGHT   (1 << PIN1)

// count of Reads for calibration
#define CAL_READS 200           


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Touch Initialization
 * ------------------------------------------------------------------*/

void Touch_Cal(void);
int Touch_X_Cal_Init(void);
int Touch_Y_Cal_Init(void);

/* ------------------------------------------------------------------*
 *            Touch Calibration
 * ------------------------------------------------------------------*/

int Touch_X_Cal(int xBereich);
int Touch_Y_Cal(int yBereich);

/* ------------------------------------------------------------------*
 *            Touch Measure
 * ------------------------------------------------------------------*/

void Touch_Clean(void);

void Touch_Y_Measure(void);
int Touch_Y_Read(void);
int Touch_Y_ReadData(void);

void Touch_X_Measure(void);
int Touch_X_Read(void);
int Touch_X_ReadData(void);

unsigned char *Touch_Read(void);

#endif