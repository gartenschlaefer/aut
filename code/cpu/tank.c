// --
// tank

#include "tank.h"
#include "page_state.h"


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Tank_Init(struct PlantState *ps)
{
  bool sonic_on = ps->settings->settings_zone->sonic_on;

  // tank measure
  ps->tank_state->f_tank_level_measure = (sonic_on ? &Tank_Level_Measure_Sonic : &Tank_Level_Measure_MPX);
  ps->tank_state->level_abs_zero_mm = (sonic_on ? ps->settings->settings_calibration->tank_level_min_sonic : ps->settings->settings_calibration->tank_level_min_pressure * 10);
}


/* ------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void Tank_Update(struct PlantState *ps)
{
  // handlers
  struct TankState *tank_state = ps->tank_state;

  // reset level flag
  tank_state->new_level_flag = false;
  tank_state->change_page_flag = false;

  // measure level
  tank_state->f_tank_level_measure(ps);

  // new level
  if(tank_state->new_level_flag)
  {
    // calculate percentage
    ps->tank_state->level_perc = Tank_Level_GetPercentage(ps);
    t_page p = ps->page_state->page;
    tank_state->change_page_flag = (p == AutoCirc || p == AutoAir || p == AutoZone) && tank_state->change_page_flag;
  }

  // change page
  if(tank_state->change_page_flag)
  {
    Tank_ChangePage(ps);
  }
}


/* ------------------------------------------------------------------*
 *            tank level set
 * ------------------------------------------------------------------*/

void Tank_SetLevel(struct TankState *tank_state, int new_level)
{
  tank_state->level_mm = new_level;
  tank_state->new_level_flag = true;
}


/* ------------------------------------------------------------------*
 *            tank measures
 * ------------------------------------------------------------------*/

void Tank_Level_Measure_Sonic(struct PlantState *ps)
{
  // --
  // measure model:
  // in ultra sonic, but reduces distance if water level increases

  // new distance
  if(!ps->sonic_state->new_distance_flag){ return; }

  // change page flag rules
  ps->tank_state->change_page_flag = true;

  // sonic handle
  //int sonic = ps->sonic_state->d_mm;

  // todo: make ist more save
  // init
  //if(!ps->sonic_state->d_mm_prev){ ps->sonic_state->d_mm_prev = sonic; }

  // // limits
  // if((sonic > (ps->sonic_state->d_mm_prev + D_LIM)) || (sonic < (ps->sonic_state->d_mm_prev - D_LIM))){ ps->sonic_state->d_error++; }
  // else{ ps->sonic_state->d_error = 0; ps->sonic_state->d_mm_prev = sonic; }

  // // tries to accept the new distance
  // if(ps->sonic_state->d_error > 4)
  // {
  //   ps->sonic_state->d_error = 0;
  //   ps->sonic_state->d_mm_prev = sonic;
  // }
  // if(ps->sonic_state->d_error){ return; }

  // relative measure 
  int level_rel_mm = ps->tank_state->level_abs_zero_mm - ps->sonic_state->d_mm;

  // set tank level
  Tank_SetLevel(ps->tank_state, level_rel_mm);
}


void Tank_Level_Measure_MPX(struct PlantState *ps)
{
  // --
  // measure model:
  // 10m = 1bar -> 1m = 100mbar -> 1cm = 1mbar

  // no update rules
  if(!ps->compressor_state->is_on_flag){ return; }
  if(!ps->mpx_state->new_mpx_av_flag){ return; }

  // change page flag rules
  ps->tank_state->change_page_flag = (bool)ps->settings->settings_circulate->sensor_in_tank;

  // relative measure 
  int level_rel_mm = 10 * ps->mpx_state->actual_mpx_av - ps->tank_state->level_abs_zero_mm;

  // set tank level
  Tank_SetLevel(ps->tank_state, level_rel_mm);
}


/* ------------------------------------------------------------------*
 *            level percentages
 * ------------------------------------------------------------------*/

int Tank_Level_GetPercentage(struct PlantState *ps)
{
  // get level
  int level_rel_mm = ps->tank_state->level_mm;
  int level_rel_mm_relu = (level_rel_mm <= 0 ? 0 : level_rel_mm);

  // percentage calculation
  return (int)((10 * level_rel_mm_relu) / (ps->settings->settings_zone->level_to_set_down));
}


/* ------------------------------------------------------------------*
 *            change page
 * ------------------------------------------------------------------*/

void Tank_ChangePage(struct PlantState *ps)
{
  // variables
  int level_rel_mm = ps->tank_state->level_mm;
  int level_to_set_down_mm = ps->settings->settings_zone->level_to_set_down * 10;
  int level_to_air_mm = ps->settings->settings_zone->level_to_air * 10;

  // change page
  switch(ps->page_state->page)
  {
    case AutoZone:
      page_state_change_page(ps, (level_rel_mm >= level_to_set_down_mm ? AutoSetDown : AutoCirc));
      break;

    case AutoCirc:
      if(level_rel_mm >= level_to_air_mm){ page_state_change_page(ps, AutoAir); }
      break;

    case AutoAir:
      if(level_rel_mm >= level_to_set_down_mm){ page_state_change_page(ps, AutoSetDown); }
      break;

    default: break;
  }
}