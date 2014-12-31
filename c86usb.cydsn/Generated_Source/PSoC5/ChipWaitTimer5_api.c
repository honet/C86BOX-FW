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
#include "ChipWaitTimer5_api.h"


void ChipWaitTimer5_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer5_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer5_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer5_STATUS_REG & ChipWaitTimer5_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer5_FIFO_PTR, count);
}

uint8_t ChipWaitTimer5_Query()
{
	return (ChipWaitTimer5_STATUS_REG & ChipWaitTimer5_COUNTING) ? 1 : 0;
}

void ChipWaitTimer5_Wait()
{
	while((ChipWaitTimer5_STATUS_REG & ChipWaitTimer5_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
