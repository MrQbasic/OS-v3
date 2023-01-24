#include "../../include/pci/pci.h"
#include "../../include/screen.h"
#include "../../include/cpu/port.h"

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