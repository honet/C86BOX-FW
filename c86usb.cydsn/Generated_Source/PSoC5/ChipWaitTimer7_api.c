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
#include "ChipWaitTimer7_api.h"


void ChipWaitTimer7_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer7_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer7_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer7_STATUS_REG & ChipWaitTimer7_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer7_FIFO_PTR, count);
}

uint8_t ChipWaitTimer7_Query()
{
	return (ChipWaitTimer7_STATUS_REG & ChipWaitTimer7_COUNTING) ? 1 : 0;
}

void ChipWaitTimer7_Wait()
{
	while((ChipWaitTimer7_STATUS_REG & ChipWaitTimer7_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
