#include "includes.h"
#include "mini12864.h"
#include "global.h"
#include "algorithm.h"
#include "GUI.h"

#define OSC_MENU_X        (6)

#define OSC_MENU_LINE1_Y  (40)
#define OSC_MENU_LINE2_Y  (OSC_MENU_LINE1_Y - 12)
#define OSC_MENU_LINE3_Y  (OSC_MENU_LINE2_Y - 12)
#define OSC_MENU_LINE4_Y  (OSC_MENU_LINE3_Y - 12)

__align(8) static  CPU_STK  OscTaskStk[OSC_TASK_STK_SIZE];
OS_TCB  OscTaskTCB;

extern __IO uint16_t  AD_Value[];



/*******************************************************************************************************/
void OscTask(void *p_arg)
{	
	CPU_INT08U loop_cnt = 0;
	KEY_TYPE *p_input_key;
	OS_MSG_SIZE input_key_data_size;
	OS_ERR err;
	CPU_INT08U menu_index = 0;
	(void)p_arg;
	
	/* create objects for oscillator task */
	BSP_OS_SemCreate(&FreqGet_Sem,0,"FreqGet_Sem");
	BSP_OS_SemCreate(&AD_StartConvert_Sem,0,"AD_StartConvert_Sem");
	BSP_OS_SemCreate(&StartPlot_Sem,0,"StartPlot_Sem");
	
	ADSampleTaskCreate();
	
	FreqGetTaskCreate();
	
  while (1)
  {
		p_input_key = (KEY_TYPE*)OSQPend(&Que_InputKeyData,0,OS_OPT_PEND_BLOCKING,&input_key_data_size,(CPU_TS *)0,&err);
		
		if(*p_input_key == key_confirm){
			menu_index = (++menu_index)%2;
		}
		else if(*p_input_key == key_left){
      gMeasureRange = (V_RANGE_TYPE)Constraint_f(gMeasureRange+1,0,3); /* see definition of  V_RANGE_TYPE */
    }
		else if(*p_input_key == key_right){
      gMeasureRange = (V_RANGE_TYPE)Constraint_f(gMeasureRange-1,0,3);
    }
		
		if(BSP_OS_SemWait(&StartPlot_Sem ,10) == DEF_OK)
		{
  		GUI_Clear();
			
			if(menu_index == 0)
			{
				/* draw graph */
				GUI_DrawGraph(gGraphBuf,GRAPH_BUF_SIZE,4,33);
				/* fixed part */
				GUI_DrawLine(0,10,127,10);
				GUI_DrawLine(0,0,127,0);
				GUI_DrawLine(0,63,127,63);
				GUI_DrawLine(0,0,0,63);
				GUI_DrawLine(127,0,127,63);
				GUI_DrawLine(68,0,68,10);
				
				if(gFlag_Trigger == 1)
				{
					switch(loop_cnt)
					{
						case 0:
						case 1:
						case 2:
							if(gSignal_Vpp/gSignalScaleFactor < 2.0f)
								GUI_PrintfStrWithBoxInvert("signal too weak",95,20,OSC_MENU_LINE2_Y);
						
							GUI_FillRect(5,57,7,59);
							break;
							
						case 4:
						case 5:
						case 6:
							GUI_FillRect(9,57,11,59);
							break;
							
						case 7:	
						case 8:
						case 9:
		        	if(gSignal_Vpp/gSignalScaleFactor < 2.0f)
								GUI_PrintfStrWithBoxInvert("signal too weak",95,20,OSC_MENU_LINE2_Y);
							GUI_FillRect(13,57,15,59);
							break;
						
						case 10:
						case 11:
						case 12:
							GUI_FillRect(17,57,19,59);
							break;
						default:
							break;
					}
				}
				else
					GUI_PrintfStrWithBoxInvert("no Trig",45,42,OSC_MENU_LINE2_Y);
				
				loop_cnt=(++loop_cnt)%13;
				
				
				if(FreqMeasured >= 100000)
					GUI_PrintStrFloat4_1Str("F:",FreqMeasured*0.001f,"kHz",58,54);
				else if(FreqMeasured >= 10000)
					GUI_PrintStrFloat4_2Str("F:",FreqMeasured*0.001f,"kHz",58,54);
				else if(FreqMeasured >= 1000)
					GUI_PrintStrFloat4_3Str("F:",FreqMeasured*0.001f,"kHz",58,54);
				else
					GUI_PrintStrFloat4_2Str("F:",FreqMeasured,"Hz",58,54);
			
				GUI_PrintStrFloat3_2Str("Vpp:",gSignal_Vpp,"V",5,1);

				GUI_PrintStrFloat3_1Str("D:",gSignal_Duty,"%",75,1);
				
				if(gMeasureRange == range_1v)
				  GUI_PrintString("<=1V",23,54);
				else if(gMeasureRange == range_3v)
					GUI_PrintString("<=3V",23,54);
				else if(gMeasureRange == range_7v)
					GUI_PrintString("<=7V",23,54);
				else if(gMeasureRange == range_13v)
          GUI_PrintString("<=13V",23,54);					
				
			}/* menu_index == 0 */
			else if(menu_index == 1){
				
				/* fixed part */
		    GUI_DrawLine(0,0,127,0);
	    	GUI_DrawLine(0,63,127,63);
	    	GUI_DrawLine(0,0,0,63);
		    GUI_DrawLine(127,0,127,63);
		    GUI_DrawLine(0,53,127,53);
		    GUI_PrintString("Oscillator",37,54);
				
        GUI_PrintStrFloat3_2Str("Vmax: ",gSignal_MaxV,"V",OSC_MENU_X,OSC_MENU_LINE1_Y);
		
		    GUI_PrintStrFloat3_2Str("Vmin: ",gSignal_MinV,"V",OSC_MENU_X,OSC_MENU_LINE2_Y);
		
		    GUI_PrintStrFloat3_2Str("Vpp : ",gSignal_Vpp,"V",OSC_MENU_X,OSC_MENU_LINE3_Y);
		
	    	GUI_PrintStrFloat3_2Str("Vavg: ",gSignal_Vavg,"V",OSC_MENU_X,OSC_MENU_LINE4_Y);

      }/* menu_index == 1 */
			mini12864_Refresh();
		}
		
		
		/* exit oscillator task */
		if(*p_input_key == key_exit){
			OSTaskDel(&FreqGetTaskTCB,&err);
			OSTaskDel(&ADSampleTaskTCB,&err);
      OSSemDel(&FreqGet_Sem,OS_OPT_DEL_ALWAYS,&err);
			OSSemDel(&AD_StartConvert_Sem,OS_OPT_DEL_ALWAYS,&err);
			OSSemDel(&StartPlot_Sem,OS_OPT_DEL_ALWAYS,&err);
			/* display exit tips */
			GUI_Clear();
			GUI_PrintfStrWithBoxInvert(" EXIT ",39,45,27);
			mini12864_Refresh();
			BSP_OS_TimeDlyMs(500);
			MenuTaskCreate();
			OSTaskDel((OS_TCB*)0,&err);
    }

  }
}


/*******************************************************************************************************/
void OscTaskCreate(void)
{
  OS_ERR err;
  OSTaskCreate((OS_TCB     *)&OscTaskTCB,
               (CPU_CHAR   *)"Oscillator Task",
               (OS_TASK_PTR )OscTask,
               (void       *)0,
               (OS_PRIO     )OSC_TASK_PRIO,
               (CPU_STK    *)&OscTaskStk[0],
               (CPU_STK_SIZE)OSC_TASK_STK_SIZE / 10,
               (CPU_STK_SIZE)OSC_TASK_STK_SIZE,
               (OS_MSG_QTY  )5u,
               (OS_TICK     )0u,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);
  if (err == OS_ERR_NONE)
  {
    APP_TRACE_INFO(("\n\rCreate Oscillator Task...\n\r"));
  }
}
