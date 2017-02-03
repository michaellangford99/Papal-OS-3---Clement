#include "../system.h"

int lock(uint32_t* l)
{
  if (*l == LOCK_FREE)
  {
    *l = LOCK_LOCKED;
    return 1;
  }
  if ((*l) == LOCK_LOCKED)
  {
    return 0;
  }
  *l = LOCK_LOCKED;
  return 1;
}

int unlock(uint32_t* l)
{
  *l = LOCK_FREE;
  return 1;  
}