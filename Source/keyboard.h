/*
Copyright (C) 2016 Michael Langford
This software is distributed under the GNU General Public License.

ascii.h,
struct keymap,
keyboard.us.c,
static struct keymap keymap[] {}, and
keyboard_map() are based off of or identical to software in the 'base_kernel' project
               by Douglas Thain (dthain, Github)
               
getScancode(), and
GetKey() are written by Michael Langford

'#define's (excepting 'KEYS') are also written by Douglas Thain.
*/

#define KEYS        128
#define KEY_INVALID 0

#define SPECIAL_SHIFT 1
#define SPECIAL_ALT   2
#define SPECIAL_CTRL  3
#define SPECIAL_SHIFTLOCK 4

#define KEYBOARD_PORT_A		0x60
#define KEYBOARD_PORT_B		0x61
#define KEYBOARD_ACK    0x80


void keyboard_init();
void keyboard_interrupt(struct x86_registers *regs);
int getScancode();
char MapKey(int scancode);
char ToUppercase(char k);
char GetKey();
