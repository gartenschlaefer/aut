// --
// evaluation and build-in functions

// include guard
#ifndef EVAL_APP_H   
#define EVAL_APP_H

#include "enums.h"


/* ------------------------------------------------------------------*
 *            function headers
 * ------------------------------------------------------------------*/

unsigned char Eval_CountDown(int *cMin, int *cSec);
void Eval_Oxygen(t_FuncCmd cmd, int min);
int Eval_Comp_OpHours (t_FuncCmd cmd);

unsigned char *Eval_Memory_NoEntry(t_textButtons data);
unsigned char *Eval_Memory_OldestEntry(t_textButtons data);
unsigned char *Eval_Memory_LatestEntry(t_textButtons data);

int Eval_SetupPlus(int value, int max);
int Eval_SetupMinus(int value, int min);

void Eval_SetupCircSensorMark(unsigned char sensor);
void Eval_SetupCircTextMark(unsigned char on, unsigned char *p_var);
void Eval_SetupAirTextMark(unsigned char on, unsigned char *p_var);

void Eval_SetupPumpMark(unsigned char mark);
void Eval_SetupWatchMark(t_DateTime time, unsigned char *p_dT);

void Eval_PinWrite(unsigned char pin, unsigned char codePos);
void Eval_PinDel(void);
void Eval_PinClr(unsigned char *pin);

#endif