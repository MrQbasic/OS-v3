#include "../../include/mem/page_map.h"
#include "../../include/screen.h"

uint64_t *pagemap_start;
uint8_t page_maxphyaddr;
uint8_t page_maxlinaddr;
uint64_t page_map_filter;

void page_map_init(uint64_t maxphyaddr, uint64_t maxlinaddr){
    uint64_t reg;
    //get addr max
    __asm__ __volatile__("movq $0x80000008, %%rax;"
                         "cpuid":
                         "=a"(reg):);
    page_maxphyaddr = (uint8_t) reg;
    page_maxlinaddr = (uint8_t) (reg >> 8);
    //generate filter for maping
    page_map_filter = 1;
    page_map_filter = page_map_filter << page_maxphyaddr;
    page_map_filter -= 1;
    page_map_filter = page_map_filter << 12;
    //get cr3 cfg
    __asm__ __volatile__ ("mov %%cr3, %%rax":
                          "=a"(reg):);
    pagemap_start = (uint64_t*) (reg & page_map_filter);
    //set backup ptrs for pageing

}


//flags bit 0-7. present = 1. size = 0
//prot is 1 bit. bit 1 = XD
void page_construct_PD_E(uint64_t* entry, uint8_t flags, uint64_t addr, uint8_t prot){
    *entry |= (uint64_t) (flags | 1);
    *entry |= (uint64_t) (addr & page_map_filter);
    *entry |= (uint64_t) (((uint64_t)(prot & 0x1)) << 63);
}

//flags bit 1-8. present bit is set to 1
//prot is 5 bit. bit 0-3 = KEY. bit 4 = XD
void page_construct_PT_E(uint64_t* entry, uint8_t flags, uint64_t addr, uint8_t prot){    
    *entry |= (uint64_t) ((flags << 1) | 1);
    *entry |= (uint64_t) (addr & page_map_filter);
    *entry |= (uint64_t) (((uint64_t)(prot & 0x1F)) << 59);
}



#define page_map_filter_pml5 0x01FF000000000000
#define page_map_filter_pml4 0x0000FF8000000000
#define page_map_filter_pdpt 0x0000007FC0000000
#define page_map_filter_pd   0x000000003FE00000
#define page_map_filter_pt   0x00000000001FF000

#define page_map_shift_pml5 56-12
#define page_map_shift_pml4 47-12
#define page_map_shift_pdpt 38-12
#define page_map_shift_pd   29-12
#define page_map_shift_pt   20-12

uint64_t page_pml5_backup;
uint64_t page_pml4_backup;
uint64_t page_pdpt_backup;
uint64_t pag_pd_backup;

void page_map_PML4(uint64_t vaddr, uint64_t paddr, uint8_t flags, uint8_t prot){
    //limit addr args
    vaddr = vaddr & page_map_filter;
    paddr = paddr & page_map_filter;
    //-
    uint64_t* PML4_E  = pagemap_start + ((vaddr & page_map_filter_pml4) >> page_map_shift_pml4);
    if(!(*PML4_E & 1)){
        screenPrintX64(0);
        while(1);
    }
    uint64_t* PDPT_B = (uint64_t*) (*PML4_E & page_map_filter);
    uint64_t* PDPT_E = PDPT_B + ((vaddr & page_map_filter_pdpt) >> page_map_shift_pdpt);
    if(!(*PDPT_E & 1)){
        screenPrintX64(1);
        while(1);
    }
    uint64_t* PD_B = (uint64_t*) (*PDPT_E & page_map_filter);
    uint64_t* PD_E = PD_B + ((vaddr & page_map_filter_pd) >> page_map_shift_pd);
    if(!(*PD_E & 1)){
        screenPrintX64(2);
        while(1);
    }
    uint64_t* PT_B = (uint64_t*) (*PD_E & page_map_filter);
    uint64_t* PT_E = PT_B + ((vaddr & page_map_filter_pt) >> page_map_shift_pt);
    page_construct_PT_E(PT_E, flags, paddr, prot);
}


void page_map_PML5(uint64_t vaddr, uint64_t paddr, uint8_t flags, uint8_t prot){
    vaddr = vaddr & page_map_filter;
    paddr = paddr & page_map_filter;
    uint64_t PML5_E  = pagemap_start[((vaddr & page_map_filter_pml5) >> page_map_shift_pml5)];
    uint64_t* PML4_B = (uint64_t*) (PML5_E & page_map_filter);
    uint64_t PML4_E  = PML4_B[((vaddr & page_map_filter_pml4) >> page_map_shift_pml4)];
    uint64_t* PDPT_B = (uint64_t*) (PML4_E & page_map_filter);
    uint64_t PDPT_E  = PDPT_B[((vaddr & page_map_filter_pdpt) >> page_map_shift_pdpt)];
    uint64_t* PD_B   = (uint64_t*) (PDPT_E & page_map_filter);
    uint64_t PD_E    =   PD_B[((vaddr & page_map_filter_pd)   >> page_map_shift_pd)];
    uint64_t* PT_B   = (uint64_t*) (PD_E & page_map_filter);
    uint64_t* PT_E   =  &(PT_B[((vaddr & page_map_filter_pt) >> page_map_shift_pt)]);
    page_construct_PT_E(PT_E, flags, paddr, prot);
}


//retuen 0->OK 1->PT 2->PD 3->PDPT 4->PML4
int page_trace_PML4(uint64_t vaddr, uint64_t* paddr){
    //limit addr args
    vaddr = vaddr & page_map_filter;
    //PML4
    uint64_t* PML4_E = pagemap_start + ((vaddr & page_map_filter_pml4) >> page_map_shift_pml4);
    if((*PML4_E & 1) != 1) return 4;
    //PDPT
    uint64_t* PDPT_B = (uint64_t*) (*PML4_E & page_map_filter);
    uint64_t* PDPT_E = PDPT_B + ((vaddr & page_map_filter_pdpt) >> page_map_shift_pdpt);
    if((*PDPT_E & 1) != 1) return 3;
    //PD
    uint64_t* PD_B = (uint64_t*) (*PDPT_E & page_map_filter);
    uint64_t* PD_E = PD_B + ((vaddr & page_map_filter_pd) >> page_map_shift_pd);
    if((*PD_E & 1) != 1) return 2;
    //PT
    uint64_t* PT_B = (uint64_t*) (*PD_E & page_map_filter);
    uint64_t* PT_E = PT_B + ((vaddr & page_map_filter_pt) >> page_map_shift_pt);
    if((*PT_E & 1) != 1) return 1;
    //write and ret
    *paddr = ((*PT_E) & page_map_filter);
    return 0;
}

//retuen 0->OK  1->PD  2->PDPT  3->PML4  4->PML5
int page_trace_PML5(uint64_t vaddr, uint64_t* paddr){
    //limit addr args
    vaddr = vaddr & page_map_filter;
    //PML5
    uint64_t* PML5_E = pagemap_start + ((vaddr & page_map_filter_pml5) >> page_map_shift_pml5);
    //PML4
    uint64_t* PML4_B = (uint64_t*) (*PML5_E & page_map_filter);
    uint64_t* PML4_E = PML4_B + ((vaddr & page_map_filter_pml4) >> page_map_shift_pml4);
    if(!(*PML4_E & 1)){return 3;}
    //PDPT
    uint64_t* PDPT_B = (uint64_t*) (*PML4_E & page_map_filter);
    uint64_t* PDPT_E = PDPT_B + ((vaddr & page_map_filter_pdpt) >> page_map_shift_pdpt);
    if(!(*PDPT_E & 1)){return 2;}
    //PD
    uint64_t* PD_B = (uint64_t*) (*PDPT_E & page_map_filter);
    uint64_t* PD_E = PD_B + ((vaddr & page_map_filter_pd) >> page_map_shift_pd);
    if(!(*PD_E & 1)){return 1;}
    //PT
    uint64_t* PT_B = (uint64_t*) (*PD_E & page_map_filter);
    uint64_t* PT_E = PT_B + ((vaddr & page_map_filter_pt) >> page_map_shift_pt);
    //write and ret
    *paddr = (*PT_E & page_map_filter);
    return 0;
}