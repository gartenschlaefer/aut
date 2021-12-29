// --
// outputs for relays and valves

#include "SL_Define_sym.h"
#include "SL_TC_func.h"
#include "SL_PORT_func.h"
#include "SL_Memory_app.h"
#include "SL_Output_app.h"


/* ------------------------------------------------------------------*
 *            SetDown
 * ------------------------------------------------------------------*/

void OUT_SetDown(void)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);
  PORT_RelaisClr(R_CLEARWATER);
}


/* ------------------------------------------------------------------*
 *            PumpOff
 * ------------------------------------------------------------------*/

void OUT_Set_PumpOff(void)
{
  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff)))
  {
    PORT_Ventil(OPEN_ClearWater);
    OUT_Set_Compressor();
  }
  else PORT_RelaisSet(R_CLEARWATER);
}

void OUT_Clr_PumpOff(void)
{
  // relays
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_CLEARWATER);

  // mammoth pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))) PORT_Ventil(CLOSE_ClearWater);
}


/* ------------------------------------------------------------------*
 *            Mud
 * ------------------------------------------------------------------*/

void OUT_Set_Mud(void)
{
  PORT_Ventil(OPEN_MudPump);
  OUT_Set_Compressor();
}

void OUT_Clr_Mud(void)
{
  OUT_Clr_Compressor();
  PORT_Ventil(CLOSE_MudPump);
}


/* ------------------------------------------------------------------*
 *            Air
 * ------------------------------------------------------------------*/

void OUT_Set_Air(void)
{
  PORT_Ventil(OPEN_Air);
  OUT_Set_Compressor();
}

void OUT_Clr_Air(void)
{
  OUT_Clr_Compressor();
  PORT_Ventil(CLOSE_Air);
}


/* ------------------------------------------------------------------*
 *            Compressor
 * ------------------------------------------------------------------*/

void OUT_Set_Compressor(void)
{
  PORT_RelaisSet(R_COMP);
  PORT_RelaisSet(R_EXT_COMP);
}

void OUT_Clr_Compressor(void)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
}


/* ------------------------------------------------------------------*
 *            Phosphor
 * ------------------------------------------------------------------*/

void OUT_Set_Phosphor(void)
{
  PORT_RelaisSet(R_PHOSPHOR);
}

void OUT_Clr_Phosphor(void)
{
  PORT_RelaisClr(R_PHOSPHOR);
}


/* ------------------------------------------------------------------*
 *            InflowPump
 * ------------------------------------------------------------------*/

void OUT_Set_InflowPump(void)
{
  static unsigned char pumpChange = 0;
  unsigned char pump = 0;

  pump = MEM_EEPROM_ReadVar(PUMP_inflowPump);
  switch(pump)
  {
    // mammoth pump
    case 0: PORT_Ventil(OPEN_Reserve); OUT_Set_Compressor(); break;

    // ext. pump 1
    case 1: PORT_RelaisSet(R_INFLOW1); break;

    // ext. pump 2
    case 2:
      if(!pumpChange){ pumpChange = 1; PORT_RelaisSet(R_INFLOW1); }
      else{ pumpChange = 0; PORT_RelaisSet(R_INFLOW2); }   
      break;

    default: break;
  }
}

void OUT_Clr_InflowPump(void)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);

  if(!MEM_EEPROM_ReadVar(PUMP_inflowPump)) PORT_Ventil(CLOSE_Reserve);
}


/* ------------------------------------------------------------------*
 *            InflowPump and Air
 * ------------------------------------------------------------------*/

void OUT_Clr_IPAir(void)
{
  OUT_Clr_Compressor();
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);
  PORT_Ventil(CLOSE_IPAir);
}


/* -------------------------------------------------------------------*
 * 						All Off
 * -------------------------------------------------------------------*/

void OUT_CloseOff(void)
{
	PORT_Relais_AllOff();
	PORT_Ventil_AllClose();	
}


/* -------------------------------------------------------------------*
 * 						Test
 * -------------------------------------------------------------------*/

void OUT_TestVentil(void)
{	
	PORT_Ventil(OPEN_Reserve);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_Reserve);
	TC_Ventil_Wait_1s();

	PORT_Ventil(OPEN_MudPump);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_MudPump);
	TC_Ventil_Wait_1s();
	
	PORT_Ventil(OPEN_Air);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_Air);
	TC_Ventil_Wait_1s();
	
	PORT_Ventil(OPEN_ClearWater);
	TC_Ventil_Wait_1s();

	PORT_Ventil(CLOSE_ClearWater);
	TC_Ventil_Wait_1s();

	PORT_Ventil_AllOpen();
}


void OUT_TestVentil2(void)
{
	while(1)
	{
		PORT_Ventil_AllClose2();
		PORT_Ventil_AllOpen2();
	}
}