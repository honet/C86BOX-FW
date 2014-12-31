/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "CBUSControl_ctrlapi.h"

void CBUSControl_Start(void)
{
	// 特にすること無し
}

void CBUSControl_Stop(void)
{
	// 特にすること無し
}

void CBUSControl_Write(uint16_t data)
{
	while((CBUSControl_STATUS_REG & CBUSControl_CMD_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	// ライト指示送る
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_WRITE;
	
	// FIFOにデータ詰める
	CY_SET_REG16(CBUSControl_DATA_FIFO_PTR, data);
}


uint16_t CBUSControl_Read()
{
	// コマンドキュー空き待ち
	while((CBUSControl_STATUS_REG & CBUSControl_CMD_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	// リード指示送る
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_READ;

	while((CBUSControl_STATUS_REG & CBUSControl_DATA_VALID) != CBUSControl_DATA_VALID)
	{
		/* wait until input data are valid */
	}

	return ((((uint16) CBUSControl_DIN_MSB_DATA_REG) << 8u) | CBUSControl_DIN_LSB_DATA_REG);
}


/* [] END OF FILE */
