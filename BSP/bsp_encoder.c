#include "bsp.h"



void BSP_Encoder_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* pins for encoder input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* pin for an extra button */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  TIM_DeInit(TIM1);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 0x000; 
  TIM_TimeBaseStructure.TIM_Period = 0xffff;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
  TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0xf;
  TIM_ICInit(TIM1, &TIM_ICInitStructure);

  TIM_SetCounter(TIM1, 0);  

  TIM_Cmd(TIM1, ENABLE);  
}


int32_t BSP_Encoder_GetSpeed(void)
{
	int32_t speed;
  speed = TIM_GetCounter(TIM1) ;
  TIM_SetCounter(TIM1, 0);  
  if(speed > 32768)
     speed = -(65536 - speed);
	
	return speed;
}
