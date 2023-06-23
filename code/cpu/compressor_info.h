// --
// compressor

// include guard
#ifndef COMPRESSOR_INFO_H   
#define COMPRESSOR_INFO_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Compressor_Info_Init(struct PlantState *ps);
void Compressor_Info_Reset_CycleO2(struct PlantState *ps);
void Compressor_Info_Reset_OpHours(struct PlantState *ps);
void Compressor_Info_Update(struct PlantState *ps);

#endif