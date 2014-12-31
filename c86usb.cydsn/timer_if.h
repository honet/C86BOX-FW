#ifndef TIMER_IF_H__
#define TIMER_IF_H__

#define NTIMERS 8

typedef struct {
	void (*Set)(uint16_t count);
	uint8_t (*Query)();
	void (*Wait)();
} TIMER_INTERFACE;

const TIMER_INTERFACE timerRes[NTIMERS];

#endif

