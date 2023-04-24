// --
// twi - two wire interface

// include guard
#ifndef TWI_FUNC_H   
#define TWI_FUNC_H


/* ------------------------------------------------------------------*
 *            defines
 * ------------------------------------------------------------------*/

#define E_TWI_NO_DATA 0x11
#define E_TWI_WAIT    0x12
#define E_TWI_ARBLOST 0x13
#define E_TWI_BUSERR  0x14
#define E_TWI_NACK    0x15
#define E_TWI_NO_SENT 0x16

#define F_TWI_DATA_SENT 0x1A

#define C_TWI_ADDRESS(x)  ((x)  << 1)
#define C_TWI_WRITE       (0x00 << 0)
#define C_TWI_READ        (0x01 << 0)


/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void TWI_Master_Init(void);
unsigned char TWI_Master_Error(void);

void TWI2_Master_Init(void);
unsigned char TWI2_Master_Error(void);

unsigned char *TWI_Master_ReadString(unsigned char address, unsigned char i);
unsigned char TWI_Master_WriteString( unsigned char address, unsigned char *sendData, unsigned char i);
unsigned char *TWI2_Master_ReadString(unsigned char address, unsigned char i);
unsigned char TWI2_Master_WriteString(unsigned char address, unsigned char *sendData, unsigned char i);

unsigned char TWI_Master_Send(unsigned char send);
unsigned char TWI_Master_AddressWriteMode(unsigned char f_address);
void TWI_Master_Reset(void);

unsigned char TWI2_Master_Send(unsigned char send);
unsigned char TWI2_Master_AddressWriteMode(unsigned char f_address);

#endif