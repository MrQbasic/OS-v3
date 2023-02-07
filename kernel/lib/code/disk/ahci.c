#include "disk/ahci.h"
#include "pci/pci.h"
#include "mem/mem.h"
#include "screen.h"

uint64_t* ahci_abar_phy;
uint64_t* ahci_abar_lin;

int ahci_init_disk(uint16_t disk_count, uint16_t* disk_addr){
    //setup abar ports
    ahci_abar_phy = (uint64_t*) mem_alloc(sizeof(uint64_t) * disk_count, 0);
    ahci_abar_lin = (uint64_t*) mem_alloc(sizeof(uint64_t) * disk_count, 0);
    for(int i=0; i<disk_count; i++){
        uint8_t  bus = (uint8_t) (disk_addr[i] >> 0);
        uint8_t slot = (uint8_t) (disk_addr[i] >> 8);
        ahci_abar_phy[i]  = (uint64_t) (pciConfigReadWord(bus, slot, 0, 0x24) <<  0);
        ahci_abar_phy[i] += (uint64_t) (pciConfigReadWord(bus, slot, 0, 0x26) << 16);
        ahci_abar_lin[i]  = mem_alloc(0x1000, 0x1000);

        int error = 0;
        uint64_t paddr;
        error |= page_trace_PML4(ahci_abar_lin[i], &paddr);
        error |= mem_pfree(paddr,1);
        page_map_PML4(ahci_abar_lin[i], ahci_abar_phy[i], 1, 0);
        //unexpected error 
        if(error != 0){
            char error[] = "OUT OF MEMORY!/n/e";
            screenClear();
            screenPrintChars(error);
            while(1);
        }
    }

    //setup 

    return 0;
}