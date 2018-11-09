/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__



/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

/*OS_CFG_TICK_TASK_PRIO             5u */

#define  APP_TASK_START_PRIO                                2u
#define  MENU_TASK_PRIO                                     3u
#define  AD_SAMPLE_TASK_PRIO                                4u
#define  AUTO_TUNE_TASK_PRIO                                5u
#define  GET_KEY_TASK_PRIO                                  5u
#define  FREQ_GET_TASK_PRIO                                 5u
#define  DC_TASK_PRIO                                       5u
#define  GENERATOR_TASK_PRIO                                10u
#define  OSC_TASK_PRIO                                      10u
#define  PRINTF_TASK_PRIO                                   11u



/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         128u
#define  MENU_TASK_STK_SIZE                              128u
#define  OSC_TASK_STK_SIZE                               180u
#define  PRINTF_TASK_STK_SIZE                            180u
#define  FREQ_GET_TASK_STK_SIZE                          64u
#define  AD_SAMPLE_TASK_STK_SIZE                         128u
#define  GENERATOR_TASK_STK_SIZE                         180u
#define  GET_KEY_TASK_STK_SIZE                           64u
#define  DC_TASK_STK_SIZE                                180u
#define  AUTO_TUNE_TASK_STK_SIZE                         150u

/*
*********************************************************************************************************
*                                           uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#include <lib_cfg.h>

/*
*********************************************************************************************************
*                                          uC/Probe CONFIGURATION
*********************************************************************************************************
*/

#define  APP_CFG_PROBE_OS_PLUGIN_EN    DEF_DISABLED
#define  APP_CFG_PROBE_COM_EN          DEF_DISABLED


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0
#define  TRACE_LEVEL_OFF                            0
#define  TRACE_LEVEL_INFO                           1
#define  TRACE_LEVEL_DEBUG                          2
#endif

#define  APP_TRACE_LEVEL                            TRACE_LEVEL_INFO
#define  APP_TRACE                                  BSP_Ser_Printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)


#endif
