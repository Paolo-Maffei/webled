/*********************************************************************************************************
** SPI0 的基本驱动程序
*********************************************************************************************************/

#include "spi.h"                                                   /* LPC11xx外设寄存器            */

/*********************************************************************************************************
** Function name:       SPI_PinInit
** Descriptions:        SPI0\SSP1引脚初始化函数
** input parameters:     无
** output parameters:    无
*********************************************************************************************************/
static void SPI_PinInit(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);                             /* 配置IOCON模块时钟            */
    /* 初始化SPI0引脚               */
    LPC_IOCON->PIO2_2 |= 0x00;                                          /* CS gpio*/
    LPC_GPIO2->DIR    |= 0x04;                                          /* direction:output */
    LPC_GPIO2->DATA   |= 0x04;                                          /*init high level*/
    
    LPC_IOCON->SCK_LOC |= 0x01;                                        /*  P2.11配置为SCK               */
    LPC_IOCON->PIO2_11 |= 0x01;                                          /* SCK */
    LPC_IOCON->PIO0_8 |= 0x01;                                          /* MISO*/
    LPC_IOCON->PIO0_9 |= 0x01;                                          /* MOSI*/
}

/*********************************************************************************************************
** Function name：      SPI_MasterInit()
** Descriptions:        将SPI0控制器设置为主机SPI。
** input parameters：   无
** output parameters：  无
** Returned value:      无
*********************************************************************************************************/
void  SPI_MasterInit(void)
{
    SPI_PinInit();                                                     /*初始化SPI0使用的IO引脚        */
    
    LPC_SYSCON->PRESETCTRL      |= 0x01;                                /* 禁止SPI0复位                 */
    LPC_SYSCON->SYSAHBCLKCTRL   |= (1 << 11);                           /* 打开SPI0外设                 */
    LPC_SYSCON->SSP0CLKDIV       = 0x03;                                /* SSP时钟分频,48M/3=16M         */

    LPC_SSP0->CR0 = (0x00 << 8) |                                       /* SCR  设置SPI时钟分频         */
                    (0x00 << 7) |                                       /* CPHA 时钟输出相位,           */
                                                                        /* 仅SPI模式有效                */
                    (0x00 << 6) |                                       /* CPOL 时钟输出极性,           */
                                                                        /* 仅SPI模式有效                */
                    (0x00 << 4) |                                       /* FRF  帧格式 00=SPI,01=SSI,   */
                                                                        /* 10=Microwire,11=保留         */
                    (0x07 << 0);                                        /* DSS  数据长度,0000-0010=保留,*/
                                                                        /* 0011=4位,0111=8位,1111=16位  */

    LPC_SSP0->CR1 = (0x00 << 3) |                                       /* SOD  从机输出禁能,1=禁止     */
                    (0x00 << 2) |                                       /* MS   主从选择,0=主机,1=从机  */
                    (0x01 << 1) |                                       /* SSE  SSP使能                 */
                    (0x00 << 0);                                        /* LBM  回写模式                */

    LPC_SSP0->CPSR = 2;                                                 /* PCLK分频值 16M/2=8M                  */
//    LPC_SSP0->ICR  = 0x03;                                              /* 中断清除寄存器               */

//   NVIC_EnableIRQ(SSP0_IRQn);
//   NVIC_SetPriority(SSP0_IRQn, 2);                                     /*  中断使能并设置优先级        */
//    LPC_SSP0->IMSC |= 0x01 << 2;                                        /*  中断使能寄存器              */
} 

/*********************************************************************************************************
** Function name：      SPI_SendData()
** Descriptions：       SSP接口向SPI总线发送数据。
** input parameters：   data        待发送的数据
** output parameters：  返回值为读取的数据
** Returned value:      无
*********************************************************************************************************/
uint8_t SPI_SendByte(uint8_t data)
{  
   /* Move on only if NOT busy and TX FIFO not full. */
    while ( (LPC_SSP0->SR & (1<<1 | 1<<4)) != (1<<1) );
    LPC_SSP0->DR = data;
    /* Move on only if NOT busy and RX FIFO not empty. */
    while ( (LPC_SSP0->SR & (1<<4 | 1<<2)) != (1<<2) );  
    return(LPC_SSP0->DR);
}

/*********************************************************************************************************
** Function name：      SPI_ReadData()
** Descriptions：       SSP接口从SPI总线读取数据，和写数据的实现一样。
** input parameters：   data        待发送的数据
** output parameters：  返回值为读取的数据
** Returned value:      无
*********************************************************************************************************/
uint8_t SPI_ReadByte(uint8_t data)
{  
   /* Move on only if NOT busy and TX FIFO not full. */
    while ( (LPC_SSP0->SR & (1<<1 | 1<<4)) != (1<<1) );
    LPC_SSP0->DR = data;
    /* Move on only if NOT busy and RX FIFO not empty. */
    while ( (LPC_SSP0->SR & (1<<4 | 1<<2)) != (1<<2) );  
    return(LPC_SSP0->DR);
}

/*********************************************************************************************************
** Function name：      void SPI_CS_High(void)
** Descriptions：       SSP接口CS拉高函数。
*********************************************************************************************************/
void SPI_CS_High(void)
{
   LPC_GPIO2->DATA   |= 0x04;
//   DelayUs(1);
}

/*********************************************************************************************************
** Function name：      void SPI_CS_Low(void)
** Descriptions：       SSP接口CS拉高函数。
*********************************************************************************************************/
void SPI_CS_Low(void)
{
   LPC_GPIO2->DATA   &= ~0x04;
//   DelayUs(1);
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
