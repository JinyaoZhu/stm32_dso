#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "stm32f10x.h"

#define PI     (3.1415926535f)

#define DEG2RAD        (PI/180.0)
#define RAD2DEG        (180.0/PI)

#define HZ2RAD         (2*PI)


typedef struct
{
  float old_data;
	float output;
}DHPF_Struct;


typedef union 
{
	float f;
	uint16_t uint[2];
}f2int16;


typedef struct
{
  uint32_t cnt;
	int32_t sum;
	int32_t FIFO_Size;
	int32_t *FIFO;
}MovAvgFIFOStruct_int;


typedef struct
{
  uint32_t cnt;
	float sum;
	uint32_t FIFO_Size;
	float *FIFO;
}MovAvgFIFOStruct_f;



float InvSqrt(float x);


/************************FILTER*******************************/
void     MoveAverage_int_FIFOStructInit(MovAvgFIFOStruct_int *p,int32_t *FIFO,int32_t size_of_FIFO);

int32_t  MoveAverage_int(int32_t new_data,MovAvgFIFOStruct_int *p);

void     MoveAverage_f_FIFOStructInit(MovAvgFIFOStruct_f *p,float *FIFO,uint32_t size_of_FIFO);

float    MoveAverage_f(float new_data,MovAvgFIFOStruct_f *p);

float    DHPF(float new_data,DHPF_Struct *p);

float    DLPF(float new_data,float old_data,float a,float dt);

/*********************OPTIMIZED ALGORITHM**********************/
void CORDIC_Init(void);

void CORDIC_SinCos(uint32_t theta, int32_t *sin, int32_t *cos);

float    fast_sin(float x);

float    fast_cos(float x);

void     fast_SinCos(float x,float *sin,float *cos);

float    fast_acos(float x);

float    fast_asin(float x);

float    fast_atan2(float x_val,float y_val);

float    WarpToPI(float a);

/*********************CONSTRAINT**************************/
float    Constraint_f(float a , float lower ,float upper);

int32_t  Constraint_int(int32_t a , int32_t low ,int32_t upper);

#endif /*__ALGORITHM_H__*/
