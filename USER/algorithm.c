#include "algorithm.h"
#include "math.h"
#include "bsp.h"

/****************CORDIC Algoritm*********************/

#define INV_2PI    (0.159154943092f)

#define QUAD1 1
#define QUAD2 2
#define QUAD3 3
#define QUAD4 4

#define NBITS   20 /* should below 30 */

static int32_t ArcTan[NBITS] = {0}; /* Rad * CordicBase */
static int32_t xInit;
static uint32_t CordicBase;
static float ConvertToReal;     /* converte sin and cos value to virtual unit */
static float ConvertToRad;
static float ConvertToCAU;      /* convert rad to Cordic angle unit */
static float MaxCAU;  /* Max Cordic angle unit */
static uint32_t Quad2Bondary;
static uint32_t Quad3Bondary;
static uint32_t Quad4Bondary;



/*
****************************************************************
*                MoveAverage_int_FIFOStructInit()
* Description : Initialize FIFO
* Parameter   : *p    MovAvgFIFO_int
*               FIFO  an int32 type array
*               size_of_FIFO
****************************************************************
*/
void MoveAverage_int_FIFOStructInit(MovAvgFIFOStruct_int *p,int32_t *FIFO,int32_t size_of_FIFO)
{
  p->cnt = 0;
	p->sum = 0;
	p->FIFO_Size = size_of_FIFO;
	p->FIFO = FIFO;
}

/*
****************************************************************
*                MoveAverage_int()
* Description : Initialize FIFO
* Parameter   :
****************************************************************
*/
int32_t MoveAverage_int(int32_t new_data,MovAvgFIFOStruct_int *p)
{
  p->sum  -= (p->FIFO[p->cnt]);
	p->FIFO[p->cnt] = new_data;
	p->sum  += p->FIFO[p->cnt];
	p->cnt = (p->cnt + 1)%(p->FIFO_Size);
	return ((p->sum)/(p->FIFO_Size));
}

/*
****************************************************************
*                MoveAverage_f_FIFOStructInit()
* Description : Initialize FIFO
* Parameter   : *p    MovAvgFIFO_int
*               FIFO  an float type array
*               size_of_FIFO
****************************************************************
*/
void MoveAverage_f_FIFOStructInit(MovAvgFIFOStruct_f *p, float *FIFO, uint32_t size_of_FIFO)
{
  p->cnt = 0;
	p->sum = 0.0;
	p->FIFO_Size = size_of_FIFO;
	p->FIFO = FIFO;
}

/*
****************************************************************
*                   MoveAverage_f()
* Description : Initialize FIFO
* Parameter   :
****************************************************************
*/
float MoveAverage_f(float new_data,MovAvgFIFOStruct_f *p)
{
  p->sum  -= (p->FIFO[p->cnt]);
	p->FIFO[p->cnt] = new_data;
	p->sum  += p->FIFO[p->cnt];
	p->cnt = (p->cnt + 1)%(p->FIFO_Size);
	return ((p->sum)/(float)(p->FIFO_Size));
}



float Constraint_f(float a , float lower ,float upper)
{
  if(a >= upper)
		a = upper;
	else if(a <= lower)
		a = lower;
	return (a);
}

int32_t Constraint_int(int32_t a , int32_t lower ,int32_t upper)
{
  if(a >= upper)
		return upper;
	else if(a <= lower)
		return lower;
	else 
		return a;
}

/*
************************************************
*               fast_sin()
* Description :  apprximation
************************************************
*/

float fast_sin(float x)  
{ 
	int32_t cos,sin;
	uint32_t theta;
	
	while(x < 0)
	{
		x += 2*PI;
	}
	
	while(x > 2*PI)
	{
    x -= 2*PI;
  }

  theta = (uint32_t)(x * ConvertToCAU + 0.5);
	
	CORDIC_SinCos(theta,&sin,&cos);
	
  return (float)(sin*ConvertToReal);
} 


/*
************************************************
*               fast_cos()
* Description :  apprximation
************************************************
*/
float fast_cos(float x)
{
	int32_t cos,sin;
	uint32_t theta;
	
	while(x < 0)
	{
		x += 2*PI;
	}
	
	while(x > 2*PI)
	{
    x -= 2*PI;
  }
	
  theta = (uint32_t)(x * ConvertToCAU + 0.5);
	
	CORDIC_SinCos(theta,&sin,&cos);
	
  return (float)(cos*ConvertToReal);
}


void fast_SinCos(float x,float *sin,float *cos)
{
	int32_t cos_tmp,sin_tmp;
	uint32_t theta;
	
	while(x < 0)
	{
		x += 2*PI;
	}
	
	while(x > 2*PI)
	{
    x -= 2*PI;
  }
	
  theta = (uint32_t)(x * ConvertToCAU + 0.5);
	
	CORDIC_SinCos(theta,&sin_tmp,&cos_tmp);
	
  *sin = (float)(sin_tmp*ConvertToReal);
	*cos = (float)(cos_tmp*ConvertToReal);
}

/*
************************************************
*               fast_acos()
* Description : 
************************************************
*/
float fast_acos(float u)
{
	return fast_atan2(1 , u * InvSqrt(1.0-u*u));
}

/*
************************************************
*               fast_asin()
* Description : 
************************************************
*/
float fast_asin(float x)
{
  return (fast_acos(-x) - PI*0.5);
}

/*
*****************************************************
*                  fast_atan2()
* Description : +- 0.000001 rad
*****************************************************
*/
float fast_atan2(float y_val, float x_val)
{
		int32_t quadrant = QUAD1;
    int32_t x = 0;
    int32_t y = 0;
    int32_t i = 0;
    int32_t x1, y1;
    int32_t angleSum = 0;
	
	 if(x_val == 0)
		 x_val += 0.000000000001;
	 if(y_val == 0)
     y_val += 0.000000000001;
	 
    x_val *= 65536;	
    y_val *= 65536;

    if( (x_val > 0) && (y_val >= 0))
		{
			x = (int32_t)(x_val + 0.5);
			y = (int32_t)(y_val + 0.5);
			quadrant = QUAD1;
		}
		else if((x_val <= 0) && (y_val > 0))
		{
      x = (int32_t)(-x_val + 0.5);
			y = (int32_t)(y_val + 0.5);
			quadrant = QUAD2;
    }
		else if((x_val < 0) && (y_val <= 0))
		{
      x = (int32_t)(-x_val + 0.5);
			y = (int32_t)(-y_val + 0.5);
			quadrant = QUAD3;
    }
		else 
		{
      x = (int32_t)(x_val + 0.5);
			y = (int32_t)(-y_val + 0.5);
			quadrant = QUAD4;
    }

		for(i = 0; i < NBITS; i++)
		{
				if(y > 0)
				{
						x1 = x + (y >> i);
						y1 = y - (x >> i);
						angleSum += ArcTan[i];
				}
				else
				{
						x1 = x - (y >> i);
						y1 = y + (x >> i);
						angleSum -= ArcTan[i];
				}
				
				x = x1;
				y = y1;
		}
		switch(quadrant)
		{
			case QUAD1:
				break;
			case QUAD2:
				angleSum = 0.5*MaxCAU - angleSum;
				break;
			case QUAD3:
				angleSum = -0.5*MaxCAU + angleSum;
				break;
			case QUAD4:
			  angleSum = -angleSum;
		    break;
			default : break;
    }
		return (angleSum * ConvertToRad);
}


/*
********************************************************
*                  CORDIC_Init
* Description : 
********************************************************
*/

void CORDIC_Init(void)
{
  int32_t i;    /* to inds ArcTan[] */
	double f;     /* to calc initial x projection */
	int64_t powr; /* powers of 2 up to 2^(2*(NBITS - 1)) */
	
	CordicBase = 1 << NBITS;
	Quad2Bondary = CordicBase << 1;
	Quad3Bondary = Quad2Bondary + CordicBase;
	Quad4Bondary = Quad3Bondary + CordicBase;
	
	ConvertToReal = 1.0/CordicBase;
	MaxCAU      = CordicBase << 2;
	ConvertToRad = PI/((float)CordicBase*2);
	ConvertToCAU = MaxCAU/(2*PI);
	powr = 1;
	for(i = 0; i< NBITS ;i++)
	{
    ArcTan[i] = (int32_t)(atan(1.0/powr)/(PI/2)*CordicBase + 0.5);
		powr <<= 1;
  }
	
	f = 1.0;
	powr = 1;
	for(i = 0; i < NBITS; i++)
	{
    f = (f * (powr + 1)) / powr;
		powr <<= 2;
  }
	f = 1.0 / sqrt(f);
	xInit = (int32_t)(CordicBase * f + 0.5);
}

/*
**************************************************************
*                     CORDIC_SinCos
* Description : 
**************************************************************
*/
void CORDIC_SinCos(uint32_t theta, int32_t *sin, int32_t *cos)
{
	int32_t quadrant;    /* quadrant of incoming angle */
  int32_t z;          /* incoming angle move to 1st quadrant */
  int32_t i;          /* to index rotation */
	int32_t x,y;        /* projections onto axes */
	int32_t x1,y1;      /* peojections of rotated vector */
	
	if(theta < CordicBase)
	{
    quadrant = QUAD1;
		z = (int32_t)theta;
  } 
	else if(theta < Quad2Bondary)
	{
    quadrant = QUAD2;
		z = (int32_t)(Quad2Bondary - theta);
  }
	else if(theta < Quad3Bondary)
	{
    quadrant = QUAD3;
		z = (int32_t)(theta - Quad2Bondary);
  }
	else
	{
    quadrant = QUAD4;
		z = (int32_t)(Quad4Bondary - theta);
  }
	
	x = xInit;
	y = 0;
	
	/* Negate z, so same rotations taking angle z to  0 */
	z = -z;
	
	for(i = 0; i < NBITS; i++)
	{
    if(z < 0)
		{
      z += ArcTan[i];
			y1 = y + (x >> i);
			x1 = x - (y >> i);
    }
		else
		{
      z -= ArcTan[i];
			y1 = y - (x >> i);
			x1 = x + (y >> i);
    }
		
		x = x1;
		y = y1;
  }
	*cos = (quadrant == QUAD1 || quadrant == QUAD4)?x : -x;
	*sin = (quadrant == QUAD1 || quadrant == QUAD2)?y : -y;
}


float DHPF(float new_data,DHPF_Struct *p)
{
	p->output *= 0.999;
	(p->output) += (new_data - (p->old_data));
	(p->old_data) = new_data;
	return (p->output);
}

/*
****************************************************************
*                      DLPF()
* Description : Low pass filter cutoff frequency 'a' (rad/s)
****************************************************************
*/
float DLPF(float new_data,float old_data,float a,float dt)
{
	float   aT = a*dt;
	aT = Constraint_f(aT,0,0.9999);
  return ((1.0 - aT) * old_data + aT * new_data);
}



float InvSqrt(float x)
{
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}

float WarpToPI(float a)
{
	while(a>PI)      
		a -= 2*PI;
	while(a < -PI)   
		a += 2*PI;
	return a;
}
