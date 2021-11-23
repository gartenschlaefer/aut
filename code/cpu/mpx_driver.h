// --
// mpx pressure sensor

// Include guard
#ifndef MPX_DRIVER_H   
#define MPX_DRIVER_H

/* ==================================================================*
 *            Enumeration
 * ==================================================================*/

typedef enum
{
  mpx_PosMin, mpx_Pos1,     mpx_Pos2,   mpx_PosMax,
  mpx_Calc,   mpx_Disabled, mpx_Alarm
}t_MpxPos;


/* ==================================================================*
 *            FUNCTIONS API
 * ==================================================================*/

int MPX_Read(void);
int MPX_ReadCal(void);

int MPX_ReadAverage(t_textButtons page, t_FuncCmd cmd);
int MPX_ReadAverage_Value(void);
int MPX_ReadAverage_UnCal(void);
int MPX_ReadAverage_UnCal_Value(void);

int     MPX_LevelCal(t_FuncCmd cmd);
t_page  MPX_ReadTank(t_page page, t_FuncCmd cmd);

#endif

