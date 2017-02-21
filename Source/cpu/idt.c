#include <system.h>

/*
	Section 1 - declarations

	Section 2 - idt_init, idt_zero, idt_set_gate

	Section 3 - extern isrs, isr_handler, isr_init

	Section 4 - extern irqs, irq_handlers, irq_init,
							irq_set_handler, irq_clear_handler, irq_remap,
							irq_send_EOI_8259, irq_zero_handlers

*/

// Section 1 - declarations

idt_entry idt_entries[idt_array_length];

char * error_codes[32] = {
	      "division by zero",
        "debug exception",
        "nonmaskable interrupt",
        "breakpoint exception",
        "overflow exception",
        "out of bounds bounds",
        "invalid instruction",
        "coprocessor error",
        "double fault!",
        "copressor overrun",
        "invalid task exception",
        "segment not present",
        "stack exception exception",
        "general protection fault",
        "page fault exception",
        "unknown exception",
        "coprocessor fault",
				"alignment check exception",
				"machine check exception",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved",
				"reserved"
};

void *irq_handlers[16];

// END Section 1 - declarations
//Section 2 - idt_init, idt_zero, idt_set_gate

struct idt_ptr
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

//idt pointer structures
struct idt_ptr idtp;
extern void idt_load();

void idt_init()
{
	interrupt_block();
	idt_zero();

	isr_init();
	irq_init();

	idtp.limit = (64*256) -1;
	idtp.base = (uint32_t)&idt_entries;

	idt_load();

	interrupt_init(); // inits 8259, and does remapping

	printf("idt: ready\n");
}

void idt_zero()
{
	for (int i = 0; i < idt_array_length; i++)
		idt_entries[i] = 0;
}

void idt_set_gate(char interrupt, offset int_offset, flags int_flags, segment int_segment)
{
	// idt_entries[interrupt * 4] is start of entry
	// 1st 16 bits (uint16_t) is the first 16 bits of int_offset
	// 2nd 16 bits is the segment selector
	// 3rd 16 bits is the int_flags
	// 4th 16 bits are the last 16 bits of int_offset

	idt_entries[(interrupt * 4) + 0] = (uint16_t)(int_offset) & 0xFFFF;
	idt_entries[(interrupt * 4) + 1] = (uint16_t)(int_segment);
	idt_entries[(interrupt * 4) + 2] = (uint16_t)(int_flags);
	idt_entries[(interrupt * 4) + 3] = (uint16_t)(int_offset >> 16) & 0xFFFF;
	if (IDT_DEBUG == 1)
	{
		printf("idt gate set with int_no %d\n, at offset %x\n, with flags %x\n, and segment %x\n",
					interrupt,
					int_offset,
					int_flags,
					int_segment);
	}
}

// END Section 2 - idt_init, idt_zero, idt_set_gate

// Section 3 - extern isrs, isr_handler, isr_init

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

void isr_init()
{
	// format is isr_no, address/offset, flags, segment
	idt_set_gate(0, (uint32_t)_isr0, isr_flags, isr_segment);
	idt_set_gate(1, (uint32_t)_isr1, isr_flags, isr_segment);
	idt_set_gate(2, (uint32_t)_isr2, isr_flags, isr_segment);
	idt_set_gate(3, (uint32_t)_isr3, isr_flags, isr_segment);
	idt_set_gate(4, (uint32_t)_isr4, isr_flags, isr_segment);
	idt_set_gate(5, (uint32_t)_isr5, isr_flags, isr_segment);
	idt_set_gate(6, (uint32_t)_isr6, isr_flags, isr_segment);
	idt_set_gate(7, (uint32_t)_isr7, isr_flags, isr_segment);
	idt_set_gate(8, (uint32_t)_isr8, isr_flags, isr_segment);
	idt_set_gate(9, (uint32_t)_isr9, isr_flags, isr_segment);
	idt_set_gate(10, (uint32_t)_isr10, isr_flags, isr_segment);
	idt_set_gate(11, (uint32_t)_isr11, isr_flags, isr_segment);
	idt_set_gate(12, (uint32_t)_isr12, isr_flags, isr_segment);
	idt_set_gate(13, (uint32_t)_isr13, isr_flags, isr_segment);
	idt_set_gate(14, (uint32_t)_isr14, isr_flags, isr_segment);
	idt_set_gate(15, (uint32_t)_isr15, isr_flags, isr_segment);
	idt_set_gate(16, (uint32_t)_isr16, isr_flags, isr_segment);
	idt_set_gate(17, (uint32_t)_isr17, isr_flags, isr_segment);
	idt_set_gate(18, (uint32_t)_isr18, isr_flags, isr_segment);
	idt_set_gate(19, (uint32_t)_isr19, isr_flags, isr_segment);
	idt_set_gate(20, (uint32_t)_isr20, isr_flags, isr_segment);
	idt_set_gate(21, (uint32_t)_isr21, isr_flags, isr_segment);
	idt_set_gate(22, (uint32_t)_isr22, isr_flags, isr_segment);
	idt_set_gate(23, (uint32_t)_isr23, isr_flags, isr_segment);
	idt_set_gate(24, (uint32_t)_isr24, isr_flags, isr_segment);
	idt_set_gate(25, (uint32_t)_isr25, isr_flags, isr_segment);
	idt_set_gate(26, (uint32_t)_isr26, isr_flags, isr_segment);
	idt_set_gate(27, (uint32_t)_isr27, isr_flags, isr_segment);
	idt_set_gate(28, (uint32_t)_isr28, isr_flags, isr_segment);
	idt_set_gate(29, (uint32_t)_isr29, isr_flags, isr_segment);
	idt_set_gate(30, (uint32_t)_isr30, isr_flags, isr_segment);
	idt_set_gate(31, (uint32_t)_isr31, isr_flags, isr_segment);
}

void isr_handler(struct x86_registers *regs)
{
	//proc_save(regs);
	if (regs->int_no < 32)
	{
		printf("unhandled ");
		printf(error_codes[regs->int_no]);
		printf(" (%d), with code %d\nSystem execution halted indefinitely", regs->int_no, regs->err_code);
		
		
		printf("gs        0x%x                      \n", regs->gs);
		printf("fs        0x%x                      \n", regs->fs);
		printf("es        0x%x                      \n", regs->es);
		printf("ds        0x%x                      \n", regs->ds);

		printf("edi       0x%x                      \n", regs->edi);
		printf("esi       0x%x                      \n", regs->esi);
		printf("ebp       0x%x                      \n", regs->ebp);
		printf("esp       0x%x                      \n", regs->esp);
		printf("ebx       0x%x                      \n", regs->ebx);
		printf("edx       0x%x                      \n", regs->edx);
		printf("ecx       0x%x                      \n", regs->ecx);
		printf("eax       0x%x                      \n", regs->eax);

		printf("int_no    %d                        \n", regs->int_no);
		printf("err_code  %d                        \n", regs->err_code);

		printf("eip       0x%x                      \n", regs->eip);
		printf("cs        0x%x                      \n", regs->cs);
		printf("eflags    0x%x                      \n", regs->eflags);
		printf("useresp   0x%x                      \n", regs->useresp);
		printf("ss        0x%x                      \n", regs->ss);
		
		for (;;);
	}
}

// END Section 3 - extern isrs, isr_handlers, isr_init

// Section 4 - extern irqs, irq_handlers, irq_init, irq_set_handler,
//             irq_clear_handler, irq_remap, irq_send_EOI_8259, irq_zero_handlers

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

void irq_init()
{
	//irq_remap();
	irq_zero_handlers();

	idt_set_gate(32, (uint32_t)_irq0, irq_flags, irq_segment);
	idt_set_gate(33, (uint32_t)_irq1, irq_flags, irq_segment);
	idt_set_gate(34, (uint32_t)_irq2, irq_flags, irq_segment);
	idt_set_gate(35, (uint32_t)_irq3, irq_flags, irq_segment);
	idt_set_gate(36, (uint32_t)_irq4, irq_flags, irq_segment);
	idt_set_gate(37, (uint32_t)_irq5, irq_flags, irq_segment);
	idt_set_gate(38, (uint32_t)_irq6, irq_flags, irq_segment);
	idt_set_gate(39, (uint32_t)_irq7, irq_flags, irq_segment);
	idt_set_gate(40, (uint32_t)_irq8, irq_flags, irq_segment);
	idt_set_gate(41, (uint32_t)_irq9, irq_flags, irq_segment);
	idt_set_gate(42, (uint32_t)_irq10, irq_flags, irq_segment);
	idt_set_gate(43, (uint32_t)_irq11, irq_flags, irq_segment);
	idt_set_gate(44, (uint32_t)_irq12, irq_flags, irq_segment);
	idt_set_gate(45, (uint32_t)_irq13, irq_flags, irq_segment);
	idt_set_gate(46, (uint32_t)_irq14, irq_flags, irq_segment);
	idt_set_gate(47, (uint32_t)_irq15, irq_flags, irq_segment);
}

/*void irq_remap()
{
	outb(0x20, 0x11);
	//...
	//...
	outb(0xA1, 0x0);
}*/

void irq_set_handler(int irq, void (*handler)(struct x86_registers *regs))
{
	interrupt_enable(irq+32);
	irq_handlers[irq] = handler;
}

void ir_clear_handler(int irq)
{
	interrupt_disable(irq+32);
	irq_handlers[irq] = 0;
}

extern uint32_t stack_bottom;
extern uint32_t stack_top;

uint32_t irq_handler(struct x86_registers *regs)
{
	//interrupt_block();
	
	proc_save(regs);
	
	void (*handler)(struct x86_registers *regs);

	handler = irq_handlers[regs->int_no - 32];
	if (handler)
		handler(regs);

	irq_send_EOI_8259(regs->int_no);
	
	regs = proc_schedule(regs);
	/*
	printf("regs      0x%x                      \n", (uint32_t)regs);
	printf("gs        0x%x                      \n", regs->gs);
	printf("fs        0x%x                      \n", regs->fs);
	printf("es        0x%x                      \n", regs->es);
	printf("ds        0x%x                      \n", regs->ds);

	printf("edi       0x%x                      \n", regs->edi);
	printf("esi       0x%x                      \n", regs->esi);
	printf("ebp       0x%x                      \n", regs->ebp);
	printf("esp       0x%x                      \n", regs->esp);
	printf("ebx       0x%x                      \n", regs->ebx);
	printf("edx       0x%x                      \n", regs->edx);
	printf("ecx       0x%x                      \n", regs->ecx);
	printf("eax       0x%x                      \n", regs->eax);

	printf("int_no    %d                        \n", regs->int_no);
	printf("err_code  %d                        \n", regs->err_code);

	printf("eip       0x%x                      \n", regs->eip);
	printf("cs        0x%x                      \n", regs->cs);
	printf("eflags    0x%x                      \n", regs->eflags);
	printf("useresp   0x%x                      \n", regs->useresp);
	printf("ss        0x%x                      \n", regs->ss);
	printf("s_bottom  0x%x                      \n", &stack_bottom);
	printf("s_top     0x%x                      \n", &stack_top);
	printf("\n\n");
	*/
	//interrupt_unblock();
	return (uint32_t)regs;
}

void irq_send_EOI_8259(uint8_t irq)
{
	if (irq >= 40)
		outb(0xA0, 0x20);

	outb(0x20, 0x20);
}

void irq_zero_handlers()
{
	for (int i = 0; i < 16; i++)
	{
		irq_handlers[i] = 0;
	}
}

// END Section 4 - extern irqs, irq_handler, irq_init, irq_set_handler,
//                 irq_clear_handler, irq_remap, irq_send_EOI_8259, irq_zero_handlers
