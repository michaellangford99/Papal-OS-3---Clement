/*
Copyright (C) 2015 The University of Notre Dame
This software is distributed under the GNU General Public License.

used as basic irq functions
*/

#include "system.h"

static void interrupt_acknowledge( int i )
{
	if(i<32) {
		/* do nothing */
	} else {
		pic_acknowledge(i-32);
	}
}

void interrupt_init()
{
	int i;
	pic_init(32,40);
	for(i=32;i<48;i++) {
		interrupt_disable(i);
		interrupt_acknowledge(i);
	}
	interrupt_unblock();
}

void interrupt_enable( int i )
{
	if(i<32) {
		/* do nothing */
	} else {
		pic_enable(i-32);
	}
}

void interrupt_disable( int i )
{
	if(i<32) {
		/* do nothing */
	} else {
		pic_disable(i-32);
	}
}

void interrupt_block()
{
	asm("cli");
}

void interrupt_unblock()
{
	asm("sti");
}

void interrupt_wait()
{
	asm("sti");
	asm("hlt");
}
