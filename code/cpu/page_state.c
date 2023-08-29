// --
// page state

#include "page_state.h"
#include "view.h"
#include "error_func.h"
#include "controller.h"
#include "output_app.h"


/* ------------------------------------------------------------------*
 *            change page
 * ------------------------------------------------------------------*/

void page_state_update(struct PlantState *ps, struct View *view, struct Controller *controller)
{
  // change of page
  if(ps->page_state->change_page_flag)
  {
    t_page new_page = ps->page_state->page;
    Controller_ChangePage(controller, ps, new_page);
    View_ChangePage(ps, view, new_page);
    Error_ChangePage(ps);
    ps->page_state->change_page_flag = false;
  }
}


/* ------------------------------------------------------------------*
 *            change a page
 * ------------------------------------------------------------------*/

void page_state_change_page(struct PlantState *ps, t_page new_page)
{
  t_page previous_page = ps->page_state->page;

  // update previous page
  ps->state_memory->previous_page = previous_page;
  
  // save auto page state
  if(previous_page >= AUTO_PAGE_START && previous_page <= AUTO_PAGE_END)
  {
    // update state memry with auto save page
    page_state_copy(ps->state_memory->auto_save_page_state, ps->page_state);
  }

  // new page
  ps->page_state->page = new_page;
  ps->page_state->change_page_flag = true;
}


/* ------------------------------------------------------------------*
 *            copy
 * ------------------------------------------------------------------*/

void page_state_copy(struct PageState *page_state, struct PageState *new_page_state)
{
  page_state->page = new_page_state->page;
  page_state->page_time->min = new_page_state->page_time->min; 
  page_state->page_time->sec = new_page_state->page_time->sec;
  page_state->change_page_flag = true;
}

/* ------------------------------------------------------------------*
 *            out page set new page
 * ------------------------------------------------------------------*/

void page_state_set_output(struct PlantState *ps, t_page new_page)
{
  // set page specific
  switch(new_page)
  {
    // other auto pages
    case AutoZone: OUT_Set_Air(ps); break;
    case AutoSetDown: OUT_SetDown(ps); break;
    case AutoPumpOff: OUT_Set_PumpOff(ps); break;

    case AutoMud: 
      if(ps->settings->settings_mud->disabled){ ps->page_state->page_time->sec = 1; }
      else{ OUT_Set_Mud(ps); }
      break;

    case AutoCirc: case AutoAir: 
      if(ps->air_circ_state->ac_state == _ac_on){ OUT_Set_Air(ps); }
      else{ if(ps->inflow_pump_state->ip_state == _ip_on){ OUT_Set_InflowPump(ps); } }
      break;

    // manual pages
    case ManualMain: OUT_Valve_Init(ps); break;
    case ManualCirc: if(ps->air_circ_state->ac_state == _ac_on){ OUT_Set_Air(ps); } break;
    case ManualAir: OUT_Set_Air(ps); break;
    case ManualSetDown: OUT_SetDown(ps); break;
    case ManualPumpOff_On: OUT_Set_PumpOff(ps);

    case ManualMud: OUT_Set_Mud(ps); break;
    case ManualCompressor: OUT_Set_Compressor(ps); break;
    case ManualPhosphor: OUT_Set_Phosphor(); break;
    case ManualInflowPump: OUT_Set_InflowPump(ps); break;

    case SetupCalPressure: if(!ps->settings->settings_zone->sonic_on){ OUT_Set_Air(ps); } break;
    default: break;
  }
}

/* ------------------------------------------------------------------*
 *            out page reset old page
 * ------------------------------------------------------------------*/

void page_state_reset_output(struct PlantState *ps, t_page reset_page)
{
  switch(reset_page)
  {
    // auto pages
    case AutoZone: OUT_Clr_Air(ps); break;
    case AutoPumpOff: OUT_Clr_PumpOff(ps); break;
    case AutoMud: if(!ps->settings->settings_mud->disabled){ OUT_Clr_Mud(ps); } break;

    case AutoCirc: case AutoAir: 
      if(ps->air_circ_state->ac_state == _ac_on){ OUT_Clr_Air(ps); }
      else{ if(ps->inflow_pump_state->ip_state == _ip_on){ OUT_Set_InflowPump(ps); } }
      break;

    // manual pages
    case ManualCirc: if(ps->air_circ_state->ac_state == _ac_on){ OUT_Clr_Air(ps); } break;
    case ManualAir: OUT_Clr_Air(ps); break;
    case ManualPumpOff_On: OUT_Clr_PumpOff(ps); break;
    case ManualMud: OUT_Clr_Mud(ps); break;
    case ManualCompressor: OUT_Clr_Compressor(ps); break;
    case ManualPhosphor: OUT_Clr_Phosphor(); break;
    case ManualInflowPump: OUT_Clr_InflowPump(ps); break;

    default: break;
  }
}


void page_state_set_page_time(struct PlantState *ps, t_page new_page)
{
  // handlers
  int *p_min = &ps->page_state->page_time->min;
  int *p_sec = &ps->page_state->page_time->sec;

  // most of the time it is zero
  *p_sec = 0; 

  // set page specific
  switch(new_page)
  {
    // auto pages
    case AutoZone: *p_min = 2; break;
    case AutoSetDown: *p_min = ps->settings->settings_set_down->time_min; break;
    case AutoPumpOff: *p_min = ps->settings->settings_pump_off->on_min; break;
    case AutoMud: *p_min = ps->settings->settings_mud->on_min; *p_sec = ps->settings->settings_mud->on_sec; break;

    case AutoCirc: 
      *p_min = ps->settings->settings_circulate->time_min;
      // circulate start time
      ps->air_circ_state->air_tms->min = ps->settings->settings_circulate->on_min;
      ps->air_circ_state->air_tms->sec = 0;
      ps->air_circ_state->ac_state = _ac_on;
      break;

    case AutoAir: 
      *p_min = ps->settings->settings_air->time_min;
      // air start time
      ps->air_circ_state->air_tms->min = ps->settings->settings_air->on_min;
      ps->air_circ_state->air_tms->sec = 0;
      ps->air_circ_state->ac_state = _ac_on;
      break;

    // manual pages
    case ManualMain: *p_min = 5;
      // save manual entry time
      ps->eeprom_state->time_manual_entry.hou = ps->time_state->tic_hou;
      ps->eeprom_state->time_manual_entry.min = ps->time_state->tic_min;
      break;

    case ManualCirc:
      ps->air_circ_state->air_tms->min = ps->settings->settings_circulate->on_min;
      ps->air_circ_state->air_tms->sec = 0;
      ps->air_circ_state->ac_state = _ac_on;
      *p_min = 60;
      break;

    case ManualAir:
      ps->air_circ_state->air_tms->min = ps->settings->settings_air->on_min;
      ps->air_circ_state->air_tms->sec = 0;
     *p_min = 60;
     break;

    case ManualSetDown: *p_min = 60; break;
    case ManualPumpOff: *p_min = 30; break;
    case ManualPumpOff_On: *p_min = 29; *p_sec = 59; break;
    case ManualMud: *p_min = 5; break;
    case ManualCompressor: *p_min = 0; *p_sec = 7; break;
    case ManualPhosphor: *p_min = 30; break;
    case ManualInflowPump: *p_min = 5; break;

    // setup pages
    case SetupPage: case SetupMain: case SetupCirculate: case SetupAir: case SetupSetDown: case SetupPumpOff: case SetupMud: 
    case SetupCompressor: case SetupPhosphor: case SetupInflowPump: case SetupCal: case SetupCalPressure: case SetupAlarm: case SetupWatch: case SetupZone:
      ps->page_state->page_time->min = 5;
      ps->page_state->page_time->sec = 60;
      break;

    // data pages
    case DataPage: case DataMain: case DataAuto: case DataManual: case DataSetup: case DataSonic: 
    case DataSonicAuto: case DataSonicBoot: case DataSonicBootR: case DataSonicBootW:
      ps->page_state->page_time->min = 5;
      break;

    default: break;
  }
}