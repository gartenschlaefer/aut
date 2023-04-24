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
void OUT_Set_PumpOff(void);
void OUT_Clr_PumpOff(void);
void OUT_Set_Mud(void);
void OUT_Clr_Mud(void);
void OUT_Set_Air(void);
void OUT_Clr_Air(void);
void OUT_Set_Compressor(void);
void OUT_Clr_Compressor(void);
void OUT_Set_Phosphor(void);
void OUT_Clr_Phosphor(void);
void OUT_Set_InflowPump(struct PlantState *ps);
void OUT_Clr_InflowPump(void);
void OUT_Clr_IPAir(void);

// valves
void OUT_Valve_CloseAll(void);
void OUT_Valve_Init(void);

#endif