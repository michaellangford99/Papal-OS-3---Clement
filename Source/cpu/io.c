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

#include "../system.h"

uint8_t inb( int port ) {
	uint8_t result;
	asm("inb %w1, %b0" : "=a" (result) : "Nd" (port) );
	return result;
}

uint16_t inw( int port ) {
	uint16_t result;
	asm("inw %w1, %w0" : "=a" (result) : "Nd" (port) );
	return result;
}

uint16_t inl( int port ) {
	uint32_t result;
	asm("inl %w1, %0" : "=a" (result) : "Nd" (port) );
	return result;
}

void outb( uint8_t value, int port ) {
	asm("outb %b0, %w1" : : "a" (value), "Nd" (port) );
}

void outw( uint16_t value, int port ) {
	asm("outw %w0, %w1" : : "a" (value), "Nd" (port) );
}

void outl( uint32_t value, int port ) {
	asm("outl %0, %w1" : : "a" (value), "Nd" (port) );
}

void iowait() {
	outb(0,0x80);
}

uint8_t inb_slow( int port ) {
	uint8_t result = inb(port);
	iowait();
	return result;
}

uint16_t inw_slow( int port ) {
	uint16_t result = inw(port);
	iowait();
	return result;
}

uint32_t inl_slow( int port ) {
	uint32_t result = inl(port);
	iowait();
	return result;
}

void outb_slow( uint8_t value, int port ) {
	outb(value,port);
	iowait();
}

void outw_slow( uint16_t value, int port ) {
	outw(value,port);
	iowait();
}

void outl_slow( uint32_t value, int port ) {
	outl(value,port);
	iowait();
}
uint32_t farpeekl(uint16_t sel, void* off)
{
    uint32_t ret;
    asm ( "push %%fs\n\t"
          "mov  %1, %%fs\n\t"
          "mov  %%fs:(%2), %0\n\t"
          "pop  %%fs"
          : "=r"(ret) : "g"(sel), "r"(off) );
    return ret;
}

bool are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

void lidt(void* base, uint16_t size)
{   // This function works in 32 and 64bit mode
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
 
    asm ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

/* GCC has a <cpuid.h> header you should use instead of this. */
void cpuid(int code, uint32_t* a, uint32_t* d)
{
    asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

uint64_t rdtsc()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

unsigned long read_cr0(void)
{
    unsigned long val;
    asm volatile ( "mov %%cr0, %0" : "=r"(val) );
    return val;
}
