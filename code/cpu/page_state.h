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

void page_state_update(struct PlantState *ps, struct View *view);
void page_state_change_page(struct PageState *page_state, t_page new_page);
void page_state_copy(struct PageState *page_state, struct PageState *new_page_state);

#endif