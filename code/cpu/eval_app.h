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

int Eval_SetupPlus(int value, int max);
int Eval_SetupMinus(int value, int min);

void Eval_SetupCircSensorMark(unsigned char sensor);
void Eval_SetupCircTextMark(unsigned char on, unsigned char *p_var);
void Eval_SetupAirTextMark(unsigned char on, unsigned char *p_var);

void Eval_SetupPumpMark(unsigned char mark);
void Eval_SetupWatchMark(t_DateTime time, unsigned char *p_dT);




#endif