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
#include "ChipWaitTimer2_api.h"


void ChipWaitTimer2_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer2_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer2_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer2_STATUS_REG & ChipWaitTimer2_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer2_FIFO_PTR, count);
}

uint8_t ChipWaitTimer2_Query()
{
	return (ChipWaitTimer2_STATUS_REG & ChipWaitTimer2_COUNTING) ? 1 : 0;
}

void ChipWaitTimer2_Wait()
{
	while((ChipWaitTimer2_STATUS_REG & ChipWaitTimer2_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
