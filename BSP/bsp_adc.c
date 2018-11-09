#include "bsp_adc.h"
#include "math.h"

__IO uint16_t  AD_Value[ADC_BUF_SIZE] = {0};  

BSP_OS_SEM ADC1_Channel1_TC_Sem;

/***************************************************************************************************************
 *@note   ��ADC1��ͨ��      0   1   2   3    4    5    6    7    8    9
           �ֱ��Ӧ�Ĺܽ�Ϊ PA0 PA1 PA2 PA3  PA4  PA5  PA6  PA7  PB0  PB1
 *@brief  ������GPIO
 *@param  ��none
 ****************************************************************************************************************/
void BSP_ADC1_GPIOConfiguration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*****************************
 *@brief : ���ø����ֵ�ʱ��
 *@param : none
 ******************************/
void BSP_ADC1_RCCConfiguration(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1| RCC_APB2Periph_AFIO, ENABLE );  
    /* Configure ADCCLK such as ADCCLK = PCLK2/6 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  /* ADC clock max = 14Mhz */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/*************************
 *@brief  : ����ADC1
 *@param  : none
 *************************/
void BSP_ADC1_ADC1Configuration(void)
{
  ADC_InitTypeDef  ADC_InitStructure;

  ADC_DeInit(ADC1);  
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;        //ģ��ת��������ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //ģ��ת������������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�ⲿ����ת���ر�
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1; 
  ADC_Init(ADC1, &ADC_InitStructure);     

  /* ADC1 regular channel11 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5 );

  /* ����ADC��DMA֧�� */
  ADC_DMACmd(ADC1, ENABLE); 

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);   
  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);   
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1)); 

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1); 
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

/********************************
 *@brief  : ����DMA1
 *@param  : none
 *********************************/
void BSP_ADC1_DMAConfiguration(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  DMA_DeInit(DMA1_Channel1);   
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA����ADC����ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA�ڴ����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;      //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
  DMA_InitStructure.DMA_BufferSize = ADC_BUF_SIZE;                 //DMAͨ����DMA����Ĵ�С
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;           //�ڴ��ַ�Ĵ�������
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          //���ݿ��Ϊ16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;    
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�� xӵ�и����ȼ� 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
}
/*****************************
 *@brief  : ��ʼ��ADC1
 *@param  : none
 ******************************/
void BSP_ADC1_Init(void)
{
	BSP_OS_SemCreate(&ADC1_Channel1_TC_Sem,0, "ADC1_Channel1_TC_Sem");
	
	BSP_IntVectSet(BSP_INT_ID_DMA1_CH1,BSP_ADC1_Channel1_ISR);
  BSP_IntPrioSet(BSP_INT_ID_DMA1_CH1,BSP_INT_ADC1_CH1_PRIO);
  BSP_IntEn(BSP_INT_ID_DMA1_CH1);	
	
  BSP_ADC1_GPIOConfiguration();
  BSP_ADC1_RCCConfiguration();
  BSP_ADC1_ADC1Configuration();
  BSP_ADC1_DMAConfiguration();
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	DMA_ClearFlag(DMA1_FLAG_TC1);
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);

	DMA_Cmd(DMA1_Channel1, DISABLE);
}


void BSP_ADC1_StartConversion(uint32_t nbr_of_samples)
{
	DMA_SetCurrDataCounter(DMA1_Channel1,nbr_of_samples);
  DMA_Cmd(DMA1_Channel1, ENABLE);  
	BSP_OS_SemWait(&ADC1_Channel1_TC_Sem,0);
	DMA_Cmd(DMA1_Channel1, DISABLE);
}


uint16_t BSP_ADC1_GetFirstADCValue(void)
{
  return AD_Value[0];
}




void BSP_ADC1_Channel1_ISR(void)
{
	if( DMA_GetITStatus(DMA1_IT_TC1) != RESET )
	{
		BSP_OS_SemPost(&ADC1_Channel1_TC_Sem);
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}
