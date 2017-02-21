#ifndef BINTOOLS_H
#define BINTOOLS_H

void set_bit(uint8_t bit, uint8_t* byte_to_set);
void clear_bit(uint8_t bit, uint8_t* byte_to_clear);
int test_bit(uint8_t bit, uint8_t* byte_to_test);

#endif