/*
The bulk of this code is based off of software in the 'base_kernel' project
by Douglas Thain (dthain, Github), but in many cases has been heavily modified.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_X_SPACING 9
#define CONSOLE_Y_SPACING 9

typedef struct {
  int x;
  int y;
} point;

void console_reset();
void console_writechar( int x, int y, char ch );
void console_heartbeat();
void console_setcolors(_24bit_color fore, _24bit_color back);
void console_putchar( char c );
void console_putstring( const char *s );
int console_write(const void *buffer, int length);
void console_refresh();
void console_init();
void printf_putchar( char c );
void printf_putstring( char *s );
void printf_puthexdigit( uint8_t i );
void printf_puthex( uint32_t i );
void printf_putint( int32_t i );
void printf( const char *s, ... );
void printf_xy(int x, int y,  const char *s, ... );
void console_set_xy(int x, int y);
void console_set_xy_point(point xy);
point console_get_xy();
point console_get_size();

#endif
