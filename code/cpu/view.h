// --
// view

// include guard
#ifndef VIEW_H   
#define VIEW_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

struct View *View_New(void);
void View_Destroy(struct View *view);
void View_Init(struct View *view);
void View_Update(struct View *view, struct PlantState *ps);
void View_ChangePage(struct View *view, t_page new_page);

void View_AutoPages(struct PlantState *ps);
void View_ManualPages(struct PlantState *ps);
void View_SetupPages(struct PlantState *ps);
void View_DataPages(struct PlantState *ps);

#endif