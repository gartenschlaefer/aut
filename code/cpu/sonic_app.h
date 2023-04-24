// --
// ultrasonic applications

// include guard
#ifndef SONIC_APP_H   
#define SONIC_APP_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

// distance available, temp availabel, errors
#define SONIC_DISA  (1 << 0)
#define SONIC_TEMPA (1 << 1)
#define SONIC_DERR  (1 << 2)
#define SONIC_TERR  (1 << 3)

#define D_LIM (50)


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Sonic_Init(struct PlantState *ps);
void Sonic_Data_Shot(struct PlantState *ps);
void Sonic_Data_Auto(struct PlantState *ps);
void Sonic_Data_Boot_On(struct PlantState *ps);
void Sonic_Data_Boot_Off(struct PlantState *ps);
void Sonic_Data_BootRead(struct PlantState *ps);
void Sonic_Data_BootWrite(struct PlantState *ps);

void Sonic_ReadTank(struct PlantState *ps);
unsigned char Sonic_GetRepeatTime(t_page page);
void Sonic_ChangePage(struct PlantState *ps);
void Sonic_LevelCal(struct PlantState *ps);

void Sonic_ReadVersion(struct PlantState *ps);
void Sonic_Query_Dist_Init(struct PlantState *ps);
void Sonic_Query_Temp_Init(struct PlantState *ps);
void Sonic_Query_Dist_Update(struct PlantState *ps);
void Sonic_Query_Temp_Update(struct PlantState *ps);
unsigned char Sonic_ReadProgram(struct PlantState *ps, unsigned char state);
unsigned char Sonic_WriteProgram(struct PlantState *ps, unsigned char state);

#endif