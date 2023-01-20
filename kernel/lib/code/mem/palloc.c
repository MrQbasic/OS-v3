#include "../../include/mem/palloc.h"
#include "../../include/screen.h"

struct MEMORYMAP_ENTRY* memorymap = (struct MEMORYMAP_ENTRY*) MEMORYMAP_START_PTR;
uint16_t* memorymap_count = (uint16_t*) MEMORYMAP_COUNT_PTR;

uint64_t memorymap_max_ram = 0;

//defined in in bytes. 1 bit = 1 page
uint64_t mem_palloc_size;
uint64_t mem_palloc_start;

void mem_palloc_init(uint64_t* kernelend){
    for(int i=0; i<(*memorymap_count-1);i++){
        struct MEMORYMAP_ENTRY entry = memorymap[i];
        //only read in if it is RAM
        if(entry.type == MEMORYMAP_TYPE_RAM){
            memorymap_max_ram += entry.length;
        }
    }
    //calc size of palloc table
    mem_palloc_size = memorymap_max_ram / 0x1000 / 8;
    //add offset to kernelend + save old as start
    mem_palloc_start = *kernelend;
    *kernelend += mem_palloc_size;
    *kernelend += 1;
    //round up kernelend to next page
    uint64_t overflow = *kernelend % 0x1000;
    *kernelend += 0x1000 - overflow;
    for(uint64_t i=0; i<mem_palloc_size; i++){
        uint8_t* byte = (uint8_t*) (mem_palloc_start + i);
        *byte = 0;
    }
    //alloc the kernel
    uint64_t number_of_pages = *kernelend / 0x1000 + 1;
    uint64_t counter = 0;
    while(number_of_pages != 0){
        uint8_t* byte = (uint8_t*) (mem_palloc_start+counter);
        if(number_of_pages < 8){
            for(int i=0; i<number_of_pages; i++){
                *byte = (1 << i);
            }
            number_of_pages = 0; // end loop
        }else{
            *byte = 0xFF;
            number_of_pages -= 8;
        }
        counter++;
    }
    
}


int mem_palloc(uint64_t* addr, uint64_t number_of_pages){
    uint64_t bits_in_row = 0;
    uint64_t start_bit = 0;
    for(int i=0; i<mem_palloc_size; i++){
        //get a byte from the list
        uint8_t *byte = (uint8_t*) (mem_palloc_start + i);
        //go over all bits in the byte
        for(int j=0; j<8; j++){
            if(((*byte >> j)&1) == 0){
                bits_in_row += 1;
                if(bits_in_row == number_of_pages){
                    //deconstruct page addr from memorymap
                    for(int l=0; l<(*memorymap_count-1);l++){
                        if(memorymap[l].type == MEMORYMAP_TYPE_RAM){
                            uint64_t size = memorymap[l].length;
                            size = size >> 12;
                            if(size < start_bit){
                                start_bit -= size;
                            }else{
                                *addr = memorymap[l].base;
                                *addr += 0x1000 * start_bit;
                                return 0;
                            }
                        }
                    }
                    return 0;
                }
            }else{
                //reset counters for start + lenght of valid find
                bits_in_row = 0;
                start_bit = (j+1)+(i*8);
            }
        }
    }
    return 1;
}