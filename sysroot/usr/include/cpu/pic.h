/*
closely based off of 'BaseKernel' project of Douglas Thain (dthain, Github)
*/

#ifndef PIC_H
#define PIC_H

void pic_init( int base0, int base1 );
void pic_enable( uint8_t irq );
void pic_disable( uint8_t irq );
void pic_acknowledge( uint8_t irq );

#endif
