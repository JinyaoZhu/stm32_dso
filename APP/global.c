#include "global.h"

KEY_TYPE gInputKey = key_none;

V_RANGE_TYPE  gMeasureRange = range_7v;

CPU_INT32U g_LED_BLINK_PERIOD = 500;  /* ms */


BSP_OS_SEM LCD_Update_Sem;
BSP_OS_SEM AD_StartConvert_Sem;
BSP_OS_SEM StartPlot_Sem;


OS_Q Que_InputKeyData;

BSP_OS_SEM FreqGet_Sem;

CPU_INT08U gFlag_FreqMeasureStart = 0;
CPU_INT08U gFlag_Trigger = 0;
CPU_INT08U gFlag_StartAutoTuning = 0;

__IO CPU_INT32U gStandardClkCntStart = 0;
__IO CPU_INT32U gStandardClkCntEnd = 0;
__IO CPU_INT32U gStandardClkCntInterval = 0;
__IO CPU_INT32U gInputClkCnt = 0;

CPU_INT32U gNumOfSamplesPerPeriod = 0;

CPU_FP32 FreqMeasured = 0;

CPU_FP32 gSignal_MaxV = 0;
CPU_FP32 gSignal_MinV = 0;
CPU_FP32 gSignal_Vpp = 0;
CPU_FP32 gSignal_Vavg = 0;
CPU_FP32 gSignal_Duty = 0;

CPU_FP32 gSignalScaleFactor = 4.00f;

CPU_INT32U gPWM_OutputFreq = 1000;
CPU_FP32   gPWM_OutputDuty = 0.50f;

CPU_INT08S gGraphBuf[GRAPH_BUF_SIZE] = {0};

CPU_FP32 gTestData1 = 0;

CPU_FP32 gTestData2 = 0;

CPU_FP32 gTestData3 = 0;

/*
******************************************************************
*                        DisplaySystemInfo()
* Description : 
*
* Parameter   : none.
*
* Caller      : Application.
******************************************************************
*/
void DisplaySystemInfo(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
  RCC_GetClocksFreq(&rcc_clocks);

  APP_TRACE_INFO(("\n\rSystem Info: \n\r"));
  APP_TRACE_INFO(("SYSCLK_Frequency = %3.1fMHz \r\n", rcc_clocks.SYSCLK_Frequency/1000000.0f));
	APP_TRACE_INFO(("HCLK_Frequency   = %3.1fMHz \r\n", rcc_clocks.HCLK_Frequency/1000000.0f));
	APP_TRACE_INFO(("PCLK1_Frequency  = %3.1fMHz \r\n", rcc_clocks.PCLK1_Frequency/1000000.0f));
	APP_TRACE_INFO(("PCLK2_Frequency  = %3.1fMHz \r\n", rcc_clocks.PCLK2_Frequency/1000000.0f));
	APP_TRACE_INFO(("ADCCLK_Frequency = %3.1fMHz \r\n\r\n", rcc_clocks.ADCCLK_Frequency/1000000.0f));
}

/*
******************************************************************
*                          Get_dt()
* Description : A hight resoluion counter use cpu timestamp to get 
*               the period of a loop.
*
* Parameter   : CPU_TS32 *timestamp_old --- used to save old stamp
*               each task whitch use this function should have an individual
*               variable to store old timestamp.
*
* Caller      : Application.
******************************************************************
*/
float Get_dt(CPU_TS32 *timestamp_old)
{
	CPU_FP32 dt;
  CPU_TS32 timestamp_new = 0;
	
	timestamp_new = CPU_TS_Get32();
	
	if(timestamp_new < *timestamp_old)
	{
		dt = (timestamp_new +(4294967296 - *timestamp_old))*0.0000000138888888889f; /* sec */
	}
	else
	{
		dt = (timestamp_new - *timestamp_old)*0.0000000138888888889f;
	}
	*timestamp_old = timestamp_new;
	return dt;
}


