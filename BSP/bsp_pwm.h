/*********************************
 (使用TIM3)
 硬件连接：
 PWM输出
 (PA.06,PA.07  &  PB.00,PB.01)
**********************************/
#ifndef __BSP_PWM_H__
#define __BSP_PWM_H__
#include "bsp.h"


#define PWM_FREQUENCY  (5000)            
#define PRESCALE       (2)           

#define TIM3_FREQUENCY (72000000u/(PRESCALE))
#define TIM3_ARR       ((TIM3_FREQUENCY / PWM_FREQUENCY) - 1u) 


/* PWM Frequency =  TIM3_FREQUENCY / (TIM3_ARR+1) */

/*----------------Private-------------------------*/
void BSP_PWM_GPIOConfig(void);
void BSP_PWM_ModeConfig(uint32_t pwm_default);
/*----------------Public--------------------------*/
void BSP_PWM_Init(void);
void BSP_PWM_SetDuty(float ch1 , float ch2 ,float ch3 , float ch4);
void BSP_PWM_SetCh1Duty(float duty);
void BSP_PWM_SetCh2Duty(float duty);
void BSP_PWM_SetCh3Duty(float duty);
void BSP_PWM_SetCh4Duty(float duty);
void BSP_PWM_SetFreq(uint32_t f);

#endif /*__BSP_PWM_H__*/
