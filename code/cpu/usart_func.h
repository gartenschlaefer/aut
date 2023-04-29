// --
// usart - serial interface

// include guard
#ifndef USART_FUNC_H   
#define USART_FUNC_H

#include "enums.h"
#include "structs.h"

/* ------------------------------------------------------------------*
 *            global variables
 * ------------------------------------------------------------------*/

extern struct USARTState global_usart_state;

/* ------------------------------------------------------------------*
 *            function header
 * ------------------------------------------------------------------*/

void USART_Init(void);
int USART_ReadByte(void);
void USART_WriteByte(char write);
void USART_WriteString(char write[]);

void USART_Rx_Buffer_AddByte(struct USARTState *usart_state, unsigned char data_byte);
void USART_Rx_Buffer_Clear(struct USARTState *usart_state);
unsigned char *USART_Rx_Buffer_Read(struct USARTState *usart_state);

#endif