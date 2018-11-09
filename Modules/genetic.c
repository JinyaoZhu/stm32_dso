#include "genetic.h"
#include "stdlib.h"
#include "global.h"
#include "algorithm.h"
#include "math.h"

extern uint16_t VirtAddVarTab[NumbOfVar];

float  Genetic_Generation;

float MutationProbability = 0.5f; 

GENETIC_PID_TYPE Genetic_PID;

DNA_TYPE GeneticBestDNA;

uint8_t Genetic_CurrentTestIndex;

float Genetic_OverShoot = 0;

float Genetic_ErrorSum = 0;

float Genetic_Health = 0;

float MaxHealth = 0;



/*
*****************************************************************
*          Genetic_PopulationInit()
* Description : Initialize the first generation individuals
* Notic       : start from the second individual
*               the first place for the best individual last store
*****************************************************************
*/
void Genetic_PopulationInit(INDIVIDUAL_TYPE* p)
{
	uint16_t i;
	 
	srand(CPU_TS_Get32());
	
  for(i = 0; i<(MAX_INDIVIDUAL_NUM); i++){
    p[i].dna = (DNA_TYPE)rand()&0x3fffffffu;
		p[i].age = 0;
		p[i].health = 0;
		p[i].test_status = untested;
		p[i].select_status = unselected;
  }
	
	Genetic_Generation = 0;
	
// 	p[0].dna = Genetic_RestoreBestDNA();
// 	p[0].age = 0;
// 	/* make it the highest health score at the start in order to recover the best PID */
// 	/* see auto tune task */
// 	p[0].health = 0; 
//   p[0].test_status = untested;
// 	p[0].select_status = unselected;
// 	
// 	Genetic_IndividualCopy(&p[0],&p[1]); 
}


/*
*****************************************************************
*                   Genetic_Breed()
* Description : every pair of parents have two babies 
*               DNA split at the 8th bit ~ 7th bit
* Parameter   : p --- the population array
*****************************************************************
*/
void Genetic_Breed(INDIVIDUAL_TYPE* p)
{
	float dice;
  uint16_t i;
	DNA_TYPE dad_dna;
	DNA_TYPE mum_dna;
	DNA_TYPE baby1_dna;
	DNA_TYPE baby2_dna;
	
	/* Generation Counter */
	Genetic_Generation++;
	
	
// 	/* change mutation probability */
// 	if(Genetic_Generation < 20)
// 		MutationProbability = 0.5f;
// 	else
// 		MutationProbability = 0.1f;
	
	for(i = 0; i < (MAX_INDIVIDUAL_NUM/2 - 1); i=i+2){
    dad_dna = p[i].dna;
		mum_dna = p[i+1].dna;
		
		p[i].age++;
		p[i+1].age++;
		
		/* reproduction */
		/* baby 1 */
		dice = (rand()%65536)/65535.0f;
		if(dice < 0.2f)
			baby1_dna = dad_dna;
		else if(dice > 0.8f)
			baby1_dna = mum_dna;
		else
		  baby1_dna = (dad_dna&(0xfff003ff))|(mum_dna&(0x000ffc00));
		
		/* baby 2 */
		dice = (rand()%65536)/65535.0f;
		if(dice < 0.2f)
			baby2_dna = dad_dna;
		else if(dice > 0.8f)
			baby2_dna = mum_dna;
		else
		  baby2_dna = (mum_dna&(0xfff003ff))|(dad_dna&(0x000ffc00));
		
		/* gene mutation */
		baby1_dna = Genetic_DNAMutation(baby1_dna);
		baby2_dna = Genetic_DNAMutation(baby2_dna);
		/* Baby 1 */
		p[i+(MAX_INDIVIDUAL_NUM/2)].dna = baby1_dna;
		p[i+(MAX_INDIVIDUAL_NUM/2)].age = 0;
		p[i+(MAX_INDIVIDUAL_NUM/2)].health = 0;
		p[i+(MAX_INDIVIDUAL_NUM/2)].test_status = untested;
		p[i+(MAX_INDIVIDUAL_NUM/2)].select_status = unselected;
		/* Baby 2 */
		p[i+(MAX_INDIVIDUAL_NUM/2)+1].dna = baby2_dna;
    p[i+(MAX_INDIVIDUAL_NUM/2)+1].age = 0;
		p[i+(MAX_INDIVIDUAL_NUM/2)+1].health = 0;
		p[i+(MAX_INDIVIDUAL_NUM/2)+1].test_status = untested;
		p[i+(MAX_INDIVIDUAL_NUM/2)+1].select_status = unselected;
  }
}



/*
*****************************************************************
*                   Genetic_DNAMutation()
* Description : DNA¡¡variation
*****************************************************************
*/
DNA_TYPE Genetic_DNAMutation(DNA_TYPE dna)
{
  if(((rand()%65536u)/65535.0f) < MutationProbability)
      dna ^= ((DNA_TYPE)(0x00000001)<<(uint32_t)(rand()%29));
 
	return dna;
}



/*
*****************************************************************
*                     Genetic_DNADecode()
* Description : get pid value
*****************************************************************
*/
void Genetic_DNADecode(DNA_TYPE dna,GENETIC_PID_TYPE *pid)
{
  pid->kp = 2.0f*((DNA_TYPE)(dna >> 20u)&0x3ff)/1023.0f;
	pid->ki = 5.0f*((DNA_TYPE)(dna >> 10u)&0x3ff)/1023.0f;
	pid->kd = 0.001f*((DNA_TYPE)(dna)&0x3ff)/1023.0f;
}


/*
*****************************************************************
*                   Genetic_Test()
* Description : Test each individual ,get its healthy score
*****************************************************************
*/
uint8_t Genetic_Test(INDIVIDUAL_TYPE* p)
{
  uint16_t i;
	
	for(i = 0; i<MAX_INDIVIDUAL_NUM; i++){
		if(p[i].test_status == untested){
			Genetic_CurrentTestIndex = i;
      p[i].health = Genetic_GetHealth(p[i]);
	  	p[i].test_status = tested;
		}
		if(gFlag_StartAutoTuning == 0)
			break;
	}
	
	if(i == MAX_INDIVIDUAL_NUM)
		return DEF_OK;
	else
		return DEF_FAIL;
}


/*
*****************************************************************
*                   Genetic_GetHealth()
* Description : get health of a individual
*****************************************************************
*/
float Genetic_GetHealth(INDIVIDUAL_TYPE individual)
{
	CPU_TS32 timestamp_old;
	float time_out;
  GENETIC_PID_TYPE pid;
	/* controller */
	const float target = 2.0f;
	float dt;
	float measure;
	float last_measure;
	float error;
	float p_term,i_term,d_term;
	float u;
	/* score */
	float error_sum;
	float u_sum;
	float over_shoot;
	float score;
	float health;
	
	/* initial state */
  BSP_PWM_SetCh2Duty(0.20f); 
	
  BSP_ADC1_StartConversion(1);
	measure = 3.3f*BSP_ADC1_GetFirstADCValue()/4095;
	
	/* wait until measure is stable */
	do{
		last_measure = measure;
		GENETIC_DELAY_MS(100);
		BSP_ADC1_StartConversion(1);
    measure = 3.3f*BSP_ADC1_GetFirstADCValue()/4095;
  }while(fabs(measure - last_measure)>0.005f);
	
  Genetic_DNADecode(individual.dna,&pid);
	
	Genetic_PID.kp = pid.kp;
	Genetic_PID.ki = pid.ki;
	Genetic_PID.kd = pid.kd;
	
	BSP_ADC1_StartConversion(1);
	
	measure = 3.3f*BSP_ADC1_GetFirstADCValue()/4095;
	
	last_measure = measure;
	
	time_out = 0;
	
	error_sum = 0;
	
	u_sum = 0;
	
	over_shoot = 0;
	
	timestamp_old = CPU_TS_Get32();
	
	while(time_out < 3.0f){
		
		dt =  Get_dt(&timestamp_old);
		
		time_out+=dt;
		
	  /* start DMA transmit */
		BSP_ADC1_StartConversion(1);
		
		/* measure */
		measure = DLPF(3.3f*BSP_ADC1_GetFirstADCValue()/4095,measure,0.5f*HZ2RAD,dt);
		
 		gTestData1 = target;
 	  gTestData2 = measure;
 		gTestData3 = error_sum;
		
		if((measure - target )>over_shoot)
			over_shoot = measure - target;
		
		/* PID¡¡control */
		error = target - measure;
		
		error_sum+=(error*error)*dt;
		
		p_term = error;
		
		i_term = Constraint_f(i_term + error*dt,-2.0f,2.0f);
		
		d_term =  DLPF(-(measure - last_measure)/dt,d_term,1*HZ2RAD,dt);
		
		last_measure = measure;
		
		u = Constraint_f((pid.kp)*p_term + (pid.ki)*i_term + (pid.kd)*d_term,0.001,0.999);
		
		u_sum += fabs(u)*dt;
		
		BSP_PWM_SetCh2Duty(u);
		
		BSP_LED_Toggle();
		
		GENETIC_DELAY_MS(PID_CONTROL_PERIOD_MS - 1u);
  }
	
	BSP_LED_Off();
	
	/* punishment */
	score = 1.0f/Constraint_f(0.7f*error_sum + 0.3f*over_shoot,0.00001f,999999.0f);
	
	health = score;
	
	/* report test data */
	Genetic_OverShoot = over_shoot;
	
	Genetic_ErrorSum = error_sum;
	
	Genetic_Health = health;
	
	return health;
}

/*
*****************************************************************
*                   Genetic_Select()
* Description : keep the good ones and kill the bad ones
*****************************************************************
*/
void Genetic_Select(INDIVIDUAL_TYPE* p)
{
  uint16_t num_of_selected = 0;
	uint16_t i,j;
	uint8_t flag_found;
	INDIVIDUAL_TYPE temp_individual;
	float health_sum_p;
	float max_health;
	float dice;
	
	/* init */
	for(i = 0; i<MAX_INDIVIDUAL_NUM; i++)
    p[i].select_status = unselected;
	
	/* find max health */
	max_health = 0;
	for(i = 0; i<MAX_INDIVIDUAL_NUM; i++){
    if(p[i].health > max_health)
			max_health = p[i].health;
  }
	
	MaxHealth = max_health;

	/* slelect  (MAX_INDIVIDUAL_NUM/2) individuals */
	/* RWS algorithm */
	while(num_of_selected < (MAX_INDIVIDUAL_NUM/2)){
		
		health_sum_p = 0;
		
    i = MAX_INDIVIDUAL_NUM - 1;		
		
		dice = (rand()%65536)/65535.0f;

		while(health_sum_p < dice){
			i = (i+1)%(MAX_INDIVIDUAL_NUM);
			if(p[i].select_status == unselected)
        health_sum_p += (p[i].health)/max_health;
    }

		p[i].select_status = selected;
		
    num_of_selected++;
  }

	
	/* sort */
	for(i = 0; i<(MAX_INDIVIDUAL_NUM/2);i++){
		
    if(p[i].select_status != selected){
			/* reset flag */
			flag_found = 0;
			/* find a selected one in the later half */
      for(j = (MAX_INDIVIDUAL_NUM/2); (j<MAX_INDIVIDUAL_NUM)&&(flag_found == 0); j++){
				/* copy */
				if(p[j].select_status == selected){
					p[i].dna = p[j].dna;
					p[i].select_status = p[j].select_status;
					p[i].test_status = p[j].test_status;
					p[i].health = p[j].health;
					p[i].age = p[j].age;
					
					p[j].select_status = unselected;
					
					flag_found = 1; 
				}
      }
    }
  }
	
	/* change the order randomly */
	i = rand()%(MAX_INDIVIDUAL_NUM/2);
	j = rand()%(MAX_INDIVIDUAL_NUM/2);

	if(i != j){
	  Genetic_IndividualCopy(&p[i],&temp_individual); 
	  Genetic_IndividualCopy(&p[j],&p[i]);  /* j to i */
	  Genetic_IndividualCopy(&temp_individual,&p[j]); /* i to j */
	}
}



/*
****************************************************************************
*                     Genetic_IndividualCopy()
****************************************************************************
*/
void Genetic_IndividualCopy(INDIVIDUAL_TYPE *from,INDIVIDUAL_TYPE *to)
{
  to->dna = from->dna;
 	to->age = from->age;
 	to->health = from->health;
  to->test_status = from->test_status;
 	to->select_status = from->select_status;
}

/*
****************************************************************************
*                     Genetic_GetKp()
****************************************************************************
*/
float Genetic_GetKp(void)
{
  return Genetic_PID.kp;
}


/*
****************************************************************************
*                        Genetic_GetKi()
****************************************************************************
*/
float Genetic_GetKi(void)
{
  return Genetic_PID.ki;
}


/*
****************************************************************************
*                        Genetic_GetKd()
****************************************************************************
*/
float Genetic_GetKd(void)
{
  return Genetic_PID.kd;
}



/*
****************************************************************************
*                         Genetic_GetOverShoot()
****************************************************************************
*/
float Genetic_GetOverShoot(void)
{
  return Genetic_OverShoot;
}


/*
****************************************************************************
*                         Genetic_GetErrorSum()
****************************************************************************
*/
float Genetic_GetErrorSum(void)
{
  return Genetic_ErrorSum;
}

/*
****************************************************************************
*                         Genetic_GetMaxHealth()
****************************************************************************
*/
float Genetic_GetMaxHealth(void)
{
  return MaxHealth;
}


/*
****************************************************************************
*                          Genetic_GetHealthScore()
****************************************************************************
*/
float Genetic_GetHealthScore(void)
{
  return Genetic_Health;
}


uint8_t Genetic_GetCurrentTestIndex(void)
{
  return Genetic_CurrentTestIndex;
}

/*
****************************************************************************
*                           Genetic_GetBestDNA()
* Description: get the best individual from population
****************************************************************************
*/
DNA_TYPE Genetic_GetBestDNA(INDIVIDUAL_TYPE *p)
{
  uint16_t index = 0;
	uint16_t i;
	float max_health = 0;
	for(i = 0; i<MAX_INDIVIDUAL_NUM; i++){
    if(p[i].health > max_health){
			max_health = p[i].health;
			index = i;
		} 
  }
	return p[index].dna;
}

/*
****************************************************************************
*                          Genetic_GetBestPID()
****************************************************************************
*/
void Genetic_GetBestPID(INDIVIDUAL_TYPE *p,GENETIC_PID_TYPE *pid)
{
  DNA_TYPE best_dna;
	best_dna = Genetic_GetBestDNA(p);
	Genetic_DNADecode(best_dna,pid);
}



float Genetic_GetGenerations(void)
{
  return Genetic_Generation;
}


/*
****************************************************************************
*                          Genetic_StoreBestDNA()
****************************************************************************
*/
void Genetic_StoreBestDNA(INDIVIDUAL_TYPE *p)
{
	DNA_TYPE best_dna;
  best_dna = Genetic_GetBestDNA(p);
	EE_WriteVariable(VirtAddVarTab[0],(uint16_t)(best_dna>>16));
	EE_WriteVariable(VirtAddVarTab[1],(uint16_t)(best_dna));
}

/*
****************************************************************************
*                          Genetic_RestoreBestDNA()
****************************************************************************
*/
DNA_TYPE Genetic_RestoreBestDNA(void)
{
	DNA_TYPE dna_h,dna_l;
	EE_ReadVariable(VirtAddVarTab[0],(uint16_t*)&dna_h);
	EE_ReadVariable(VirtAddVarTab[1],(uint16_t*)&dna_l);
	return ((dna_h<<16)|dna_l);
}


