// --
// evaluation and build-in functions

// Include guard
#ifndef EVAL_APP_H   
#define EVAL_APP_H

/* ==================================================================*
 *            Defines
 * ==================================================================*/

typedef enum
{ Oxygen_Count,
  Oxygen_Write2Entry,
  Oxygen_Clear
}t_Oxygen;


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

unsigned char Eval_CountDown    (int *cMin, int *cSec);
void          Eval_Oxygen(t_FuncCmd cmd, int min);


/* ------------------------------------------------------------------*
 *            Pages
 * ------------------------------------------------------------------*/

int Eval_Comp_OpHours (t_FuncCmd cmd);

unsigned char *Eval_Memory_NoEntry(t_textButtons data);
unsigned char *Eval_Memory_OldestEntry(t_textButtons data);
unsigned char *Eval_Memory_LatestEntry(t_textButtons data);

int Eval_SetupPlus    (int value, int max);
int Eval_SetupMinus (int value, int min);

void Eval_SetupCircSensorMark(unsigned char sensor);
void Eval_SetupCircTextMark(unsigned char on, unsigned char *p_var);
void Eval_SetupAirTextMark(unsigned char on, unsigned char *p_var);

void Eval_SetupPumpMark(unsigned char mark);
void Eval_SetupWatchMark(t_DateTime time, unsigned char *p_dT);


/* ------------------------------------------------------------------*
 *            Pin
 * ------------------------------------------------------------------*/

void Eval_PinWrite(unsigned char pin, unsigned char codePos);
void Eval_PinDel(void);
void Eval_PinClr(unsigned char *pin);

#endif

