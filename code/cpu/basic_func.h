// --
// basic functions, such as init, watchdog, clock

// include guard
#ifndef BASIC_FUNC_H   
#define BASIC_FUNC_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define BASIC_WDT_RESET asm("wdr")


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Basic_Init(struct PlantState *ps);
void Basic_Init_Mem(void);
void Basic_Clock_Init(void);
void Basic_Watchdog_Init(void);
void Basic_TimeState_Init(struct PlantState *ps);
void Basic_TimeState_Update(struct PlantState *ps);
unsigned char Basic_CountDown(struct PlantState *ps);

int Basic_LimitAdd(int value, int max);
int Basic_LimitDec(int value, int min);

#endif