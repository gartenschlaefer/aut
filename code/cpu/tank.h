// --
// tank

// include guard
#ifndef TANK_H   
#define TANK_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Tank_Init(struct PlantState *ps);
void Tank_Update(struct PlantState *ps);
void Tank_SetLevel(struct TankState *tank_state, int new_level);
void Tank_Level_Measure_Sonic(struct PlantState *ps);
void Tank_Level_Measure_MPX(struct PlantState *ps);
int Tank_Level_GetPercentage(struct PlantState *ps);
void Tank_ChangePage(struct PlantState *ps);

#endif