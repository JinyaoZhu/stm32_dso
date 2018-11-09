#include "includes.h"
#include "global.h"

__align(8)  CPU_STK  GetKeyTaskStk[GET_KEY_TASK_STK_SIZE];
OS_TCB  GetKeyTaskTCB;


/* pointer of InputKey Data */
KEY_TYPE *p_InputKeyMsgQ[INPUT_KEY_QUE_MAX];

KEY_TYPE InputKeyBuffer[INPUT_KEY_QUE_MAX];


/*******************************************************************************************************/
void GetKeyTask(void *p_arg)
{
  CPU_FP32 encoder_speed;
	OS_ERR err;
	CPU_INT16U  buf_counter;
	KEY_TYPE input_key;
	CPU_INT16U time_out_cnt = 0;
	CPU_FP32 dt;
	CPU_TS32 timestamp_old;
	(void)p_arg;
	
	/* let pointer point to buffers */
  for(buf_counter = 0; buf_counter < INPUT_KEY_QUE_MAX; buf_counter++){
		InputKeyBuffer[buf_counter] = key_none;
    p_InputKeyMsgQ[buf_counter] = &InputKeyBuffer[buf_counter];
	}
	
	buf_counter = 0;
	
  while (1)
  {
		dt =  Get_dt(&timestamp_old);
		
		encoder_speed = BSP_Encoder_GetSpeed()/dt;
		
    if(encoder_speed > 25){
			if(encoder_speed <120)
			  input_key = key_right;
			else if(encoder_speed < 250)
				input_key = key_right_fast;
			else if(encoder_speed < 500)
			  input_key = key_right_very_fast;
		}
		else if(encoder_speed < -25){
			if(encoder_speed > -120)
			  input_key = key_left;
			else if(encoder_speed > -250)
				input_key = key_left_fast;
			else if(encoder_speed > -500)
			  input_key = key_left_very_fast;
		}
		else
			input_key = key_none;
		
		/* if confirm */
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == RESET){
			BSP_OS_TimeDlyMs(5);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == RESET)
			  input_key = key_confirm;
		}
		
		time_out_cnt = 0;
		/* hold 1s to exit */
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == RESET){
			time_out_cnt++;
      BSP_OS_TimeDlyMs(10);
			if(time_out_cnt > 90){
        input_key = key_exit;
				break;
      }		
    }
			
		
		/* fill buffer */
		InputKeyBuffer[buf_counter] = input_key;
		
		/* post msg */
	  OSQPost(&Que_InputKeyData,(void*)p_InputKeyMsgQ[buf_counter],
						sizeof(KEY_TYPE),OS_OPT_POST_FIFO,&err);
			
		if(OS_ERR_NONE == err)
				buf_counter = (buf_counter+1)%INPUT_KEY_QUE_MAX;
		
		/* wait until button release */
	  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == RESET){
      BSP_OS_TimeDlyMs(10);	
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == RESET)
				BSP_OS_TimeDlyMs(10);	
    }
		
    BSP_OS_TimeDlyMs(50);
  }
}


/*******************************************************************************************************/
void GetKeyTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&GetKeyTaskTCB,
               (CPU_CHAR   *)"GetKey Task",
               (OS_TASK_PTR )GetKeyTask,
               (void       *)0,
               (OS_PRIO     )GET_KEY_TASK_PRIO,
               (CPU_STK    *)&GetKeyTaskStk[0],
               (CPU_STK_SIZE)GET_KEY_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)GET_KEY_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate GetKey Task...\n\r"));
  }
}
