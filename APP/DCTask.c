#include "includes.h"
#include "global.h"
#include "GUI.h"
#include "algorithm.h"
#include "genetic.h"

__align(8) static  CPU_STK  DCTaskStk[DC_TASK_STK_SIZE];
OS_TCB  DCTaskTCB;

#define  DC_OUTPUT_MENU_LINE1_X       (6)
#define  DC_OUTPUT_MENU_LINE2_X       (DC_OUTPUT_MENU_LINE1_X+63)

#define  DC_OUTPUT_MENU_LINE1_Y (40)
#define  DC_OUTPUT_MENU_LINE2_Y (DC_OUTPUT_MENU_LINE1_Y - 12)
#define  DC_OUTPUT_MENU_LINE3_Y (DC_OUTPUT_MENU_LINE2_Y - 12)
#define  DC_OUTPUT_MENU_LINE4_Y (DC_OUTPUT_MENU_LINE3_Y - 12)


extern __IO uint16_t  AD_Value[];

extern INDIVIDUAL_TYPE Population[MAX_INDIVIDUAL_NUM];

extern GENETIC_PID_TYPE BestPID;

extern CPU_INT08U Flag_Genetic_TestFinished;


/*******************************************************************************************************/
void DCTask(void *p_arg)
{
	OS_ERR err;
	KEY_TYPE *p_input_key;
	OS_MSG_SIZE input_key_data_size;
  /* for auto tuning */
	CPU_INT08U menu_index = 3;
	CPU_FP32 plot_scale = 1;
	CPU_INT16U plot_step = 1;
	CPU_INT08U i;
  /* for dc out */
	CPU_TS32 timestamp_old;
	CPU_FP32 dt;
	CPU_FP32 target_v = 1;
	CPU_FP32 measure_v = 1;
	CPU_FP32 u_pwm_duty = 0.5f;
	CPU_FP32 p_term;
	CPU_FP32 i_term = 0;
	CPU_FP32 d_term;
	CPU_FP32 error;
	CPU_FP32 last_measure_v = 0;
	
	(void)p_arg;
	
	BSP_PWM_SetFreq(10000);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); /* run ADC at slowest mode 35.714kHz */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );
	
	BSP_PWM_SetCh2Duty(0.36); /* about 1V */
	
	AutoTuneTaskCreate();
	
  while (1)
  {
		/* pend 100ms */
		p_input_key = (KEY_TYPE*)OSQPend(&Que_InputKeyData,100,OS_OPT_PEND_BLOCKING,&input_key_data_size,(CPU_TS *)0,&err);
		
		/* exit DC task */
	 if(*p_input_key == key_exit){	
			BSP_PWM_SetFreq(1000);
	    BSP_PWM_SetCh2Duty(0.5f);
			/* display exit tips */
			GUI_Clear();
			GUI_PrintfStrWithBoxInvert(" EXIT ",39,45,27);
			mini12864_Refresh();
		  gFlag_StartAutoTuning = 0;
		  /* wait the test in auto tune task finish */
			while(Flag_Genetic_TestFinished != 1)
				BSP_OS_TimeDlyMs(50);
		  OSTaskDel(&AutoTuneTaskTCB,&err);
			BSP_OS_TimeDlyMs(500);
			MenuTaskCreate();
			OSTaskDel((OS_TCB*)0,&err);
    }
		
				
		if(*p_input_key == key_confirm){
      menu_index = (++menu_index)%4;
    }

				
		/* diaplay fixed part */
	  GUI_Clear();
		GUI_DrawLine(0,0,127,0);
		GUI_DrawLine(0,63,127,63);
		GUI_DrawLine(0,0,0,63);
		GUI_DrawLine(127,0,127,63);
		GUI_DrawLine(0,53,127,53);

		
		
		/* page one */
		if(menu_index == 0){
			
			i_term = 0;
			
			gFlag_StartAutoTuning = 1;
			
			GUI_PrintString("Auto Tune",37,54);
			
			GUI_PrintStrFloat3_2Str("OverShoot :",Genetic_GetOverShoot(),"V",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE1_Y);
			
			GUI_PrintStrFloat3_2Str("ErrorSum  :",Genetic_GetErrorSum(),"V",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE2_Y);
			
			GUI_PrintStrFloat3_2Str("Health    :",Genetic_GetHealthScore(),"",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE3_Y);
				
			GUI_PrintStrInt3Str("Generation:",Genetic_GetGenerations(),"",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE4_Y);
			
			GUI_PrintStrInt3Str("",Genetic_GetCurrentTestIndex(),"",DC_OUTPUT_MENU_LINE1_X+95,DC_OUTPUT_MENU_LINE4_Y);
		}
		/* page two */
		else if(menu_index == 1){
			
			gFlag_StartAutoTuning = 1;
			
			GUI_PrintString("PID coefficient",17,54);
			
			GUI_DrawLine(63,1,63,53);
			
			GUI_PrintString("Current:",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE1_Y);
			
		  GUI_PrintStrFloat4_3Str("KP:",Genetic_GetKp(),"",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE2_Y);
			
			GUI_PrintStrFloat4_3Str("KI:",Genetic_GetKi(),"",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE3_Y);
				
			GUI_PrintStrFloat4_3Str("KD:",Genetic_GetKd(),"",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE4_Y);
			
		  GUI_PrintString("Best:",DC_OUTPUT_MENU_LINE2_X,DC_OUTPUT_MENU_LINE1_Y);
			
		  GUI_PrintStrFloat4_3Str("KP:",BestPID.kp,"",DC_OUTPUT_MENU_LINE2_X,DC_OUTPUT_MENU_LINE2_Y);
			
			GUI_PrintStrFloat4_3Str("KI:",BestPID.ki,"",DC_OUTPUT_MENU_LINE2_X,DC_OUTPUT_MENU_LINE3_Y);
				
			GUI_PrintStrFloat4_3Str("KD:",BestPID.kd,"",DC_OUTPUT_MENU_LINE2_X,DC_OUTPUT_MENU_LINE4_Y);
    }
		else if(menu_index == 2){
			
			gFlag_StartAutoTuning = 1;
			
			GUI_PrintString("Health Distribute",14,54);
			
			GUI_PrintStrFloat3_2Str("MaxHealth :",Genetic_GetMaxHealth(),"",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE1_Y);	
			
      plot_scale = 20.0f/Genetic_GetMaxHealth();
			
			plot_step = (GRAPH_BUF_SIZE-18)/(MAX_INDIVIDUAL_NUM/2);
			
			/* reset buffer */
			for(i = 0 ; i < GRAPH_BUF_SIZE; i++)
			  gGraphBuf[i] = 0;	
			
			/* fill buffer */
			for(i = 0 ; i < (MAX_INDIVIDUAL_NUM/2); i++){
				GUI_PrintStrInt3Str("",Population[i].age,"",10+i*plot_step - 12,plot_scale*(Population[i].health)+5+0.5f);
			  gGraphBuf[10+i*plot_step] = Constraint_int(plot_scale*(Population[i].health)+0.5f,0,30);	
			}
			
		  /* draw graph */
				GUI_DrawGraph(gGraphBuf,GRAPH_BUF_SIZE,4,3);
    }
		/* disable auto tuning */
		else if(menu_index == 3){
			
			  /* indicate auto tune task to stop tunning */
			  gFlag_StartAutoTuning = 0;
			
			  /* wait the test in auto tune task finish */
			  while(Flag_Genetic_TestFinished != 1)
				  BSP_OS_TimeDlyMs(50);
			
				if(*p_input_key == key_left){
					target_v = Constraint_f(target_v-0.01f,0.0f,3.0f);
				}
				else if(*p_input_key == key_right){
					target_v = Constraint_f(target_v+0.01f,0.0f,3.0f);
				}
				else if((*p_input_key == key_left_fast)||(*p_input_key == key_left_very_fast)){
					target_v = Constraint_f(target_v-0.10f,0.0f,3.0f);
				}
				else if((*p_input_key == key_right_fast)||(*p_input_key == key_right_very_fast)){
					target_v = Constraint_f(target_v+0.10f,0.0f,3.0f);
				}
					
			  dt =  Get_dt(&timestamp_old);
		
				/* start DMA transmit */
				BSP_ADC1_StartConversion(1);
				
				measure_v = DLPF(3.3f*AD_Value[0]/4095,measure_v,0.5*HZ2RAD,dt);
				
				/* PID¡¡routine */
				
				error = target_v - measure_v;
				
				p_term = error;
				
				i_term = Constraint_f(i_term + error*dt,-2.0f,2.0f);
				
				d_term =  DLPF(-(measure_v - last_measure_v)/dt,d_term,1*HZ2RAD,dt);
				
				last_measure_v = measure_v;
				
				u_pwm_duty = Constraint_f(BestPID.kp*p_term + BestPID.ki*i_term + BestPID.kd*d_term,0.00001,0.99999);
				
				gTestData1 = target_v;
				gTestData2 = measure_v;
				gTestData3 = u_pwm_duty;
				
				BSP_PWM_SetCh2Duty(u_pwm_duty);
				
				GUI_PrintString("DC Output",37,54);
				
				GUI_PrintStrFloat3_2Str("Ts:",dt*1000.0f,"ms",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE1_Y);
				
				GUI_PrintStrFloat3_2Str("Duty:",100.0f*u_pwm_duty,"%",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE2_Y);
				
				GUI_PrintStrFloat3_2Str("Vdc target :",target_v,"V",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE3_Y);
				
				GUI_PrintStrFloat3_2Str("Vdc measure:",measure_v,"V",DC_OUTPUT_MENU_LINE1_X,DC_OUTPUT_MENU_LINE4_Y);
    }
		
		mini12864_Refresh();
		

		if(gFlag_StartAutoTuning == 1)
		  BSP_OS_TimeDlyMs(50);

  }
}


/*******************************************************************************************************/
void DCTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&DCTaskTCB,
               (CPU_CHAR   *)"DC Task",
               (OS_TASK_PTR )DCTask,
               (void       *)0,
               (OS_PRIO     )DC_TASK_PRIO,
               (CPU_STK    *)&DCTaskStk[0],
               (CPU_STK_SIZE)DC_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)DC_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate DC Task...\n\r"));
  }
}
