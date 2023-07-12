// --
// controller

#include <stdlib.h>
#include "controller.h"
#include "lcd_app.h"
#include "config.h"


/* ------------------------------------------------------------------*
 *            new
 * ------------------------------------------------------------------*/

struct Controller *Controller_New(void)
{
  // allocate memory
  struct Controller *controller = malloc(sizeof(struct Controller));
  if(!controller){ return NULL; }

  // init
  Controller_Init(controller);

  return controller;
}


/* ------------------------------------------------------------------*
 *            destroy controller
 * ------------------------------------------------------------------*/

void Controller_Destroy(struct Controller *controller)
{
  if(!controller){ return; }
  free(controller);
}


/* ------------------------------------------------------------------*
 *            init
 * ------------------------------------------------------------------*/

void Controller_Init(struct Controller *controller)
{
  Controller_ChangePage(controller, CONTROL_START_PAGE);
}


/* ------------------------------------------------------------------*
 *            update
 * ------------------------------------------------------------------*/

void Controller_Update(struct Controller *controller, struct PlantState *ps)
{
  // update actual page
  controller->f_controller_update(ps);
}


/* ------------------------------------------------------------------*
 *            change page
 * ------------------------------------------------------------------*/

void Controller_ChangePage(struct Controller *controller, t_page new_page)
{
  // GreatLinker
  switch(new_page)
  {
    // auto pages
    case AutoPage: case AutoZone: case AutoSetDown: case AutoPumpOff: case AutoMud: case AutoCirc: case AutoAir:
      controller->f_controller_update = &LCD_AutoPage; 
      break;

    // manual pages
    case ManualPage: case ManualMain: case ManualCirc: case ManualAir: case ManualSetDown: case ManualPumpOff: case ManualPumpOff_On: case ManualMud:
    case ManualCompressor: case ManualPhosphor: case ManualInflowPump: case ManualValveTest:
      controller->f_controller_update = &LCD_ManualPage;
      break;

    // setup pages
    case SetupPage: case SetupMain: case SetupCirculate: case SetupAir: case SetupSetDown: case SetupPumpOff: case SetupMud: case SetupCompressor:
    case SetupPhosphor: case SetupInflowPump: case SetupCal: case SetupCalPressure: case SetupAlarm: case SetupWatch: case SetupZone:
      controller->f_controller_update = &LCD_SetupPage;
      break;

    // data pages
    case DataPage: case DataMain: case DataAuto: case DataManual: case DataSetup: case DataSonic: case DataSonicAuto: case DataSonicBoot: case DataSonicBootR: case DataSonicBootW:
      controller->f_controller_update = &LCD_DataPage;
      break;

    // pin-pages
    case PinManual: case PinSetup: 
      controller->f_controller_update = &LCD_PinPage_Main; 
      break;

    default: controller->f_controller_update = &LCD_AutoPage; break;
  }
}