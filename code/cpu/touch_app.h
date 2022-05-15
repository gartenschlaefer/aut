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

unsigned char Touch_Matrix(void);
void Touch_SelectLinker(struct PlantState *ps);

void Touch_AutoLinker(struct PlantState *ps);
void Touch_ManualLinker(struct PlantState *ps);
void Touch_SetupLinker(struct PlantState *ps);
void Touch_DataLinker(struct PlantState *ps);
void Touch_PinLinker(struct PlantState *ps);

void Touch_SetupCirculateLinker(struct PlantState *ps);
void Touch_SetupAirLinker(struct PlantState *ps);
void Touch_SetupWatchLinker(struct PlantState *ps);
void Touch_SetupSetDownLinker(struct PlantState *ps);
void Touch_SetupPumpOffLinker(struct PlantState *ps);
void Touch_SetupMudLinker(struct PlantState *ps);
void Touch_SetupCompressorLinker(struct PlantState *ps);
void Touch_SetupPhosphorLinker(struct PlantState *ps);
void Touch_SetupInflowPumpLinker(struct PlantState *ps);
void Touch_SetupCalLinker(struct PlantState *ps);
void Touch_SetupAlarmLinker(struct PlantState *ps);
void Touch_SetupZoneLinker(struct PlantState *ps);

void Touch_DataAutoLinker(struct PlantState *ps);
void Touch_DataManualLinker(struct PlantState *ps);
void Touch_DataSetupLinker(struct PlantState *ps);
void Touch_DataSonicLinker(struct PlantState *ps);

#endif