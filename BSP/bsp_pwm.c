#include "BSP_PWM.h"

static uint32_t tim3_prescaler = PRESCALE;
static uint32_t tim3_arr = TIM3_ARR;
static uint32_t pwm_freq = PWM_FREQUENCY;

/****************************************************
 *@brief  : TIM3四个通道的pwm信号初始化
 *@param  : none
 *@retval : none
 *****************************************************/
void BSP_PWM_Init(void)
{
  BSP_PWM_GPIOConfig();
  BSP_PWM_ModeConfig((tim3_arr+1u)/2u-1u);
	BSP_PWM_SetCh1Duty(0.5f);
	BSP_PWM_SetCh2Duty(0.5f);
  BSP_Ser_Printf("PWM Initialize succeeded... \r\n");
}

/**************************************************
 *@brief  : 配置输出PWM的GPIO Pin(PA.06,PA.07  &  PB.00,PB.01)
 *@param  : none
 *@retval : none
 ***************************************************/
void BSP_PWM_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/***********************************************************************
 *@brief  : 配置TIM3输出的PWM信号的模式，如周期，极性，占空比
 *@param  : none
 *@retval : none
 *@note   :
						*---------------------------------------------------------------------------+
						| TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:  |
						| TIM3CLK = 72MHz, Prescale = 0, TIM3 counter clock = 72 MHz                |
						| TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR+1)    |
						| TIM3(PWM) Frequency = 72kHZ																						    |
						| TIM3 Channel 1 duty cycle = (TIM3_CCR1/TIM3_ARR)*100                      |
						| TIM3 Channel 2 duty cycle = (TIM3_CCR2/TIM3_ARR)*100                      | 
						+---------------------------------------------------------------------------*
 ************************************************************************/
void BSP_PWM_ModeConfig(uint32_t pwm_default)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
	
  /*ARR值*/
  TIM_TimeBaseStructure.TIM_Period    = tim3_arr;
  TIM_TimeBaseStructure.TIM_Prescaler = tim3_prescaler - 1 ;
	
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = pwm_default;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
  TIM_OC1Init(TIM3,&TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
  TIM_OC2Init(TIM3,&TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
// 	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
//   TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
// 	
// 	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
//   TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
  TIM_ARRPreloadConfig(TIM3,ENABLE);

  TIM_Cmd(TIM3,ENABLE);
}

/************************************************************************
 *@brief  : 设置通道1,2,3,4的占空比 = abs(输入)/((PWM_FREQUENCY/72)-1)
 *@param  : -ch1
 * 				  -ch2
            -ch3
            -ch4
 *@retval : none
 *************************************************************************
*/
void BSP_PWM_SetDuty(float ch1 , float ch2 ,float ch3 , float ch4)
{
	uint16_t arr1,arr2,arr3,arr4;
	
	arr1 = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*ch1 + 0.5f)-1u;
	arr2 = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*ch2 + 0.5f)-1u;
	arr3 = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*ch3 + 0.5f)-1u;
	arr4 = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*ch4 + 0.5f)-1u;
	
  TIM_SetCompare1(TIM3 , arr1);
  TIM_SetCompare2(TIM3 , arr2);
	TIM_SetCompare3(TIM3 , arr3);
	TIM_SetCompare4(TIM3 , arr4);
}

/*
*********************************************************************
*                 BSP_PWM_SetCh1Duty()
*********************************************************************
*/
void BSP_PWM_SetCh1Duty(float duty)
{
	uint16_t arr;
	arr = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*duty + 0.5f)-1u;
  TIM_SetCompare1(TIM3 , arr);
}

/*
*********************************************************************
*                 BSP_PWM_SetCh2Duty()
*********************************************************************
*/
void BSP_PWM_SetCh2Duty(float duty)
{
	uint16_t arr;
	arr = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*duty + 0.5f)-1u;
  TIM_SetCompare2(TIM3 , arr);
}

/*
*********************************************************************
*                 BSP_PWM_SetCh3Duty()
*********************************************************************
*/
void BSP_PWM_SetCh3Duty(float duty)
{
	uint16_t arr;
	arr = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*duty + 0.5f)-1u;
  TIM_SetCompare3(TIM3 , arr);
}

/*
*********************************************************************
*                 BSP_PWM_SetCh4Duty()
*********************************************************************
*/
void BSP_PWM_SetCh4Duty(float duty)
{
	uint16_t arr;
	arr = ((float)(72000000.0f/tim3_prescaler / pwm_freq)*duty + 0.5f)-1u;
  TIM_SetCompare4(TIM3 , arr);
}



void BSP_PWM_SetFreq(uint32_t f)
{
	pwm_freq = f;
	
	if(f >= 1000)
		tim3_prescaler = 2;
	else if(f >= 500)
    tim3_prescaler = 4;
	else if(f >= 200)
    tim3_prescaler = 8;
	else if(f >= 100)
    tim3_prescaler = 16;
	else if(f >= 50)
    tim3_prescaler = 32;
	else if(f >= 25)
    tim3_prescaler = 64;
	else if(f >= 15)
    tim3_prescaler = 128;
	else
    tim3_prescaler = 1024;
	
	TIM_PrescalerConfig(TIM3,tim3_prescaler-1,TIM_PSCReloadMode_Immediate);
	
	tim3_arr = (72000000.0f/tim3_prescaler/f - 1.0f + 0.5f);
	
	TIM_SetAutoreload(TIM3,tim3_arr);
}
