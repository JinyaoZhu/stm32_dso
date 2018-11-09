#ifndef __GUI_H__
#define __GUI_H__

#include "bsp.h"
#include "mini12864.h"

#define GUI_DrawPoint     mini12864_DrawPoint
#define GUI_CleanPoint    mini12864_CleanPoint
#define GUI_InvertPoint   mini12864_InvertPoint
#define GUI_Clear         mini12864_CacheClear



void GUI_DrawLine(int16_t x1,int16_t y1,int16_t x2,int16_t y2);
void GUI_DrawGraph(int8_t *p_data,uint16_t len,uint16_t x,uint16_t y);
void GUI_FillRect(uint16_t x1, uint16_t y1,uint16_t x2,uint16_t y2);
void GUI_CleanRect(uint16_t x1, uint16_t y1,uint16_t x2,uint16_t y2);
void GUI_InvertRect(uint16_t x1, uint16_t y1,uint16_t x2,uint16_t y2);
void GUI_PrintChar(int8_t c,int16_t x, int16_t y);
void GUI_PrintString(int8_t *p_str,int16_t x,int16_t y);
void GUI_PrintInteger(int32_t num,int16_t x,int16_t y);
void GUI_PrintFloat(float num,int16_t x,int16_t y);
void GUI_PrintStrInt6Str(int8_t *str1,int32_t num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintStrInt3Str(int8_t *str1,int32_t num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintStrFloat4_3Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintStrFloat4_2Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintStrFloat4_1Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintStrFloat3_1Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintStrFloat3_2Str(int8_t *str1,float num,int8_t *str2,int16_t x,int16_t y);
void GUI_PrintfStrWithBoxInvert(int8_t *p_str,uint8_t len,int16_t x,int16_t y);
void GUI_PrintfStrInt6StrWithBoxInvert(int8_t *str1,int32_t num,int8_t *str2,uint8_t len,int16_t x,int16_t y);
void GUI_PrintfStrFloat3_1StrWithBoxInvert(int8_t *str1,float num,int8_t *str2,uint8_t len,int16_t x,int16_t y);
void GUI_PrintfStrFloat3_2StrWithBoxInvert(int8_t *str1,float num,int8_t *str2,uint8_t len,int16_t x,int16_t y);

#endif /* __GUI_H__ */
