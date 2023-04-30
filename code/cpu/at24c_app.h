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

void AT24C_WriteVar(t_at24c_eeprom_var var, unsigned char sData);
unsigned char AT24C_ReadVar(struct TWIState *twi_state, t_at24c_eeprom_var var);
void AT24C_TeleNr_ReadToModem(struct PlantState *ps);
void AT24C_TeleNr_Write(struct TeleNr *tele_nr);

#endif