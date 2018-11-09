#include "includes.h"
#include "global.h"
#include "algorithm.h"

__align(8) static  CPU_STK  ADSampleTaskStk[AD_SAMPLE_TASK_STK_SIZE];
OS_TCB  ADSampleTaskTCB;

CPU_FP32  ADCFreq = (12000000.0f/14.0f); /* ADC Clock = 12Mhz   14 Cycles per sample */



extern BSP_OS_SEM ADC1_Channel1_TC_Sem;
extern __IO uint16_t  AD_Value[];

/*******************************************************************************************************/
void ADSampleTask(void *p_arg)
{
	CPU_INT16U i;
	CPU_INT16U plot_step;
	CPU_INT16U Vmax;
	CPU_INT16U Vmin;
	CPU_INT16U Vpp;
	CPU_FP32 plot_scaler;
  CPU_INT16U duty_h_cnt;
	CPU_INT16U duty_samples_num;
	CPU_FP32 Vavg;
	CPU_INT08S plot_max;
	CPU_INT08S plot_min;
	CPU_INT08S plot_offset;
	CPU_INT08U adc_freq_index;

  (void)p_arg;
	
	adc_freq_index = 0;
	
  while (1)
  {
		/* determin adc samples freqency by signal's freqency */
		if((FreqMeasured > 500)||(gFlag_Trigger == 0)){
			if(adc_freq_index != 0){
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5 );
				ADCFreq = (12000000.0f/14.0f);
				adc_freq_index = 0;
			}
		}
		else if(FreqMeasured > 300){
			if(adc_freq_index != 1){
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5 );
				ADCFreq = (12000000.0f/20.0f);
				adc_freq_index = 1;
			}
		}
		else if(FreqMeasured > 200){
			if(adc_freq_index != 2){
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_13Cycles5 );
				ADCFreq = (12000000.0f/26.0f);	
				adc_freq_index = 2;
			}				
		}
		else if(FreqMeasured > 150){
			if(adc_freq_index != 3){
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5 );
				ADCFreq = (12000000.0f/41.0f);		
				adc_freq_index = 3;
			}				
		}		
		else  if(FreqMeasured > 100){
			if(adc_freq_index != 4){
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_71Cycles5 );
				ADCFreq = (12000000.0f/84.0f);			
				adc_freq_index = 4;
			}
		}		
		else if(FreqMeasured > 50){
			if(adc_freq_index != 6){
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );
				ADCFreq = (12000000.0f/252.0f);		
				adc_freq_index = 6;
			}				
		}				
	  else {
			if(adc_freq_index != 7){
				RCC_ADCCLKConfig(RCC_PCLK2_Div8);
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );
				ADCFreq = (9000000.0f/252.0f);		
				adc_freq_index = 7;
			}				
		}	
		
		if(BSP_OS_SemWait(&AD_StartConvert_Sem,100) == DEF_OK){
			/* calculate number of samples per period */
		  gNumOfSamplesPerPeriod = Constraint_int((ADCFreq/FreqMeasured + 0.5f),0,ADC_BUF_SIZE);
		}
		else{
			gNumOfSamplesPerPeriod = ADC_BUF_SIZE;
			/* start ad conversion without trigger */
			DMA_SetCurrDataCounter(DMA1_Channel1,ADC_BUF_SIZE);
      DMA_Cmd(DMA1_Channel1, ENABLE);  
		}
		
		/* wait DMA transmit complete */
   	BSP_OS_SemWait(&ADC1_Channel1_TC_Sem,50);
	  DMA_Cmd(DMA1_Channel1, DISABLE);
		
		Vmax = 0x0000;
		Vmin = 0xffff;
		duty_h_cnt = 0;
		Vavg = 0;
		
		duty_samples_num = (uint16_t)(ADC_BUF_SIZE/gNumOfSamplesPerPeriod)*gNumOfSamplesPerPeriod;
		
		
		/* find max and min value */
		for(i = 0; i<ADC_BUF_SIZE ; i++){
			
			Vavg += AD_Value[i];
			
      if(AD_Value[i] > Vmax)
				Vmax = AD_Value[i];
		  if(AD_Value[i] <= Vmin)
				Vmin = AD_Value[i];
			
			if(i < duty_samples_num){
				if(AD_Value[i] > 2048)
					duty_h_cnt++;
			}
    }
		
		Vpp = (Vmax - Vmin);
		Vavg /= (float)ADC_BUF_SIZE;
		
		switch(gMeasureRange)
		{
      case range_1v:
				gSignalScaleFactor = 0.4;
				break;
			case range_3v:
				gSignalScaleFactor = 1;
				break;
			case range_7v:
				gSignalScaleFactor = 2;
				break;
			case range_13v:
				gSignalScaleFactor = 4;
				break;
			default:break;
    }
		
		/* get signal characteristic */
		gSignal_MaxV = 0.8f*gSignal_MaxV + 0.2f*(gSignalScaleFactor*(Vmax*VREF/4095 - 1.65f)); /* max voltage in float max 3.3 */
		gSignal_MinV = 0.8f*gSignal_MinV + 0.2f*(gSignalScaleFactor*(Vmin*VREF/4095 - 1.65f)); /* max voltage in float min 0 */
		gSignal_Vpp  = 0.8f*gSignal_Vpp  + 0.2f*(gSignalScaleFactor*Vpp*VREF/4095);
		gSignal_Vavg = 0.8f*gSignal_Vavg + 0.2f*(gSignalScaleFactor*(Vavg*VREF/4095 - 1.65f));
		gSignal_Duty = 0.8f*gSignal_Duty + 0.2f*(float)duty_h_cnt/(duty_samples_num)*100;
		
		/* determin plot step by number of samples */
		if(gNumOfSamplesPerPeriod > (ADC_BUF_SIZE*0.5))
		  plot_step = Constraint_int(gNumOfSamplesPerPeriod / GRAPH_BUF_SIZE,1,ADC_BUF_SIZE/(GRAPH_BUF_SIZE-1u)-1u);
		else
		  plot_step = (2*gNumOfSamplesPerPeriod) / GRAPH_BUF_SIZE;
		

	  for(i = 0 ; i < GRAPH_BUF_SIZE; i++)
				gGraphBuf[i] = Constraint_int((38.0f/4095)*AD_Value[i*plot_step],0,38);		
			
		plot_max = 0;
		plot_min = 38;
			
		/* adjust graph scale */
		for(i = 0 ; i < GRAPH_BUF_SIZE; i++){
			if(gGraphBuf[i] > plot_max)
				plot_max = gGraphBuf[i];
		  if(gGraphBuf[i] < plot_min)	
				plot_min = gGraphBuf[i];
		}
		
		
		/* if signal is big enough scale the signal */
		if((plot_max - plot_min) > 5)
		  plot_scaler = 0.7f*plot_scaler + 0.3f*33.0f/(plot_max - plot_min);
		else
			 plot_scaler = 1.5;
		
		plot_offset = (0.7f*plot_offset + 0.3f*(plot_max + plot_min)/2.0f) + 0.5f;
		
		for(i = 0 ; i < GRAPH_BUF_SIZE; i++)
			gGraphBuf[i] = Constraint_int(plot_scaler*(gGraphBuf[i] - plot_offset)+0.5f,-19,18);			

		/* post to oscillator task */
		BSP_OS_SemPost(&StartPlot_Sem);
  }
}


/*******************************************************************************************************/
void ADSampleTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&ADSampleTaskTCB,
               (CPU_CHAR   *)"ADSample Task",
               (OS_TASK_PTR )ADSampleTask,
               (void       *)0,
               (OS_PRIO     )AD_SAMPLE_TASK_PRIO,
               (CPU_STK    *)&ADSampleTaskStk[0],
               (CPU_STK_SIZE)AD_SAMPLE_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)AD_SAMPLE_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate ADSampleTask Task...\n\r"));
  }
}
