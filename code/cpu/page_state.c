// --
// page state

#include "page_state.h"
#include "view.h"
#include "error_func.h"
#include "port_func.h"


/* ------------------------------------------------------------------*
 *            change page
 * ------------------------------------------------------------------*/

void page_state_update(struct PlantState *ps, struct View *view)
{
  // change of page
  if(ps->page_state->change_page_flag)
  {
    t_page new_page = ps->page_state->page;
    View_ChangePage(view, new_page);
    PORT_ChangePage(ps, new_page);
    Error_ChangePage(ps);
    ps->page_state->change_page_flag = false;
  }
}


/* ------------------------------------------------------------------*
 *            change a page
 * ------------------------------------------------------------------*/

void page_state_change_page(struct PageState *page_state, t_page new_page)
{
  page_state->page = new_page;
  page_state->change_page_flag = true;
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