#include "bsp.h"
#include "cpal_i2c.h"

CPAL_TransferTypeDef I2C1_TxStruct = {pNULL,0,0,0};
CPAL_TransferTypeDef I2C1_RxStruct = {pNULL,0,0,0};
extern CPAL_InitTypeDef I2C1_DevStructure; 



BSP_OS_MUTEX  Mutex_I2C1_Lock;
BSP_OS_SEM    Sem_I2C1_TxWait;
BSP_OS_SEM    Sem_I2C1_RxWait;

/************************************
 *@brief : BSP_I2C_Soft_Init
 *@param : none
 *************************************/
CPU_BOOLEAN BSP_I2C_Init(void)
{	   
  BSP_Ser_Printf("\n\rI2C1 Init...\r\n");
  /*Create OS Semaphore */
  BSP_OS_MutexCreate((BSP_OS_MUTEX*)&Mutex_I2C1_Lock,(CPU_CHAR*) "I2C1 Lock"); 
  BSP_OS_SemCreate((BSP_OS_SEM*)&Sem_I2C1_TxWait,(BSP_OS_SEM_VAL) 0,(CPU_CHAR*) "I2C1 TxWait"); 
	BSP_OS_SemCreate((BSP_OS_SEM*)&Sem_I2C1_RxWait,(BSP_OS_SEM_VAL) 0,(CPU_CHAR*) "I2C1 RxWait"); 
	
	BSP_IntVectSet(BSP_INT_ID_I2C1_EV,  BSP_I2C1_EV_ISR);
	BSP_IntVectSet(BSP_INT_ID_I2C1_ER,  BSP_I2C1_ER_ISR);
	BSP_IntVectSet(BSP_INT_ID_DMA1_CH6, BSP_DMA1_Channel6_ISR);
	BSP_IntVectSet(BSP_INT_ID_DMA1_CH7, BSP_DMA1_Channel7_ISR);
	
	CPAL_I2C_StructInit(&I2C1_DevStructure);
	I2C1_DevStructure.pCPAL_I2C_Struct->I2C_ClockSpeed = I2C1_SPEED;
	I2C1_DevStructure.CPAL_Direction     = CPAL_DIRECTION_TXRX;               /* Transmitter and Receiver direction selected */
  I2C1_DevStructure.CPAL_Mode          = CPAL_MODE_MASTER;                  /* Mode Master selected */
  I2C1_DevStructure.CPAL_ProgModel     = CPAL_PROGMODEL_DMA;                /* DMA Programming Model selected */
  I2C1_DevStructure.pCPAL_TransferTx   = &I2C1_TxStruct;                    /* Point pCPAL_TransferTx to a Null pointer */
  I2C1_DevStructure.pCPAL_TransferRx   = &I2C1_RxStruct;                    /* Point pCPAL_TransferRx to a Null pointer */ 
  I2C1_DevStructure.CPAL_State         = CPAL_STATE_READY;                  /* Device Disabled */
  I2C1_DevStructure.wCPAL_DevError     = CPAL_I2C_ERR_NONE;                 /* No Device Error */
  I2C1_DevStructure.wCPAL_Options      = 0;             
  I2C1_DevStructure.wCPAL_Timeout      = I2C1_TIMEOUT;                      /* Set timeout value to CPAL_I2C_TIMEOUT_DEFAULT */
	CPAL_I2C_Init(&I2C1_DevStructure);
	if(CPAL_I2C_IsDeviceReady(&I2C1_DevStructure) == CPAL_PASS)
    return DEF_OK;
	else
		return DEF_FAIL;
}  


/**********************************************
 *@brief : 写入单个字节
 *@param :   -RegAddr     寄存器地址
 * 					 -DataToWrite 要写入的数据
 * 					 -SlaveAddr   设备地址
 **********************************************/
CPU_BOOLEAN  BSP_I2C_SingleWrite(CPU_INT08U RegAddr,CPU_INT08U DataToWrite,CPU_INT08U SlaveAddr)
{
	
	  BSP_OS_MutexWait(&Mutex_I2C1_Lock,0);
		I2C1_TxStruct.pbBuffer = &DataToWrite;
		I2C1_TxStruct.wNumData = 1;
		I2C1_TxStruct.wAddr1   = SlaveAddr;
		I2C1_TxStruct.wAddr2   = RegAddr;
	  I2C1_DevStructure.wCPAL_Options = 0;  
	
		if(CPAL_I2C_Write(&I2C1_DevStructure) == CPAL_PASS)
		{
			if(BSP_OS_SemWait(&Sem_I2C1_TxWait,I2C1_TIMEOUT) == DEF_OK)
			{
				BSP_OS_MutexPost(&Mutex_I2C1_Lock);
				return DEF_OK;
			}
			else
			{
				BSP_OS_MutexPost(&Mutex_I2C1_Lock);
				return DEF_FAIL;
			}
		}
		else
		{
      BSP_OS_MutexPost(&Mutex_I2C1_Lock);
			return DEF_FAIL;
    }
}

/**********************************************
 *@brief : Write single byte without register address
 *@param :  
 * 					 -DataToWrite 要写入的数据
 * 					 -SlaveAddr   设备地址
 **********************************************/
CPU_BOOLEAN  BSP_I2C_SingleWriteNoMemAddr(CPU_INT08U DataToWrite,CPU_INT08U SlaveAddr)
{
	
	  BSP_OS_MutexWait(&Mutex_I2C1_Lock,0);
		I2C1_TxStruct.pbBuffer = &DataToWrite;
		I2C1_TxStruct.wNumData = 1;
		I2C1_TxStruct.wAddr1   = SlaveAddr;
	  I2C1_DevStructure.wCPAL_Options = CPAL_OPT_NO_MEM_ADDR;  
	
		if(CPAL_I2C_Write(&I2C1_DevStructure) == CPAL_PASS)
		{
			if(BSP_OS_SemWait(&Sem_I2C1_TxWait,I2C1_TIMEOUT) == DEF_OK)
			{
				BSP_OS_MutexPost(&Mutex_I2C1_Lock);
				return DEF_OK;
			}
			else
			{
				BSP_OS_MutexPost(&Mutex_I2C1_Lock);
				return DEF_FAIL;
			}
		}
		else
		{
      BSP_OS_MutexPost(&Mutex_I2C1_Lock);
			return DEF_FAIL;
    }
}

/**********************************************
 *@brief : 读取一个字节
 *@param : -RegAddr 寄存器地址
 * 				 -SlaveAddr 设备地址
 ***********************************************/
CPU_INT08U  BSP_I2C_SingleRead(CPU_INT08U RegAddr,CPU_INT08U SlaveAddr)
{
	uint8_t temp;
	BSP_OS_MutexWait(&Mutex_I2C1_Lock,0);
	I2C1_RxStruct.pbBuffer = &temp;
	I2C1_RxStruct.wNumData = 1;
	I2C1_RxStruct.wAddr1   = SlaveAddr;
	I2C1_RxStruct.wAddr2   = RegAddr;
	I2C1_DevStructure.wCPAL_Options = 0;  
	
	if(CPAL_I2C_Read(&I2C1_DevStructure)==CPAL_PASS)
	{
		if(BSP_OS_SemWait(&Sem_I2C1_RxWait,I2C1_TIMEOUT) == DEF_OK)
		{
			BSP_OS_MutexPost(&Mutex_I2C1_Lock);
			return temp;
		}
		else
		{
			BSP_OS_MutexPost(&Mutex_I2C1_Lock);
			return DEF_FAIL;
		}
	}
	else
	{
		BSP_OS_MutexPost(&Mutex_I2C1_Lock);
		return DEF_FAIL;
	}

}

/**********************************************
 *@brief : Read two byte(No register address)
 *@param : 
 * 				 -SlaveAddr 设备地址
 ***********************************************/
CPU_INT16U  BSP_I2C_SingleRead16NoMemAddr(CPU_INT08U SlaveAddr)
{
	  uint8_t temp[2];
	  BSP_OS_MutexWait(&Mutex_I2C1_Lock,0);
		I2C1_RxStruct.pbBuffer = &temp[0];
		I2C1_RxStruct.wNumData = 2;
		I2C1_RxStruct.wAddr1   = SlaveAddr;
	  I2C1_DevStructure.wCPAL_Options = CPAL_OPT_NO_MEM_ADDR; 
	
		if(CPAL_I2C_Read(&I2C1_DevStructure)==CPAL_PASS)
		{
			if(BSP_OS_SemWait(&Sem_I2C1_RxWait,I2C1_TIMEOUT) == DEF_OK)
			{
				BSP_OS_MutexPost(&Mutex_I2C1_Lock);
				return (temp[0]<<8|temp[1]);
			}
			else
			{
        BSP_OS_MutexPost(&Mutex_I2C1_Lock);
		  	return DEF_FAIL;
      }
		}
		else
		{
      BSP_OS_MutexPost(&Mutex_I2C1_Lock);
			return DEF_FAIL;
    }

}

/**********************************************
 *@brief : Read three byte(No register address)
 *@param : 
 * 				 -SlaveAddr 设备地址
 ***********************************************/
CPU_INT32U  BSP_I2C_SingleRead24NoMemAddr(CPU_INT08U SlaveAddr)
{
	  uint8_t temp[3];
	  BSP_OS_MutexWait(&Mutex_I2C1_Lock,0);
		I2C1_RxStruct.pbBuffer = &temp[0];
		I2C1_RxStruct.wNumData = 3;
		I2C1_RxStruct.wAddr1   = SlaveAddr;
	  I2C1_DevStructure.wCPAL_Options = CPAL_OPT_NO_MEM_ADDR;  
	
		if(CPAL_I2C_Read(&I2C1_DevStructure)==CPAL_PASS)
		{
			if(BSP_OS_SemWait(&Sem_I2C1_RxWait,I2C1_TIMEOUT) == DEF_OK)
			{
				BSP_OS_MutexPost(&Mutex_I2C1_Lock);
				return ((temp[0]<<16|temp[1]<<8)|temp[0]);
			}
			else
			{
        BSP_OS_MutexPost(&Mutex_I2C1_Lock);
		  	return DEF_FAIL;
      }
		}
		else
		{
      BSP_OS_MutexPost(&Mutex_I2C1_Lock);
			return DEF_FAIL;
    }

}
/*
**************************************************
 *              BSP_I2C_MultRead()
 * Description : Read n data from i2c bus
 * Argument    : *buf_ptr
 * Note        : None
 **************************************************
 */
CPU_BOOLEAN  BSP_I2C_MultRead(CPU_INT08U RegAddr,CPU_INT08U SlaveAddr,CPU_INT08U *buf_ptr,CPU_INT16U nbr_of_byte)
{
	BSP_OS_MutexWait(&Mutex_I2C1_Lock,0);
	I2C1_RxStruct.pbBuffer = buf_ptr;
	I2C1_RxStruct.wNumData = nbr_of_byte;
	I2C1_RxStruct.wAddr1   = SlaveAddr;
	I2C1_RxStruct.wAddr2   = RegAddr;
	I2C1_DevStructure.wCPAL_Options = 0;  
	
	if(CPAL_I2C_Read(&I2C1_DevStructure) == CPAL_PASS)
	{
		if(BSP_OS_SemWait(&Sem_I2C1_RxWait,I2C1_TIMEOUT) == DEF_OK)
		{
		  BSP_OS_MutexPost(&Mutex_I2C1_Lock);
		  return DEF_OK;
		}
		else/* Timeout */
		{
      BSP_OS_MutexPost(&Mutex_I2C1_Lock);
		  return DEF_FAIL;
    }
	}
	else
	{
		BSP_OS_MutexPost(&Mutex_I2C1_Lock);
		return DEF_FAIL;
	}

}

/*
*********************************************************************
*                            I2C1 ISR
*********************************************************************
*/
void BSP_I2C1_EV_ISR(void)
{
  I2C1_EV_IRQHandler();
}

void BSP_I2C1_ER_ISR(void)
{
  I2C1_ER_IRQHandler();
}

void BSP_DMA1_Channel6_ISR(void)
{
  DMA1_Channel6_IRQHandler();
}

void BSP_DMA1_Channel7_ISR(void)
{
  DMA1_Channel7_IRQHandler();
}
