#include "bsp.h"
#include "mini12864.h"

extern BSP_OS_MUTEX  Mutex_SPI2Lock;

uint8_t mini12864_DisplayCache[128][8]={0};


/*
*************************************************************
*                 mini12864_WriteCmd()
*************************************************************
*/
void mini12864_WriteCmd(uint8_t byte)
{
  MINI12864_CD_L;
	BSP_OS_MutexWait(&Mutex_SPI2Lock,0);
  MINI12864_CS_L;
  BSP_SPI2_SendReceiveByte(byte);
  MINI12864_CS_H;
	BSP_OS_MutexPost(&Mutex_SPI2Lock);
}


/*
*************************************************************
*                 mini12864_WriteData()
*************************************************************
*/
void mini12864_WriteData(uint8_t byte)
{
  MINI12864_CD_H;
	BSP_OS_MutexWait(&Mutex_SPI2Lock,0);
  MINI12864_CS_L;
  BSP_SPI2_SendReceiveByte(byte);
  MINI12864_CS_H;
	BSP_OS_MutexPost(&Mutex_SPI2Lock);
}

/*
*************************************************************
*                 mini12864_Init()
*************************************************************
*/
void mini12864_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	BSP_Ser_Printf("LCM Init...\r\n");

  BSP_PeriphEn(BSP_PERIPH_ID_IOPB);

  /* Configure GPIO */
  GPIO_InitStructure.GPIO_Pin = MINI12864_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MINI12864_CS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MINI12864_RES_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MINI12864_RES_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = MINI12864_CD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MINI12864_CD_PORT, &GPIO_InitStructure);

  MINI12864_CS_L;
  MINI12864_RES_L;
  MINI12864_DelayMS(10);
  MINI12864_RES_H;
  MINI12864_DelayMS(10);
  mini12864_WriteCmd(0xe2);//system reset
  MINI12864_DelayMS(120);

  mini12864_WriteCmd(0x24);/* set Vlcd resistor ratio */
  mini12864_WriteCmd(0xa2);/* set bias ratio BR=1/9 */
  mini12864_WriteCmd(0xa0);/* set seg direction */
  mini12864_WriteCmd(0xc8);/* set com direction */
  mini12864_WriteCmd(0x2f);/* set power control */
  mini12864_WriteCmd(0x40);/* set scroll line */
  mini12864_WriteCmd(0x81);/* set electronic volume */
  mini12864_WriteCmd(26);  /* adjust contrast of LCD panel display (0~63 default 32) */
  mini12864_WriteCmd(0xa6);/* set inverse display	   a6 off, a7 on */
  mini12864_WriteCmd(0xa4);/* set all pixel on */
  mini12864_WriteCmd(0xaf);/* set display enable */
  mini12864_CacheClear();
	mini12864_Refresh();
}

/*
*************************************************************
*               mini12864_CacheClear()
*************************************************************
*/
void mini12864_CacheClear()
{
  uint8_t x, y;
  for (y = 0; y < 8; y++){
    for (x = 0; x < 128; x++){
      mini12864_DisplayCache[x][y] = 0x00;
		}
  }
}


/*
*************************************************************
*                 mini12864_CacheFill()
*************************************************************
*/
void mini12864_CacheFill(uint8_t byte)
{
  uint8_t x, y;
  for (y = 0; y < 8; y++){
    for (x = 0; x < 128; x++){
      mini12864_DisplayCache[x][y] = byte;
		}
  }
}


/*
*************************************************************
*                 mini12864_Refresh()
*************************************************************
*/
void mini12864_Refresh(void)
{
	uint8_t x,y;

  for (y = 0; y < 8; y++)
  {
		mini12864_WriteCmd(0xb0 + y); /* Set page address */
		mini12864_WriteCmd(0x10);     /* Set column address [3..0] */
    mini12864_WriteCmd(0x00);     /* Set column address [7..4] */
    for (x = 0; x < 128; x++){
      mini12864_WriteData(mini12864_DisplayCache[x][y]);
			//MINI12864_DelayMS(50);
		}
  }
}



/*
*************************************************************
*                 mini12864_DrawPoint()
*************************************************************
*/
void mini12864_DrawPoint(uint16_t x,uint16_t y)
{
	uint8_t temp = mini12864_DisplayCache[x][(63u-y)>>3u]; /* page = (63-y)/8, column = x */
	temp |= (0x80u)>>(y%8u);
	mini12864_DisplayCache[x][(63u-y)>>3u] = temp;
}

/*
*************************************************************
*                 mini12864_CleanPoint()
*************************************************************
*/
void mini12864_CleanPoint(uint16_t x,uint16_t y)
{
	uint8_t temp = mini12864_DisplayCache[x][(63u-y)>>3u]; /* page = (63-y)/8, column = x */
	temp &= (0x7fu)>>(y%8u);
	mini12864_DisplayCache[x][(63u-y)>>3u] = temp;
}


/*
*************************************************************
*                 mini12864_InvertPoint()
*************************************************************
*/
void mini12864_InvertPoint(uint16_t x,uint16_t y)
{
	uint8_t temp = mini12864_DisplayCache[x][(63u-y)>>3u]; /* page = (63-y)/8, column = x */
	temp ^= (0x80u)>>(y%8u);
	mini12864_DisplayCache[x][(63u-y)>>3u] = temp;
}
