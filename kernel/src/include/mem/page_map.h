#include "../def.h"

//PLM5 / PLM4 / PDPT / PD  FLAGS
#define PAGE_FLAG_P   0x01
#define PAGE_FLAG_RW  0x02
#define PAGE_FLAG_US  0x04
#define PAGE_FLAG_PWT 0x08
#define PAGE_FLAG_PCD 0x10
#define PAGE_FLAG_A   0x20
//PT FLAGS
#define PAGE_PT_FLAG_RW  0x01
#define PAGE_PT_FLAG_US  0x02
#define PAGE_PT_FLAG_PWT 0x04
#define PAGE_PT_FLAG_PCD 0x08
#define PAGE_PT_FLAG_A   0x10
#define PAGE_PT_FLAG_D   0x20
#define PAGE_PT_FLAG_PAT 0x40
#define PAGE_PT_FLAG_G   0x80

void page_map_init(uint64_t maxphyaddr, uint64_t maxlinaddr);

int page_trace_PML4(uint64_t vaddr, uint64_t* paddr);
int page_trace_PML5(uint64_t vaddr, uint64_t* paddr);

void page_map_PML4(uint64_t vaddr, uint64_t paddr, uint8_t flags, uint8_t prot);
void page_map_PML5(uint64_t vaddr, uint64_t paddr, uint8_t flags, uint8_t prot);