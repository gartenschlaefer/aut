// --
// ultrasonic applications

// include guard
#ifndef SONIC_APP_H   
#define SONIC_APP_H

/* ==================================================================*
 *            Defines
 * ==================================================================*/

#define SONIC_DISA  (1 << 0)    //Data Distance available
#define SONIC_TEMPA (1 << 1)    //Temp available
#define SONIC_DERR  (1 << 2)    //DistanceError
#define SONIC_TERR  (1 << 3)    //TempError

#define D_LIM 50



/* ==================================================================*
 *            Enumerations
 * ==================================================================*/

typedef enum
{
  US_wait,  US_exe,   US_reset,
  T_ini,    D1_ini,   D5_ini,     //Init
  T_wo,     D1_wo,    D5_wo,      //Working
  T_err,    D_err,                //Error
  R_sreg,   R_dreg,   R_treg      //ReadReg
}t_US;



/* ==================================================================*
 *            FUNCTIONS API
 * ==================================================================*/

void Sonic_Data_Shot(void);
void Sonic_Data_Auto(void);
void Sonic_Data_Boot(t_FuncCmd cmd);
void Sonic_Data_BootRead(void);
void Sonic_Data_BootWrite(void);

t_page Sonic_ReadTank(t_page page, t_FuncCmd cmd);
unsigned char Sonic_getRepeatTime(t_page page);
t_page Sonic_ChangePage(t_page page, int sonic);
int Sonic_LevelCal(t_FuncCmd cmd);
unsigned char Sonic_sVersion(void);

#endif