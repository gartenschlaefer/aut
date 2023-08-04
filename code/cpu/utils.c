// --
// useful functions

#include "utils.h"

/* ------------------------------------------------------------------*
 *  limit functions
 * ------------------------------------------------------------------*/

int f_limit_add(int value, int max){ if(value < max){ value++; } return value; }
int f_limit_dec(int value, int min){ if(value > min){ value--; } return value; }

void f_limit_add_vl(int *value, struct ValueLimit *vl){ if(*value < vl->max_value){ *value += 1; } }
void f_limit_dec_vl(int *value, struct ValueLimit *vl){ if(*value > vl->min_value){ *value -= 1; } }

