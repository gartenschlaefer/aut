// --
// touch applications

// include guard
#ifndef TOUCH_DRIVER_H   
#define TOUCH_DRIVER_H

#include <avr/io.h>

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            Defines
 * ------------------------------------------------------------------*/

#define TOP     (1 << PIN2)
#define BOTTOM  (1 << PIN4)
#define LEFT    (1 << PIN3)
#define RIGHT   (1 << PIN1)

// count of Reads for calibration
#define CAL_READS 200           


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Touch_Cal_Main(void);
int Touch_Cal_X_Init(void);
int Touch_Cal_Y_Init(void);
int Touch_Cal_X_ReadData(void);
int Touch_Cal_Y_ReadData(void);

int Touch_Cal_X_Value(float x_space);
int Touch_Cal_Y_Value(float y_space);

void Touch_Clean(void);
void Touch_Y_Measure(void);
int Touch_Y_Read(void);
void Touch_X_Measure(void);
int Touch_X_Read(void);

void Touch_Read(struct TouchState *touch_state);

#endif