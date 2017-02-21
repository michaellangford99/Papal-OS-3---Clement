#include <system.h>

void set_bit(uint8_t bit, uint8_t* byte_to_set)
{
  byte_to_set[0] |= (1 << bit);
  //printf("%d / 0x%x\n", byte_to_set[0], byte_to_set[0]);
}

void clear_bit(uint8_t bit, uint8_t* byte_to_clear)
{
  byte_to_clear[0] &= ~(1 << bit);
  //printf("%d / 0x%x\n", byte_to_clear[0], byte_to_clear[0]);
}

int test_bit(uint8_t bit, uint8_t* byte_to_test)
{
  if ((byte_to_test[0] & (1 << bit)) != 0)
  {
    return 1;
  }
  return 0;
}