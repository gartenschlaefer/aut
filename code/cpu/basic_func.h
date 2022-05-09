// --
// basic functions, such as init, watchdog, clock

// include guard
#ifndef BASIC_FUNC_H   
#define BASIC_FUNC_H

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define BASIC_WDT_RESET asm("wdr")


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Basic_Init(void);
void Basic_Init_Mem(void);
void Clock_Init(void);
void Watchdog_Init(void);

#endif