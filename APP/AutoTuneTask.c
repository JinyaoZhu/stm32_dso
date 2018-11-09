#include "includes.h"
#include "global.h"
#include "genetic.h"

__align(8) static  CPU_STK  AutoTuneTaskStk[AUTO_TUNE_TASK_STK_SIZE];
OS_TCB  AutoTuneTaskTCB;


CPU_INT08U Flag_Genetic_TestFinished;

CPU_INT08U Flag_PopulationIsInited = 0;

GENETIC_PID_TYPE BestPID;

/* half of the population are the parents(0~MAX_INDIVIDUAL_NUM/2-1) and the other part are babies */
INDIVIDUAL_TYPE Population[MAX_INDIVIDUAL_NUM];

/*******************************************************************************************************/
void AutoTuneTask(void *p_arg)
{

	(void)p_arg;
	
	if(Flag_PopulationIsInited == 0){
	  Genetic_PopulationInit(Population);
		Flag_PopulationIsInited = 1;
	}
	
	Flag_Genetic_TestFinished = 1;
	
	Genetic_GetBestPID(Population,&BestPID);
	
  while (1)
  {   
		if((gFlag_StartAutoTuning == 1)&&(Genetic_GetMaxHealth()<10.0f)){
			
			Flag_Genetic_TestFinished = 0;
			
	  	if(Genetic_Test(Population) == DEF_OK){
				
				Genetic_GetBestPID(Population,&BestPID);
				
			  Genetic_StoreBestDNA(Population);
				
				srand(CPU_TS_Get32());
				
				Genetic_Select(Population);
				
				srand(CPU_TS_Get32());
				
				Genetic_Breed(Population);
			}
			
			Flag_Genetic_TestFinished = 1;
		}
		else
			BSP_OS_TimeDlyMs(100);
  }
}


/*******************************************************************************************************/
void AutoTuneTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&AutoTuneTaskTCB,
               (CPU_CHAR   *)"Auto Tune Task",
               (OS_TASK_PTR )AutoTuneTask,
               (void       *)0,
               (OS_PRIO     )AUTO_TUNE_TASK_PRIO,
               (CPU_STK    *)&AutoTuneTaskStk[0],
               (CPU_STK_SIZE)AUTO_TUNE_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)AUTO_TUNE_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate Autotune Task...\n\r"));
  }
}
