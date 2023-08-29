// --
// page state

// include guard
#ifndef PAGE_STATE_H   
#define PAGE_STATE_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void page_state_update(struct PlantState *ps, struct View *view, struct Controller *controller);
void page_state_change_page(struct PlantState *ps, t_page new_page);
void page_state_copy(struct PageState *page_state, struct PageState *new_page_state);

void page_state_set_output(struct PlantState *ps, t_page new_page);
void page_state_reset_output(struct PlantState *ps, t_page reset_page);
void page_state_set_page_time(struct PlantState *ps, t_page new_page);

#endif