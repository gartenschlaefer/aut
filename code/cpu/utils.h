// --
// basic functions, such as init, watchdog, clock

// include guard
#ifndef UTILS_H   
#define UTILS_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define f_min(x, y) ({ typeof (x) _x = (x); typeof (y) _y = (y); (_x < _y ? _x : _y) })
#define f_max(x, y) ({ typeof (x) _x = (x); typeof (y) _y = (y); (_x > _y ? _x : _y) })
#define f_lim(x, y, z) (f_max(f_min(x, z), y) )


#endif