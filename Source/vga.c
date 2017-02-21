/*
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

#include <system.h>
#include <font.h>


int basic = 1;
//===============GRAPHICS====================
uint32_t video_width =0;
uint32_t video_height=0;

size_t memory_location = 0xE0000000; //default
size_t pitch = 3;

pixel* graphics_buffer;
pixel* fbuffer;

void putpixel(int x,int y, _24bit_color color) {
  unsigned where = x*pitch + y*video_width*pitch;
  fbuffer[where] = color.b;
  fbuffer[where+1] = color.g;
  fbuffer[where+2] = color.r;
}

void graphics_update_fb() {
  memcpy((char*)graphics_buffer, (char*)fbuffer, video_width*video_height*pitch);
}

void graphics_clear(_24bit_color color) {
	for (size_t y = 0; y < video_height; y++) {
		for (size_t x = 0; x < video_width; x++) {
			putpixel(x, y, color);
		}
	}
}

void graphics_fillrect(size_t x, size_t y, size_t width, size_t height, _24bit_color color) {
	for (size_t ypos = y; ypos < (y+height); ypos++) {
		for (size_t xpos = x; xpos < (x+width); xpos++) {
			putpixel(xpos, ypos, color);
		}
	}
}

void graphics_rect(size_t x, size_t y, size_t width, size_t height, _24bit_color color) {
	graphics_drawline(x, y, x + width, y, color);
  graphics_drawline(x, y + height, x + width, y + height, color);
  
  graphics_drawline(x, y, x, y + height, color);
  graphics_drawline(x + width, y, x + width, y + height, color);
}

void graphics_drawline(size_t x1, size_t y1, size_t x2, size_t y2, _24bit_color color) {
	double xpos = (double)x1;
	double ypos = (double)y1;
	double xdir=(double)x2-(double)x1;
	double ydir=(double)y2-(double)y1;

    double length = sqroot(sqr(xdir)+sqr(ydir));

    if (length != 0){
        xdir = xdir/length;
        ydir = ydir/length;
    }
    while(true)
	{
		if (sqroot(sqr(xpos-(double)x1)+sqr(ypos-(double)y1)) >= length)
			break;
		putpixel(xpos, ypos, color);
		xpos += xdir;
		ypos += ydir;
	}
}

void graphics_bitmap( int x, int y, int width, int height, uint8_t *data, _24bit_color forecolor, _24bit_color backcolor) {
	int i,j,b;
	int value;

	b=0;

	for(j=0;j<height;j++) {
		for(i=0;i<width;i++) {
			value = ((*data)<<b)&0x80;
			if(value) {
				putpixel(x+i,y+j,forecolor);
			} else {
				putpixel(x+i,y+j,backcolor);
			}
			b++;
			if(b==8) {
				data++;
				b=0;
			}
		}
	}
}

void graphics_char( int x, int y, char ch, _24bit_color forecolor, _24bit_color backcolor) {
  int u = ((int)ch)*FONT_WIDTH*FONT_HEIGHT/8;
	return graphics_bitmap(x,y,FONT_WIDTH,FONT_HEIGHT,&fontdata[u],forecolor,backcolor);
}

void graphics_string( int x, int y, const char *ch, _24bit_color forecolor, _24bit_color backcolor) {
	int _char=0;
	while(*ch) {
		graphics_char( x + _char*8, y, *ch, forecolor, backcolor);
		ch++;
		_char++;
	}
}

void graphics_test() {
  for (uint32_t i = 0; i < video_width; i+= (video_width/256))
  {
    for (uint32_t j = 0; j < video_height; j += (video_height/256))
    {
      graphics_fillrect(i, 
                        j, 
                        (video_width/256), 
                        (video_height/256), 
                        create_24bit_color(i / (video_width/256), 0, j / (video_height/256))
                       );
    }
  }
}

graphics_point get_display_size() {
	graphics_point gp;
	gp.x = video_width;
  gp.y = video_height;
  return gp;
}

_24bit_color create_24bit_color(uint8_t r, uint8_t g, uint8_t b) {
  _24bit_color c;
  c.r = r;
  c.g = g;
  c.b = b;
  return c;
}

void startup_graphics_init(vbe_info_t* vbe_info) {
  basic = 1;
  memory_location = vbe_info->physbase;
  video_width = vbe_info->Xres;
  video_height = vbe_info->Yres;
  
  pitch = vbe_info->bpp/8;
  
  fbuffer = (pixel*)memory_location;
  
	graphics_buffer = (pixel*)memory_location;
  
	graphics_clear(create_24bit_color(0,0,0));
}

void graphics_init(vbe_info_t* vbe_info) {
  console_setcolors(create_24bit_color(0,255,0), create_24bit_color(0,0,0));
  
  fbuffer = (pixel*)kmalloc(video_width*video_height*pitch);
  graphics_buffer = (pixel*) memory_location;
  graphics_clear(create_24bit_color(0,0,0));
  console_refresh();
	basic = 0;
}
