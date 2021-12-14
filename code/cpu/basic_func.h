// --
// basic functions, such as init, watchdog, clock

// include guard
#ifndef BASIC_FUNC_H   
#define BASIC_FUNC_H

/* ==================================================================*
 *            Defines
 * ==================================================================*/

#define WDT_RESET asm("wdr")


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

void Basic_Init(void);
void Basic_Init_Mem(void);
void Clock_Init(void);
void Watchdog_Init(void);

#endif