#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "bsp.h"

#define ADC_BUF_SIZE    (2000u)
#define VREF            (3.3f)


void BSP_ADC1_GPIOConfiguration(void);
void BSP_ADC1_RCCConfiguration(void);
void BSP_ADC1_ADC1Configuration(void);
void BSP_ADC1_DMAConfiguration(void);
void BSP_ADC1_Init(void);
void BSP_ADC1_StartConversion(uint32_t nbr_of_samples);
void BSP_ADC1_Channel1_ISR(void);
uint16_t BSP_ADC1_GetFirstADCValue(void);

#endif /*__BSP_ADC_H__*/
