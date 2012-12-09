/*********************************************************************************************************
** SPI0 �Ļ�����������
*********************************************************************************************************/

#include "spi.h"                                                   /* LPC11xx����Ĵ���            */

/*********************************************************************************************************
** Function name:       SPI_PinInit
** Descriptions:        SPI0\SSP1���ų�ʼ������
** input parameters:     ��
** output parameters:    ��
*********************************************************************************************************/
static void SPI_PinInit(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);                             /* ����IOCONģ��ʱ��            */
    /* ��ʼ��SPI0����               */
    LPC_IOCON->PIO2_2 |= 0x00;                                          /* CS gpio*/
    LPC_GPIO2->DIR    |= 0x04;                                          /* direction:output */
    LPC_GPIO2->DATA   |= 0x04;                                          /*init high level*/
    
    LPC_IOCON->SCK_LOC |= 0x01;                                        /*  P2.11����ΪSCK               */
    LPC_IOCON->PIO2_11 |= 0x01;                                          /* SCK */
    LPC_IOCON->PIO0_8 |= 0x01;                                          /* MISO*/
    LPC_IOCON->PIO0_9 |= 0x01;                                          /* MOSI*/
}

/*********************************************************************************************************
** Function name��      SPI_MasterInit()
** Descriptions:        ��SPI0����������Ϊ����SPI��
** input parameters��   ��
** output parameters��  ��
** Returned value:      ��
*********************************************************************************************************/
void  SPI_MasterInit(void)
{
    SPI_PinInit();                                                     /*��ʼ��SPI0ʹ�õ�IO����        */
    
    LPC_SYSCON->PRESETCTRL      |= 0x01;                                /* ��ֹSPI0��λ                 */
    LPC_SYSCON->SYSAHBCLKCTRL   |= (1 << 11);                           /* ��SPI0����                 */
    LPC_SYSCON->SSP0CLKDIV       = 0x03;                                /* SSPʱ�ӷ�Ƶ,48M/3=16M         */

    LPC_SSP0->CR0 = (0x00 << 8) |                                       /* SCR  ����SPIʱ�ӷ�Ƶ         */
                    (0x00 << 7) |                                       /* CPHA ʱ�������λ,           */
                                                                        /* ��SPIģʽ��Ч                */
                    (0x00 << 6) |                                       /* CPOL ʱ���������,           */
                                                                        /* ��SPIģʽ��Ч                */
                    (0x00 << 4) |                                       /* FRF  ֡��ʽ 00=SPI,01=SSI,   */
                                                                        /* 10=Microwire,11=����         */
                    (0x07 << 0);                                        /* DSS  ���ݳ���,0000-0010=����,*/
                                                                        /* 0011=4λ,0111=8λ,1111=16λ  */

    LPC_SSP0->CR1 = (0x00 << 3) |                                       /* SOD  �ӻ��������,1=��ֹ     */
                    (0x00 << 2) |                                       /* MS   ����ѡ��,0=����,1=�ӻ�  */
                    (0x01 << 1) |                                       /* SSE  SSPʹ��                 */
                    (0x00 << 0);                                        /* LBM  ��дģʽ                */

    LPC_SSP0->CPSR = 2;                                                 /* PCLK��Ƶֵ 16M/2=8M                  */
//    LPC_SSP0->ICR  = 0x03;                                              /* �ж�����Ĵ���               */

//   NVIC_EnableIRQ(SSP0_IRQn);
//   NVIC_SetPriority(SSP0_IRQn, 2);                                     /*  �ж�ʹ�ܲ��������ȼ�        */
//    LPC_SSP0->IMSC |= 0x01 << 2;                                        /*  �ж�ʹ�ܼĴ���              */
} 

/*********************************************************************************************************
** Function name��      SPI_SendData()
** Descriptions��       SSP�ӿ���SPI���߷������ݡ�
** input parameters��   data        �����͵�����
** output parameters��  ����ֵΪ��ȡ������
** Returned value:      ��
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
** Function name��      SPI_ReadData()
** Descriptions��       SSP�ӿڴ�SPI���߶�ȡ���ݣ���д���ݵ�ʵ��һ����
** input parameters��   data        �����͵�����
** output parameters��  ����ֵΪ��ȡ������
** Returned value:      ��
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
** Function name��      void SPI_CS_High(void)
** Descriptions��       SSP�ӿ�CS���ߺ�����
*********************************************************************************************************/
void SPI_CS_High(void)
{
   LPC_GPIO2->DATA   |= 0x04;
//   DelayUs(1);
}

/*********************************************************************************************************
** Function name��      void SPI_CS_Low(void)
** Descriptions��       SSP�ӿ�CS���ߺ�����
*********************************************************************************************************/
void SPI_CS_Low(void)
{
   LPC_GPIO2->DATA   &= ~0x04;
//   DelayUs(1);
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
