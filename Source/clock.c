//closely based off of 'BaseKernel' project of Douglas Thain (dthain, Github)
#include "system.h"

#define CLICKS_PER_SECOND 60

#define TIMER0		0x40
#define TIMER_MODE	0x43
#define SQUARE_WAVE     0x36
#define TIMER_FREQ	1193182
#define TIMER_COUNT	(((unsigned)TIMER_FREQ)/CLICKS_PER_SECOND/2)

static uint32_t clicks=0;
static uint32_t seconds=0;

static void clock_interrupt(struct x86_registers *regs)
{
	clicks++;
	graphics_update_fb(); //copy the framebuffer to video memory
	if(clicks>=CLICKS_PER_SECOND) {
		clicks=0;
		seconds++;
		console_heartbeat();
	}
	#ifdef DEBUG_CLOCK
		printf("gs        %d\n", regs->gs);
		printf("fs        %d\n", regs->fs);
		printf("es        %d\n", regs->es);
		printf("ds        %d\n", regs->ds);
		
		printf("edi       %d\n", regs->edi);
		printf("esi       %d\n", regs->esi);
		printf("ebp       %d\n", regs->ebp);
		printf("esp       %d\n", regs->esp);
		printf("ebx       %d\n", regs->ebx);
		printf("edx       %d\n", regs->edx);
		printf("ecx       %d\n", regs->ecx);
		printf("eax       %d\n", regs->eax);
		
		printf("int_no    %d\n", regs->int_no);
		printf("err_code  %d\n", regs->err_code);
		
		printf("eip       %d\n", regs->eip);
		printf("cs        %d\n", regs->cs);
		printf("eflags    %d\n", regs->eflags);
		printf("useresp   %d\n", regs->useresp);
		printf("ss        %d\n", regs->ss);
	#endif
}

clock_t clock_read()
{
	clock_t result;
	result.seconds = seconds;
	result.millis = 1000*clicks/CLICKS_PER_SECOND;
	return result;
}

clock_t clock_diff( clock_t start, clock_t stop )
{
	clock_t result;
	if(stop.millis<start.millis) {
		stop.millis+=1000;
		stop.seconds-=1;
	}
	result.seconds = stop.seconds-start.seconds;
	result.millis = stop.millis-start.millis;
	return result;
}

void clock_wait( uint32_t millis )
{
	clock_t start, elapsed;
	uint32_t total;
	
	start = clock_read();
	do {
		elapsed = clock_diff(start,clock_read());
		total = elapsed.millis + elapsed.seconds*1000;
		iowait();
	} while(total<millis);
}

void clock_init()
{
	outb(SQUARE_WAVE,TIMER_MODE);
	outb((TIMER_COUNT&0xff),TIMER0);
	outb((TIMER_COUNT>>8)&0xff,TIMER0);

	irq_set_handler(0,clock_interrupt);

	printf("clock: ticking\n");	
}