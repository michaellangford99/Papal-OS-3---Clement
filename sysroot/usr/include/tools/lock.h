#ifndef LOCK_H
#define LOCK_H

#define LOCK_FREE    0
#define LOCK_LOCKED  99

//typedef uint32_t lock_t;

int lock(uint32_t* l);
int unlock(uint32_t* l);

#endif
