#include "bsp.h"



BSP_OS_MUTEX  Mutex_SPI2Lock;

/*
*******************************************************
*                  BSP_SPI2_Init()
*******************************************************
*/
void BSP_SPI2_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
 /* ------------------ INIT OS OBJECTS ----------------- */
  BSP_OS_MutexCreate(&Mutex_SPI2Lock, "SPI2 Lock");     

  /* Enable SPI and GPIO clocks */
  BSP_PeriphEn(BSP_PERIPH_ID_SPI2);
	BSP_PeriphEn(BSP_PERIPH_ID_IOPB);	

  /* Configure SPI pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN | SPI2_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI2_PORT, &GPIO_InitStructure);

  /* Configure MISO */
  GPIO_InitStructure.GPIO_Pin = SPI2_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPI2_PORT, &GPIO_InitStructure);
	
  /* SPI configuration */
	SPI_I2S_DeInit(SPI2);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
	/* Disable SPI2 CRC calculation */
  SPI_CalculateCRC(SPI2, DISABLE);

  /* Enable the SPI  */
  SPI_Cmd(SPI2, ENABLE);
}



/*
*******************************************************
*                  BSP_SPI1_SendReceiveByte()
*******************************************************
*/
CPU_INT08U BSP_SPI2_SendReceiveByte(CPU_INT08U data)
{
	uint32_t time_out = 0;
	/* Tx */
  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET)
	{
    time_out++;
		if(time_out>SPI2_TIMEOUT_LIMIT){
			SPI2_TxTimeoutCallback();
			break;
		}
  }
	time_out = 0;
  SPI_I2S_SendData(SPI2,data);

	/* Rx */
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET)
	{
    time_out++;
		if(time_out>SPI2_TIMEOUT_LIMIT){
			SPI2_RxTimeoutCallback();
			break;
		}
  }
  return SPI_I2S_ReceiveData(SPI2);
}

/*
*********************************************************************************************************
*                                            SPI Timeout Callback
*********************************************************************************************************
*/
void SPI2_TxTimeoutCallback(void)
{
  BSP_Ser_Printf("SPI2_TxTimeoutCallback\r\n");
}

void SPI2_RxTimeoutCallback(void)
{
  BSP_Ser_Printf("SPI2_RxTimeoutCallback\r\n");
}


