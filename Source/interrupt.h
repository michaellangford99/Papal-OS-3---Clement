/*
Copyright (C) 2015 The University of Notre Dame
This software is distributed under the GNU General Public License.
*/

#ifndef INTERRUPT_H
#define INTERRUPT_H

void interrupt_init();
void interrupt_enable( int i );
void interrupt_disable( int i );
void interrupt_block();
void interrupt_unblock();
void interrupt_wait();

/*
PC Interrupts:
IRQ	Interrupt
0	32	System Timer
1	33	Keyboard
2	34	Cascade
3	35	Serial Port 2
4	36	Serial Port 1
5	37	Parallel Port 2 or Sound Card
6	38	Floppy
7	39	Parallel Port 1
8	40	Real Time Clock
9	41	Network (or ATA 3)
10	42	Network/Sound/SCSI
11	43	SCSI Other (or ATA 2)
12	44	PS/2 Mouse
13	45	FPU
14	46	ATA 0
15	47	ATA 1
*/


#endif
