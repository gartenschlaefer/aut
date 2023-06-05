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
#define f_round_int(x) ( (int)(x + (x > 0 ? 0.5 : -0.5)) )
#define f_toogle_bool(x) ( (x & (1 << 0)) ^ (1 << 0) )


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

void Basic_LimitAdd_VL(int *value, struct ValueLimit *vl);
void Basic_LimitDec_VL(int *value, struct ValueLimit *vl);

int Basic_LimitAdd(int value, int max);
int Basic_LimitDec(int value, int min);

#endif