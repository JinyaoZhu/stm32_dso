#ifndef __GLOBAL__
#define __GLOBAL__

#include "bsp.h"

#define AUTO_TUNE_EN       (1u)

#define   GRAPH_BUF_SIZE   (120u)
#define  INPUT_KEY_QUE_MAX  (16u)


typedef enum 
{key_left = 0,key_left_fast,key_left_very_fast,key_right,key_right_fast,key_right_very_fast,\
              key_confirm,key_none,key_exit}KEY_TYPE;

typedef enum {range_1v = 0,range_3v,range_7v,range_13v}V_RANGE_TYPE;

extern V_RANGE_TYPE  gMeasureRange;

extern KEY_TYPE gInputKey;

extern CPU_INT32U g_LED_BLINK_PERIOD;

extern BSP_OS_SEM LCD_Update_Sem;
extern BSP_OS_SEM FreqGet_Sem;
extern BSP_OS_SEM AD_StartConvert_Sem;
extern BSP_OS_SEM StartPlot_Sem;

extern OS_Q Que_InputKeyData;;


/* For frequency measure */
extern CPU_INT08U gFlag_FreqMeasureStart;
extern CPU_INT08U gFlag_Trigger;
extern CPU_INT08U gFlag_StartAutoTuning;

extern __IO CPU_INT32U gStandardClkCntStart;
extern __IO CPU_INT32U gStandardClkCntEnd;
extern __IO CPU_INT32U gStandardClkCntInterval;
extern __IO CPU_INT32U gInputClkCnt;

extern CPU_INT32U gNumOfSamplesPerPeriod;

extern CPU_FP32 FreqMeasured;
extern CPU_FP32 gSignal_MaxV;
extern CPU_FP32 gSignal_MinV;
extern CPU_FP32 gSignal_Vpp;
extern CPU_FP32 gSignal_Vavg;
extern CPU_FP32 gSignal_Duty;

extern CPU_FP32 gSignalScaleFactor;

extern CPU_INT32U gPWM_OutputFreq;
extern CPU_FP32   gPWM_OutputDuty;

extern CPU_INT08S gGraphBuf[];

extern CPU_FP32 gTestData1;
extern CPU_FP32 gTestData2;
extern CPU_FP32 gTestData3;

void   DisplaySystemInfo(void);
float  Get_dt(CPU_TS32 *timestamp_old);

#endif /* __GLOBAL__ */
