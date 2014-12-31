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
#include "ChipWaitTimer1_api.h"


void ChipWaitTimer1_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer1_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer1_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer1_STATUS_REG & ChipWaitTimer1_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer1_FIFO_PTR, count);
}

uint8_t ChipWaitTimer1_Query()
{
	return (ChipWaitTimer1_STATUS_REG & ChipWaitTimer1_COUNTING) ? 1 : 0;
}

void ChipWaitTimer1_Wait()
{
	while((ChipWaitTimer1_STATUS_REG & ChipWaitTimer1_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
