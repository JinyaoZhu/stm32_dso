#include "includes.h"
#include "global.h"
#include "algorithm.h"
#include "mini12864.h"

extern CPU_INT32U  BSP_CPU_ClkFreq_MHz;


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB  AppTaskStartTCB; 

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

__align(8) static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void AppTaskCreate         (void);
static  void AppObjCreate          (void);
static  void AppTaskStart          (void *p_arg);


/*
*********************************************************************************************************
*                                       Application Object
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
  OS_ERR  err;

  OSInit(&err);                                               /* Init uC/OS-III.                                      */
	
	App_OS_SetAllHooks();

  OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
               (CPU_CHAR   *)"App Task Start",
               (OS_TASK_PTR ) AppTaskStart,
               (void       *) 0,
               (OS_PRIO     ) APP_TASK_START_PRIO,
               (CPU_STK    *)&AppTaskStartStk[0],
               (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
               (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
               (OS_MSG_QTY  ) 5u,
               (OS_TICK     ) 0u,
               (void       *) 0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);

  OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                         AppTaskStart()
*
* Description : Create other application task.
*               
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
  OS_ERR      err;
  CPU_INT32U  cnts;

  (void)p_arg;
	
	CORDIC_Init();
  
  BSP_Init();                                                 /* Initialize BSP functions                             */
	
  CPU_Init();

  cnts = (BSP_CPU_ClkFreq_MHz * 1000000) / (CPU_INT32U)OSCfg_TickRate_Hz; /* Determine nbr SysTick increments                     */
  OS_CPU_SysTickInit((CPU_INT32U) cnts);                                  /* Init uC/OS periodic time src (SysTick).              */

  Mem_Init();                                                 /* Initialize Memory Management Module         	*/
	
#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  BSP_Ser_Init(115200);                                         /* Enable Serial Interface                              */

  BSP_EXTI_PA11_Init();   /* tigger */
	
	BSP_Encoder_Init();

  BSP_TIM2_Init();

  BSP_PWM_Init();
	
	BSP_SPI2_Init();
		
	//BSP_I2C_Init();
	
	BSP_ADC1_Init();
	
	mini12864_Init();
	
//	DisplaySystemInfo();
		
  AppObjCreate();

  AppTaskCreate();                                            /* Create Application Tasks                             */
	
//	OSTaskChangePrio(&AppTaskStartTCB,PRINTF_TASK_PRIO + 1,&err);
	
	OSTaskDel((OS_TCB *)0,&err); /* delete self */
 
  while (DEF_TRUE) {
		
    BSP_LED_Toggle();
 		BSP_OS_TimeDlyMs(g_LED_BLINK_PERIOD);
  }
}





/*
*********************************************************************************************************
*                                         CREATE OBJECTS
*
* Description:  This function creates the application Objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/
static void AppObjCreate(void)
{
	OS_ERR err;
	
  BSP_OS_SemCreate(&LCD_Update_Sem,0,"LCD_Update_Sem");
	/* create message queue for input key */
	OSQCreate(&Que_InputKeyData, "InputKeyMsgQ", INPUT_KEY_QUE_MAX, &err);
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	APP_TRACE_INFO(("\n\rCreating Application Tasks... \n\r"));
	
	//OscTaskCreate();
	
	//GeneratorTaskCreate();
	
	GetKeyTaskCreate();
		
//	LcdRefreshTaskCreate();
	
	MenuTaskCreate();
	
	PrintfTaskCreate();
}





