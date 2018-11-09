#include "bsp_tim2.h"

extern OS_SEM    Sem_ReadSensor;
/*
*********************************************************************************************************
*                                           BSP_TIM2_Init()
*
* Description : Initialize Timer2,use as a counter,external trigger pin PA0
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	
// 	  BSP_IntVectSet(BSP_PERIPH_ID_TIM2, BSP_TIM2_ISR_Handler);
// 		BSP_IntPrioSet(BSP_PERIPH_ID_TIM2,BSP_INT_TIM2_PRIO);	   
// 	  BSP_IntEn(BSP_PERIPH_ID_TIM2);
	
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period = 0xffff;		 							
    TIM_TimeBaseStructure.TIM_Prescaler= 0;				   
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//     TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    	
//     TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
    TIM_TIxExternalClockConfig(TIM2,TIM_TIxExternalCLK1Source_TI1,TIM_ICPolarity_Rising,0x02);
    TIM_SetCounter(TIM2, 0);  
    TIM_Cmd(TIM2, DISABLE);                                 
}


void BSP_TIM2_EnableCounter(void)
{
  TIM_Cmd(TIM2, ENABLE); 
}


void BSP_TIM2_DisableCounter(void)
{
  TIM_Cmd(TIM2, DISABLE);   
}


void BSP_TIM2_SetCounter(uint16_t cnt)
{
  TIM_SetCounter(TIM2, cnt);  
}


uint16_t BSP_TIM2_GetCounter(void)
{
  return TIM_GetCounter(TIM2);
}


/*
*********************************************************************************************************
*                                         BSP_TIM4_ISRHandler()
*
* Description : TIM4 ISR.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/
// void BSP_TIM2_ISR_Handler(void)
// {
// 	BSP_LED_Toggle();
// 	if ( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET ) 
// 	{	
// 		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
// 		//BSP_OS_SemPost(&Sem_ReadSensor);
// 	}		 	
// }
