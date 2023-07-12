// --
// controller

// include guard
#ifndef CONTROLLER_H   
#define CONTROLLER_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

struct Controller *Controller_New(void);
void Controller_Destroy(struct Controller *controller);
void Controller_Init(struct Controller *controller);
void Controller_Update(struct Controller *controller, struct PlantState *ps);
void Controller_ChangePage(struct Controller *controller, t_page new_page);

#endif