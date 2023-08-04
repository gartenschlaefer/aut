// --
// basic functions

// include guard
#ifndef UTILS_H   
#define UTILS_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define WDT_RESET asm("wdr")

#define f_round_int(x) ( (int)(x + (x > 0 ? 0.5 : -0.5)) )
#define f_toogle_bool(x) ( (x & (1 << 0)) ^ (1 << 0) )

#define f_min(x, y) ({ typeof (x) _x = (x); typeof (y) _y = (y); (_x < _y ? _x : _y) })
#define f_max(x, y) ({ typeof (x) _x = (x); typeof (y) _y = (y); (_x > _y ? _x : _y) })
#define f_lim(x, y, z) (f_max(f_min(x, z), y) )


/* ------------------------------------------------------------------*
 *            function api
 * ------------------------------------------------------------------*/

int f_limit_add(int value, int max);
int f_limit_dec(int value, int min);

void f_limit_add_vl(int *value, struct ValueLimit *vl);
void f_limit_dec_vl(int *value, struct ValueLimit *vl);

#endif