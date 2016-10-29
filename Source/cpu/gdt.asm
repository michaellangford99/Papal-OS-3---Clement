global setGDT
global reloadSegments

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