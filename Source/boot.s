# This .s file is based off of the "Bare Bones" demo on osdev.org

# Declare constants used for creating a multiboot header.

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set VIDINFO,  1<<2
.set FLAGS,    ALIGN | MEMINFO | VIDINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

#the multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long 0 #set graphics mode
.long 1024, 768, 32  #width, height, depth. This is the preferred mode for the OS, 
                     #however, other modes chosen by GRUB will be detected and used

.section .bootstrap_stack, "aw", @nobits
.global stack_bottom
stack_bottom:
.skip 32768 # 32 KiB
.global stack_top
stack_top:

# The linker script specifies _start as the entry point to the kernel and the
# bootloader will jump to this position once the kernel has been loaded. It
# doesn't make sense to return from this function as the bootloader is gone.
.section .text
.global _start
.type _start, @function
_start:

	# To set up a stack, we simply set the esp register to point to the top of
	# our stack (as it grows downwards).
	movl $stack_top, %esp
  /* Make sure our stack is 16-byte aligned */
  and $-16, %esp
	movl $stack_bottom, %ebp

	pushl %eax
	pushl %ebx # Pointer to multiboot header

  cli
	# We are now ready to actually execute C code.
	call kernel_main

	# In case the function returns, we'll want to put the computer into an
	# infinite loop. To do that, we use the clear interrupt ('cli') instruction
	# to disable interrupts, the halt instruction ('hlt') to stop the CPU until
	# the next interrupt arrives, and jumping to the halt instruction if it ever
	# continues execution, just to be safe. We will create a local label rather
	# than real symbol and jump to there endlessly.
	cli
	hlt
.Lhang:
	jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
