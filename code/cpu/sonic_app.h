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

#define SONIC_DISA  (1 << 0)    //Data Distance available
#define SONIC_TEMPA (1 << 1)    //Temp available
#define SONIC_DERR  (1 << 2)    //DistanceError
#define SONIC_TERR  (1 << 3)    //TempError

#define D_LIM (50)


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Sonic_Data_Shot(void);
void Sonic_Data_Auto(void);
void Sonic_Data_Boot(t_FuncCmd cmd);
void Sonic_Data_BootRead(void);
void Sonic_Data_BootWrite(void);

void Sonic_ReadTank(struct PlantState *ps, t_FuncCmd cmd);
unsigned char Sonic_getRepeatTime(t_page page);
void Sonic_ChangePage(struct PlantState *ps, int sonic);
int Sonic_LevelCal(t_FuncCmd cmd);
unsigned char Sonic_sVersion(void);

#endif