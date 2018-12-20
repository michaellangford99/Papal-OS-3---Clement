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

global user_mode_function
user_mode_function:

  jmp user_mode_function

global user_mode_function2
user_mode_function2:
  push 0xdeadbeef
  int 0x80
  pop eax
  jmp user_mode_function2
