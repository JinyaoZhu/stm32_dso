#include "includes.h"
#include "global.h"
#include "mini12864.h"

__align(8) static  CPU_STK  LcdRefreshTaskStk[PRINTF_TASK_STK_SIZE];
OS_TCB  LcdRefreshTaskTCB;


/*******************************************************************************************************/
void LcdRefreshTask(void *p_arg)
{

	(void)p_arg;
  while (1)
  {
    BSP_OS_SemWait(&LCD_Update_Sem,0);
		mini12864_Refresh();
  }
}


/*******************************************************************************************************/
void LcdRefreshTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&LcdRefreshTaskTCB,
               (CPU_CHAR   *)"LcdRefresh Task",
               (OS_TASK_PTR )LcdRefreshTask,
               (void       *)0,
               (OS_PRIO     )LCD_REFRESH_TASK_PRIO,
               (CPU_STK    *)&LcdRefreshTaskStk[0],
               (CPU_STK_SIZE)LCD_REFRESH_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)LCD_REFRESH_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate LcdRefresh Task...\n\r"));
  }
}
