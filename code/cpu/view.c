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

void View_ChangePage(struct PlantState *ps, struct View *view, t_page new_page)
{
  // segment pages
  // auto pages
  if(f_page_is_auto_page(new_page))
  {
    // set symbols
    LCD_Sym_Auto_SetManager(ps, new_page);

    // update view
    view->f_main_page_view_update = &View_AutoPages; 
  }

  // manual pages
  else if(f_page_is_manual_page(new_page))
  {
    switch(new_page)
    {
      case ManualMain: LCD_Sym_Manual_Main(ps); break;
      case ManualPumpOff: LCD_Sym_Manual_PumpOff_OkButton(true); break;
      case ManualPumpOff_On: LCD_Sym_Manual_PumpOff_OkButton_Clr(); LCD_Sym_Manual_Text(ps); break;
      default: break;
    }

    // page time
    LCD_Sym_Manual_PageTime_Print(ps);

    // update
    view->f_main_page_view_update = &View_ManualPages;
  }

  // setup pages
  else if(f_page_is_setup_page(new_page))
  {
    // update
    view->f_main_page_view_update = &View_SetupPages;

    // page dependent handling
    switch(new_page)
    {
      case SetupMain: LCD_Sym_Setup_Page(); break;
      case SetupCirculate: LCD_Sym_Setup_Circulate(); break;
      case SetupAir: LCD_Sym_Setup_Air(); break;
      case SetupSetDown: LCD_Sym_Setup_SetDown(); break;
      case SetupPumpOff: LCD_Sym_Setup_PumpOff(); break;
      case SetupMud: LCD_Sym_Setup_Mud(); break;
      case SetupCompressor: LCD_Sym_Setup_Compressor(); break;
      case SetupPhosphor: LCD_Sym_Setup_Phosphor(); break;
      case SetupInflowPump: LCD_Sym_Setup_InflowPump(); break;
      case SetupAlarm: LCD_Sym_Setup_Alarm(ps); break;
      case SetupWatch: LCD_Sym_Setup_Watch(); break;
      case SetupZone: LCD_Sym_Setup_Zone(); break;

      // calibration
      case SetupCal: LCD_Sym_Setup_Cal(ps);
      case SetupCalPressure: view->f_main_page_view_update = &View_SetupPagesCal; break;

      default: break;
    }
  }

  // data pages
  else if(f_page_is_data_page(new_page))
  {
    switch(new_page)
    {
      case DataMain: LCD_Sym_Data_Page(ps); break;
      case DataAuto: LCD_Sym_Data_Auto(); break;
      case DataManual: LCD_Sym_Data_Manual(); break;
      case DataSetup: LCD_Sym_Data_Setup(); break;
      case DataSonic: LCD_Sym_Data_Sonic(ps); break;
      default: break;
    }
    view->f_main_page_view_update = &View_DataPages;
  }

  // all other pages
  else
  {
    switch(new_page)
    {
      case PinManual: case PinSetup: LCD_Sym_PinPage(); break;
      default: break;
    }
    view->f_main_page_view_update = &View_DataPages;
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

void View_SetupPages(struct PlantState *ps){ ; }

void View_SetupPagesCal(struct PlantState *ps)
{
  // mpx value
  if(ps->mpx_state->new_mpx_av_flag){ LCD_Sym_Setup_Cal_MPX_AverageValue(ps->mpx_state->actual_mpx_av); } 
}


/* ------------------------------------------------------------------*
 *            view data pages
 * ------------------------------------------------------------------*/

void View_DataPages(struct PlantState *ps){ ; }