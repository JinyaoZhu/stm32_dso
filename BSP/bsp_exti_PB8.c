#include "bsp.h"
#include "global.h"



void BSP_EXTI_PA08_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/*config the extiline(PB08) clock and AFIO clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	
	/*EXTI line gpio config(PB08)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/*EXTI line gpio config(PB05)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
  BSP_IntVectSet(BSP_INT_ID_EXTI9_5, BSP_EXTI_PA08_ISR_Handler);
	BSP_IntPrioSet(BSP_INT_ID_EXTI9_5,BSP_INT_EXTI_PA08_PRIO);
  BSP_IntEn(BSP_INT_ID_EXTI9_5);
	
	EXTI_ClearITPendingBit(EXTI_Line8);  
	
	/*EXTI line(PB08) mode config*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/*
*******************************************************************
*                 BSP_EXTI_PA08_ISR_Handler()
* Description : 
*******************************************************************
*/
void BSP_EXTI_PA08_ISR_Handler(void)
{
	if( EXTI_GetITStatus(EXTI_Line8) != RESET )
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == SET)
			gInputKey = key_right;
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == RESET)
			gInputKey = key_left;
		EXTI_ClearITPendingBit(EXTI_Line8);    
	}
}
