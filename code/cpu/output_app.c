// --
// outpust for relais and ventils


#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"

#include "memory_app.h"
#include "tc_func.h"
#include "port_func.h"


/* ------------------------------------------------------------------*
 *            SetDown
 * ------------------------------------------------------------------*/

void OUT_SetDown(void)
{
  PORT_RelaisClr(R_COMP);     
  PORT_RelaisClr(R_EXT_COMP);
  PORT_RelaisClr(R_INFLOW1);    
  PORT_RelaisClr(R_INFLOW2);
  PORT_RelaisClr(R_CLEARWATER);
}


/* ------------------------------------------------------------------*
 *            PumpOff
 * ------------------------------------------------------------------*/

void OUT_Set_PumpOff(void)
{
  // mammut pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff)))
  {
    PORT_Ventil(OPEN_ClearWater, 0);
    PORT_RelaisSet(R_COMP);
    PORT_RelaisSet(R_EXT_COMP);
  }
  else PORT_RelaisSet(R_CLEARWATER);
}

void OUT_Clr_PumpOff(void)
{
  // relais
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
  PORT_RelaisClr(R_CLEARWATER);

  // mammut pump
  if(!(MEM_EEPROM_ReadVar(PUMP_pumpOff))) PORT_Ventil(CLOSE_ClearWater, 0);
}


/* ------------------------------------------------------------------*
 *            Mud
 * ------------------------------------------------------------------*/

void OUT_Set_Mud(void)
{
  PORT_Ventil(OPEN_MudPump, 0);
  PORT_RelaisSet(R_COMP);
  PORT_RelaisSet(R_EXT_COMP);
}

void OUT_Clr_Mud(void)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
  PORT_Ventil(CLOSE_MudPump, 0);
}


/* ------------------------------------------------------------------*
 *            Air
 * ------------------------------------------------------------------*/

void OUT_Set_Air(void)
{
  PORT_Ventil(OPEN_Air, 0);
  PORT_RelaisSet(R_COMP);
  PORT_RelaisSet(R_EXT_COMP);
}

void OUT_Clr_Air(void)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
  PORT_Ventil(CLOSE_Air, 0);
}


/* ------------------------------------------------------------------*
 *            Compressor
 * ------------------------------------------------------------------*/

void OUT_Set_Compressor(void)
{
  PORT_RelaisSet(R_COMP);
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
    // mammut pump
    case 0:
      PORT_Ventil(OPEN_Reserve, 0);
      PORT_RelaisSet(R_COMP);
      PORT_RelaisSet(R_EXT_COMP);     
      break;

    // ext. pump1
    case 1:
      PORT_RelaisSet(R_INFLOW1);      
      break;

    // ext. pump2
    case 2:
      if(!pumpChange){
        pumpChange = 1;
        PORT_RelaisSet(R_INFLOW1);}
      else{
        pumpChange = 0;
        PORT_RelaisSet(R_INFLOW2);}   
      break;

    default:                          
      break;
  }
}

void OUT_Clr_InflowPump(void)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);

  if(!MEM_EEPROM_ReadVar(PUMP_inflowPump))
    PORT_Ventil(CLOSE_Reserve, 0);
}


/* ------------------------------------------------------------------*
 *            InflowPump and Air
 * ------------------------------------------------------------------*/

void OUT_Clr_IPAir(void)
{
  PORT_RelaisClr(R_COMP);
  PORT_RelaisClr(R_EXT_COMP);
  PORT_RelaisClr(R_INFLOW1);
  PORT_RelaisClr(R_INFLOW2);
  PORT_Ventil(CLOSE_IPAir, 0);
}


/* ------------------------------------------------------------------*
 *            All Off
 * ------------------------------------------------------------------*/

void OUT_CloseOff(void)
{
  PORT_Relais_AllOff();
  PORT_Ventil_AllClose();
}