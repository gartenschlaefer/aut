// --
// error detection and treatment

// include guard
#ifndef ERROR_FUNC_H   
#define ERROR_FUNC_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            Defines - Error Positions
 * ------------------------------------------------------------------*/

#define   E_T     (1 << 0)      //Temperature
#define   E_OP    (1 << 1)      //OverPressure
#define   E_UP    (1 << 2)      //UnderPressure
#define   E_IT    (1 << 3)      //MaxinTank
#define   E_OT    (1 << 4)      //OutTank


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Error_ON(struct LcdBacklight *b);
void Error_OFF(struct LcdBacklight *b);

unsigned char Error_Read(t_page page);

t_page Error_Detection(t_page page, int min, int sec, struct PlantState *plant_state);
struct ErrTreat Error_Treatment(struct ErrTreat treat, struct LcdBacklight *b);

unsigned char Error_Action_OP_Air(t_page page, struct LcdBacklight *b);
unsigned char Error_Action_UP_Air(t_page page, struct LcdBacklight *b);

void Error_Action_Temp_SetError(struct LcdBacklight *b);
void Error_Action_OP_SetError(struct LcdBacklight *b);
void Error_Action_UP_SetError(struct LcdBacklight *b);
void Error_Action_IT_SetError(struct LcdBacklight *b);
void Error_Action_OT_SetError(struct LcdBacklight *b);

void Error_ModemAction(unsigned char error);
void Error_Symbol(unsigned char err);

#endif