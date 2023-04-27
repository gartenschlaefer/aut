// --
// outpust for relais and valves

// include guard
#ifndef OUTPUT_APP_H   
#define OUTPUT_APP_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

// set or clear states
void OUT_SetDown(void);
void OUT_Set_PumpOff(struct PlantState *ps);
void OUT_Clr_PumpOff(struct PlantState *ps);
void OUT_Set_Mud(struct PlantState *ps);
void OUT_Clr_Mud(struct PlantState *ps);
void OUT_Set_Air(struct PlantState *ps);
void OUT_Clr_Air(struct PlantState *ps);
void OUT_Set_Compressor(void);
void OUT_Clr_Compressor(void);
void OUT_Set_Phosphor(void);
void OUT_Clr_Phosphor(void);
void OUT_Set_InflowPump(struct PlantState *ps);
void OUT_Clr_InflowPump(struct PlantState *ps);
void OUT_Clr_IPAir(struct PlantState *ps);

// valves
void OUT_Valve_CloseAll(struct PlantState *ps);
void OUT_Valve_Init(struct PlantState *ps);

#endif