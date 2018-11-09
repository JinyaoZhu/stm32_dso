#ifndef __BSP_TIM2_H__
#define __BSP_TIM2_H__
#include "bsp.h"



void BSP_TIM2_Init(void);

void BSP_TIM2_EnableCounter(void);

void BSP_TIM2_DisableCounter(void);

void BSP_TIM2_SetCounter(uint16_t cnt);

uint16_t BSP_TIM2_GetCounter(void);


#endif /* __BSP_TIM2_H__ */
