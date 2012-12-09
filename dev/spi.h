#ifndef _SPI_H_
#define _SPI_H_

#include "LPC11xx.h" 

static void SPI_PinInit(void);
void  SPI_MasterInit(void);
uint8_t SPI_SendByte(uint8_t data);
uint8_t SPI_ReadByte(uint8_t data);
void SPI_CS_High(void);
void SPI_CS_Low(void);


#endif  /*_SPI_H_*/