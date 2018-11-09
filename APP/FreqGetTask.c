#include "includes.h"
#include "global.h"

#define TRG_INT_EN       BSP_EXTI_PA11_Enable()
#define TRG_INT_DIS      BSP_EXTI_PA11_Disable()

__align(8) static  CPU_STK  FreqGetTaskStk[FREQ_GET_TASK_STK_SIZE];
OS_TCB  FreqGetTaskTCB;


/*******************************************************************************************************/
void FreqGetTask(void *p_arg)
{
	(void)p_arg;
	
	gFlag_FreqMeasureStart = 0;
	gFlag_Trigger = 0;
	TRG_INT_DIS;
	
  while (1)
  {
		/* start measure */
		gFlag_FreqMeasureStart = 1;
		TRG_INT_EN;
		BSP_OS_TimeDlyMs(100);
		/* stop measure */
		gFlag_FreqMeasureStart = 0;
		TRG_INT_EN;
		
		if(BSP_OS_SemWait(&FreqGet_Sem,100) == DEF_OK){
			
			gFlag_Trigger = 1;
			
			if(gStandardClkCntStart < gStandardClkCntEnd)
				gStandardClkCntInterval = gStandardClkCntEnd - gStandardClkCntStart;
			else
				gStandardClkCntInterval = gStandardClkCntEnd +(4294967296u - gStandardClkCntStart);
			
			//gStandardClkCntInterval += 50u;  /* compensate */
			
			FreqMeasured = 0.90f*FreqMeasured + 0.10f*((float)(gInputClkCnt)*72000000.0f)/(gStandardClkCntInterval); /* standard clock = 72M */
	  }
		else{ /* no trigger */
			TRG_INT_DIS;
			FreqMeasured = 0.0001f;
			gFlag_Trigger = 0;
		}
		
		//BSP_OS_TimeDlyMs(10);
  }
}


/*******************************************************************************************************/
void FreqGetTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&FreqGetTaskTCB,
               (CPU_CHAR   *)"FreqGet Task",
               (OS_TASK_PTR )FreqGetTask,
               (void       *)0,
               (OS_PRIO     )FREQ_GET_TASK_PRIO,
               (CPU_STK    *)&FreqGetTaskStk[0],
               (CPU_STK_SIZE)FREQ_GET_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)FREQ_GET_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate FreqGet Task...\n\r"));
  }
}
