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
#include "ChipWaitTimer3_api.h"


void ChipWaitTimer3_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer3_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer3_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer3_STATUS_REG & ChipWaitTimer3_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer3_FIFO_PTR, count);
}

uint8_t ChipWaitTimer3_Query()
{
	return (ChipWaitTimer3_STATUS_REG & ChipWaitTimer3_COUNTING) ? 1 : 0;
}

void ChipWaitTimer3_Wait()
{
	while((ChipWaitTimer3_STATUS_REG & ChipWaitTimer3_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
