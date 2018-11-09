#include "includes.h"
#include "global.h"
#include "GUI.h"
#include "algorithm.h"

__align(8)  CPU_STK  GeneratorTaskStk[GENERATOR_TASK_STK_SIZE];
OS_TCB  GeneratorTaskTCB;

#define  GENERATOR_MENU_X       (6)
#define  GENERATOR_MENU_LINE1_Y (40)
#define  GENERATOR_MENU_LINE2_Y (GENERATOR_MENU_LINE1_Y - 12)
#define  GENERATOR_MENU_LINE3_Y (GENERATOR_MENU_LINE2_Y - 12)


/*******************************************************************************************************/
void GeneratorTask(void *p_arg)
{
	KEY_TYPE *p_input_key;
	OS_MSG_SIZE input_key_data_size;
	OS_ERR err;
	CPU_INT08S menu_index;
	CPU_INT08U setting_flag; /* indicate if in the setting mode */
	(void)p_arg;
	
	setting_flag = 0;

	
	BSP_PWM_SetFreq(gPWM_OutputFreq); /* >=18Hz */
	BSP_PWM_SetCh1Duty(gPWM_OutputDuty);
	
	menu_index = 0;
	
  while (1)
  {
		/* pend 100ms */
		p_input_key = (KEY_TYPE*)OSQPend(&Que_InputKeyData,100,OS_OPT_PEND_BLOCKING,&input_key_data_size,(CPU_TS *)0,&err);
		
		/* diaplay fixed part */
	  GUI_Clear();
		GUI_DrawLine(0,0,127,0);
		GUI_DrawLine(0,63,127,63);
		GUI_DrawLine(0,0,0,63);
		GUI_DrawLine(127,0,127,63);
		GUI_DrawLine(0,53,127,53);
		GUI_PrintString("Generator",37,54);
		
		
    if(setting_flag == 0){
			
		  if(*p_input_key == key_left){
				menu_index = Constraint_int(menu_index+1,0,2);
			}
			else if(*p_input_key == key_right){
				menu_index = Constraint_int(menu_index-1,0,2);
			}
			
			/* variable part */
			if(menu_index == 0){
				GUI_PrintfStrInt6StrWithBoxInvert("Freq:",gPWM_OutputFreq,"Hz",85,GENERATOR_MENU_X,GENERATOR_MENU_LINE1_Y);
				if(*p_input_key == key_confirm){
						setting_flag = 1; /* set freqency */
				}
			}
			else
				GUI_PrintStrInt6Str("Freq:",gPWM_OutputFreq,"Hz",GENERATOR_MENU_X,GENERATOR_MENU_LINE1_Y);
			
			if(menu_index == 1){
				GUI_PrintfStrFloat3_1StrWithBoxInvert("Duty:   ",gPWM_OutputDuty*100,"%",85,GENERATOR_MENU_X,GENERATOR_MENU_LINE2_Y);
				if(*p_input_key == key_confirm){
					 setting_flag = 1;  /* set duty circle */
				}
			}
			else
				GUI_PrintStrFloat3_1Str("Duty:   ",gPWM_OutputDuty*100,"%",GENERATOR_MENU_X,GENERATOR_MENU_LINE2_Y);
			
			/* reset */
			if(menu_index == 2){
				GUI_PrintfStrWithBoxInvert("Reset",85,GENERATOR_MENU_X,GENERATOR_MENU_LINE3_Y);
				if(*p_input_key == key_confirm){
					 gPWM_OutputFreq = 1000;
					 gPWM_OutputDuty = 0.5f;
					 BSP_PWM_SetFreq(gPWM_OutputFreq);
			     BSP_PWM_SetCh1Duty(gPWM_OutputDuty);
				}
			}
			else
				GUI_PrintString("Reset",GENERATOR_MENU_X,GENERATOR_MENU_LINE3_Y);
		}
		
		/* setting mode */
		else{
			/* setting freqency */
			if(menu_index == 0){
				if(*p_input_key == key_right)
					gPWM_OutputFreq+=1;
				else if(*p_input_key == key_left)
					gPWM_OutputFreq-=1;
				else if(*p_input_key == key_right_fast)
					gPWM_OutputFreq+=10;
				else if(*p_input_key == key_left_fast)
					gPWM_OutputFreq-=10;
				else if(*p_input_key == key_right_very_fast){
					if(gPWM_OutputFreq < 10000)
					  gPWM_OutputFreq+=500;
					else
						gPWM_OutputFreq+=2000;
				}
				else if(*p_input_key == key_left_very_fast){
					if(gPWM_OutputFreq < 10000)
					  gPWM_OutputFreq-=500;
					else
						gPWM_OutputFreq-=2000;
				}
			
				else if(*p_input_key == key_confirm){
					setting_flag = 0;
				}
				gPWM_OutputFreq = Constraint_int(gPWM_OutputFreq,18,500000);
				GUI_PrintStrInt6Str("Set Freq:",gPWM_OutputFreq,"Hz",GENERATOR_MENU_X,GENERATOR_MENU_LINE3_Y);
      }
			
			/* setting duty circle */
			else if(menu_index == 1){
				
				if(*p_input_key == key_right)
					gPWM_OutputDuty+=0.001;
				else if(*p_input_key == key_left)
					gPWM_OutputDuty-=0.001;
				else if((*p_input_key == key_right_fast)||(*p_input_key == key_right_very_fast))
					gPWM_OutputDuty+=0.01;
				else if((*p_input_key == key_left_fast)||(*p_input_key == key_left_very_fast))
					gPWM_OutputDuty-=0.01;
				
				else if(*p_input_key == key_confirm){
					setting_flag = 0;
				}
				gPWM_OutputDuty = Constraint_f(gPWM_OutputDuty,0.001,0.999f);
				GUI_PrintStrFloat3_1Str("Set Duty:   ",gPWM_OutputDuty*100,"%",GENERATOR_MENU_X,GENERATOR_MENU_LINE3_Y);
      }
			BSP_PWM_SetFreq(gPWM_OutputFreq); /* >=18Hz */
			BSP_PWM_SetCh1Duty(gPWM_OutputDuty);
    }
		
		/* exit generator task */
		if(*p_input_key == key_exit){
			/* display exit tips */
			GUI_Clear();
			GUI_PrintfStrWithBoxInvert(" EXIT ",39,45,27);
			mini12864_Refresh();
			BSP_OS_TimeDlyMs(500);
			/* create menu task */
			MenuTaskCreate();
			/* delete self */
			OSTaskDel((OS_TCB*)0,&err);
    }
		
		mini12864_Refresh();
  }
}


/*******************************************************************************************************/
void GeneratorTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&GeneratorTaskTCB,
               (CPU_CHAR   *)"Generator Task",
               (OS_TASK_PTR )GeneratorTask,
               (void       *)0,
               (OS_PRIO     )GENERATOR_TASK_PRIO,
               (CPU_STK    *)&GeneratorTaskStk[0],
               (CPU_STK_SIZE)GENERATOR_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)GENERATOR_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate Generator Task...\n\r"));
  }
}
