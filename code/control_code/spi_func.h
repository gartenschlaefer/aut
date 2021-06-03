// --
// spi

// Include guard
#ifndef SPI_FUNC_H   
#define SPI_FUNC_H

/* ==================================================================*
 *            Defines
 * ==================================================================*/

#define CS_SET    (PORTE.OUTSET= PIN4_bm)
#define CS_CLR    (PORTE.OUTCLR= PIN4_bm)
#define E_SPI   (0x11)
#define F_SPI_SENT  (0x01)


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

void SPI_Init(void);
unsigned char SPI_WriteByte(unsigned char wByte);
unsigned char SPI_ReadByte(void);

#endif
