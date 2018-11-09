#include "GUI.h"
#include "math.h"
#include "Fonts.h"
#include "stdlib.h"
#include "algorithm.h"


/*
*************************************************************
*                 GUI_DrawLine()
*************************************************************
*/
void GUI_DrawLine(int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
	int16_t i;
	int16_t temp;
	float k;
	float inv_k;
	
	/* keep x1 < x2 */
	if(x1 > x2){/* swap */
    temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
  }
	
	/* if H line or V line */
	if((x2 != x1)&&(y1 != y2))
	{
	  k = (float)(y2-y1)/(float)(x2-x1); /* calculate slope */
		if(fabs(k)<1.0f)
		{
			for(i = x1; i <= x2; i++){
				GUI_DrawPoint(i,(int16_t)((i - x1)*k+0.5f)+y1);
			}
		}
		else
		{
			inv_k = 1.0f/k;

			/* pointing upward positive slope*/
			if(y2 > y1){
				for(i = y1; i <= y2; i++)
					GUI_DrawPoint((int16_t)((i - y1)*inv_k + 0.5f)+x1,i);
			}
			/* pointing downward negative slope */
			else{
				for(i = y1; i >= y2; i--)
					GUI_DrawPoint((int16_t)((i - y1)*inv_k - 0.5f) + x1,i);				
			}
		}
	}
	/* x1 = x2 vertical line */
	else if(x1 == x2)
	{
		if(y2>y1){
			for(i = y1; i <= y2; i++)
				GUI_DrawPoint(x1,i);
		}
		/* y1 >= y2 */
		else{
			for(i = y2; i <= y1; i++)
			  GUI_DrawPoint(x1,i);
		}
  }
	/* y1 = y2 horizontal line */
	else{
		for(i = x1; i <= x2; i++)
			GUI_DrawPoint(i,y1);
  }
}


/*
*************************************************************
*                   GUI_DrawGraph()
* Description : x,y stand for the start of the central line(x = 0)
*************************************************************
*/
void GUI_DrawGraph(int8_t *p_data,uint16_t len,uint16_t x,uint16_t y)
{
	uint16_t last_x = x;
	uint16_t last_y = Constraint_int(p_data[0] + y,0,127);
	uint16_t x1 = 0;
	uint16_t y1 = 0;
	
	GUI_DrawPoint(last_x,last_y);
	
  for(x1 = x+1u ; x1<= (x + len - 1u); x1++ ){
		y1 = Constraint_int(p_data[x1-x]+y,0,127);
	  GUI_DrawLine(last_x,last_y,x1,y1);
		last_x = x1;
		last_y = y1;
	}
}


/*
*************************************************************
*                   GUI_FillRect()
* Description : x1,y1 stand for the left bottom's coordinate
*               x2,y2 stand for the right top's coordinate
*************************************************************
*/
void GUI_FillRect(uint16_t x1, uint16_t y1,uint16_t x2,uint16_t y2)
{
  uint16_t i,j;
	for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
		   GUI_DrawPoint(i,j);
}

/*
*************************************************************
*                     GUI_CleanRect()
* Description : x1,y1 stand for the left bottom's coordinate
*               x2,y2 stand for the right top's coordinate
*************************************************************
*/
void GUI_CleanRect(uint16_t x1, uint16_t y1,uint16_t x2,uint16_t y2)
{
  uint16_t i,j;
	for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
		   GUI_CleanPoint(i,j);
}

/*
*************************************************************
*                      GUI_InvertRect()
* Description : x1,y1 stand for the left bottom's coordinate
*               x2,y2 stand for the right top's coordinate
*************************************************************
*/
void GUI_InvertRect(uint16_t x1, uint16_t y1,uint16_t x2,uint16_t y2)
{
  uint16_t i,j;
	for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
		   GUI_InvertPoint(i,j);
}


/*
*************************************************************
*                   GUI_PrintChar()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintChar(int8_t c,int16_t x, int16_t y)
{
  uint16_t index = 6*c - 188u;
	uint16_t i,j;
	uint8_t temp;
	
	for(j = x; j < (x+6); j++)
	{
		temp = SmallFont[index];
    for(i = y; i < (y+8); i++)
		{
      if(temp&0x80u)
				GUI_DrawPoint(j,i);
			temp<<=1u;
    }
		index++;
  }
}


/*
*************************************************************
*                     GUI_PrintString()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintString(int8_t *p_str,int16_t x,int16_t y)
{
	uint8_t i = 0;
  while(p_str[i]!='\0')
	{
    GUI_PrintChar(p_str[i],x,y);
		x+=6;
		i++;
  }
}


/*
*************************************************************
*                     GUI_PrintInteger()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintInteger(int32_t num,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%5d",num);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintFloat()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintFloat(float num,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%6.5f",num);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintStrInt6Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrInt6Str(int8_t *str1,int32_t num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[12] = {0};
  sprintf((char*)buf_str,"%s%6d%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintStrInt3Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrInt3Str(int8_t *str1,int32_t num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[10] = {0};
  sprintf((char*)buf_str,"%s%3d%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}


/*
*************************************************************
*                     GUI_PrintStrFloat4_3Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrFloat4_3Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%s%4.3f%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintStrFloat4_2Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrFloat4_2Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%s%4.2f%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintStrFloat4_1Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrFloat4_1Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%s%4.1f%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintStrFloat3_2Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrFloat3_2Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%s%3.2f%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}

/*
*************************************************************
*                     GUI_PrintStrFloat3_1Str()
* Description : x,y stand for the left bottom's coordinate
*************************************************************
*/
void GUI_PrintStrFloat3_1Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y)
{
  int8_t buf_str[16] = {0};
  sprintf((char*)buf_str,"%s%3.1f%s",str1,num,str2);
  GUI_PrintString(buf_str,x,y);
}


/*
*************************************************************
*                GUI_PrintfStrWithBoxInvert()
* Description : x,y stand for the left bottom's coordinate
                len : length of the box
*************************************************************
*/
void GUI_PrintfStrWithBoxInvert(int8_t *p_str,uint8_t len,int16_t x,int16_t y)
{
	GUI_CleanRect(x-2,y-1,x-2+len,y+9);
  GUI_PrintString(p_str,x,y);
	GUI_InvertRect(x-2,y-1,x-2+len,y+9);
}

/*
*************************************************************
*                GUI_PrintfStrInt6StrWithBoxInvert()
* Description : x,y stand for the left bottom's coordinate
                len : length of the box
*************************************************************
*/
void GUI_PrintfStrInt6StrWithBoxInvert(int8_t *str1,int32_t num,int8_t *str2,uint8_t len,int16_t x,int16_t y)
{
  int8_t buf_str[20] = {0};
  sprintf((char*)buf_str,"%s%6d%s",str1,num,str2);
  GUI_CleanRect(x-2,y-1,x-2+len,y+9);
  GUI_PrintString(buf_str,x,y);
	GUI_InvertRect(x-2,y-1,x-2+len,y+9);
}

/*
*************************************************************
*                GUI_PrintfStrFloat3_1StrWithBoxInvert()
* Description : x,y stand for the left bottom's coordinate
                len : length of the box
*************************************************************
*/
void GUI_PrintfStrFloat3_1StrWithBoxInvert(int8_t *str1,float num,int8_t *str2,uint8_t len,int16_t x,int16_t y)
{
  int8_t buf_str[20] = {0};
  sprintf((char*)buf_str,"%s%3.1f%s",str1,num,str2);
  GUI_CleanRect(x-2,y-1,x-2+len,y+9);
  GUI_PrintString(buf_str,x,y);
	GUI_InvertRect(x-2,y-1,x-2+len,y+9);
}


/*
*************************************************************
*                GUI_PrintfStrFloat3_2StrWithBoxInvert()
* Description : x,y stand for the left bottom's coordinate
                len : length of the box
*************************************************************
*/
void GUI_PrintfStrFloat3_2StrWithBoxInvert(int8_t *str1,float num,int8_t *str2,uint8_t len,int16_t x,int16_t y)
{
  int8_t buf_str[20] = {0};
  sprintf((char*)buf_str,"%s%3.2f%s",str1,num,str2);
  GUI_CleanRect(x-2,y-1,x-2+len,y+9);
  GUI_PrintString(buf_str,x,y);
	GUI_InvertRect(x-2,y-1,x-2+len,y+9);
}


