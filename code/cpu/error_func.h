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

#define NUM_ERRORS (5)

#define E_T (1 << 0)
#define E_OP (1 << 1)
#define E_UP (1 << 2)
#define E_IT (1 << 3)
#define E_OT (1 << 4)

#define ERROR_ID_T (0)
#define ERROR_ID_OP (1)
#define ERROR_ID_UP (2)
#define ERROR_ID_IT (3)
#define ERROR_ID_OT (4)

#define ERROR_ON_SYM_SEC (2)
#define ERROR_ON_TIME_SEC (5)
#define ERROR_COUNT_NEW_ACTION (250)


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Error_Init(struct PlantState *ps);
void Error_Update(struct PlantState *ps);
void Error_ChangePage(struct PlantState *ps);

void Error_On(struct PlantState *ps);
void Error_Off(struct PlantState *ps);

void Error_Read(struct PlantState *ps);
void Error_Detection_Update(struct PlantState *ps);
void Error_Treatment(struct PlantState *ps, unsigned char error_id);

unsigned char Error_Action_OP_Air(struct PlantState *ps);
unsigned char Error_Action_UP_Air(struct PlantState *ps);

void Error_Action_Temp_SetError(struct PlantState *ps);
void Error_Action_OP_SetError(struct PlantState *ps);
void Error_Action_UP_SetError(struct PlantState *ps);
void Error_Action_IT_SetError(struct PlantState *ps);
void Error_Action_OT_SetError(struct PlantState *ps);
void Error_ModemAction(struct PlantState *ps, unsigned char error);

#endif