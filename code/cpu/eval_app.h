// --
// evaluation and build-in functions

// include guard
#ifndef EVAL_APP_H   
#define EVAL_APP_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            function headers
 * ------------------------------------------------------------------*/

unsigned char *Eval_Memory_NoEntry(t_text_buttons data);
unsigned char *Eval_Memory_OldestEntry(t_text_buttons data);
unsigned char *Eval_Memory_LatestEntry(t_text_buttons data);

#endif