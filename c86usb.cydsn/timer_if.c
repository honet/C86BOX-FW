#include <project.h>
#include "timer_if.h"

const TIMER_INTERFACE timerRes[NTIMERS] = {
	{
		ChipWaitTimer0_Set,
		ChipWaitTimer0_Query,
		ChipWaitTimer0_Wait
	},{
		ChipWaitTimer1_Set,
		ChipWaitTimer1_Query,
		ChipWaitTimer1_Wait
	},{
		ChipWaitTimer2_Set,
		ChipWaitTimer2_Query,
		ChipWaitTimer2_Wait
	},{
		ChipWaitTimer3_Set,
		ChipWaitTimer3_Query,
		ChipWaitTimer3_Wait
	},{
		ChipWaitTimer4_Set,
		ChipWaitTimer4_Query,
		ChipWaitTimer4_Wait
	},{
		ChipWaitTimer5_Set,
		ChipWaitTimer5_Query,
		ChipWaitTimer5_Wait
	},{
		ChipWaitTimer6_Set,
		ChipWaitTimer6_Query,
		ChipWaitTimer6_Wait
	},{
		ChipWaitTimer7_Set,
		ChipWaitTimer7_Query,
		ChipWaitTimer7_Wait
	}
};

