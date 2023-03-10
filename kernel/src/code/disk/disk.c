#include "disk/disk.h"
#include "def.h"
#include "screen.h"
#include "pci/pci.h"

uint32_t *ahci_address;

uint16_t disk_found_ahci_amount = 0;
uint16_t disk_found_ahci_addr[DISK_MAX_AHCI];


int disk_searchDisks(){
    //search pci bus
    uint16_t  pci_devices_amount;
    uint16_t* pci_devices_addess = pciGetDeviceTable(&pci_devices_amount);
    for(int i=0; i<pci_devices_amount; i++){
        //get addresses of pci devices
        uint8_t  bus = (uint8_t) (pci_devices_addess[i] >> 0);
        uint8_t slot = (uint8_t) (pci_devices_addess[i] >> 8);
        //check headder
        uint16_t cfg = pciConfigReadWord(bus, slot, 0, 0x0A);
        uint8_t subclass = (uint8_t) (cfg >> 0);
        uint8_t    class = (uint8_t) (cfg >> 8);
        if(class == 0x01 && subclass == 0x06){
            if(disk_found_ahci_amount == (DISK_MAX_AHCI-1)){
                screenPrint("DISK ERROR: TOO MANY AHCI INTERFACES FOUND!/e");
                while(1);
            }
            disk_found_ahci_addr[disk_found_ahci_amount] = pci_devices_addess[i];
            disk_found_ahci_amount++; 
        }
    }

    if(disk_found_ahci_amount != 0){
        screenPrint("NUMBER OF AHCI DEV: /xW /n/e",disk_found_ahci_amount);
        return 1;
    }

    return 0;
}


void disk_init(){
    if(disk_found_ahci_amount != 0){
        int error = ahci_init_disk(disk_found_ahci_amount, disk_found_ahci_addr);
    }
}
