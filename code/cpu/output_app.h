// --
// outpust for relais and ventils

// include guard
#ifndef OUTPUT_APP_H   
#define OUTPUT_APP_H

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

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
void OUT_Set_InflowPump(void);
void OUT_Clr_InflowPump(void);
void OUT_Clr_IPAir(void);
void OUT_CloseOff(void);
void OUT_Init_Valves(void);

#endif