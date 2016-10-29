/*
The bulk of this code is based off of software in the 'base_kernel' project
by Douglas Thain (dthain, Github), but in many cases has been heavily modified.
*/

#include "system.h"

int xpos=0;
int ypos=0;

int xsize = 0;
int ysize = 0;

_24bit_color bgcolor = {0, 0, 0};
_24bit_color fgcolor = {0, 255, 0};

char console_buffer[1024*768*2];//the times 2 is just in case

void console_reset()
{
	xsize = get_display_size().x/CONSOLE_X_SPACING;
	ysize = get_display_size().y/CONSOLE_Y_SPACING;
	
	for (int i = 0; i < xsize*ysize; i++)
		console_buffer[i]=' ';
	xpos = ypos = 0;
	graphics_clear(bgcolor);
}

void console_writechar( int x, int y, char ch )
{
	console_buffer[x + y*xsize] = ch;
	graphics_char(x*CONSOLE_X_SPACING,y*CONSOLE_Y_SPACING,ch,fgcolor,bgcolor);
}

void console_heartbeat()
{
	static int onoff=0;
	if(onoff) {
		graphics_char(xpos*CONSOLE_X_SPACING,ypos*CONSOLE_Y_SPACING,'_',fgcolor,bgcolor);
	} else {
		graphics_char(xpos*CONSOLE_X_SPACING,ypos*CONSOLE_Y_SPACING,'_',bgcolor,bgcolor);
	}
	onoff = !onoff;
}

void console_setcolors(_24bit_color fore, _24bit_color back)
{
	fgcolor=fore;
	bgcolor=back;
}

void console_putchar( char c )
{
	// erase '_' char that is already there
	console_writechar(xpos,ypos,' ');

	// check for special keys (i.e. enter, backspace, TAB, ctrl, alt...)
	switch(c) {
		case 10:
		case 13:
			xpos=0;
			ypos++;
			break;
		case '\f':
			console_reset();
			break;
		case '\b':
			xpos--;
			break;
		default:
			console_writechar(xpos,ypos,c);
			xpos++;
			break;
	}

	// check if x position is out of bounds <
	if(xpos<0) {
		xpos=xsize-1;
		ypos--;
	}

	// check if x position is out of bounds >
	if(xpos>=xsize) {
		xpos=0;
		ypos++;
	}

	if (ypos < 0)
	{
		xpos=0;
		ypos=0;
	}
	// check if y position is out of bounds >
	if(ypos>=ysize) {
		// move the data in the buffer one line up
		for (int i = 0; i < ysize - 1; i++)
		{
			for (int j = 0; j < xsize; j++)
			{
				console_buffer[j + i*xsize] = console_buffer[j + (i+1)*xsize];

			}
		}

		// reprint the data
		for (int i = 0; i < ysize; i++)
		{
			for (int j = 0; j < xsize; j++)
			{
				console_writechar(j, i, console_buffer[j + i*xsize]);

			}
		}

		// clear the bottom line
		for (int i = 0; i < xsize; i++)
		{
			console_writechar(i, ysize-1, ' ');
		}

		// reset console position to safe spot
		xpos=0;
		ypos = ysize-1;
	}

	// write the '_' position char in front of current char
	console_writechar(xpos,ypos,'_');
}

void console_putstring( const char *s )
{
	while(*s) {
		console_putchar(*s);
		s++;
	}
}

int console_write(const void *buffer, int length)
{
	char *cbuffer = (char*)buffer;
	while(length>0) {
		console_putchar(*cbuffer);
		cbuffer++;
		length--;
	}
	return 1;
}

void console_refresh()
{
	// reprint the data
	for (int i = 0; i < ysize; i++)
	{
		for (int j = 0; j < xsize; j++)
		{
			console_writechar(j, i, console_buffer[j + i*xsize]);

		}
	}
}

void console_init()
{
	console_reset();
	console_putstring("console: ready\n");
}

void printf_putchar( char c )
{
	console_write(&c,1);
}

void printf_putstring( char *s )
{
	console_write(s,strlen(s));
}

void printf_puthexdigit( uint8_t i )
{
	if(i<10) {
		printf_putchar('0'+i);
	} else {
		printf_putchar('a'+i-10);
	}
}

void printf_puthex( uint32_t i )
{
	int j;
	for(j=28;j>=0;j=j-4) {
		printf_puthexdigit((i>>j)&0x0f);
	}
}

void printf_putint( int32_t i )
{
	int f, d;
	if(i<0 && i!=0) {
		printf_putchar('-');
		i=-i;
	}

	f = 1;
	while((i/f)>0) {
		f*=10;
	}
	f=f/10;
	if(f==0) f=1;
	while(f>0) {
		d = i/f;
		printf_putchar('0'+d);
		i = i-d*f;
		f = f/10;
	}
}

void printf( const char *s, ... )
{
	va_list args;

	uint32_t u;
	int32_t i;
	char c;
	char *str;

	va_start(args,s);

	while(*s) {
		if(*s!='%') {
			printf_putchar(*s);
		} else {
			s++;
			switch(*s) {
				case 'd':
					i = va_arg(args,int32_t);
					printf_putint(i);
					break;
				case 'x':
					u = va_arg(args,uint32_t);
					printf_puthex(u);
					break;
				case 'c':
					c = va_arg(args,uint32_t);
					printf_putchar(c);
					break;					
				case 's':
					str = va_arg(args,char*);
					printf_putstring(str);
					break;
				case 0:
					return;
					break;
				default:
					printf_putchar(*s);
					break;
			}
		}
		s++;
	}
	va_end(args);
}

void printf_xy(int x, int y,  const char *s, ... )
{
	int reg_x = xpos;
	int reg_y = ypos;
	
	xpos = x;
	ypos = y;
	
	va_list args;

	uint32_t u;
	int32_t i;
	char *str;

	va_start(args,s);

	while(*s) {
		if(*s!='%') {
			printf_putchar(*s);
		} else {
			s++;
			switch(*s) {
				case 'd':
					i = va_arg(args,int32_t);
					printf_putint(i);
					break;
				case 'x':
					u = va_arg(args,uint32_t);
					printf_puthex(u);
					break;
				case 's':
					str = va_arg(args,char*);
					printf_putstring(str);
					break;
				case 0:
					return;
					break;
				default:
					printf_putchar(*s);
					break;
			}
		}
		s++;
	}
	va_end(args);
	
	xpos = reg_x;
	ypos = reg_y;
}

point console_get_xy()
{
	point p;
	p.x = xpos;
	p.y = ypos;
	return p;
}

point console_get_size()
{
	point p;
	p.x = (int)xsize;
	p.y = (int)ysize;
	return p;
}

