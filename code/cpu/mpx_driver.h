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

int MPX_ReadAverage(struct PlantState *ps, t_FuncCmd cmd);
int MPX_ReadAverage_Value(void);
int MPX_ReadAverage_UnCal(struct MPXState *mpx_state);

void MPX_LevelCal(struct PlantState *ps, t_FuncCmd cmd);
void MPX_ReadTank(struct PlantState *ps, t_FuncCmd cmd);

#endif