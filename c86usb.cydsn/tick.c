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
#include <project.h>
#include "tick.h"

volatile uint32_t upperTick = 0;

CY_ISR(TickCarryISR)
{
	// clear sticky bits.
    TickCarryINT_GetState();
    upperTick++;
}

uint64_t GetTick(){
    uint32_t t = TickCounter_ReadCounter();
    return ((uint64_t)upperTick<<32) | t;
}

void TickWait(uint64_t till){
    while(GetTick()<till);
}

void TickWaitDelta(uint32_t delta){
    uint64_t t = GetTick();
    TickWait(t+delta);
}

void TickSetup(){
    upperTick = 0;
    TickCarryINT_ClearPending();
	TickCarryINT_StartEx(TickCarryISR);
    TickCounter_WriteCounter(0);
    TickCounter_Start();
}

/* [] END OF FILE */
