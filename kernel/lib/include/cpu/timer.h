#include "../def.h"

#define SYSCOUNTER_SEGSEL 0x08
#define SYSCOUNTER_IDTE   0x20
#define SYSCOUNTER_FLAG   0x8E

uint64_t syscounter_get();

void syscounter_init();