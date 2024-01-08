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
#include <stdint.h>

#define WAIT1MS (8000) // 8000000==1sec (1tick = 8MHz = 125ns)

uint64_t GetTick();
void TickWait(uint64_t till);
void TickWaitDelta(uint32_t delta);
void TickSetup();

/* [] END OF FILE */
