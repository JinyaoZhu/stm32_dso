#ifndef __MINI12864_H__
#define __MINI12864_H__

#include "stm32f10x.h"

#define MINI12864_CS_PIN   GPIO_Pin_10
#define MINI12864_CS_PORT  GPIOB

#define MINI12864_RES_PIN  GPIO_Pin_11
#define MINI12864_RES_PORT GPIOB

#define MINI12864_CD_PIN   GPIO_Pin_12
#define MINI12864_CD_PORT  GPIOB


#define MINI12864_CS_H  GPIO_SetBits(MINI12864_CS_PORT, MINI12864_CS_PIN)
#define MINI12864_CS_L  GPIO_ResetBits(MINI12864_CS_PORT, MINI12864_CS_PIN)

#define MINI12864_RES_H  GPIO_SetBits(MINI12864_CS_PORT, MINI12864_RES_PIN)
#define MINI12864_RES_L  GPIO_ResetBits(MINI12864_CS_PORT, MINI12864_RES_PIN)

#define MINI12864_CD_H  GPIO_SetBits(MINI12864_CD_PORT, MINI12864_CD_PIN)
#define MINI12864_CD_L  GPIO_ResetBits(MINI12864_CD_PORT, MINI12864_CD_PIN)

#define MINI12864_DelayMS(x)    BSP_OS_TimeDlyMs(x)

void mini12864_WriteCmd(uint8_t byte);
void mini12864_WriteData(uint8_t byte);
void mini12864_Init(void);

void mini12864_CacheFill(uint8_t byte);
void mini12864_CacheClear(void);
void mini12864_Refresh(void);

void mini12864_DrawPoint(uint16_t x,uint16_t y);
void mini12864_CleanPoint(uint16_t x,uint16_t y);
void mini12864_InvertPoint(uint16_t x,uint16_t y);

#endif /* __MINI12864_H__ */



