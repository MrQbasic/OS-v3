#include "mem/mem_access.h"

void mem_write(void* addr, uint8_t val, uint64_t count){
    uint8_t* ptr = (uint8_t*) addr;
    for(int i=0; i<count; i++){
        ptr[i] = val;
    }
}