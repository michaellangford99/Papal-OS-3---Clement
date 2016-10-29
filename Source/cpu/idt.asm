; idt.asm

;============================================idt_load=====================================================================
global idt_load
extern idtp
idt_load:
  lidt [idtp]
  ret


;=========================================== global isrs======================================================================
global  _isr0
global  _isr1
global  _isr2
global  _isr3
global  _isr4
global  _isr5
global  _isr6
global  _isr7
global  _isr8
global  _isr9
global  _isr10
global  _isr11
global  _isr12
global  _isr13
global  _isr14
global  _isr15
global  _isr16
global  _isr17
global  _isr18
global  _isr19
global  _isr20
global  _isr21
global  _isr22
global  _isr23
global  _isr24
global  _isr25
global  _isr26
global  _isr27
global  _isr28
global  _isr29
global  _isr30
global  _isr31

extern isr_handler

;===========================================isr handlers======================================================================

_isr0:
  
  push byte 0
  push byte 0
  
  jmp _isr_stub
  
_isr1:
  
  push byte 0
  push byte 1
  
  jmp _isr_stub
  
_isr2:
  
  push byte 0
  push byte 2
  
  jmp _isr_stub
  
_isr3:
  
  push byte 0
  push byte 3
  
  jmp _isr_stub
  
_isr4:
  
  push byte 0
  push byte 4
  
  jmp _isr_stub
  
_isr5:
  
  push byte 0
  push byte 5
  
  jmp _isr_stub
  
_isr6:
  
  push byte 0
  push byte 6
  
  jmp _isr_stub
  
_isr7:
  
  push byte 0
  push byte 7
  
  jmp _isr_stub
  
_isr8:
  
  
  push byte 8
  
  jmp _isr_stub
  
_isr9:
  
  push byte 0
  push byte 9
  
  jmp _isr_stub
  
_isr10:
  
  
  push byte 10
  
  jmp _isr_stub
  
_isr11:
  
  
  push byte 11
  
  jmp _isr_stub
  
_isr12:
  
  
  push byte 12
  
  jmp _isr_stub
  
_isr13:
  
  
  push byte 13
  
  jmp _isr_stub
  
_isr14:
  
  
  push byte 14
  
  jmp _isr_stub
  
_isr15:
  
  push byte 0
  push byte 15
  
  jmp _isr_stub
  
_isr16:
  
  push byte 0
  push byte 16
  
  jmp _isr_stub
  
_isr17:
  
  push byte 0
  push byte 17
  
  jmp _isr_stub
  
_isr18:
  
  push byte 0
  push byte 18
  
  jmp _isr_stub
  
_isr19:
  
  push byte 0
  push byte 19
  
  jmp _isr_stub
  
_isr20:
  
  push byte 0
  push byte 20
  
  jmp _isr_stub
  
_isr21:
  
  push byte 0
  push byte 21
  
  jmp _isr_stub
  
_isr22:
  
  push byte 0
  push byte 22
  
  jmp _isr_stub
  
_isr23:
  
  push byte 0
  push byte 23
  
  jmp _isr_stub
  
_isr24:
  
  push byte 0
  push byte 24
  
  jmp _isr_stub
  
_isr25:
  
  push byte 0
  push byte 25
  
  jmp _isr_stub
  
_isr26:
  
  push byte 0
  push byte 26
  
  jmp _isr_stub
  
_isr27:
  
  push byte 0
  push byte 27
  
  jmp _isr_stub
  
_isr28:
  
  push byte 0
  push byte 28
  
  jmp _isr_stub
  
_isr29:
  
  push byte 0
  push byte 29
  
  jmp _isr_stub
  
_isr30:
  
  push byte 0
  push byte 30
  
  jmp _isr_stub
  
_isr31:
  
  push byte 0
  push byte 31
  
  jmp _isr_stub

;===========================================_isr_stub======================================================================
_isr_stub:
  pusha
  push ds
  push es
  push fs
  push gs

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  mov eax, esp
  push eax
  
  mov eax, isr_handler
  call eax
  
  pop eax
    
  pop gs
  pop fs
  pop es
  pop ds
  popa
  
  add esp, 8

  
  iret


;===========================================global irqs======================================================================

global _irq0  
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15

extern irq_handler

;===========================================irq handlers======================================================================

_irq0:
  
  push byte 0
  push byte 32
  
  jmp _irq_stub
  
_irq1:
  
  push byte 0
  push byte 33
  
  jmp _irq_stub
  
_irq2:
  
  push byte 0
  push byte 34
  
  jmp _irq_stub
  
_irq3:
  
  push byte 0
  push byte 35
  
  jmp _irq_stub
  
_irq4:
  
  push byte 0
  push byte 36
  
  jmp _irq_stub
  
_irq5:
  
  push byte 0
  push byte 37
  
  jmp _irq_stub
  
_irq6:
  
  push byte 0
  push byte 38
  
  jmp _irq_stub
  
_irq7:
  
  push byte 0
  push byte 39
  
  jmp _irq_stub
  
_irq8:
  
  push byte 0
  push byte 40
  
  jmp _irq_stub
  
_irq9:
  
  push byte 0
  push byte 41
  
  jmp _irq_stub
  
_irq10:
  
  push byte 0
  push byte 42
  
  jmp _irq_stub
  
_irq11:
  
  push byte 0
  push byte 43
  
  jmp _irq_stub
  
_irq12:
  
  push byte 0
  push byte 44
  
  jmp _irq_stub
  
_irq13:
  
  push byte 0
  push byte 45
  
  jmp _irq_stub
  
_irq14:
  
  push byte 0
  push byte 46
  
  jmp _irq_stub
  
_irq15:
  
  push byte 0
  push byte 47
  
  jmp _irq_stub  

;===========================================_irq_stub======================================================================
_irq_stub:
  pusha
  push ds
  push es
  push fs
  push gs

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov eax, esp
  push eax
  
  mov eax, irq_handler
  call eax
  
  pop eax

  pop gs
  pop fs
  pop es
  pop ds
  popa
  add esp, 8
  
  iret
  