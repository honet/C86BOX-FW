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
#include "ChipWaitTimer0_api.h"


void ChipWaitTimer0_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer0_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer0_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer0_STATUS_REG & ChipWaitTimer0_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer0_FIFO_PTR, count);
}

uint8_t ChipWaitTimer0_Query()
{
	return (ChipWaitTimer0_STATUS_REG & ChipWaitTimer0_COUNTING) ? 1 : 0;
}

void ChipWaitTimer0_Wait()
{
	while((ChipWaitTimer0_STATUS_REG & ChipWaitTimer0_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
