// --
// touch applications

// include guard
#ifndef TOUCH_APP_H   
#define TOUCH_APP_H

/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Touch Matrix
 * ------------------------------------------------------------------*/

unsigned char Touch_Matrix(void);


/* ------------------------------------------------------------------*
 *            Main Linker
 * ------------------------------------------------------------------*/

t_page Touch_AutoLinker(unsigned char matrix, t_page page, int *p_min, int *p_sec);
t_page Touch_ManualLinker(unsigned char matrix, t_page page);
t_page Touch_SetupLinker(unsigned char matrix, t_page page);
t_page Touch_DataLinker(unsigned char matrix, t_page page);
t_page Touch_PinLinker(unsigned char matrix, t_page page);


/* ------------------------------------------------------------------*
 *            Manual Linker
 * ------------------------------------------------------------------*/

void Touch_ManualPumpOffLinker(unsigned char matrix);


/* ------------------------------------------------------------------*
 *            Setup Linker
 * ------------------------------------------------------------------*/

t_page Touch_SetupCirculateLinker(unsigned char matrix, t_page page);
t_page Touch_SetupAirLinker(unsigned char matrix, t_page page);
t_page Touch_SetupWatchLinker(unsigned char matrix, t_page page);
t_page Touch_SetupSetDownLinker(unsigned char matrix, t_page page);
t_page Touch_SetupPumpOffLinker(unsigned char matrix, t_page page);
t_page Touch_SetupMudLinker(unsigned char matrix, t_page page);
t_page Touch_SetupCompressorLinker(unsigned char matrix, t_page page);
t_page Touch_SetupPhosphorLinker(unsigned char matrix, t_page page);
t_page Touch_SetupInflowPumpLinker(unsigned char matrix, t_page page);
t_page Touch_SetupCalLinker(unsigned char matrix, t_page page);
t_page Touch_SetupAlarmLinker(unsigned char matrix, t_page page);
t_page Touch_SetupZoneLinker(unsigned char matrix, t_page page);

/* ------------------------------------------------------------------*
 *            Data Linker
 * ------------------------------------------------------------------*/

t_page Touch_DataAutoLinker(unsigned char matrix, t_page pa);
t_page Touch_DataManualLinker(unsigned char matrix, t_page pa);
t_page Touch_DataSetupLinker(unsigned char matrix, t_page pa);
t_page Touch_DataSonicLinker(unsigned char matrix, t_page page);

#endif