// --
// touch applications

// include guard
#ifndef TOUCH_APP_H   
#define TOUCH_APP_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define PIN_SECRET_ENTER_0 (2)
#define PIN_SECRET_ENTER_1 (5)
#define PIN_SECRET_ENTER_2 (8)
#define PIN_SECRET_ENTER_3 (0)

#define PIN_SECRET_COMP_0 (1)
#define PIN_SECRET_COMP_1 (5)
#define PIN_SECRET_COMP_2 (9)
#define PIN_SECRET_COMP_3 (3)

#define PIN_SECRET_TEL1_0 (0)
#define PIN_SECRET_TEL1_1 (0)
#define PIN_SECRET_TEL1_2 (0)
#define PIN_SECRET_TEL1_3 (0)

#define PIN_SECRET_TEL2_0 (0)
#define PIN_SECRET_TEL2_1 (0)
#define PIN_SECRET_TEL2_2 (0)
#define PIN_SECRET_TEL2_3 (1)

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

unsigned char Touch_Matrix(struct TouchState *touch_state);
void Touch_SelectLinker(struct PlantState *ps);

void Touch_Auto_Linker(struct PlantState *ps);
void Touch_Manual_Linker(struct PlantState *ps);
void Touch_Manual_Linker_Select(struct PlantState *ps, t_any_symbol sym);
void Touch_Setup_Linker(struct PlantState *ps);
void Touch_Data_Linker(struct PlantState *ps);
void Touch_Pin_Linker(struct PlantState *ps);
void Touch_Pin_Linker_TeleTemp_AddDigit(struct PlantState *ps, unsigned char digit);

void Touch_Setup_CirculateLinker(struct PlantState *ps);
void Touch_Setup_AirLinker(struct PlantState *ps);
void Touch_Setup_WatchLinker(struct PlantState *ps);
void Touch_Setup_SetDownLinker(struct PlantState *ps);
void Touch_Setup_PumpOffLinker(struct PlantState *ps);
void Touch_Setup_MudLinker(struct PlantState *ps);
void Touch_Setup_CompressorLinker(struct PlantState *ps);
void Touch_Setup_PhosphorLinker(struct PlantState *ps);
void Touch_Setup_InflowPumpLinker(struct PlantState *ps);
void Touch_Setup_CalLinker(struct PlantState *ps);
void Touch_Setup_AlarmLinker(struct PlantState *ps);
void Touch_Setup_ZoneLinker(struct PlantState *ps);

void Touch_Data_AutoLinker(struct PlantState *ps);
void Touch_Data_ManualLinker(struct PlantState *ps);
void Touch_Data_SetupLinker(struct PlantState *ps);
void Touch_Data_SonicLinker(struct PlantState *ps);

void Touch_Matrix_MainLinker(struct PlantState *ps, unsigned char touch_matrix);
void Touch_Matrix_MainLinker_Data(struct PlantState *ps, unsigned char touch_matrix);

#endif