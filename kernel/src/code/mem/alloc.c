#include "mem/alloc.h"
#include "mem/page_map.h"
#include "mem/palloc.h"
#include "screen.h"

struct MEM_ALLOC_TAIL *mem_listStart;

uint64_t mem_end;
uint64_t maxlinaddr;

void mem_init(uint64_t* kernelend, uint64_t memoryend, uint8_t maxlin){
    //alloc space of kernel
    struct MEM_ALLOC_TAIL *kernel_entry = (struct MEM_ALLOC_TAIL *) *kernelend;
    mem_listStart = kernel_entry; //the kernel entry is the first entry
    kernel_entry->prev = 0;  //there is no entry before. This is the first entry
    kernel_entry->next = 0;  //there is no entry after. This is the last entry
    kernel_entry->vaddr = 0x2000; //the kernel entry starts at addr 0
    //pass val
    mem_end = memoryend;
    maxlinaddr = (1 << maxlin) - 1;
}

void mem_print_tail(struct MEM_ALLOC_TAIL* tail){
    screenPrint("/n/n/e");
    screenPrint("NEXT : /xQ/n/e", (uint64_t) tail->next);
    screenPrint("PREV : /xQ/n/e", (uint64_t) tail->prev);
    screenPrint("START: /xQ/n/e", (uint64_t) tail->vaddr);
}

uint64_t paddr = 0;
char errormsg[] = "OUT OF MEMORY! KERNEL HALT! /e";
void mem_alloc_check_page(uint64_t start, uint64_t end){
    for(uint64_t page=(start & 0xFFFFFFFFFFFFF000); page<end; page+=0x1000){
        int error = page_trace_PML4(page, &paddr);
        if(error != 0){
            error = mem_palloc(&paddr, 1);
            if(error == 0){
                page_map_PML4(page, paddr, 0x1, 0);
            }else{
                screenClear();
                screenPrintChars(errormsg);
                while(1);
            }
        }
    } 
}

uint64_t mem_alloc(size_t size, uint64_t align){
    struct MEM_ALLOC_TAIL* entry = mem_listStart;
    //loop over all entries in list
    while(1){
        //skip if its the first entry
        if(entry->prev != 0){
            uint64_t inserted_start;
            inserted_start = (uint64_t) entry->prev;
            inserted_start += sizeof(struct MEM_ALLOC_TAIL);
            if(align != 0){
                inserted_start += align - (inserted_start % align);
            }
            //check if start sits in space btween entries after alignment
            if(inserted_start < entry->vaddr){
                //check for space
                if(size + sizeof(struct MEM_ALLOC_TAIL) <= ((uint64_t)entry->vaddr) - inserted_start){
                    struct MEM_ALLOC_TAIL* inserted_entry = (struct MEM_ALLOC_TAIL*) inserted_start;
                    inserted_entry += size;
                    mem_alloc_check_page(inserted_start, ((uint64_t)inserted_entry)+sizeof(struct MEM_ALLOC_TAIL));
                    //construct entry
                    inserted_entry->next = entry;
                    inserted_entry->prev = entry->prev;
                    inserted_entry->vaddr = inserted_start;
                    //insert into list
                    entry->prev->next = inserted_entry;
                    entry->prev       = inserted_entry;
                    //-
                    return inserted_start;
                }
            }
        }
        //check for end
        if(entry->next == 0) break;
        //goto next entry in list
        entry = entry->next;
    }
    //apend entry
    uint64_t append_start;
    struct MEM_ALLOC_TAIL* appended_entry;
    //calculate new tail position
    append_start = (uint64_t) entry;
    append_start += sizeof(struct MEM_ALLOC_TAIL);
    if(align != 0){
        append_start += align - (append_start % align);
    }
    //-
    appended_entry = (struct MEM_ALLOC_TAIL*) (append_start + size);
    mem_alloc_check_page(append_start, ((uint64_t)appended_entry)+sizeof(struct MEM_ALLOC_TAIL));
    //construct entry
    appended_entry->next = 0;
    appended_entry->prev = entry;
    appended_entry->vaddr = append_start;
    //add to list
    entry->next = appended_entry;
    //-
    return append_start;
}

int mem_free(uint64_t addr){
    struct MEM_ALLOC_TAIL* entry = mem_listStart;
    while(entry != 0){
        //check current entry
        if(entry->vaddr == addr){
            //remove entry from list
            entry->prev->next = entry->next;
            entry->next->prev = entry->prev;
            return 0;
        }
        //goto next entry
        entry = entry->next;
    }
    //nothing found
    return 1;
}