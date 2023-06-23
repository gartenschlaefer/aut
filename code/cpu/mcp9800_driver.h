// --
//  MCP9800 Temperature Sensor

// include guard
#ifndef MCP_9800_DRIVER_H   
#define MCP_9800_DRIVER_H

#include <avr/io.h>
#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define TEMP_UPDATE_SEC_MOD (5)

// PORTC, TWI PC0 and PC1
#define MCP_PORT  (PORTC)
#define MCP_ALERT (1 << 2)


/* ------------------------------------------------------------------*
 *            TWI Address
 * ------------------------------------------------------------------*/

#define MCP_ADDR  (0x48)


/* ------------------------------------------------------------------*
 *            Register Pointer
 * ------------------------------------------------------------------*/

#define MCP_TAREG     (0x00)        //Temperature Ambient
#define MCP_CONFREG   (0x01)        //Configuration
#define MCP_HYSTREG   (0x02)        //Hysteresis
#define MCP_LIMREG    (0x00)        //Temp Limit


/* ------------------------------------------------------------------*
 *            Alert
 * ------------------------------------------------------------------*/

#define MCP_LIMIT   (0x50)        //80°C
#define MCP_HYST    (0x4B)        //75°C


/* ------------------------------------------------------------------*
 *            Config REG
 * ------------------------------------------------------------------*/

#define MCP_ONESHOT_ON    (1 << 7)
#define MCP_ONESHOT_OFF   (0 << 7)
#define MCP_SHUTDOWN_ON   (1 << 0)
#define MCP_SHUTDOWN_OFF  (0 << 0)

#define MCP_RES           (1 << 5)      //10Bit Resolution
#define MCP_FAULTQ        (1 << 3)      //2 Fault Queue
#define MCP_ALERTPOL      (0 << 2)      //Alert Aktive Low
#define MCP_COMP_INT      (0 << 1)      //Comparator Mode


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void MCP9800_Init(struct PlantState *ps);
void MCP9800_Temp_Update(struct PlantState *ps);
void MCP9800_OneShot(void);
unsigned char MCP9800_ReceiveByte(struct TWIState *twi_state, unsigned char byte);
void MCP9800_SendByte(unsigned char byte, unsigned char sData);
void MCP9800_WriteTemp(struct TWIState *twi_state);

#endif