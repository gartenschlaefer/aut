/*********************************************************************\
*	Author:			  Christian Walter
\**********************************************************************/

/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

void CAN_Init(void);
void CAN_ClearMOb(void);
void CAN_RXMOb_Init(void);

unsigned char CAN_TXOK(void);
unsigned char CAN_RXOK(void);

void CAN_TXMOB	(unsigned char *object);
unsigned char *CAN_RXMOB	(void);

void CAN_RXMOb_SetAddr(unsigned char addr);




/*********************************************************************\
 * End of file
\**********************************************************************/
