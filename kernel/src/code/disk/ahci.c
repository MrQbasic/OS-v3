#include "disk/ahci.h"
#include "pci/pci.h"
#include "mem/mem.h"
#include "screen.h"

uint64_t* ahci_abar_phy;
AHCI_HBA** ahci_abar;

int ahci_init_disk(uint16_t disk_count, uint16_t* disk_addr){
    //setup abar ports
    ahci_abar_phy = (uint64_t*) mem_alloc(sizeof(uint64_t) * disk_count, 0);
    ahci_abar = (AHCI_HBA**) mem_alloc(sizeof(AHCI_HBA*) * disk_count, 0);
    for(int i=0; i<disk_count; i++){
        uint8_t  bus = (uint8_t) (disk_addr[i] >> 0);
        uint8_t slot = (uint8_t) (disk_addr[i] >> 8);
        ahci_abar_phy[i]  = 0x000000000000FFFF & (pciConfigReadWord(bus, slot, 0, 0x24) <<  0);
        ahci_abar_phy[i] |= 0x00000000FFFF0000 & (pciConfigReadWord(bus, slot, 0, 0x26) << 16);
        ahci_abar[i] = (AHCI_HBA*) mem_alloc(0x1000, 0x1000);
        //remap memory so match abar
        int error = 0;
        uint64_t paddr;
        error |= page_trace_PML4((uint64_t) ahci_abar[i], &paddr);
        error |= mem_pfree(paddr,1);
        //unexpected error 
        if(error != 0){
            screenPrint("OUT OF MEMORY!/n/e");
            while(1);
        }
        page_map_PML4((uint64_t) ahci_abar[i], ahci_abar_phy[i], 0b00010001, 0x00);
        //print implemented ports / devices
        uint32_t impl = ahci_abar[i]->ports_implemented;
        int port_index = 0;
        while(impl != 0){
            int type = ahci_port_check_type(&(ahci_abar[i]->ports[port_index]));
            if(type == AHCI_DEV_PM)     screenPrint("PM /e");
            if(type == AHCI_DEV_SATA)   screenPrint("SATA /e");
            if(type == AHCI_DEV_SATAPI) screenPrint("SATAPI /e");
            if(type == AHCI_DEV_SEMB)   screenPrint("SEMB /e");
            if(type != AHCI_DEV_NULL){
                screenPrint("DEV @ PORT: /xB LIN: /xQ PHY: /xQ/n/e",(uint8_t)port_index, ahci_abar[i],ahci_abar_phy[i]);
            }
            port_index++;
            impl = impl >> 1;
        }
        //rebase ports
        ahci_port_rebase(ahci_abar[i]);
    }
    //setup 

    return 0;
}

int ahci_port_check_type(AHCI_HBA_PORT* port){
    uint32_t status = port->sata_status;
    uint8_t ipm = (status >> 8) & 0x0F;
    uint8_t det = status & 0x0F;
    //check if a drive is usable
    if(det != AHCI_HBA_PORT_DET_PRESENT) return AHCI_DEV_NULL;
    if(ipm != AHCI_HBA_PORT_IPM_ACTIVE ) return AHCI_DEV_NULL;
    //check type
    uint32_t signature = port->signature;
    if(signature == SATA_SIG_ATAPI) return AHCI_DEV_SATAPI;
    if(signature == SATA_SIG_SEMB)  return AHCI_DEV_SEMB;
    if(signature == SATA_SIG_PM)    return AHCI_DEV_PM;
    return AHCI_DEV_SATA;
}

void ahci_port_stop_cmd(AHCI_HBA_PORT* port){
    port->cmd &= ~AHCI_HBA_PORT_CMD_ST;
    port->cmd &= ~AHCI_HBA_PORT_CMD_FRE;
    while(1){
        if(port->cmd & AHCI_HBA_PORT_CMD_FR) continue;
        if(port->cmd & AHCI_HBA_PORT_CMD_CR) continue;
        break;
    }
}

void ahci_port_start_cmd(AHCI_HBA_PORT* port){
    while(port->cmd & AHCI_HBA_PORT_CMD_CR);
    port->cmd |= AHCI_HBA_PORT_CMD_FRE;
    port->cmd |= AHCI_HBA_PORT_CMD_ST;
}

void ahci_port_rebase(AHCI_HBA* ahci_hba){
    uint64_t impl = (uint64_t) ahci_hba->ports_implemented;
    impl = impl << 1;
    int port_index = -1;
    while(impl != 0){
        //next port
        port_index++;
        impl = impl >> 1;
        int type = ahci_port_check_type(&(ahci_hba->ports[port_index]));
        if(type != AHCI_DEV_SATA) continue;
        //get port ready
        AHCI_HBA_PORT* port = &(ahci_hba->ports[port_index]);
        ahci_port_stop_cmd(port);
        //remap command list
        uint64_t cmd_list_virt = mem_alloc(0x400,0x400);
        uint64_t cmd_list_phy;
        page_trace_PML4(cmd_list_virt, &cmd_list_phy);
        cmd_list_phy += cmd_list_virt & 0x0FFF;
        port->clb  = (uint32_t)  cmd_list_phy;
        port->clbu = (uint32_t) (cmd_list_phy >> 32);
        mem_write((void*) cmd_list_virt, 0,1024);
        //remap fis
        uint64_t fis_virt = mem_alloc(0x100, 0x100);
        uint64_t fis_phy;
        page_trace_PML4(fis_virt, &fis_phy);
        fis_phy += cmd_list_virt & 0x0FFF;
        port->fb  = (uint32_t)  cmd_list_phy;
        port->fbu = (uint32_t) (cmd_list_phy >> 32);
        mem_write((void*) fis_virt, 0, 256);


        //screenPrint("ASDF/n/e");
    }
}