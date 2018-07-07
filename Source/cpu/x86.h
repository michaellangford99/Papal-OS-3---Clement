#ifndef X86_H
#define X86_H

#define DPL_0 0
#define DPL_3 3

struct x86_registers
{
  uint32_t gs, fs, es, ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
};

struct x86_tss {
	uint16_t	prev;
	uint16_t	reserved;
	uint32_t	esp0;
	uint16_t	ss0;
	uint16_t	reserved0;
	uint32_t	esp1;
	uint16_t	ss1;
	uint16_t	reserved1;
	uint32_t	esp2;
	uint16_t	ss2;
	uint16_t	reserved2;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
	uint32_t	ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint16_t	es;
	uint16_t	reserved3;
	uint16_t	cs;
	uint16_t	reserved4;
	uint16_t	ss;
	uint16_t	reserved5;
	uint16_t	ds;
	uint16_t	reserved6;
	uint16_t	fs;
	uint16_t	reserved7;
	uint16_t	gs;
	uint16_t	reserved8;
	uint16_t	ldt;
	uint16_t	reserved9;
	uint16_t	t;
	uint16_t	iomap;
}  __attribute__((packed));

struct x86_eflags {
	unsigned carry:1;
	unsigned reserved0:1;
	unsigned parity:1;
	unsigned reserved1:1;

	unsigned auxcarry:1;
	unsigned reserved2:1;
	unsigned zero:1;
	unsigned sign:1;

	unsigned trap:1;
	unsigned interrupt:1;
	unsigned direction:1;
	unsigned overflow:1;

	unsigned iopl:2;
	unsigned nested:1;
	unsigned reserved3:1;

	unsigned resume:1;
	unsigned v86:1;
	unsigned align:1;
	unsigned vinterrupt:1;

	unsigned vpending:1;
	unsigned id:1;
};

#endif
