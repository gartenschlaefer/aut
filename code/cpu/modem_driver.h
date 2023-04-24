// --
//  Telit-GSM-Modem GC864Quad_v2

// include guard
#ifndef MODEM_DRIVER_H
#define MODEM_DRIVER_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define MO_HANG_UP_TIME   25
#define MO_STARTUP_DELAY  200

#define MO_PW_OFF    (!(PORTF.IN & PIN2_bm))
#define MO_PORT_ON   (PORTF.OUTSET = PIN0_bm)
#define MO_PORT_OFF  (PORTF.OUTCLR = PIN0_bm)

#define CHAR_STX (0x02)
#define CHAR_ETX (0x03)
#define CHAR_CR (0x0D)
#define CHAR_LF (0x0A)
#define CHAR_ESC (0x1B)
#define CHAR_CTR_Z (0x1A)


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void Modem_Port_Init(void);

unsigned char Modem_Check(struct PlantState *ps);
unsigned char Modem_TurnOn(struct Modem *mo);
void Modem_TurnOff (void);

void Modem_ReadPWR(void);
void Modem_ReadSLED(t_page page);
unsigned char Modem_CTS_ready(void);
void Modem_ReadCTS(void);

char Modem_TelNr(t_FuncCmd cmd, struct TelNr nr);

unsigned char Modem_Call(struct TelNr nr);
void Modem_SMS(struct TelNr nr, char msg[]);

void Modem_CallAllNumbers(void);
void Modem_SMSAllNumbers(char msg[]);
void Modem_Alert(char msg[]);

void Modem_SendTest(void);
void Modem_Test(struct PlantState *ps);

void Modem_WriteSMS_Test(char msg[]);
void Modem_DialNumber(void);
void Modem_GetSoftwareversion(void);
void Modem_Shutdown(void);

#endif