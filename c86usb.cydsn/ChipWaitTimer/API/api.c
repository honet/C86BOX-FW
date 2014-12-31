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
#include "`$INSTANCE_NAME`_api.h"


void `$INSTANCE_NAME`_Start(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Stop(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Set(uint16_t count)
{
    if(count==0) return;
    
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOにデータ詰める
	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_PTR, count);
}

uint8_t `$INSTANCE_NAME`_Query()
{
	return (`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_COUNTING) ? 1 : 0;
}

void `$INSTANCE_NAME`_Wait()
{
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
