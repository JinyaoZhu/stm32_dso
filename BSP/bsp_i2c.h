#ifndef __IIC_H
#define	__IIC_H


/*----------Public-----------------*/
CPU_BOOLEAN 	 BSP_I2C_Init(void);
CPU_BOOLEAN    BSP_I2C_SingleWrite(CPU_INT08U RegAddr,CPU_INT08U DataToWrite,CPU_INT08U SlaveAddr);
CPU_BOOLEAN    BSP_I2C_SingleWriteNoMemAddr(CPU_INT08U DataToWrite,CPU_INT08U SlaveAddr);
CPU_INT08U     BSP_I2C_SingleRead(CPU_INT08U RegAddr,CPU_INT08U SlaveAddr);
CPU_INT16U     BSP_I2C_SingleRead16NoMemAddr(CPU_INT08U SlaveAddr);
CPU_INT32U     BSP_I2C_SingleRead24NoMemAddr(CPU_INT08U SlaveAddr);
CPU_BOOLEAN    BSP_I2C_MultRead(CPU_INT08U RegAddr,CPU_INT08U SlaveAddr,CPU_INT08U *buf_ptr,CPU_INT16U nbr_of_byte);

/****************I2C ISR*******************/
void BSP_I2C1_EV_ISR(void);
void BSP_I2C1_ER_ISR(void);
void BSP_DMA1_Channel6_ISR(void);
void BSP_DMA1_Channel7_ISR(void);

#endif /* __IIC_H */
