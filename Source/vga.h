/*
Copyright (C) 2016 Michael Langford
This software is distributed under the GNU General Public License.

'enum vga_color' and
'putpixel()' are basic vga code based off of osdev "Drawing in Protected Mode" demo.

graphics_clear(),
graphics_rect(),
graphics_fillrect(),
graphics_drawline(), and 
graphics_initialize() are all written by Michael Langford

"font.h",
graphics_bitmap(),
graphics_char(), and
graphics_string() are based off of (or identical to) software in the 'base_kernel' project
				  by Douglas Thain (dthain, Github)

*/

#ifndef VGA_H
#define VGA_H

typedef uint8_t pixel;

typedef struct {
	uint32_t x;
	uint32_t y;
} graphics_point;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} _24bit_color;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} _24bit_rgba_color;

void putpixel(int x,int y, _24bit_color color);
void graphics_update_fb();
void graphics_clear(_24bit_color color);
void graphics_fillrect(size_t x, size_t y, size_t width, size_t height, _24bit_color color);
// WARNING ----- Will have to be updated for different screen resolution
void graphics_rect(size_t x, size_t y, size_t width, size_t height, _24bit_color color);
void graphics_drawline(size_t x1, size_t y1, size_t x2, size_t y2, _24bit_color color);
void graphics_bitmap( int x, int y, int width, int height, uint8_t *data, _24bit_color forecolor, _24bit_color backcolor);
void graphics_char( int x, int y, char ch, _24bit_color forecolor, _24bit_color backcolor);
void graphics_string( int x, int y, const char *ch, _24bit_color forecolor, _24bit_color backcolor);
void graphics_test();
_24bit_color create_24bit_color(uint8_t r, uint8_t g, uint8_t b);
graphics_point get_display_size();
void graphics_init(vbe_info_t* vbe_info);

#endif
