// --
// view

#include <stdlib.h>
#include "view.h"
#include "lcd_sym.h"


/* ------------------------------------------------------------------*
 *            new
 * ------------------------------------------------------------------*/

struct View *View_New(void)
{
  // allocate memory
  struct View *view = malloc(sizeof(struct View));
  if(!view){ return NULL; }

  // init
  View_Init(view);

  return view;
}


/* ------------------------------------------------------------------*
 *            destroy view
 * ------------------------------------------------------------------*/

void View_Destroy(struct View *view)
{
  if(!view){ return; }
  free(view);
}


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void View_Init(struct View *view)
{
  view->f_main_page_view_update = &View_DataPages;
}


/* ------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void View_Update(struct View *view, struct PlantState *ps)
{
  // update actual page
  view->f_main_page_view_update(ps);
}


/* ------------------------------------------------------------------*
 *            change page
 * ------------------------------------------------------------------*/

void View_ChangePage(struct View *view, t_page new_page)
{
  // page dependent handling
  switch(new_page)
  {
    // auto pages
    case AutoSetDown: case AutoMud: case AutoPumpOff: case AutoZone: case AutoCirc: case AutoAir: 
      view->f_main_page_view_update = &View_AutoPages; 
      break;

    // manual pages
    case ManualMain: case ManualPumpOff_On: case ManualCirc: case ManualAir: case ManualSetDown: case ManualPumpOff:
    case ManualMud: case ManualCompressor: case ManualPhosphor: case ManualInflowPump: 
      view->f_main_page_view_update = &View_ManualPages;
      break;

    // setup pages
    case SetupCal: case SetupCalPressure: 
      view->f_main_page_view_update = &View_SetupPages;
      break;

    default: break;  
  }
}


/* ------------------------------------------------------------------*
 *            view auto pages
 * ------------------------------------------------------------------*/

void View_AutoPages(struct PlantState *ps)
{
  t_page p = ps->page_state->page;

  // mpx value
  if(ps->mpx_state->new_mpx_av_flag){ LCD_Sym_Auto_MPX_AverageValue(ps->mpx_state->actual_mpx_av); }

  // level perc
  if(ps->tank_state->new_level_flag){ LCD_Sym_Auto_Tank_LevelPerc((bool)(p != AutoPumpOff && p != AutoMud), ps->tank_state->level_perc); }
}


/* ------------------------------------------------------------------*
 *            view manual pages
 * ------------------------------------------------------------------*/

void View_ManualPages(struct PlantState *ps)
{
  // mpx value
  if(ps->mpx_state->new_mpx_av_flag){ LCD_Sym_Manual_MPX_AverageValue(ps->mpx_state->actual_mpx_av); }
}


/* ------------------------------------------------------------------*
 *            view setup pages
 * ------------------------------------------------------------------*/

void View_SetupPages(struct PlantState *ps)
{
  // mpx value
  if(ps->mpx_state->new_mpx_av_flag){ LCD_Sym_Setup_Cal_MPX_AverageValue(ps->mpx_state->actual_mpx_av); } 
}


/* ------------------------------------------------------------------*
 *            view data pages
 * ------------------------------------------------------------------*/

void View_DataPages(struct PlantState *ps)
{
  ;
}