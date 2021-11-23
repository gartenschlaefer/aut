// --
// usart - serial interface

// Include guard
#ifndef USART_FUNC_H   
#define USART_FUNC_H


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

void USART_Init(void);
int USART_ReadByte(void);
void USART_WriteByte(char write);
void USART_WriteString(char write[]);

unsigned char *USART_Rx_Buffer(t_FuncCmd cmd, char c);

#endif
