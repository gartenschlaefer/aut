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

int MPX_Read(void);
int MPX_ReadCal(void);

int MPX_ReadAverage(t_textButtons page, t_FuncCmd cmd);
int MPX_ReadAverage_Value(void);
int MPX_ReadAverage_UnCal(void);
int MPX_ReadAverage_UnCal_Value(void);

int MPX_LevelCal(t_FuncCmd cmd);
void MPX_ReadTank(struct PlantState *ps, t_FuncCmd cmd);

#endif