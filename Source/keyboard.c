/*
ascii.h,
struct keymap,
keyboard.us.c,
static struct keymap keymap[] {}, and
keyboard_map() are based off of or identical to software in the 'base_kernel' project
               by Douglas Thain (dthain, Github)

getScancode(), and
GetKey() are written by Michael Langford
*/

#include "system.h"

struct keymap {
	char	normal;
	char	shifted;
	char	ctrled;
	char	special;
};

static struct keymap keymap[] = {
	#include "keymap.us.h"
};


int getScancode() {
	int c=0;
	c=inb(KEYBOARD_PORT_A);
	return c;
}

static int shift_mode = 0;
static int alt_mode = 0;
static int ctrl_mode = 0;
static int shiftlock_mode = 0;

void keyboard_init()
{
	irq_set_handler(1, keyboard_interrupt);
}
void keyboard_interrupt(struct x86_registers *regs)
{
	char c = GetKey();
	if (c != 0)
	{
		console_putchar(c);
	}
	#ifdef DEBUG_KB
		printf("gs        %d\n", regs->gs);
		printf("fs        %d\n", regs->fs);
		printf("es        %d\n", regs->es);
		printf("ds        %d\n", regs->ds);
		
		printf("edi       %d\n", regs->edi);
		printf("esi       %d\n", regs->esi);
		printf("ebp       %d\n", regs->ebp);
		printf("esp       %d\n", regs->esp);
		printf("ebx       %d\n", regs->ebx);
		printf("edx       %d\n", regs->edx);
		printf("ecx       %d\n", regs->ecx);
		printf("eax       %d\n", regs->eax);
		
		printf("int_no    %d\n", regs->int_no);
		printf("err_code  %d\n", regs->err_code);
		
		printf("eip       %d\n", regs->eip);
		printf("cs        %d\n", regs->cs);
		printf("eflags    %d\n", regs->eflags);
		printf("useresp   %d\n", regs->useresp);
		printf("ss        %d\n", regs->ss);
	#endif
}


static char keyboard_map( int code )
{
	int direction;

	if(code&0x80) {
		direction = 0;
		code = code&0x7f;
	} else {
		direction = 1;
	}

	if(keymap[code].special==SPECIAL_SHIFT) {
		shift_mode = direction;
		return KEY_INVALID;
	} else if(keymap[code].special==SPECIAL_ALT) {
		alt_mode = direction;
		return KEY_INVALID;
	} else if(keymap[code].special==SPECIAL_CTRL) {
		ctrl_mode = direction;
		return KEY_INVALID;
	} else if(keymap[code].special==SPECIAL_SHIFTLOCK) {
		if(direction==0) shiftlock_mode = !shiftlock_mode;
		return KEY_INVALID;
	} else if(direction) {
		if(shiftlock_mode) {
			if(shift_mode) {
				return keymap[code].normal;
			} else {
				return keymap[code].shifted;
			}
		} else if(shift_mode) {
			return keymap[code].shifted;
		} else if(ctrl_mode) {
			return keymap[code].ctrled;
		} else {
			return keymap[code].normal;
		}
	} else {
		return KEY_INVALID;
	}
}


char Key;
char lKey;
char GetKey()
{
	size_t code = getScancode();

	Key = keyboard_map(code);

	if (code > KEYS)
		Key = 0;

	if (Key != lKey)
	{
		lKey = Key;
		return Key;
	}
	else
	{
		lKey = Key;
		return 0;
	}
}
