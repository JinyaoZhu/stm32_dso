#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#define SPI2_TIMEOUT_LIMIT     100 /* ms */


/* SPI2 Pin */
#define SPI2_SCK_PIN     GPIO_Pin_13
#define SPI2_MISO_PIN    GPIO_Pin_14
#define SPI2_MOSI_PIN    GPIO_Pin_15
#define SPI2_PORT        GPIOB

#define SPI2_DR_Addr   ((u32)0x4000380c)


void BSP_SPI2_Init(void);

CPU_INT08U BSP_SPI2_SendReceiveByte(CPU_INT08U data);

void SPI2_TxTimeoutCallback(void);

void SPI2_RxTimeoutCallback(void);

#endif /* __BSP_SPI_H__ */
