// --
// time state

// include guard
#ifndef TIME_STATE_H   
#define TIME_STATE_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void TimeState_Init(struct PlantState *ps);
void TimeState_Update(struct PlantState *ps);

void TimeState_TicSecUpdate(struct PlantState *ps);


#endif