// --
// mpx pressure sensor

// include guard
#ifndef MPX_DRIVER_H   
#define MPX_DRIVER_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void MPX_Init(struct PlantState *ps);
int MPX_Read(void);
int MPX_ReadCal(void);

int MPX_ReadAverage_Update(struct PlantState *ps);
int MPX_ReadAverage_Value(void);
int MPX_ReadAverage_UnCal(struct MPXState *mpx_state);

void MPX_LevelCal_New(struct PlantState *ps);
void MPX_LevelCal_SaveToEEPROM(struct PlantState *ps);
void MPX_ReadTank(struct PlantState *ps);

#endif