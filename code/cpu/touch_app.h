// --
// touch applications

// include guard
#ifndef TOUCH_APP_H   
#define TOUCH_APP_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

unsigned char Touch_Matrix(struct TouchState *touch_state);
void Touch_SelectLinker(struct PlantState *ps);

void Touch_Auto_Linker(struct PlantState *ps);
void Touch_Manual_Linker(struct PlantState *ps);
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

#endif