#include "includes.h"
#include "global.h"
#include "GUI.h"
#include "algorithm.h"

__align(8)  CPU_STK  MenuTaskStk[MENU_TASK_STK_SIZE];
OS_TCB  MenuTaskTCB;

#define MENU_X        (6)

#define MENU_LINE1_Y  (40)
#define MENU_LINE2_Y  (MENU_LINE1_Y - 12)
#define MENU_LINE3_Y  (MENU_LINE2_Y - 12)


/*******************************************************************************************************/
void MenuTask(void *p_arg)
{
	CPU_INT08S menu_index;
	KEY_TYPE *p_input_key;
	OS_MSG_SIZE msg_size;
	OS_ERR err;
	(void)p_arg;
	
	menu_index = 0;
	
  while (1)
  {

		/* msg size could not be void */
	  p_input_key = (KEY_TYPE*)OSQPend(&Que_InputKeyData,100,OS_OPT_PEND_BLOCKING,&msg_size,(CPU_TS *)0,&err);
		
		if(*p_input_key == key_left){
			menu_index = Constraint_int(menu_index+1,0,2);
		}
		else if(*p_input_key == key_right){
			 menu_index = Constraint_int(menu_index-1,0,2);
		}

		
		GUI_Clear();
		GUI_DrawLine(0,0,127,0);
		GUI_DrawLine(0,63,127,63);
		GUI_DrawLine(0,0,0,63);
		GUI_DrawLine(127,0,127,63);
		GUI_DrawLine(0,53,127,53);
		GUI_PrintString("M E N U",40,54);

		
		if(menu_index == 0){
			GUI_PrintfStrWithBoxInvert("Oscillator",85,MENU_X,MENU_LINE1_Y);
			if(*p_input_key == key_confirm){
				GUI_Clear();
				OscTaskCreate();
				OSTaskDel((OS_TCB *)0,&err); /* delete self */
			}
		}
		else
			GUI_PrintString("Oscillator",MENU_X,MENU_LINE1_Y);
		
		if(menu_index == 1){
			GUI_PrintfStrWithBoxInvert("PWM Generator",85,MENU_X,MENU_LINE2_Y);
			if(*p_input_key == key_confirm){
				GUI_Clear();
				GeneratorTaskCreate();
				OSTaskDel((OS_TCB *)0,&err); /* delete self */
			}
		}
		else
			GUI_PrintString("PWM Generator",MENU_X,MENU_LINE2_Y);
		
		if(menu_index == 2){
			GUI_PrintfStrWithBoxInvert("DC Output",85,MENU_X,MENU_LINE3_Y);
			if(*p_input_key == key_confirm){
				GUI_Clear();
				DCTaskCreate();
				OSTaskDel((OS_TCB *)0,&err); /* delete self */
			}
		}
		else
			GUI_PrintString("DC Output",MENU_X,MENU_LINE3_Y);
		
	 // BSP_OS_SemPost(&LCD_Update_Sem);
			
		mini12864_Refresh();
  }
}


/*******************************************************************************************************/
void MenuTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&MenuTaskTCB,
               (CPU_CHAR   *)"Menu Task",
               (OS_TASK_PTR )MenuTask,
               (void       *)0,
               (OS_PRIO     )MENU_TASK_PRIO,
               (CPU_STK    *)&MenuTaskStk[0],
               (CPU_STK_SIZE)MENU_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)MENU_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate Menu Task...\n\r"));
  }
}
