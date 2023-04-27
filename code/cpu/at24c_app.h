// --
// EEPROM AT24C512 applications

// include guard
#ifndef AT24C_APP_H
#define AT24C_APP_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void AT24C_WriteVar(t_EEvar var, unsigned char sData);
unsigned char AT24C_ReadVar(struct TWIState *twi_state, t_EEvar var);

#endif