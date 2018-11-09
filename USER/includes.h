#ifndef __INCLUDES_H
#define __INCLUDES_H

/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>


/*
*********************************************************************************************************
*                                              LIBRARIES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>


/*
*********************************************************************************************************
*                                              ST
*********************************************************************************************************
*/
#include "stm32f10x.h"



/*
*********************************************************************************************************
*                                              UC/OS
*********************************************************************************************************
*/
#include "os.h"
#include "os_type.h"
#include "os_cfg_app.h"
#include <os_app_hooks.h>

/*
*********************************************************************************************************
*                                             APP/BSP
*********************************************************************************************************
*/

#include "BSP.h"
#include "app.h"


extern OS_TCB  FreqGetTaskTCB;
extern OS_TCB  ADSampleTaskTCB;
extern OS_TCB  AutoTuneTaskTCB;

void PrintfTaskCreate(void);
void OscTaskCreate(void);
void FreqGetTaskCreate(void);
void ADSampleTaskCreate(void);
void MenuTaskCreate(void);
void GeneratorTaskCreate(void);
void GetKeyTaskCreate(void);
void DCTaskCreate(void);
void AutoTuneTaskCreate(void);

#endif /* __INCLUDES_H */
