#include "pci/pci.h"
#include "screen.h"
#include "cpu/port.h"
#include "mem/alloc.h"

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
    uint32_t addr = (uint32_t) 
                (bus << 16)  |
                (slot << 11) |
                (func << 8)  |
                (offset & 0xFC) |
                0x80000000;
    
    outl(PCI_CONFIG_ADDRESS, addr);

    uint16_t ret = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);;

    return ret;
}


uint16_t  pci_devices_amount = 0;
uint16_t *pci_devices_address;


void pciCheckBus(){
    //check if we need to clear prev dev cfg
    if(pci_devices_address != 0){
        pci_devices_amount = 0;
        mem_free((uint64_t) pci_devices_address);
    }
    //check amount of devices
    for(uint16_t bus=0; bus<=0xFF; bus++){
        for(uint16_t slot=0; slot<=0xFF; slot++){
            uint16_t cfg = pciConfigReadWord((uint8_t) bus, (uint8_t) slot, 0x00, 0x00);
            if(cfg != 0XFFFF){
                pci_devices_amount++;
            }
        }
    }
    //alloc space for adress save
    pci_devices_address = (uint16_t*) mem_alloc(sizeof(uint16_t) * pci_devices_amount, 0);
    if(pci_devices_address == 0) return;
    //save all valid adresses
    int i = 0;
    for(uint16_t bus=0; bus<=0xFF; bus++){
        for(uint16_t slot=0; slot<=0xFF; slot++){
            uint16_t cfg = pciConfigReadWord((uint8_t) bus, (uint8_t) slot, 0x00, 0x00);
            if(cfg != 0XFFFF){
                pci_devices_address[i] = bus + (slot << 8);
                i++;
            }
        }
    }
}