// --
// mpx pressure sensor

// include guard
#ifndef MPX_DRIVER_H   
#define MPX_DRIVER_H


/* ==================================================================*
 *            FUNCTIONS API
 * ==================================================================*/

int MPX_Read(void);
int MPX_ReadCal(void);

int MPX_ReadAverage(t_textButtons page, t_FuncCmd cmd);
int MPX_ReadAverage_Value(void);
int MPX_ReadAverage_UnCal(void);
int MPX_ReadAverage_UnCal_Value(void);

int MPX_LevelCal(t_FuncCmd cmd);
t_page MPX_ReadTank(t_page page, t_FuncCmd cmd);

#endif