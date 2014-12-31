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
#include "ChipWaitTimer4_api.h"


void ChipWaitTimer4_Start(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer4_Stop(void)
{
	// ì¡Ç…Ç∑ÇÈÇ±Ç∆ñ≥Çµ
}

void ChipWaitTimer4_Set(uint16_t count)
{
    if(count==0) return;
    
	while((ChipWaitTimer4_STATUS_REG & ChipWaitTimer4_QUEUE_FULL) != 0u)
	{
		/* Command queue is full */
	}
	
	// FIFOÇ…ÉfÅ[É^ãlÇﬂÇÈ
	CY_SET_REG16(ChipWaitTimer4_FIFO_PTR, count);
}

uint8_t ChipWaitTimer4_Query()
{
	return (ChipWaitTimer4_STATUS_REG & ChipWaitTimer4_COUNTING) ? 1 : 0;
}

void ChipWaitTimer4_Wait()
{
	while((ChipWaitTimer4_STATUS_REG & ChipWaitTimer4_COUNTING) != 0u){
	};
}


/* [] END OF FILE */
