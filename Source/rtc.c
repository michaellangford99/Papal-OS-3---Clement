#include "system.h"

uint8_t rtc_get_year()
{
outb(0x70, 0x09);
clock_wait(5);
return inb(0x71);
}

uint8_t rtc_get_month()
{
outb(0x70, 0x08);
return inb(0x71);
}

uint8_t rtc_get_day()
{
outb(0x70, 0x07);
return inb(0x71);
}

uint8_t rtc_get_weekday()
{
outb(0x70, 0x06);
return inb(0x71);
}

uint8_t rtc_get_hour()
{
outb(0x70, 0x04);
return inb(0x71);
}

uint8_t rtc_get_minute()
{
outb(0x70, 0x02);
return inb(0x71);
}

uint8_t rtc_get_second()
{
outb(0x70, 0x00);
return inb(0x71);
}