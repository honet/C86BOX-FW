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

#include "`$INSTANCE_NAME`_ctrlapi.h"

void `$INSTANCE_NAME`_Start(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Stop(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Write(uint16_t data)
{
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_BUS_BUSY) != 0u)
	{
	}
	// ライト指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_WRITE;
	
	// FIFOにデータ詰める
	CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, data);
}


uint16_t `$INSTANCE_NAME`_Read()
{
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_BUS_BUSY) != 0u)
	{
	}
	// リード指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_READ;

	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_DATA_VALID) != `$INSTANCE_NAME`_DATA_VALID)
	{
		/* wait until input data are valid */
	}

	return ((((uint16) `$INSTANCE_NAME`_DIN_MSB_DATA_REG) << 8u) | `$INSTANCE_NAME`_DIN_LSB_DATA_REG);
}

void `$INSTANCE_NAME`_BusyWait()
{
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_BUS_BUSY) != 0u)
	{
	}
}

/* [] END OF FILE */
