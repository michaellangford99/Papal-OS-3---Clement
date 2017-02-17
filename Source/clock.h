/*
closely based off of 'BaseKernel' project of Douglas Thain (dthain, Github)
*/

#ifndef CLOCK_H
#define CLOCK_H

typedef struct {
	uint32_t seconds;
	uint32_t millis;
} clock_t;

#define CLICKS_PER_SECOND 60

#define TIMER0		0x40
#define TIMER_MODE	0x43
#define SQUARE_WAVE     0x36
#define TIMER_FREQ	1193182
#define TIMER_COUNT	(((uint32_t)TIMER_FREQ)/CLICKS_PER_SECOND/2)

#define CLOCK_DEBUG__DUMP 0

void    clock_init();
clock_t clock_read();
clock_t clock_diff( clock_t start, clock_t stop );
void    clock_wait( uint32_t millis );

#endif
