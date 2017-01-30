#ifndef IDT_H
#define IDT_H

#define idt_entries_length    (256)
#define idt_array_length      (idt_entries_length * 4)
#define isr_flags             0x8E00
#define isr_segment           0x0008
#define irq_flags             0x8E00
#define irq_segment           0x0008

//set to 1 to see output of 'idt_set_gate'
#define IDT_DEBUG 0

typedef uint16_t segment;
typedef uint32_t offset;
typedef uint16_t flags;

typedef uint16_t idt_entry;

struct x86_registers
{
  uint32_t gs, fs, es, ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
};

void idt_init();
void idt_zero();
void idt_set_gate(char interrupt, offset int_offset, flags int_flags, segment int_segment);
void isr_init();
void isr_handler(struct x86_registers *regs);
void irq_init();
void irq_set_handler(int irq, void (*handler)(struct x86_registers *regs));
void ir_clear_handler(int irq);
void irq_handler(uint32_t ebp, struct x86_registers *regs);
void irq_send_EOI_8259(uint8_t irq);
void irq_zero_handlers();

#endif
