/*
Copyright (C) 2016 Michael Langford
This software is distributed under the GNU General Public License.

are_interrupts_enabled(),
lidt(),
cpuid(),
rdtsc(), and
read_cr0() are basic io register functions based off of (or identical to) osdev "Assembly/Examples"

wait() is written by Michael Langford

inb(),
inw(),
inl(),
outb(),
outw(),
outl(),
iowait(),
inb_slow(),
inw_slow(),
inl_slow(),
outb_slow(),
outw_slow(),
outl_slow(), and
farpeekl() are based off of (or identical to) software in the 'base_kernel' project
		   by Douglas Thain (dthain, Github)

*/
#ifndef IO_H
#define IO_H

uint8_t inb( int port );
uint16_t inw( int port );
uint16_t inl( int port );
void outb( uint8_t value, int port );
void outw( uint16_t value, int port );
void outl( uint32_t value, int port );
void iowait();
uint8_t inb_slow( int port );
uint16_t inw_slow( int port );
uint32_t inl_slow( int port );
void outb_slow( uint8_t value, int port );
void outw_slow( uint16_t value, int port );
void outl_slow( uint32_t value, int port );
uint32_t farpeekl(uint16_t sel, void* off);
bool are_interrupts_enabled();
void lidt(void* base, uint16_t size);
void wait(int seconds);
/* GCC has a <cpuid.h> header you should use instead of this. */
void cpuid(int code, uint32_t* a, uint32_t* d);
uint64_t rdtsc();
unsigned long read_cr0(void);
extern void _breakpoint(void);
#endif