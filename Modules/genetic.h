#ifndef __GENETIC_H__
#define __GENETIC_H__

#define MAX_INDIVIDUAL_NUM          (12u)  /* should be an even number bigger than 2 */

#include "bsp.h"

#define GENETIC_DELAY_MS(ms)        BSP_OS_TimeDlyMs(ms)

#define PID_CONTROL_PERIOD_MS       (50u)

typedef enum {unselected = 0,selected}SELECT_STATUS_TYPE;

typedef enum {untested = 0,tested}TEST_STATUS_TYPE;

typedef uint32_t DNA_TYPE;

typedef struct
{
  float kp;
	float ki;
	float kd;
}GENETIC_PID_TYPE;

typedef struct 
{
  DNA_TYPE dna; /* bit0~9:D, bit10~19:I, bit20~29:P */
	uint16_t age;
	SELECT_STATUS_TYPE select_status;
	TEST_STATUS_TYPE   test_status;
	float health;
}INDIVIDUAL_TYPE;

/*************************Private**********************************/
DNA_TYPE Genetic_DNAMutation(DNA_TYPE dna);
float Genetic_GetHealth(INDIVIDUAL_TYPE individual);

/*************************Public*************************************/
void Genetic_PopulationInit(INDIVIDUAL_TYPE* p);
void Genetic_DNADecode(DNA_TYPE dna,GENETIC_PID_TYPE *pid);
void Genetic_Breed(INDIVIDUAL_TYPE* p);
uint8_t Genetic_Test(INDIVIDUAL_TYPE* p);
void Genetic_Select(INDIVIDUAL_TYPE* p);
void Genetic_IndividualCopy(INDIVIDUAL_TYPE *from,INDIVIDUAL_TYPE *to);

float Genetic_GetKp(void);
float Genetic_GetKi(void);
float Genetic_GetKd(void);
float Genetic_GetOverShoot(void);
float Genetic_GetErrorSum(void);
float Genetic_GetHealthScore(void);
float Genetic_GetMaxHealth(void);
float Genetic_GetGenerations(void);
uint8_t Genetic_GetCurrentTestIndex(void);
DNA_TYPE Genetic_GetBestDNA(INDIVIDUAL_TYPE *p);
void Genetic_GetBestPID(INDIVIDUAL_TYPE *p,GENETIC_PID_TYPE *pid);
void Genetic_StoreBestDNA(INDIVIDUAL_TYPE *p);
DNA_TYPE Genetic_RestoreBestDNA(void);

#endif /* __GENETIC_H__ */
