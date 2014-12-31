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
#include "ChipWaitTimer6_api.h"


void ChipWaitTimer6_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer6_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer6_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer6_STATUS_REG & ChipWaitTimer6_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer6_FIFO_PTR, count);
}

uint8_t ChipWaitTimer6_Query()
{
	return (ChipWaitTimer6_STATUS_REG & ChipWaitTimer6_COUNTING) ? 1 : 0;
}

void ChipWaitTimer6_Wait()
{
	while((ChipWaitTimer6_STATUS_REG & ChipWaitTimer6_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
