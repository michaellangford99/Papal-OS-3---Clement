global setGDT
global reloadSegments
global setTSS

gdtr dw 0
     dd 0

setGDT:
  mov  eax, [esp + 4]
  mov  [gdtr + 2], eax
  mov  ax, [esp + 8]
  mov  [gdtr], ax
  lgdt [gdtr]
  ret

reloadSegments:
  jmp  0x08:reload_CS

reload_CS:
  mov  ax, 0x10
  mov  ds, ax
  mov  es, ax
  mov  fs, ax
  mov  gs, ax
  mov  ss, ax
  ret

setTSS:
  mov ax, 0x2B;no idea why 0x2B
  ltr ax
  ret

;uint32_t eip, cs, eflags, useresp, ss;
extern save_ring_0_esp

global run_user_mode
run_user_mode:

  ;push 0xdeadbeef
  ;int 1

  mov eax, esp
  push eax
  call save_ring_0_esp

  ;switch segment data registers
  mov ax, 0x23
  mov  ds, ax
  mov  es, ax
  mov  fs, ax
  mov  gs, ax
  ;  ss, ax

  ;save esp
  mov eax, esp

  ;push ss
  push 0x23

  ;push esp
  push eax

  ;push flags
  ;cli
  pushf

  ;push cs
  push 0x1B

  ;push eip
  push user_mode_function2

  iret

global user_mode_function
user_mode_function:

  jmp user_mode_function

global user_mode_function2
user_mode_function2:
  int 0x1
  jmp user_mode_function2
