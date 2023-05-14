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

// valves
void OUT_Valve_Init(struct PlantState *ps);
void OUT_Valve_Wait1(t_valve_action valve_action);
void OUT_Valve_Wait2(t_valve_action valve_action);
void OUT_Valve_Action(struct PlantState *ps, t_valve_action valve);
void OUT_Valve_Update(struct PlantState *ps);
void OUT_Valve_SetOutput(t_valve_action valve_action);
void OUT_Valve_ClrOutput(struct PlantState *ps);

#endif