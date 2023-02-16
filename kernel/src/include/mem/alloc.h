#include "../def.h"

struct MEM_ALLOC_TAIL {
    struct MEM_ALLOC_TAIL* next; 
    struct MEM_ALLOC_TAIL* prev;
    uint64_t vaddr;
};

void mem_init(uint64_t* kernelend, uint64_t memoryend, uint8_t maxlin);

void mem_print_tail(struct MEM_ALLOC_TAIL* tail);

//size = number of bytes
//align = 0 --> no alignment
uint64_t mem_alloc(size_t size, uint64_t align);

//returns:
//  0 -> OK
//  1 -> NOT FOUND
int mem_free(uint64_t addr);