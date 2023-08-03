#include "tools/checksum.h"

uint64_t checksumMemory(uintptr_t start, uint32_t size){
    uint64_t checksum = 0;
    for(uint32_t i=0; i<size; i++){
        checksum += *((uint8_t*)(start + i));
    }
    return checksum;
}