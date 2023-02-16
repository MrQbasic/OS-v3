#include "mem/palloc.h"
#include "mem/page_map.h"
#include "screen.h"

struct MEMORYMAP_ENTRY* memorymap = (struct MEMORYMAP_ENTRY*) MEMORYMAP_START_PTR;
uint16_t* memorymap_count = (uint16_t*) MEMORYMAP_COUNT_PTR;

uint64_t memorymap_max_ram = 0;

//defined in in bytes. 1 bit = 1 page  |  0=allocated  1=free
uint64_t mem_palloc_size;
uint64_t mem_palloc_start;

void mem_palloc_init(uint64_t* kernelstart, uint64_t* kernelend){
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
        *byte = 0xFF;
    }
    //---ALLOC KERNEL---
    uint64_t number_of_pages = (*kernelend - *kernelstart) / 0x1000;
    uint64_t paddr_start = 0; 
    uint64_t paddr_end = 0;
    int error = 0;
    error |= page_trace_PML4((*kernelstart), &paddr_start);
    error |= page_trace_PML4(*kernelend, &paddr_end);
    //unexpected error
    if(error != 0){
        char errormsg[] = "/nCANT GET PADDR OF KERNEL! ERROR: /e";
        screenPrintChars(errormsg);
        screenPrintX16(error);screenNl();
        while(1);
    }
    //find addr of kernel in palloc table an palloc it
    uint8_t* tbl_entry_start = (uint8_t*) mem_palloc_start;
    for(int i=0; i<*memorymap_count; i++){
        if(memorymap[i].type != MEMORYMAP_TYPE_RAM) continue;
        uint64_t entry_start = memorymap[i].base;
        uint64_t entry_size  = memorymap[i].length;
        if((entry_start <= paddr_start) && (entry_start + entry_size >= paddr_end)){
            uint64_t entry_offset = (paddr_start - entry_start) / 0x1000;
            //go over all bits
            for(int j=0; j<number_of_pages; j++){
                int bit_offset  = (entry_offset + j) % 8;
                int byte_offset = (entry_offset + j) / 8;
                tbl_entry_start[byte_offset] = tbl_entry_start[byte_offset] & (0xFF ^ (1 << bit_offset));
                
            }
            return;
        }
        tbl_entry_start = &tbl_entry_start[entry_size / 0x8000];
    }
    //unexpected error
    char errormsg[] = "/nCANT GET ENTRY OF MEMORYMAP MATCHING KERNEL ADDR!/e";
    screenPrintChars(errormsg);
    while(1);
}


int mem_palloc(uint64_t* addr, uint64_t number_of_pages){
    //get address in table
    uint8_t* tbl_entry_start = (uint8_t*) mem_palloc_start;
    //go over all memorymap entries
    for(int map_entry=0; map_entry<*memorymap_count; map_entry++){
        //skipp if current entry is not ram
        if(memorymap[map_entry].type != MEMORYMAP_TYPE_RAM) continue;
        //get length of memory
        uint64_t map_entry_size = memorymap[map_entry].length / 0x1000;
        //check if entry fits
        if(map_entry_size >= number_of_pages){
            //scan all bytes
            uint64_t bits_left          = map_entry_size / 8;
            uint64_t bits_in_row_amout  = 0; //in bits
            uint64_t bits_in_row_start  = 0; //in bits
            uint64_t byte_index = 0;
            //go over all full bytes
            while(bits_left > 7){
                //get byte
                uint8_t byte = tbl_entry_start[byte_index];
                for(int bit_index=0; bit_index<8; bit_index++){
                    if((byte & 1) == 1){
                        //found a bit so inc counter
                        bits_in_row_amout++;
                        //check if there are more bits needed
                        if(bits_in_row_amout == number_of_pages){
                            //alloc pages
                            for(int i=0; i<number_of_pages; i++){
                                int bit_offset  = (bits_in_row_start + i) % 8;
                                int byte_offset = (bits_in_row_start + i) / 8;
                                tbl_entry_start[byte_offset] = tbl_entry_start[byte_offset] & (0xFF ^ (1 << bit_offset));
                            }
                            //calc page addr
                            *addr = memorymap[map_entry].base;
                            *addr += 0x1000 * bits_in_row_start;
                            return 0;
                        }
                    }else{
                        //not a valid bit so reset counter
                        bits_in_row_amout = 0;
                        //set start index to next bit
                        bits_in_row_start = byte_index * 8 + bit_index + 1;

                    }
                    //get ready for next bit in byte
                    byte = byte >> 1;
                }
                //set byte index to next byte
                byte_index++;
                //bits left to search is -1 byte (8 bits)
                bits_left -= 8;
            }
        }
        //set ptr to start of next entry in table
        tbl_entry_start = &tbl_entry_start[map_entry_size / 8];
    }
    return 1;
}

int mem_pfree(uint64_t addr, uint64_t number_of_pages){
    uint8_t* tbl_entry_start = (uint8_t*) mem_palloc_start;
    //go over all map entries
    for(int map_entry=0; map_entry<*memorymap_count; map_entry++){
        //only care for ram
        if(memorymap[map_entry].type != MEMORYMAP_TYPE_RAM) continue;
        //check if the curren is the correct page
        uint64_t entry_size  = memorymap[map_entry].length / 0x1000;
        uint64_t entry_start = memorymap[map_entry].base;
        if((entry_start <= addr) && ((entry_start + entry_size * 0x1000) >= (addr + number_of_pages * 0x1000))){
            uint64_t entry_offset = (addr - entry_start) / 0x1000;
            //go over all bits
            for(int j=0; j<number_of_pages; j++){
                int bit_offset  = (entry_offset + j) % 8;
                int byte_offset = (entry_offset + j) / 8;
                tbl_entry_start[byte_offset] = tbl_entry_start[byte_offset] | (1 << bit_offset);
                
            }
            return 0;
        }
        tbl_entry_start = &tbl_entry_start[entry_size / 8];
    }
    return 1;
}