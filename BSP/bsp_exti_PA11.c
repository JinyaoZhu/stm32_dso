#include "bsp.h"
#include "global.h"



void BSP_EXTI_PA11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/*config the extiline(PA11) clock and AFIO clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	
	/*EXTI line gpio config(PA11)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
  BSP_IntVectSet(BSP_INT_ID_EXTI15_10, BSP_EXTI_PA11_ISR_Handler);
	BSP_IntPrioSet(BSP_INT_ID_EXTI15_10,BSP_INT_EXTI_PA11_PRIO);
  BSP_IntDis(BSP_INT_ID_EXTI15_10);
	
	EXTI_ClearITPendingBit(EXTI_Line11);  
	
	/*EXTI line(PA11) mode config*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*
*******************************************************************
*                 BSP_EXTI_PA11_Enable()
* Description : 
*******************************************************************
*/
void BSP_EXTI_PA11_Enable(void)
{
	EXTI_ClearITPendingBit(EXTI_Line11);  
  BSP_IntEn(BSP_INT_ID_EXTI15_10);
}



/*
*******************************************************************
*                 BSP_EXTI_PA11_Disable()
* Description : 
*******************************************************************
*/
void BSP_EXTI_PA11_Disable(void)
{
  BSP_IntDis(BSP_INT_ID_EXTI15_10);
}

/*
*******************************************************************
*                 BSP_EXTI_PA11_ISR_Handler()
* Description : 
*******************************************************************
*/
void BSP_EXTI_PA11_ISR_Handler(void)
{
	if( EXTI_GetITStatus(EXTI_Line11) != RESET )
	{
		if(gFlag_FreqMeasureStart == 1){
			gStandardClkCntStart = CPU_TS_Get32();
		  BSP_TIM2_EnableCounter();
			/* start ad conversion */
			DMA_SetCurrDataCounter(DMA1_Channel1,ADC_BUF_SIZE);
      DMA_Cmd(DMA1_Channel1, ENABLE);  
			BSP_OS_SemPost(&AD_StartConvert_Sem);
		}
		else{
			gStandardClkCntEnd = CPU_TS_Get32();
			gInputClkCnt = BSP_TIM2_GetCounter();
			BSP_TIM2_DisableCounter();
			BSP_TIM2_SetCounter(0);
			BSP_OS_SemPost(&FreqGet_Sem);
    }
		BSP_EXTI_PA11_Disable();
		EXTI_ClearITPendingBit(EXTI_Line11);    
	}
}
