#include "includes.h"
#include "global.h"

__align(8) static  CPU_STK  PrintfTaskStk[PRINTF_TASK_STK_SIZE];
OS_TCB  PrintfTaskTCB;


/*******************************************************************************************************/
void PrintfTask(void *p_arg)
{

	(void)p_arg;
  while (1)
  {
		//BSP_Ser_Printf("%d,%d,\r\n",gInputClkCnt,gStandardClkCntInterval);
		//BSP_Ser_Printf("%d,\r\n",gInputKey);
		
		BSP_Ser_Printf("%8.5f,%8.5f,%8.5f,T\r\n",gTestData1,gTestData2,gTestData3);
    BSP_OS_TimeDlyMs(50);
  }
}


/*******************************************************************************************************/
void PrintfTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&PrintfTaskTCB,
               (CPU_CHAR   *)"Printf Task",
               (OS_TASK_PTR )PrintfTask,
               (void       *)0,
               (OS_PRIO     )PRINTF_TASK_PRIO,
               (CPU_STK    *)&PrintfTaskStk[0],
               (CPU_STK_SIZE)PRINTF_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)PRINTF_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate Printf Task...\n\r"));
  }
}
