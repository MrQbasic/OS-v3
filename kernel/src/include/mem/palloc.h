#include "def.h"

enum {
    MEMORYMAP_TYPE_RAM = 1,
    MEMORYMAP_TYPE_RES = 2,
    MEMORYMAP_TYPE_ACPI_RECLAIM = 3,
    MEMORYMAP_TYPE_ACPI_NVS = 4,
    MEMORYMAP_TYPE_BAD = 5
};

struct MEMORYMAP_ENTRY{
    uint64_t base;
    uint64_t length;
    uint32_t type;
}__attribute__((packed));


#define MEMORYMAP_COUNT_PTR 0x9000
#define MEMORYMAP_START_PTR 0x9002


//return: addidtional kernelend offset
void mem_palloc_init(uint64_t* kernelstart, uint64_t* kernelend);


//returns:
//  0 -> OK
//  1 -> NO SPACE
int mem_palloc(uint64_t* addr, uint64_t number_of_pages);

//returns:
//  0 -> OK
//  1 -> NO MATCHES
int mem_pfree(uint64_t addr, uint64_t number_of_pages);

void print_memorymap();